#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <WinSock2.h>		// 较新的头文件
#include <MSWSock.h>		// 一定要放在WinSock2.h的下面
#pragma comment(lib, "ws2_32.lib")
#include "IOCP.h"
#include "OverlappedIOInfo.h"
using namespace std;

enum IOOperType
{ 
	TYPE_ACP,		// accept事件到达，有新的连接请求
	TYPE_RECV,		// 数据接收事件
	TYPE_SEND,		// 数据发送事件
	TYPE_CLOSE		// 关闭事件
};

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
	void SetDataAdapter(void(*p)(COverlappedIOInfo*));
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

	// 服务端接收到数据的处理
	//void DoRecv(COverlappedIOInfo* pInfo);
private:
	WSAData m_wsaData;		// winsock版本类型
	SOCKET m_sListen;		// 监听的套接字
	CIOCP m_iocp;		// 完成端口的封装

	LPFN_ACCEPTEX m_lpfnAcceptEx;		// AcceptEx函数指针
	LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptSockAddrs;		// GetAcceptExSockaddrs函数指针

	vector<thread> m_workThreadPool;		// 存放工作线程的容器
	vector<SOCKET> m_acceptSockets;		// 存放提前创建好的套接字，用来给AccrptEx使用
	vector<COverlappedIOInfo*> m_clientSockets;		// 存放已经连接的结构，里面有缓冲区，对端IP，客户端套接字等

	mutex m_mu;		// 已经连接套接字容器的互斥体

	/*不能用std::function，functional.h里居然TMD有个叫bind的函数与Socket的bind重合了*/
	void(*DoRecv)(COverlappedIOInfo*);		// 函数指针，服务端接收到数据的处理		
};