#pragma once

// 完成端口的封装类，全部内联，让源文件美观一点
class CIOCP
{
public:
	CIOCP() :m_hIOCP(NULL) {}		// 默认构造函数
	~CIOCP()		// 析构函数
	{
		// 删除句柄
		if (m_hIOCP != NULL)
		{
			CloseHandle(m_hIOCP);
		}
	}
public:
	// 创建完成端口
	bool Create()
	{
		// 创建完成端口
		// FileHandle: 打开的文件句柄，因为现在是创建完成端口，所以设置为INVALID_HANDLE_VALUE
		// ExistingCompletionPort: 现有的I/O端口句柄，因为现在是创建完成端口，所以置NULL
		// CompletionKey: 完成密钥
		// NumberOfConcurrentThreads: 最大线程数，0=当前CPU核心数
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		return (m_hIOCP != NULL);
	}

	// 将Socket绑定到完成端口上
	bool AssociateSocket(SOCKET hSocket, ULONG_PTR CompKey)
	{
		return AssociateDevice((HANDLE)hSocket, CompKey);
	}

	//为设备（文件，socket,邮件槽，管道等）关联一个IOCP
	bool AssociateDevice(HANDLE hDevice, ULONG_PTR Compkey)
	{
		bool fOk = (CreateIoCompletionPort(hDevice, m_hIOCP, Compkey, 0) == m_hIOCP);
		return fOk;
	}

	// 返回完成端口句柄常量
	const HANDLE GetIOCP()
	{
		return m_hIOCP;
	}

	// 向完成端口发送指定状态
	bool PostStatus(ULONG_PTR CompKey, DWORD dwNumBytes = 0, OVERLAPPED* po = NULL)
	{
		return PostQueuedCompletionStatus(m_hIOCP, dwNumBytes, CompKey, po);
	}
private:
	HANDLE m_hIOCP;		// 完成端口的句柄
};

