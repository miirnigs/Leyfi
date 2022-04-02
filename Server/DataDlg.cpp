// DataDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "DataDlg.h"
#include "afxdialogex.h"


// CDataDlg 对话框

IMPLEMENT_DYNAMIC(CDataDlg, CDialogEx)

CDataDlg::CDataDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_DATA, pParent)
{
	m_columnData[0] = { "ID" , 0 , 70 };
	m_columnData[1] = { "变量名称" , 1 , 150 };
	m_columnData[2] = { "变量数据" , 2 , 400 };
	m_columnData[3] = { "所属软件" , 3 , 150 };
	m_columnData[4] = { "创建时间" , 4 , 175 };
	m_columnData[5] = { "状态" , 5 , 100 };
	m_columnData[6] = { "备注" , 6 , 340 };
}

void CDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_listdata);
}


BEGIN_MESSAGE_MAP(CDataDlg, CDialogEx)
END_MESSAGE_MAP()


// CDataDlg 消息处理程序
BOOL CDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 反正这是独立对话框类，懒得重写封装ListControl控件了，直接在初始化函数里初始化控件得了
	// List Control
	// 风格:整行高亮，网格线，复选框
	m_listdata.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 添加列
	for (auto szColumn : m_columnData)
	{
		m_listdata.InsertColumn(szColumn.nIndex, szColumn.Name, LVCFMT_LEFT, szColumn.nWidth);
	}

	//// 测试数据
	//for (int i = 0; i < 100; i++)
	//{
	//	m_listdata.InsertItem(i, to_string(i).c_str());
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
}

// 调用以在调度消息之前对其进行筛选
BOOL CDataDlg::PreTranslateMessage(MSG* pMsg)
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
