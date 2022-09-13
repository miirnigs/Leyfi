// ServerDlg.h: 头文件
#pragma once
#include "OnlineDlg.h"
#include "TabSheet.h"
#include "KeyDlg.h"
#include "SoftwareDlg.h"
#include "BlockDlg.h"
#include "DataDlg.h"
#include "ConfigDlg.h"
#include "Sqlite.h"
#include "Functions.h"
#include "IOCPServer.h"
#include "Setting.h"
#include "OverlappedIOInfo.h"
using namespace functions;

// CServerDlg 对话框
class CServerDlg : public CDialogEx
{
// 构造
public:
	CServerDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CServerDlg() = default;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_FORM };
#endif
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
private:
	// 控件初始化
	void InitCtrl();
	// 数据库初始化
	void InitSQL(std::string sqlPath);
	// 读取数据库数据
	void LoadData();
	// 在状态栏里获取并显示显示当前各个数据的数据量
	void ShowDataCount();
	// 处理数据接收完毕
	void DoRecv(COverlappedIOInfo* pInfo);
	// 处理数据发送完毕
	void DoSend(COverlappedIOInfo* pInfo);
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnExit();
	afx_msg void OnTcnSelchangeTabManage(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnSetting();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	// 自定义消息
	afx_msg LRESULT OnListControlChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnListControlAdd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStartIOCP(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStopIOCP(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	// 列的信息
	struct
	{
		char* Name;
		int nIndex;
		int nWidth;
	}m_columnData[2];

	// IOCP服务器是否正在运行
	bool m_running;
	// 当前监听端口
	unsigned short m_port;
	
	// Timer的ID
	UINT uTimerID;
	// ListControl对象
	CListCtrl m_listevent;
	// ComboBox对象
	CComboBox m_combotype;
	// StatusBarCtrl对象
	CStatusBar m_statusbar;
	// TabSheet对象
	CTabSheet m_tabsheet;
	// OnlineDlg对话框对象
	COnlineDlg m_tabonline;
	// KeyDlg对话框对象
	CKeyDlg m_tabkey;
	// SoftwareDlg对话框对象
	CSoftwareDlg m_tabsoftware;
	// BlockDlg对话框对象
	CBlockDlg m_tabblock;
	// DataDlg对话框对象
	CDataDlg m_tabdata;
	// ConfigDlg对话框对象
	CConfigDlg m_tabconfig;
	// EditSearch对象
	CEdit m_editsearch;
	// Button对象
	CButton m_buttonsearch;
	// EditSearch值
	CString cstrSearch;
	// IOCP服务器的类对象
	CIOCPServer m_iocpServer;
};