#include "IOCPServer.h"

CIOCPServer::CIOCPServer() :DoRecv(NULL), m_lpfnAcceptEx(NULL), m_lpfnGetAcceptSockAddrs(NULL), m_sListen(NULL), m_wsaData()
{
}

CIOCPServer::~CIOCPServer()
{
	// 关闭服务器并
	CloseServer();
	WSACleanup();
}

bool CIOCPServer::InitWSA()
{
	// WinSock初始化，最新版本2.2
	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		// 初始化失败
		return FALSE;
	}
	return TRUE;
}

bool CIOCPServer::StartListen(unsigned short port)
{
	// 创建TCP套接字，Socket需要将accept操作投递到完成端口，因此socket属性必须有重叠IO
	// 普通Socket不支持IO重叠，所以要用WSASocket
	// af: IP 地址类型，可以是AF_INET[IPv4]，也可以是AF_INET6[IPv6]
	// type; 套接字类型，可以是SOCK_STREAM[面向连接的套接字]，也可以是SOCK_DGRAM[无连接的套接字]
	// protocol: 传输协议，可以是IPPROTO_TCP[TCP传输协议]，也可以是IPPTOTO_UDP[UDP传输协议]
	// lpProtocolInfo: 置NULL就好，非NULL则前三个属性无效
	// g: 已存在的套接字群组的ID，置0即可
	// dwFlags: 套接字属性描述，因为需要重叠IO，所以使用WSA_FLAG_OVERLAPPED
	m_sListen = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_sListen == INVALID_SOCKET)
	{
		// 创建套接字失败
		return FALSE;
	}

	// 创建完成端口
	if (!m_iocp.Create())
	{
		// 完成端口创建失败
		return FALSE;
	}

	// 绑定套接字到完成端口上，当有投递的AcceptEx有客户端连接了，就会监听端口会得到通知
	// 工作线程内调用GetQueuedCompletionStatus获取到的completionKey就会是TYPE_ACP
	if (!m_iocp.AssociateSocket(m_sListen, TYPE_ACP))
	{
		return false;
	}
	
	// 初始化Socket的一系列参数
	sockaddr_in server;		// 一般用sockaddr_in，不用sockaddr
	server.sin_family = AF_INET;
	server.sin_port = htons(port);		// 把端口号主机字节序转换为网络字节序
	server.sin_addr.s_addr = INADDR_ANY;		// 0.0.0.0，任何地址
	
	if (bind(m_sListen, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		return FALSE;
	}

	if (listen(m_sListen, SOMAXCONN) == SOCKET_ERROR)
	{
		return FALSE;
	}

	//启动工作者线程
	if (StartWorkThreadPool() == 0)
	{
		return FALSE;
	}

	// 获取AcceptEx和GetAcceptExSockaddrs的函数指针
	if (!GetLPFNAcceptExAndGetAcceptSockaddrs())
	{
		return FALSE;
	}

	// 投递AcceptEx请求
	for (int i = 0; i < 15; i++)
	{
		COverlappedIOInfo* pInfo = new COverlappedIOInfo();		// 分配内存，需要delete
		if (!PostAccept(pInfo))
		{
			delete pInfo;
			return FALSE;
		}
	}

	return TRUE;
}

void CIOCPServer::SetDataAdapter(void(*p)(COverlappedIOInfo*))
{
	DoRecv = p;
}

void CIOCPServer::CloseServer()
{
	// 清空IOCP线程队列， 退出工作线程， 给所有的线程发送TYPE_CLOSE信息
	for (int i = 0; i < m_workThreadPool.size(); i++)
	{
		m_iocp.PostStatus(TYPE_CLOSE);
	}

	// 清空提前创建好的的套接字
	for (auto iter = m_acceptSockets.begin(); iter != m_acceptSockets.end(); iter++)
	{
		closesocket(*iter);
	}
	m_acceptSockets.clear();

	// 清空已连接的套接字并清空缓存
	for (auto iter = m_clientSockets.begin(); iter != m_clientSockets.end(); iter++)
	{
		COverlappedIOInfo* ol = *iter;
		closesocket(ol->m_clientSocket);
		delete ol;
	}
	m_clientSockets.clear();
}

