#pragma once	

class CTabSheet : public CTabCtrl
{
public:
	CTabSheet();
	~CTabSheet() = default;
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	BOOL AddPage(LPCTSTR title, CDialog* pDialog, UINT ID, bool bEnable);
	BOOL Show();
	void SetRect();
private:
	static const int nMaxPage = 10;		// 最大选项卡数量
	static const int nOffset = 10;		// 对话框与Tab control的边界距离
private:
	// 选项卡结构体
	struct TAB_SHEET_PAGE
	{
		LPCTSTR Title;
		UINT nIDD;
		CDialog* pDialog;
	}m_tsPage[nMaxPage];

	int m_nPageNum;
	int m_nCurrentPage;
};

