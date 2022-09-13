// ClientDlg.h: 头文件
//
#pragma once
#include "AntiCrack.h"
#include <imm.h>
#include <regex>
#pragma comment(lib,"imm32.lib")
// CClientDlg 对话框
class CClientDlg : public CDialogEx
{
// 构造
public:
	CClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
private:
	AntiCrack m_anticrack;
private:
	// 初始化控件
	void InitCtrl();
private:
	// 编辑框的卡密
	CString m_edit_data;
	// StatusBarCtrl对象
	CStatusBar m_statusbar;
};
