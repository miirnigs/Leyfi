#pragma once

// COnlineDlg 对话框
class COnlineDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COnlineDlg)

public:
	COnlineDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~COnlineDlg() = default;

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_ONLINE };
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
	
	// ListControl对象
	CListCtrl m_listonline;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
