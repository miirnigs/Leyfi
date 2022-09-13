#pragma once
#include "ServerDlg.h"
#include "Functions.h"
#include "IOCPServer.h"
#include "OverlappedIOInfo.h"
using namespace functions;

// CSetting 对话框
class CSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CSetting)

public:
	CSetting(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetting();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORM_SETTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	// 开始监听按钮的对象
	CButton m_btnstart;
	// 停止监听按钮的对象
	CButton m_btnstop;
	// 端口编辑框的对象
	CEdit m_editport;
};
