
// ToolDlg.h : ͷ�ļ�
//

#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "../Humble/LNetDisp.h"
#include "../Humble/LTick.h"
#include "../Humble/LuaTask.h"

using namespace Humble;

// CToolDlg �Ի���
class CToolDlg : public CDialogEx
{
// ����
public:
	CToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
    void setStyle(void);
    void getAllFileName(const char *pszParentPathName, std::list<std::string> &lstFileName);
    void setCommand(void);
    void initParser(void);
    void startSV(void);

// ʵ��
protected:
    unsigned short m_usType;
	HICON m_hIcon;
    int m_iLinkID;
    std::list<std::string> m_lstCommName;
    struct lua_State *m_pLState;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnMenuClick(UINT nID);
    afx_msg void OnBnClickedButton4();
    afx_msg void OnCbnSelchangeCombo2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg LRESULT ShowMsg(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT EnableLinkButt(WPARAM wParam, LPARAM lParam);
    afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

public:
    CFont m_objFont;
    CEdit m_CtrInput;
    CListCtrl m_CtrOutput;
    CIPAddressCtrl m_CtrIp;
    CEdit m_CtrPort;
    CComboBox m_CtrParser;
    CComboBox m_CtrCommand;
    CButton m_CtrLink;
    CButton m_CtrClose;
    CStatic m_CtrTotalNum;    
};

extern HWND g_hWnd;
