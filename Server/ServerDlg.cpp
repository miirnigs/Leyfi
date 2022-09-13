// ServerDlg.cpp: 实现文件
//
#include "pch.h"
#include "framework.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CServerDlg 对话框
CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_FORM, pParent), uTimerID(NULL)
	, cstrSearch(_T("")), m_running(FALSE), m_port(5487)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_columnData[0] = { "时间" ,0 ,140 };
	m_columnData[1] = { "事件" ,1 ,320 };
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MANAGE, m_tabsheet);
	DDX_Control(pDX, IDC_LIST_EVENT, m_listevent);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_combotype);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_editsearch);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_buttonsearch);
	DDX_Text(pDX, IDC_EDIT_SEARCH, cstrSearch);
}

void CServerDlg::InitCtrl()
{
	/*初始化各种控件*/
	// Tab Control
	m_tabsheet.AddPage("在线客户端", &m_tabonline, IDD_TAB_ONLINE, true);
	m_tabsheet.AddPage("卡密管理", &m_tabkey, IDD_TAB_KEY, true);
	m_tabsheet.AddPage("软件管理", &m_tabsoftware, IDD_TAB_SOFTWARE, true);
	m_tabsheet.AddPage("黑名单", &m_tabblock, IDD_TAB_BLOCK, true);
	m_tabsheet.AddPage("远程数据", &m_tabdata, IDD_TAB_DATA, true);
	m_tabsheet.AddPage("高级设置", &m_tabconfig, IDD_TAB_CONFIG, true);
	m_tabsheet.Show();

	// StatusBar
	// StatusBar的子成员ID
	UINT indicators[] = { 10001,10002,10003,10004,10005,10006 };
	m_statusbar.Create(this);		// StatusBar的父窗口
	m_statusbar.SetIndicators(indicators, sizeof(indicators) / sizeof(indicators[0]));		// 设置StatusBar的子成员

	m_statusbar.SetPaneInfo(0, indicators[0], SBPS_POPOUT, 115);		// 设置子成员样式跟宽度
	m_statusbar.SetPaneInfo(1, indicators[1], SBPS_POPOUT, 115);
	m_statusbar.SetPaneInfo(2, indicators[2], SBPS_POPOUT, 115);
	m_statusbar.SetPaneInfo(3, indicators[3], SBPS_POPOUT, 115);
	m_statusbar.SetPaneInfo(4, indicators[4], SBPS_POPOUT, 570);
	m_statusbar.SetPaneInfo(5, indicators[5], SBPS_STRETCH, 100);
	
	m_statusbar.SetPaneText(0, "在线用户: 0", true);		// 设置子成员显示文本
	m_statusbar.SetPaneText(1, "卡密数量: 0", true);
	m_statusbar.SetPaneText(2, "软件数量: 0", true);
	m_statusbar.SetPaneText(3, "封禁数量: 0", true);
	m_statusbar.SetPaneText(4, "          你有过这种感觉没有，就是你吃不准自己是醒着还是在做梦          ", true);
	m_statusbar.SetPaneText(5, "当前时间: 00:00:00", true);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);		// 重新定位子窗口

	// ListControl
	// 风格:整行高亮，网格线
	m_listevent.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 添加列
	for (auto szColumn : m_columnData)
	{
		m_listevent.InsertColumn(szColumn.nIndex, szColumn.Name, LVCFMT_LEFT, szColumn.nWidth);
	}

	// Timer
	// 很想用匿名函数，tmd函数内要调用成员，SetTimer的回调函数写死了，匿名函数的捕获列表传不进去this指针 
	// 失败: SetTimer(1, 1000, [this](HWND, UINT, UINT_PTR, DWORD) {	});
	if (!uTimerID)
	{
		SetTimer(1, 1000, NULL);		// 设置ID为1，1000毫秒，事件回调的定时器
	}

	// ComboBox
	// 设置默认索引
	m_combotype.SetCurSel(0);
	
	// 往事件列表里添加成功信息
	SendMessage(WM_LIST_ADD, NULL, (LPARAM)"控件初始化成功");
}

