// Setting.cpp: 实现文件
//
#include "pch.h"
#include "Server.h"
#include "Setting.h"
#include "afxdialogex.h"
#include "IOCPServer.h"

// CSetting 对话框
IMPLEMENT_DYNAMIC(CSetting, CDialogEx)

CSetting::CSetting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FORM_SETTING, pParent)
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
	CServerDlg* pServerdlg = (CServerDlg*)this->GetParent();
	if (pServerdlg->m_running)
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
	m_editport.SetWindowTextA(std::to_string(pServerdlg->m_port).c_str());
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSetting::OnBnClickedButtonStart()
{
	// 发送开启事件到主界面
	if (::SendMessage(this->GetParent()->m_hWnd, WM_START_IOCP, NULL, (LPARAM)GetDlgItemInt(IDC_EDIT_PORT, NULL, TRUE)))
	{
		m_editport.SetReadOnly(TRUE);
		m_btnstart.EnableWindow(FALSE);
		m_btnstop.EnableWindow(TRUE);

		MessageBoxA(Converter::combine_string("服务器开启成功，开始监听", GetDlgItemInt(IDC_EDIT_PORT, NULL, TRUE), "端口！").c_str(), "成功", MB_ICONINFORMATION | MB_OK);
	}
	return;
}

void CSetting::OnBnClickedButtonStop()
{
	// 发送关闭事件到主界面
	::SendMessage(this->GetParent()->m_hWnd, WM_STOP_IOCP, NULL, NULL);

	// 关闭成功
	m_editport.SetReadOnly(FALSE);
	m_btnstart.EnableWindow(TRUE);
	m_btnstop.EnableWindow(FALSE);
	
	MessageBoxA("服务器已停止监听！", "成功", MB_ICONINFORMATION | MB_OK);
	return;
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
