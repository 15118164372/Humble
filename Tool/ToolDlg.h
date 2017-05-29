
// ToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <list>
#include <string>

// CToolDlg 对话框
class CToolDlg : public CDialogEx
{
// 构造
public:
	CToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
    void setConsole(void);
    void setSVParam(void);
    void runSV(void);
    void initData(void);
    void stopSV(void);
    void initCMDLua(void);
    void setCommand(void);
    void getAllFileName(const char *pszParentPathName, std::list<std::string> &lstFileName);

// 实现
protected:    
	HICON m_hIcon;
    CFont m_objFont;
    struct lua_State *m_pCmdState;
    std::list<std::string> m_lstCommName;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedLinkBtn();
    afx_msg void OnBnClickedSendBtn();
    afx_msg void OnBnClickedButton4();
    afx_msg void OnCbnSelchangeCombo2();
    afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

public:    
    CIPAddressCtrl m_input_ip;
    CEdit m_input_port;
    CComboBox m_lstParser;
    CComboBox m_input_cmd;    
    CEdit m_input_msg;    
    CButton m_link_btn;
    CEdit m_input_socktype;
};
