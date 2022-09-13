#pragma once
// CDataDlg 对话框

class CDataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDataDlg)

public:
	CDataDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDataDlg() = default;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_DATA };
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
	}m_columnData[7];

	// ListControl对象
	CListCtrl m_listdata;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
