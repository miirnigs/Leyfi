// OnlineDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "OnlineDlg.h"
#include "afxdialogex.h"


// COnlineDlg 对话框

IMPLEMENT_DYNAMIC(COnlineDlg, CDialogEx)

COnlineDlg::COnlineDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_ONLINE, pParent)
{
	m_columnData[0] = { "ID" , 0 , 70 };
	m_columnData[1] = { "IP地址" , 1 , 170 };
	m_columnData[2] = { "卡密" , 2 , 230 };
	m_columnData[3] = { "机器码" , 3 , 230 };
	m_columnData[4] = { "所属软件" , 4 , 150 };
	m_columnData[5] = { "上线时间" , 5 , 250 };
	m_columnData[6] = { "到期时间" , 6 , 170 };
	m_columnData[7] = { "状态" , 7 , 70 };
	m_columnData[8] = { "Ping" , 8 , 70 };
	m_columnData[9] = { "备注" , 9 , 200 };
}

void COnlineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ONLINE, m_listonline);
}

BEGIN_MESSAGE_MAP(COnlineDlg, CDialogEx)
END_MESSAGE_MAP()


// COnlineDlg 消息处理程序
BOOL COnlineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 反正这是独立对话框类，懒得重写封装ListControl控件了，直接在初始化函数里初始化控件得了
	// List Control
	// 风格:整行高亮，网格线，复选框
	m_listonline.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 添加列
	for (auto szColumn: m_columnData)
	{
		m_listonline.InsertColumn(szColumn.nIndex, szColumn.Name, LVCFMT_LEFT, szColumn.nWidth);
	}

	//// 测试数据
	//for (int i = 0; i < 100; i++)
	//{
	//	m_listonline.InsertItem(i, to_string(i).c_str());
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
}

// 调用以在调度消息之前对其进行筛选
BOOL COnlineDlg::PreTranslateMessage(MSG* pMsg)
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
