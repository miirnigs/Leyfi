#pragma once
#include "IOCP.h"
#include "OverlappedIOInfo.h"

class CIOCPServer
{
public:
	CIOCPServer();		// 默认构造函数
	~CIOCPServer();		// 析构函数
public:
	// 初始化WSA版本
	bool InitWSA();
	// 监听端口
	bool StartListen(unsigned short port);
	// 设置数据处理函数，服务端在接收到数据后会调用传入的函数指针
	// PS: 如果没设置，就默认不会处理接收到的消息
	void SetDataRecvAdapter(std::function<void(COverlappedIOInfo* pInfo)> Func)
	{
		DoRecv = Func;
	}
	// 设置数据处理函数，服务端在发送完数据后会调用传入的函数指针
	// PS: 如果没设置，就默认不会处理发送完的消息
	void SetDataSendAdapter(std::function<void(COverlappedIOInfo* pInfo)> Func)
	{
		DoSend = Func;
	}
	// 关闭服务器
	void CloseServer();
private:
	// 创建工作者线程
	int StartWorkThreadPool();
	// 获取AcceptEx和GetAcceptExSockaddrs的函数指针
	bool GetLPFNAcceptExAndGetAcceptSockaddrs();
	// 从容器里删除已经连接的客户端的结构
	void DelectLink(SOCKET socket);
	// 有客户接入，处理客户端接入事件
	bool DoAccept(COverlappedIOInfo* pInfo, DWORD NumberOfBytes);

	// 投递AcceptEx请求，利用AcceptEx监听accept请求
	bool PostAccept(COverlappedIOInfo* pInfo);
	// 投递一个接收数据WSARecv，完成时会自动通知完成端口
	bool PostRecv(COverlappedIOInfo* pInfo);
	// 投递一个发送数据WSASend，完成时会自动通知完成端口
	bool PostSend(COverlappedIOInfo* pInfo, const std::string& strData);

	// 获取当前在线IP列表
	std::vector<std::string> GetOnlineIPs();
private:
	WSAData m_wsaData;		// winsock版本类型
	SOCKET m_sListen;		// 监听的套接字
	CIOCP m_iocp;		// 完成端口的封装

	LPFN_ACCEPTEX m_lpfnAcceptEx;		// AcceptEx函数指针
	LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptSockAddrs;		// GetAcceptExSockaddrs函数指针

	std::vector<std::thread> m_workThreadPool;		// 存放工作线程的容器
	std::vector<SOCKET> m_acceptSockets;		// 存放提前创建好的套接字，用来给AccrptEx使用
	std::vector<COverlappedIOInfo*> m_clientSockets;		// 存放已经连接的结构，里面有缓冲区，对端IP，客户端套接字等

	std::mutex m_mu;		// 已经连接套接字容器的互斥体

	std::function<void(COverlappedIOInfo* pInfo)> DoRecv;		// 数据已到达-消息处理函数指针
	std::function<void(COverlappedIOInfo* pInfo)> DoSend;		// 数据已发送-消息处理函数指针
	
};