#pragma once
#include "afxwin.h"
#include <string>

// CShowDlg �Ի���

class CShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowDlg)

public:
	CShowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowDlg();


    void setShow(const char *pszMsg);
    void setFont(CFont *pFont);



// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    std::string m_strVal;
    CFont *m_pFont;
    CEdit m_CtrShowMsg;
};
