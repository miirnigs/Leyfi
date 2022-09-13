#pragma once
#define MAX_BUFFER 1024 * 8		// 收发缓冲区大小

// 因为完成端口绑定的handle是客户端的套接字，对于完成端口而言
// 一个Handle只能绑定一个Compkey，所以不可能将一个套接字同时绑定多个事件
// 解决方案是在COverlappedIOInfo类里加一个标志位，判断IO返回的事件是Send,Recv,还是accept
enum class IOEventType
{
	TYPE_INVALID = 1001,
	TYPE_ACP = 1002,
	TYPE_SEND = 1003,
	TYPE_RECV = 1004,
};

// 继承OVERLAPPED结构
// 因为AcceptEx是异步的，投递完AcceptEx后再见到就是在工作线程的GetQueuedCompletionStatus里
// 事实上会有很多不同的投递，工作线程里实际上是没法知道到底是那个客户端套接字或者具体的消息之类的
// 继承了OVERLAPPED结构就可以在末尾加上客户端套接字，在工作线程里就可以得知是哪个客户端套接字
class COverlappedIOInfo : public OVERLAPPED
{
public:
	COverlappedIOInfo()
	{
		m_clientSocket = INVALID_SOCKET;		// 初始化为无效的套接字
		ResetOverlapped();
		ResetRecvBuffer();
		ResetSendBuffer();
	}

	~COverlappedIOInfo()
	{
		if (m_clientSocket != INVALID_SOCKET)
		{
			closesocket(m_clientSocket);
			m_clientSocket = INVALID_SOCKET;
		}
	}

	void ResetOverlapped()
	{
		Internal = InternalHigh = 0;
		Offset = OffsetHigh = 0;
		hEvent = NULL;
	}

	void ResetRecvBuffer()
	{
		ZeroMemory(m_crecvBuf, MAX_BUFFER);
		m_recvBuf.buf = m_crecvBuf;
		m_recvBuf.len = MAX_BUFFER;
	}

	void ResetSendBuffer()
	{
		ZeroMemory(m_csendBuf, MAX_BUFFER);
		m_sendBuf.buf = m_csendBuf;
		m_sendBuf.len = MAX_BUFFER;
	}
public:
	SOCKET m_clientSocket;		// 客户端的套接字

	// 接收缓冲区，用于AcceptEx, WSARecv操作
	WSABUF m_recvBuf;
	char m_crecvBuf[MAX_BUFFER];

	// 发送缓冲区， 用于WSASend操作
	WSABUF m_sendBuf;
	char m_csendBuf[MAX_BUFFER];

	sockaddr_in m_addr;		// 对端地址

	IOEventType m_type;		// 事件的类型
};
