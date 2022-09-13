#pragma once
#include "Sqlite.h"

// CKeyDlg 对话框
class CKeyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CKeyDlg)

public:
	CKeyDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CKeyDlg() = default;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_KEY };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	// 列的信息
	struct
	{
		char* Name;
		int nIndex;
		int nWidth;
	}m_columnData[10];

	// List Control对象
	CListCtrl m_listkey;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLvnItemchangedListKey(NMHDR* pNMHDR, LRESULT* pResult);
};