void CServerDlg::InitSQL(std::string sqlPath)
{
	// 判断数据库是否存在
	if (_access(Converter::combine_string(Path::get_exe_path(false), "Server.db").c_str(), 0) == -1)
	{
		// 数据库文件不存在
		if (MessageBoxA("未在运行目录找到Server.db数据库文件！\n请导入数据库文件后重试！", "错误", MB_ICONERROR | MB_OK) == IDOK)
		{
			// 退出程序
			PostQuitMessage(0);
		}
	}

	// 数据库文件存在，创建数据库对象
	// PS: 虽然就算数据库文件不存在也会自动创建，不过自动创建完还要建表太麻烦，在外面用sqlite studio手动快一点
	if (!Sqlite::GetInstance().OpenDB("Server.db"))
	{
		// 数据库未连接且连接数据库失败
		if (MessageBoxA("连接数据库文件失败！", "错误", MB_ICONERROR | MB_OK) == IDOK)
		{
			// 退出程序
			PostQuitMessage(0);
		}
	}

	// 数据库对象创建成功，要查表，看看表齐不齐全
	// 因为有自增id，所以数据库内一定会有sqlite_sequence表
	std::vector<std::string> lstTables;
	if (Sqlite::GetInstance().GetTables(lstTables))
	{
		// 获取数据库表成功
		char* tables[] = { "Client","Key","Software","Block","RemoteData","Setting","sqlite_sequence" };
		for (auto table : tables)
		{
			if (find(lstTables.begin(), lstTables.end(), table) == lstTables.end())
			{
				// 没找到其中的任意一个表
				if (MessageBoxA(Converter::combine_string("数据库表\"", table, "\"缺失！").c_str(), "错误", MB_ICONERROR | MB_OK) == IDOK)
				{
					// 退出程序
					PostQuitMessage(0);
				}
			}
		}
	}

	// 验证表成功，验证列
	try
	{
		// 不想用太多if嵌套，用try...catch如果出问题就直接把表名跟列名throw出来就行
		// TODO: 这里可能可以用函数封装起来，不然看起来太复杂了
		char* column_1[] = { "ID","IPaddress","Key","MachineCode","Software","OnlineTime","ExpireTime","Note" };
		for (auto column : column_1)
		{
			if (!Sqlite::GetInstance().GetColumn("Client", column))
			{
				throw std::make_pair("Client", column);
			}
		}

		char* column_2[] = { "ID","Key","IPaddress","MachineCode","Software","ActivationTime","ExpireTime","Ban","Note" };
		for (auto column : column_2)
		{
			if (!Sqlite::GetInstance().GetColumn("Key", column))
			{
				throw std::make_pair("Key", column);
			}
		}

		char* column_3[] = { "ID","Software","AppKey","AppSecret","CreateTime","KeyCount","Status","Note" };
		for (auto column : column_3)
		{
			if (!Sqlite::GetInstance().GetColumn("Software", column))
			{
				throw std::make_pair("Software", column);
			}
		}

		char* column_4[] = { "ID","BlockKey","BlockIP","BlockMachineCode","StartTime","EndTime","Software","Status","Reason" };
		for (auto column : column_4)
		{
			if (!Sqlite::GetInstance().GetColumn("Block", column))
			{
				throw std::make_pair("Block", column);
			}
		}

		char* column_5[] = { "ID","Name","Data","Software","CreateTime","Status","Note" };
		for (auto column : column_5)
		{
			if (!Sqlite::GetInstance().GetColumn("RemoteData", column))
			{
				throw std::make_pair("RemoteData", column);
			}
		}

		char* column_6[] = { "ID","Key","Value","Note" };
		for (auto column : column_6)
		{
			if (!Sqlite::GetInstance().GetColumn("Setting", column))
			{
				throw std::make_pair("Setting", column);
			}
		}
	}
	catch (std::pair<const char*, char*> msg)		// 注意！！第一个模板类型是const char*，不小心填成char*编译器居然不会报错，但是运行会崩
	{
		// 没找到任何一个列
		if (MessageBoxA(Converter::combine_string("数据库表\"", msg.first, "\"", "的\"", msg.second, "\"列缺失！").c_str(), "错误", MB_ICONERROR | MB_OK) == IDOK)
		{
			// 退出程序
			PostQuitMessage(0);
		}
	}

	// 往事件列表里添加成功信息
	SendMessage(WM_LIST_ADD, NULL, (LPARAM)"数据库初始化成功");
}

