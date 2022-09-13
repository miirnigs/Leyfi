#pragma once
// CBlockDlg 对话框

class CBlockDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBlockDlg)

public:
	CBlockDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CBlockDlg() = default;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAB_BLOCK };
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
	}m_columnData[9];

	// List Control对象
	CListCtrl m_listblock;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
