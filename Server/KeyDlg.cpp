// KeyDlg.cpp: 实现文件
//

#include "pch.h"
#include "Server.h"
#include "KeyDlg.h"
#include "afxdialogex.h"


// CKeyDlg 对话框

IMPLEMENT_DYNAMIC(CKeyDlg, CDialogEx)

CKeyDlg::CKeyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TAB_KEY, pParent)
{
	m_columnData[0] = { "ID" , 0 , 70 };
	m_columnData[1] = { "卡密" , 1 , 230 };
	m_columnData[2] = { "IP地址" , 2 , 170 };
	m_columnData[3] = { "机器码" , 3 , 230 };
	m_columnData[4] = { "所属软件" , 4 , 150 };
	m_columnData[5] = { "激活时间" , 5 , 150 };
	m_columnData[6] = { "到期时间" , 6 , 150 };
	m_columnData[7] = { "在线状态" , 7 , 100 };
	m_columnData[8] = { "封禁状态" , 8 , 100 };
	m_columnData[9] = { "备注" , 9 , 150 };
}

void CKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_KEY, m_listkey);
}


BEGIN_MESSAGE_MAP(CKeyDlg, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_KEY, &CKeyDlg::OnLvnItemchangedListKey)
END_MESSAGE_MAP()


// CKeyDlg 消息处理程序


BOOL CKeyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 反正这是独立对话框类，懒得重写封装ListControl控件了，直接在初始化函数里初始化控件得了
	// List Control
	// 风格:整行高亮，网格线，复选框
	m_listkey.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 添加列
	for (auto szColumn : m_columnData)
	{
		m_listkey.InsertColumn(szColumn.nIndex, szColumn.Name, LVCFMT_LEFT, szColumn.nWidth);
	}

	//// 测试数据
	//for (int i = 0; i < 100; i++)
	//{
	//	m_listkey.InsertItem(i, to_string(i).c_str());
	//}

	return TRUE;  // return TRUE unless you set the focus to a control
}

// 调用以在调度消息之前对其进行筛选
BOOL CKeyDlg::PreTranslateMessage(MSG* pMsg)
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

// 指示视图中包含的某项已更改
void CKeyDlg::OnLvnItemchangedListKey(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 循环每一行
	for (int i = 0; i < m_listkey.GetItemCount(); i++)
	{
		if (m_listkey.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			// 有选中，因为ListControl有设置单行，所以可以直接发送消息
			::SendMessage(this->GetParent()->GetParent()->m_hWnd, (WM_USER + 150), NULL, i);		// 把选中的项的nItem发过去
			break;
		}
		else if (i == (m_listkey.GetItemCount() - 1))
		{
			// 没有选中，发送一个FALSE
			::SendMessage(this->GetParent()->GetParent()->m_hWnd, (WM_USER + 150), NULL, -1);		// 因为没有选中，所以发-1
		}
	}
	*pResult = 0;
}
