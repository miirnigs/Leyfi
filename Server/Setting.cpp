// Setting.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "Setting.h"
#include "afxdialogex.h"


// CSetting 对话框
IMPLEMENT_DYNAMIC(CSetting, CDialogEx)

CSetting::CSetting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FORM_SETTING, pParent), isRunning(FALSE), m_port(5487)
{
}

CSetting::~CSetting()
{
}

void CSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_START, m_btnstart);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnstop);
	DDX_Control(pDX, IDC_EDIT_PORT, m_editport);
}


BEGIN_MESSAGE_MAP(CSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_START, &CSetting::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CSetting::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// CSetting 消息处理程序
BOOL CSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 先判断IOCP服务器是否开启
	if (isRunning)
	{
		// 正在运行
		m_editport.SetReadOnly(TRUE);
		m_btnstart.EnableWindow(FALSE);
		m_btnstop.EnableWindow(TRUE);
	}
	else
	{
		// 不在运行
		m_editport.SetReadOnly(FALSE);
		m_btnstart.EnableWindow(TRUE);
		m_btnstop.EnableWindow(FALSE);
	}

	// 编辑框显示当前的端口，可能是默认端口也可能是服务器启动后的端口
	m_editport.SetWindowTextA(Converter::CombineString(m_port).c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
}

unsigned int CSetting::GetPort()const
{
	return m_port;
}


void CSetting::OnBnClickedButtonStart()
{
	// 开启服务器
	// 1.初始化WSA
	if (!m_iocpServer.InitWSA())
	{
		if (MessageBoxA("初始化WSA失败！", "错误", MB_ICONERROR | MB_OK) == IDOK)
		{
			return;
		}
	}

	// 2.设置数据处理函数
	m_iocpServer.SetDataAdapter([](COverlappedIOInfo* pInfo)
		{
			// TODO: 接收到数据的处理
		});

	// 3.服务端开始监听
	m_port = GetDlgItemInt(IDC_EDIT_PORT, NULL, TRUE);
	if (!m_iocpServer.StartListen(m_port))
	{
		if (MessageBoxA("监听指定端口失败！", "错误", MB_ICONERROR | MB_OK) == IDOK)
		{
			return;
		}
	}

	// 监听成功
	isRunning = TRUE;
	m_editport.SetReadOnly(TRUE);
	m_btnstart.EnableWindow(FALSE);
	m_btnstop.EnableWindow(TRUE);

	// 往主界面事件列表添加项
	::SendMessage(this->GetParent()->m_hWnd, (WM_USER + 151), NULL, (LPARAM)(Converter::CombineString("服务器开启成功，开始监听", m_port, "端口").c_str()));

	if (MessageBoxA(Converter::CombineString("服务器开启成功，开始监听", m_port, "端口！").c_str(), "成功", MB_ICONINFORMATION | MB_OK) == IDOK)
	{
		// 服务器开启成功
		return;
	}
}

void CSetting::OnBnClickedButtonStop()
{
	// 关闭服务器
	m_iocpServer.CloseServer();

	// 关闭成功
	isRunning = FALSE;
	m_editport.SetReadOnly(FALSE);
	m_btnstart.EnableWindow(TRUE);
	m_btnstop.EnableWindow(FALSE);

	// 往主界面事件列表添加项
	::SendMessage(this->GetParent()->m_hWnd, (WM_USER + 151), NULL, (LPARAM)"服务器已停止监听");

	if (MessageBoxA("服务器已停止监听！", "成功", MB_ICONINFORMATION | MB_OK) == IDOK)
	{
		// 服务器开启成功
		return;
	}
}

// 调用以在调度消息之前对其进行筛选
BOOL CSetting::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:		// Enter键，不管什么时候都禁用
			if (GetDlgItem(IDC_EDIT_PORT) == GetFocus())		// 焦点在搜索编辑框
			{
				// 向按钮发送被点击的事件
				PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_START, BN_CLICKED), NULL);
			}
			return TRUE;
		case VK_ESCAPE:		// ESC键，不管什么时候都禁用掉
			return TRUE;
		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