int CIOCPServer::StartWorkThreadPool()
{
	// 让系统自动决定创建多少线程
	int nCount = thread::hardware_concurrency();

	for (int i = 0; i < nCount; i++)
	{
		// 被创建的线程会自动执行
		m_workThreadPool.emplace_back(
			[this]()
			{
				// 死循环去检查完成端口的状态
				while (1)
				{
					DWORD NumberOfBytes = 0;
					ULONG_PTR completionKey = 0;		// PULONG_PTR是ULONG_PTR的指针
					OVERLAPPED* ol = NULL;

					// 阻塞调用GetQueuedCompletionStatus获取完成端口的状态
					if (GetQueuedCompletionStatus(this->m_iocp.GetIOCP(), &NumberOfBytes, &completionKey, &ol, WSA_INFINITE))
					{
						COverlappedIOInfo* olinfo = (COverlappedIOInfo*)ol;
						if (completionKey == TYPE_CLOSE)
						{
							// 退出工作线程 
							break;
						}
						if (NumberOfBytes == 0 && (completionKey == TYPE_RECV || completionKey == TYPE_SEND))
						{
							// 如果接收到数据或者发送完数据的字节数为0，意味着客户端断开连接
							this->DelectLink(olinfo->m_clientSocket);
							continue;
						}

						// 会进来GetQueuedCompletionStatus代表事件已经完成
						switch (completionKey)
						{
						case TYPE_ACP:
						{
							// 客户端接入完成
							this->DoAccept(olinfo, NumberOfBytes);
							this->PostAccept(olinfo);		// 投递一个新的AcceptEx
							break;
						}
						case TYPE_RECV:
						{
							// 数据接收完成
							if (this->DoRecv != NULL)
							{
								// 有数据处理的函数
								this->DoRecv(olinfo);
							}
							this->PostRecv(olinfo);
							break;
						}
						case TYPE_SEND:
						{
							//数据发送完成
							break;
						}
						}
					}
				}
			}
		);
	}
	return nCount;
}

bool CIOCPServer::GetLPFNAcceptExAndGetAcceptSockaddrs()
{
	DWORD ByteReturned = 0;

	// 获取AcceptEx函数指针
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	if (SOCKET_ERROR == WSAIoctl(
		m_sListen,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx,
		sizeof(GuidAcceptEx),
		&m_lpfnAcceptEx,
		sizeof(m_lpfnAcceptEx),
		&ByteReturned,
		NULL, NULL
	))
	{
		return FALSE;
	}

	// 获取GetAcceptExSockAddrs函数指针
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
	if (SOCKET_ERROR == WSAIoctl(
		m_sListen,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidGetAcceptExSockAddrs,
		sizeof(GuidGetAcceptExSockAddrs),
		&m_lpfnGetAcceptSockAddrs,
		sizeof(m_lpfnGetAcceptSockAddrs),
		&ByteReturned,
		NULL, NULL
	))
	{
		return FALSE;
	}
	return TRUE;
}

bool CIOCPServer::PostAccept(COverlappedIOInfo* pInfo)
{
	// 理论上函数指针不会是NULL
	if (m_lpfnAcceptEx == NULL)
	{
		return FALSE;
	}

	// 先把当前的socket保存一下，这个socket有可能是Invalid_sockets，也有可能是工作线程那边传过来的已经用过的socket
	SOCKET socket = pInfo->m_clientSocket;

	// 清空指针里的缓冲区跟原本结构，同时创建新的客户端套接字
	pInfo->ResetSendBuffer();
	pInfo->ResetRecvBuffer();
	pInfo->ResetOverlapped();
	pInfo->m_clientSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (pInfo->m_clientSocket == INVALID_SOCKET)
	{
		// 创建新的客户端套接字失败
		return FALSE;
	}

	// 调用AcceptEx监听accept请求，传入新创建的结构
	DWORD byteReceived = 0;
	if (FALSE == m_lpfnAcceptEx
	(
		m_sListen,		// 监听的套接字
		pInfo->m_clientSocket,		// 创建的客户端套接字
		pInfo->m_recvBuf.buf,		// 缓冲区
		pInfo->m_recvBuf.len - (sizeof(SOCKADDR_IN) + 16) * 2,		// 缓冲区大小
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&byteReceived,
		pInfo		// 传入的结构在工作线程里面能够看到
	))
	{
		// AcceptEx如果返回FALSE不一定代表失败，如果错误码是997其实也是成功
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			// 如果错误码不是997那就是失败
			return FALSE;
		}
	}

	// 在容器中搜索有没有使用过的套接字，替换成未使用的套接字
	vector<SOCKET>::iterator iter = m_acceptSockets.begin();
	for (; iter != m_acceptSockets.end(); ++iter)
	{
		if (*iter == socket)
		{
			*iter = pInfo->m_clientSocket;
		}
	}

	// 列表中没有找到socket
	if (iter == m_acceptSockets.end())
	{
		m_acceptSockets.push_back(pInfo->m_clientSocket);
	}

	return TRUE;
}

