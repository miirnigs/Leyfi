#pragma once
#include <Windows.h>
#define MAX_BUFFER 1024 * 8		// 收发缓冲区大小

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
};
