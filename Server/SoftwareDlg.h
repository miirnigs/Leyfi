#pragma once


// CSoftwareDlg 对话框

class CSoftwareDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSoftwareDlg)

public:
	CSoftwareDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSoftwareDlg() = default;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_SOFTWARE };
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
	}m_columnData[8];

	// ListControl对象
	CListCtrl m_listsoftware;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
