
// ClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientDlg 对话框
CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENT_DIALOG, pParent)
	, m_edit_data(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_KEY, m_edit_data);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HELPINFO()
	ON_WM_NCPAINT()
	ON_WM_DRAWITEM()
	ON_WM_NCHITTEST()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CClientDlg::OnBnClickedButtonLogin)
END_MESSAGE_MAP()

// 控件初始化
void CClientDlg::InitCtrl()
{
	// 窗口标题随机字符串
	this->SetWindowText(m_anticrack.GetRandomString().c_str());

	// 修改编辑框文本的上下高度，使其垂直对齐
	CRect rect;
	GetDlgItem(IDC_EDIT_KEY)->GetClientRect(&rect);
	OffsetRect(&rect, 0, 3);
	SendMessageW(((CEdit*)GetDlgItem(IDC_EDIT_KEY))->m_hWnd, EM_SETRECT, 0, (LPARAM)&rect);

	// 设置编辑框最大文本长度
	((CEdit*)this->GetDlgItem(IDC_EDIT_KEY))->SetLimitText(32);		// 限制示例编辑框输入长度为6字节

	// 编辑框禁用输入法
	HWND hWnd = GetDlgItem(IDC_EDIT_KEY)->m_hWnd;
	if (hWnd && IsWindow(hWnd))
	{
		HIMC m_hImc = ImmGetContext(hWnd);

		if (m_hImc)
		{
			ImmAssociateContext(hWnd, NULL);
		}
		ImmReleaseContext(hWnd, m_hImc);
	}

	// StatusBar
	// StatusBar的子成员ID
	UINT indicators[] = { 10001,10002,10003,10004,10005,10006 };
	m_statusbar.Create(this);		// StatusBar的父窗口
	m_statusbar.SetIndicators(indicators, sizeof(indicators) / sizeof(indicators[0]));		// 设置StatusBar的子成员

	m_statusbar.SetPaneInfo(0, indicators[0], SBPS_POPOUT, 100);		// 设置子成员样式跟宽度
	m_statusbar.SetPaneInfo(1, indicators[1], SBPS_POPOUT, 80);
	m_statusbar.SetPaneInfo(2, indicators[2], SBPS_POPOUT, 100);
	m_statusbar.SetPaneInfo(3, indicators[3], SBPS_POPOUT, 80);
	m_statusbar.SetPaneInfo(4, indicators[4], SBPS_POPOUT, 100);
	m_statusbar.SetPaneInfo(5, indicators[5], SBPS_STRETCH, 100);

	m_statusbar.SetPaneText(0, "服务器状态:", true);		// 设置子成员显示文本
	m_statusbar.SetPaneText(1, "  未连接  ", true);
	m_statusbar.SetPaneText(2, "  当前版本:", true);
	m_statusbar.SetPaneText(3, "  未获取 ", true);
	m_statusbar.SetPaneText(4, "  最新版本:", true);
	m_statusbar.SetPaneText(5, "  未获取  ", true);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);		// 重新定位子窗口
}

// CClientDlg 消息处理程序
BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	// 执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 初始化控件
	InitCtrl();

	// TODO: 连接服务器

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 调用以在调度消息之前对其进行筛选
BOOL CClientDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		// 按键按下
		switch (pMsg->wParam)
		{
		case VK_RETURN:		// Enter键，不管什么时候都禁用，只在指定控件有焦点时有反应
			if (GetDlgItem(IDC_EDIT_KEY) == GetFocus())
			{
				// 编辑框允许使用Enter
				((CButton*)this->GetDlgItem(IDC_BUTTON_LOGIN))->SendMessage(WM_LBUTTONDOWN);		// 触发按下效果
				Sleep(200);		// 200毫秒后弹起
				((CEdit*)this->GetDlgItem(IDC_EDIT_KEY))->SetFocus();		// 编辑框设置焦点
				PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_LOGIN, BN_CLICKED), NULL);		// 执行按钮按下事件
			}
			return TRUE;
		case VK_ESCAPE:		// ESC键
			SendMessage(WM_CLOSE, 0, 0);
			return TRUE;
		case 0x41:
			if (::GetKeyState(VK_CONTROL) & 0x80)
			{
				// 组合键 Ctrl+A
				((CEdit*)this->GetDlgItem(IDC_EDIT_KEY))->SetSel(0, -1);
			}
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

// 当用户点击F1键时调用
BOOL CClientDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	//return CDialogEx::OnHelpInfo(pHelpInfo);
	return TRUE;		// 返回TRUE不显示HelpInfo界面
}

// 发出信号要某个窗口或应用程序终止
void CClientDlg::OnClose()
{
	// 处理所有退出程序的消息
	if (MessageBoxA("确定要退出验证程序吗?", "确定退出?", MB_YESNO) == IDYES)
	{
		CDialogEx::OnClose();
	}
}

// 指示窗口即将销毁
void CClientDlg::OnDestroy()
{
	// 必须在此进行资源释放，OnClose()->OnDestory()->PostQuitMessage()
	CDialogEx::OnDestroy();

	// TODO: 添加通信部分的释放，断开与服务器连接等
}

// 指示用户单击了按钮
void CClientDlg::OnBnClickedButtonLogin()
{
	// 正则判断输入卡密格式
	UpdateData(true);
	if (!regex_match(m_edit_data.GetString(), regex("[A-Z 0-9]{32}")))
	{
		MessageBoxA("请使用正确的格式!", "卡密格式有误!", MB_OK);
		((CEdit*)this->GetDlgItem(IDC_EDIT_KEY))->SetFocus();		// 编辑框设置焦点
		return;
	}

	// TODO: 服务器通信
}