void CServerDlg::LoadData()
{
	// 从数据库读取数据
	std::vector<std::vector<std::string> > lstData;		// 使用一个vector从数据库读取数据，不用clear或者swap
	char* tables[] = { "Key","Software","Block","RemoteData","Setting" };

	for (int i = 0; i < sizeof(tables) / sizeof(tables[0]); i++)
	{
		// 一次循环读取一个表
		if (!Sqlite::GetInstance().GetData(tables[i], lstData))
		{
			// 读取数据失败
			if (MessageBoxA(Converter::combine_string("从数据库表\"", tables[i], "\"读取数据失败！").c_str(), "错误", MB_ICONERROR | MB_OK) == IDOK)
			{
				// 退出程序
				PostQuitMessage(0);
			}
		}

		// 读取成功，把数据放到List里
		for (auto row : lstData)
		{
			// 每一行数据
			// 用switch分离每一张表
			switch (i)
			{
			case 0:
			{
				// 卡密表
				// 当前item的数量
				int nIndex = m_tabkey.m_listkey.GetItemCount();

				// 第八行是在线状态，数据库不保存这个的，所以默认是离线
				row.insert(row.begin() + 7, "离线");

				for (unsigned int j = 0; j < row.size(); j++)
				{
					if (!j)
					{
						m_tabkey.m_listkey.InsertItem(nIndex, row[j].c_str());
					}
					else
					{
						m_tabkey.m_listkey.SetItemText(nIndex, j, row[j].c_str());
					}
				}
				break;
			}
			case 1:
			{
				// 软件表
				// 当前item的数量
				int nIndex = m_tabsoftware.m_listsoftware.GetItemCount();
				for (unsigned int j = 0; j < row.size(); j++)
				{
					if (!j)
					{
						m_tabsoftware.m_listsoftware.InsertItem(nIndex, row[j].c_str());
					}
					else
					{
						m_tabsoftware.m_listsoftware.SetItemText(nIndex, j, row[j].c_str());
					}
				}
				break;
			}
			case 2:
			{
				// 封禁表
				// 当前item的数量
				int nIndex = m_tabblock.m_listblock.GetItemCount();
				for (unsigned int j = 0; j < row.size(); j++)
				{
					if (!j)
					{
						m_tabblock.m_listblock.InsertItem(nIndex, row[j].c_str());
					}
					else
					{
						m_tabblock.m_listblock.SetItemText(nIndex, j, row[j].c_str());
					}
				}
				break;
			}
			case 3:
			{
				// 远程数据表
				// 当前item的数量
				int nIndex = m_tabdata.m_listdata.GetItemCount();
				for (unsigned int j = 0; j < row.size(); j++)
				{
					if (!j)
					{
						m_tabdata.m_listdata.InsertItem(nIndex, row[j].c_str());
					}
					else
					{
						m_tabdata.m_listdata.SetItemText(nIndex, j, row[j].c_str());
					}
				}
				break;
			}
			case 4:
			{
				// 设置表
				// 当前item的数量
				int nIndex = m_tabconfig.m_listconfig.GetItemCount();
				for (unsigned int j = 0; j < row.size(); j++)
				{
					if (!j)
					{
						m_tabconfig.m_listconfig.InsertItem(nIndex, row[j].c_str());
					}
					else
					{
						m_tabconfig.m_listconfig.SetItemText(nIndex, j, row[j].c_str());
					}
				}
				break;
			}
			}
		}
		lstData.clear();
	}

	// 往事件列表里添加成功信息
	SendMessage(WM_LIST_ADD, NULL, (LPARAM)"数据库数据读取成功");
}

void CServerDlg::ShowDataCount()
{
	// 获取各个ListControl的行数
	int keyCount = m_tabkey.m_listkey.GetItemCount();
	int softwareCount = m_tabsoftware.m_listsoftware.GetItemCount();
	int blockCount = m_tabblock.m_listblock.GetItemCount();
	
	m_statusbar.SetPaneText(1, Converter::combine_string("卡密数量: ", keyCount).c_str(), true);
	m_statusbar.SetPaneText(2, Converter::combine_string("软件数量: ", softwareCount).c_str(), true);
	m_statusbar.SetPaneText(3, Converter::combine_string("封禁数量: ", blockCount).c_str(), true);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDM_EXIT, &CServerDlg::OnExit)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MANAGE, &CServerDlg::OnTcnSelchangeTabManage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CServerDlg::OnBnClickedButtonSearch)
	ON_MESSAGE(WM_LIST_CHANGE, OnListControlChanged)
	ON_MESSAGE(WM_LIST_ADD, OnListControlAdd)
	ON_MESSAGE(WM_START_IOCP, OnStartIOCP)
	ON_MESSAGE(WM_STOP_IOCP, OnStopIOCP)
	ON_COMMAND(IDM_SETTING, &CServerDlg::OnSetting)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// CServerDlg 消息处理程序
BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 控件初始化
	InitCtrl();

	// 数据库初始化
	InitSQL("Server.db");
	
	// 读取数据库数据，放入List Control
	LoadData();

	// 读取完往状态栏显示各个数据的数量
	ShowDataCount();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CServerDlg::OnPaint()
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

