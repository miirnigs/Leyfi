#include "pch.h"
#include "TabSheet.h"

BEGIN_MESSAGE_MAP(CTabSheet, CTabCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CTabSheet::CTabSheet() :m_nPageNum(0), m_nCurrentPage(0)
{
	// 结构体数组初始化0
	memset(&m_tsPage, NULL, sizeof(TAB_SHEET_PAGE));
}

void CTabSheet::OnLButtonDown(UINT nFlags, CPoint point)
{
	CTabCtrl::OnLButtonDown(nFlags, point);

	int selectedPage = GetCurFocus();
	if (m_nCurrentPage != selectedPage)
	{
		// 选项卡改变
		// 禁用当前选项卡
		m_tsPage[m_nCurrentPage].pDialog->ShowWindow(SW_HIDE);

		// 显示选中选项卡
		m_nCurrentPage = selectedPage;
		m_tsPage[m_nCurrentPage].pDialog->ShowWindow(SW_SHOW);
	}
}

BOOL CTabSheet::AddPage(LPCTSTR title, CDialog* pDialog, UINT ID, bool bEnable)
{
	if (m_nPageNum == nMaxPage)
	{
		return FALSE;
	}

	m_tsPage[m_nPageNum].pDialog = pDialog;
	m_tsPage[m_nPageNum].nIDD = ID;
	m_tsPage[m_nPageNum].Title = title;
	m_nPageNum++;

	return TRUE;
}

BOOL CTabSheet::Show()
{
	if (m_nPageNum == 0)
	{
		// 没有选项卡
		return FALSE;
	}

	for (int i = 0; i < m_nPageNum; i++)
	{
		// 创建对话框
		// 重复创建对话框会直接崩
		m_tsPage[i].pDialog->Create(m_tsPage[i].nIDD, this);

		// 插入选项卡
		InsertItem(i, m_tsPage[i].Title);
	}

	// 是当前页面就显示，不是就隐藏
	for (int i = 0; i < m_nPageNum; i++)
	{
		if (i == m_nCurrentPage)
		{
			m_tsPage[i].pDialog->ShowWindow(SW_SHOW);
		}
		else
		{
			m_tsPage[i].pDialog->ShowWindow(SW_HIDE);
		}
	}

	SetRect();		// 放置对话框
	return TRUE;
}

void CTabSheet::SetRect()
{
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;		//左、顶、宽、高

	GetClientRect(&tabRect);		// 获取整个Tab控件的位置大小。
	GetItemRect(0, &itemRect);		// item指的只是标签页，这里是获得标签页的位置大小

	DWORD style = GetStyle();
	if (style & TCS_VERTICAL)
	{
		// 选项卡在TAB控件的侧边
		nY = tabRect.top + nOffset;
		nYc = tabRect.bottom - nY - (nOffset + 1);
		if (style & TCS_BOTTOM)
		{
			// 选项卡在右边
			nX = tabRect.left + nOffset;
			nXc = itemRect.left - nX - (nOffset + 1);
		}
		else
		{
			// 选项卡在左边
			nX = itemRect.right + nOffset;
			nXc = tabRect.right - nX - (nOffset + 1);
		}
	}
	else
	{
		// 选项卡在TAB控件的顶部或底部
		nX = tabRect.left + nOffset;
		nXc = tabRect.right - nX - (nOffset + 1);
		if (style & TCS_BOTTOM)
		{
			// 选项卡在底部
			nY = tabRect.top + nOffset;
			nYc = itemRect.top - nY - (nOffset + 1);
		}
		else
		{
			// 选项卡在顶部
			nY = itemRect.bottom + nOffset;
			nYc = tabRect.bottom - nY - (nOffset + 1);
		}
	}
	// 移动全部对话框，因为只有当前选项卡对应的对话框显示，所以不担心对话框重叠
	for (int i = 0; i < m_nPageNum; i++)
	{
		// 不知道为什么用WindowPos死活不显示，只好改用MoveWindow
		m_tsPage[i].pDialog->MoveWindow(nX, nY, nXc, nYc);
	}
}
