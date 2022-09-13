// BlockDlg.cpp: 实现文件
//
#include "pch.h"
#include "Server.h"
#include "BlockDlg.h"
#include "afxdialogex.h"

// CBlockDlg 对话框

IMPLEMENT_DYNAMIC(CBlockDlg, CDialogEx)

CBlockDlg::CBlockDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_BLOCK, pParent)
{
	m_columnData[0] = { "ID" , 0 , 70 };
	m_columnData[1] = { "封禁卡密" , 1 , 230 };
	m_columnData[2] = { "封禁IP" , 2 , 170 };
	m_columnData[3] = { "封禁机器码" , 3 , 230 };
	m_columnData[4] = { "封禁时间" , 4 , 175 };
	m_columnData[5] = { "结束时间" , 5 , 175 };
	m_columnData[6] = { "所属软件" , 6 , 150 };
	m_columnData[7] = { "封禁状态" , 8 , 100 };
	m_columnData[8] = { "封禁原因" , 9 , 300 };
}

void CBlockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BLOCK, m_listblock);
}


BEGIN_MESSAGE_MAP(CBlockDlg, CDialogEx)
END_MESSAGE_MAP()


// CBlockDlg 消息处理程序


BOOL CBlockDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 反正这是独立对话框类，懒得重写封装ListControl控件了，直接在初始化函数里初始化控件得了
	// List Control
	// 风格:整行高亮，网格线，复选框
	m_listblock.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 添加列
	for (auto szColumn : m_columnData)
	{
		m_listblock.InsertColumn(szColumn.nIndex, szColumn.Name, LVCFMT_LEFT, szColumn.nWidth);
	}

	//// 测试数据
	//for (int i = 0; i < 100; i++)
	//{
	//	m_listblock.InsertItem(i, to_string(i).c_str());
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
}

// 调用以在调度消息之前对其进行筛选
BOOL CBlockDlg::PreTranslateMessage(MSG* pMsg)
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
