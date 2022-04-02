// ConfigDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"


// CConfigDlg 对话框

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_CONFIG, pParent)
{
	m_columnData[0] = { "ID" , 0 , 70 };
	m_columnData[1] = { "键" , 1 , 300 };
	m_columnData[2] = { "值" , 2 , 150 };
	m_columnData[3] = { "备注" , 3 , 850 };
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CONFIG, m_listconfig);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
END_MESSAGE_MAP()


// CConfigDlg 消息处理程序
BOOL CConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 反正这是独立对话框类，懒得重写封装ListControl控件了，直接在初始化函数里初始化控件得了
	// List Control
	// 风格:整行高亮，网格线，复选框
	m_listconfig.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 添加列
	for (auto szColumn : m_columnData)
	{
		m_listconfig.InsertColumn(szColumn.nIndex, szColumn.Name, LVCFMT_LEFT, szColumn.nWidth);
	}

	//// 测试数据
	//for (int i = 0; i < 100; i++)
	//{
	//	m_listconfig.InsertItem(i, to_string(i).c_str());
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
}

// 调用以在调度消息之前对其进行筛选
BOOL CConfigDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:		// Enter键，不管什么时候都禁用
			return TRUE;
		case VK_ESCAPE:		// ESC键，不管什么时候都禁用掉
			return TRUE;
		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