// 当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 退出程序
void CServerDlg::OnExit()
{
	if (MessageBoxA("确定要退出程序?", "退出程序", MB_ICONWARNING | MB_YESNO) == IDYES)
	{
		PostQuitMessage(0);
	}
}

// 指示选定项已从一项变为另一项
void CServerDlg::OnTcnSelchangeTabManage(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nCurSel = m_tabsheet.GetCurSel();

	// 除了Index = 1的tab以外的控件全部禁用
	switch (nCurSel)
	{
	case 1:
		m_combotype.EnableWindow(TRUE);		// 启用控件
		m_editsearch.EnableWindow(TRUE);
		m_buttonsearch.EnableWindow(TRUE);
		break;
	default:
		m_combotype.EnableWindow(FALSE);		// 禁用控件
		m_editsearch.EnableWindow(FALSE);
		m_buttonsearch.EnableWindow(FALSE);
		break;
	}
	*pResult = 0;
}

// 类的定时器回调
void CServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:		// 定时器id为1的定时器
	{
		// 状态栏显示当前时间
		tm time = Time::get_date_time();
		m_statusbar.SetPaneText(5, Converter::combine_string("当前时间: ",
			time.tm_year + 1900, "年",
			time.tm_mon + 1, "月",
			time.tm_mday, "日 ",
			time.tm_hour, ":",
			time.tm_min, ":",
			time.tm_sec).c_str(), true);		// 设置状态栏时间
		break;
	}
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


// 指示用户单击了按钮
void CServerDlg::OnBnClickedButtonSearch()
{
	UpdateData(TRUE);		// 更新数据到内存
	if (cstrSearch.GetLength() <= 0)
	{
		// 查找的字符串为空
		if (MessageBoxA("搜索字符串不能为空", "错误", MB_ICONERROR | MB_OK) == IDOK)
		{
			return;
		}
	}

	// 获取当前索引
	int nType = m_combotype.GetCurSel();
	switch (nType)
	{
	case 0:		// IP地址
		for (int i = 0; i < m_tabkey.m_listkey.GetItemCount(); i++)
		{
			if (m_tabkey.m_listkey.GetItemText(i, 2) == cstrSearch)
			{
				// 控件设置焦点
				m_tabkey.m_listkey.SetFocus();
				// 高亮指定项
				m_tabkey.m_listkey.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				break;		// 跳出for
			}
			else if (i == (m_tabkey.m_listkey.GetItemCount() - 1))
			{
				// 未查找到指定字符串
				if (MessageBoxA("未搜索到指定IP地址！", "错误", MB_ICONERROR | MB_OK) == IDOK)
				{
					break;
				}
			}
		}
		break;
	case 1:		// 备注
		for (int i = 0; i < m_tabkey.m_listkey.GetItemCount(); i++)
		{
			if (m_tabkey.m_listkey.GetItemText(i, 9) == cstrSearch)
			{
				// 控件设置焦点
				m_tabkey.m_listkey.SetFocus();
				// 高亮指定项
				m_tabkey.m_listkey.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				break;		// 跳出for
			}
			else if (i == (m_tabkey.m_listkey.GetItemCount() - 1))
			{
				// 未查找到指定字符串
				if (MessageBoxA("未搜索到指定备注！", "错误", MB_ICONERROR | MB_OK) == IDOK)
				{
					break;
				}
			}
		}
		break;
	case 2:		// 机器码
		for (int i = 0; i < m_tabkey.m_listkey.GetItemCount(); i++)
		{
			if (m_tabkey.m_listkey.GetItemText(i, 3) == cstrSearch)
			{
				// 控件设置焦点
				m_tabkey.m_listkey.SetFocus();
				// 高亮指定项
				m_tabkey.m_listkey.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				break;		// 跳出for
			}
			else if (i == (m_tabkey.m_listkey.GetItemCount() - 1))
			{
				// 未查找到指定字符串
				if (MessageBoxA("未搜索到指定机器码！", "错误", MB_ICONERROR | MB_OK) == IDOK)
				{
					break;
				}
			}
		}
		break;
	case 3:		// 卡密
		for (int i = 0; i < m_tabkey.m_listkey.GetItemCount(); i++)
		{
			if (m_tabkey.m_listkey.GetItemText(i, 1) == cstrSearch)
			{
				// 控件设置焦点
				m_tabkey.m_listkey.SetFocus();
				// 高亮指定项
				m_tabkey.m_listkey.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
				break;		// 跳出for
			}
			else if (i == (m_tabkey.m_listkey.GetItemCount() - 1))
			{
				// 未查找到指定字符串
				if (MessageBoxA("未搜索到指定卡密！", "错误", MB_ICONERROR | MB_OK) == IDOK)
				{
					break;
				}
			}
		}
		break;
	}
}

