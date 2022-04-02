// SoftwareDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "SoftwareDlg.h"
#include "afxdialogex.h"


// CSoftwareDlg 对话框

IMPLEMENT_DYNAMIC(CSoftwareDlg, CDialogEx)

CSoftwareDlg::CSoftwareDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_SOFTWARE, pParent)
{
	m_columnData[0] = { "ID" , 0 , 70 };
	m_columnData[1] = { "软件名称" , 1 , 200 };
	m_columnData[2] = { "AppKey" , 2 , 290 };
	m_columnData[3] = { "AppSecret" , 3 , 290 };
	m_columnData[4] = { "创建时间" , 4 , 175 };
	m_columnData[5] = { "卡密数量" , 5 , 100 };
	m_columnData[6] = { "状态" , 6 , 70 };
	m_columnData[7] = { "备注" , 7 , 250 };
}

void CSoftwareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SOFTWARE, m_listsoftware);
}


BEGIN_MESSAGE_MAP(CSoftwareDlg, CDialogEx)
END_MESSAGE_MAP()


// CSoftwareDlg 消息处理程序
BOOL CSoftwareDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 反正这是独立对话框类，懒得重写封装ListControl控件了，直接在初始化函数里初始化控件得了
	// List Control
	// 风格:整行高亮，网格线，复选框
	m_listsoftware.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 添加列
	for (auto szColumn : m_columnData)
	{
		m_listsoftware.InsertColumn(szColumn.nIndex, szColumn.Name, LVCFMT_LEFT, szColumn.nWidth);
	}

	//// 测试数据
	//for (int i = 0; i < 100; i++)
	//{
	//	m_listsoftware.InsertItem(i, to_string(i).c_str());
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
}

// 调用以在调度消息之前对其进行筛选
BOOL CSoftwareDlg::PreTranslateMessage(MSG* pMsg)
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