bool CIOCPServer::PostRecv(COverlappedIOInfo* pInfo)
{
	DWORD BytesRecevd = 0, dwFlags = 0;

	// 清空接收缓冲区
	pInfo->ResetOverlapped();
	pInfo->ResetRecvBuffer();

	// WSARecv应该一个套接字一次只能投递一个，不像WSASend可以一次投递多个
	if (WSARecv(pInfo->m_clientSocket, &pInfo->m_recvBuf, 1, &BytesRecevd, &dwFlags, (OVERLAPPED*)pInfo, NULL) != 0)
	{
		// WSARecv如果返回0不一定代表失败，如果错误码是997其实也是成功
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			// 如果错误码不是997那就是失败
			return FALSE;
		}
	}
	return TRUE;
}

void CIOCPServer::DelectLink(SOCKET socket)
{
	// 自动上锁，函数结束这个对象销毁时自动解锁
	lock_guard<mutex> lock(m_mu);
	for (auto iter = m_clientSockets.begin(); iter != m_clientSockets.end(); iter++)
	{
		if ((*iter)->m_clientSocket == socket)
		{
			// 如果找到相同的socket那就删除整个结构
			// 不能够直接删除，要把迭代器取址到类指针，再删除这个类指针，这个类指针指向DoAccept里创建的那片内存，不取址直接delete是删除这个迭代器
			COverlappedIOInfo* ol = *iter;
			closesocket(socket);
			m_clientSockets.erase(iter);
			delete ol;
			break;
		}
	}
}

bool CIOCPServer::DoAccept(COverlappedIOInfo* pInfo, DWORD NumberOfBytes)
{
	// AcceptEx至少需要接收到一个字节才会触发，所以NumberOfBytes一定大于0
	SOCKADDR_IN* ClientAddr = NULL;
	int remoteLen = sizeof(SOCKADDR_IN);
	if (NumberOfBytes > 0)
	{
		if (m_lpfnGetAcceptSockAddrs)
		{
			// GetAcceptExSockaddrs函数指针可用
			/*接收的数据分为3部分：1. 客户端发来的数据  2. 本地地址  3. 远端地址*/
			SOCKADDR_IN* LocalAddr = NULL;
			int localLen = sizeof(SOCKADDR_IN);

			m_lpfnGetAcceptSockAddrs(
				pInfo->m_recvBuf.buf,
				pInfo->m_recvBuf.len - (sizeof(SOCKADDR_IN) + 16) * 2,
				sizeof(SOCKADDR_IN) + 16,
				sizeof(SOCKADDR_IN) + 16,
				(LPSOCKADDR*)&LocalAddr,
				&localLen,
				(LPSOCKADDR*)&ClientAddr,
				&remoteLen
			);

			// 如果获取到对端地址，就接收这个客户端，与他建立连接，如果获取不到，就不接收直接返回
			if (ClientAddr != NULL)
			{
				// 如果获取到对端地址
				COverlappedIOInfo* olinfo = new COverlappedIOInfo();		// 新的结构会被放到m_clientSockets里
				olinfo->m_clientSocket = pInfo->m_clientSocket;
				olinfo->m_addr = *ClientAddr;

				// 为这个客户端的套接字绑定Recv跟Send到完成端口
				if (m_iocp.AssociateSocket(olinfo->m_clientSocket, TYPE_RECV) && m_iocp.AssociateSocket(olinfo->m_clientSocket, TYPE_SEND))
				{
					// 绑定成功，把结构放到存放已经连接的结构的容器里并投递Recv
					PostRecv(olinfo);
					m_clientSockets.push_back(olinfo);
					return FALSE;
				}
				else
				{
					// 绑定失败就删除创建的结构不与他连接
					delete olinfo;
					return FALSE;
				}
			}
			return FALSE;
		}
		return FALSE;
	}
	return FALSE;
}