// 自实现的消息，卡密对话框的listcontrol项被更改就会触发这个事件
LRESULT CServerDlg::OnListControlChanged(WPARAM wParam, LPARAM lParam)
{
	// 因为有ListControl里的项有改变才会来到这里
	if (lParam >= 0)
	{
		GetDlgItem(IDC_EDIT_KEY)->SetWindowTextA(m_tabkey.m_listkey.GetItemText((int)lParam, 1));
	}
	else
	{
		GetDlgItem(IDC_EDIT_KEY)->SetWindowTextA(NULL);
	}
	return NULL;
}

// 自实现的消息，任何界面需要往主界面的事件ListControl添加项都可以触发这个事件
LRESULT CServerDlg::OnListControlAdd(WPARAM wParam, LPARAM lParam)
{
	// 往事件列表里添加成功信息
	int nIndex = m_listevent.GetItemCount();
	tm time = Time::get_date_time();

	m_listevent.InsertItem(nIndex, Converter::combine_string(
		time.tm_year + 1900, "年",
		time.tm_mon + 1, "月",
		time.tm_mday, "日 ",
		time.tm_hour, ":",
		time.tm_min, ":",
		time.tm_sec).c_str());
	m_listevent.SetItemText(nIndex, 1, (char*)lParam);
	return NULL;
}

// 调用以在调度消息之前对其进行筛选
BOOL CServerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:		// Enter键，不管什么时候都禁用，只在指定控件有焦点时有反应
			if (GetDlgItem(IDC_EDIT_SEARCH) == GetFocus())		// 焦点在搜索编辑框
			{
				// 向按钮发送被点击的事件
				PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_SEARCH, BN_CLICKED), NULL);
			}
			else if(GetDlgItem(IDC_BUTTON_SEARCH) == GetFocus())		// 焦点在搜索按钮
			{
				PostMessage(WM_COMMAND, MAKEWPARAM(IDC_BUTTON_SEARCH, BN_CLICKED), NULL);
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

// 服务器设置
void CServerDlg::OnSetting()
{
	// Setting对话框对象
	CSetting m_setting;

	// 传入父窗口指针
	// 模态形式打开服务器设置界面
	//m_setting.m_serverdlg = this;
	m_setting.DoModal();
}

// 当用户点击F1键时调用
BOOL CServerDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	//return CDialogEx::OnHelpInfo(pHelpInfo);
	return TRUE;		// 返回TRUE不显示HelpInfo界面
}

// 自实现的消息，开启IOCP服务器
LRESULT CServerDlg::OnStartIOCP(WPARAM wParam, LPARAM lParam)
{
	// 开启服务器
	// 1.初始化WSA
	if (!m_iocpServer.InitWSA())
	{
		MessageBoxA("初始化WSA失败！", "错误", MB_ICONERROR | MB_OK);
		return FALSE;
	}

	// 2.设置数据处理函数
	// 这里创建一个函数，然后传进去
	m_iocpServer.SetDataRecvAdapter(std::bind(&CServerDlg::DoRecv, this, std::placeholders::_1));
	m_iocpServer.SetDataSendAdapter(std::bind(&CServerDlg::DoSend, this, std::placeholders::_1));

	// 3.服务端开始监听
	if (((unsigned short)lParam == 0) &&
		(!m_iocpServer.StartListen((unsigned short)lParam)))
	{
		MessageBoxA("监听指定端口失败！", "错误", MB_ICONERROR | MB_OK);
		return FALSE;
	}

	// 监听成功
	m_port = (unsigned short)lParam;
	m_running = TRUE;

	// 往事件列表添加项
	SendMessage(WM_LIST_ADD, NULL, (LPARAM)(Converter::combine_string("服务器开启成功，开始监听", m_port, "端口").c_str()));
	return TRUE;
}

// 自实现的消息，关闭IOCP服务器
LRESULT CServerDlg::OnStopIOCP(WPARAM wParam, LPARAM lParam)
{
	// 关闭服务器
	m_iocpServer.CloseServer();

	m_running = FALSE;

	// 往事件列表添加项
	SendMessage(WM_LIST_ADD, NULL, (LPARAM)"服务器已停止监听");
	return TRUE;
}

// 数据接收完毕
void CServerDlg::DoRecv(COverlappedIOInfo* pInfo)
{
}

// 数据发送完毕
void CServerDlg::DoSend(COverlappedIOInfo* pInfo)
{
}
