// ShowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Tool.h"
#include "ShowDlg.h"
#include "afxdialogex.h"


// CShowDlg �Ի���

IMPLEMENT_DYNAMIC(CShowDlg, CDialogEx)

CShowDlg::CShowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CShowDlg::~CShowDlg()
{
}

BOOL CShowDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_CtrShowMsg.SetFont(m_pFont);
    m_CtrShowMsg.SetWindowTextA(m_strVal.c_str());

    return TRUE;
}

void CShowDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT2, m_CtrShowMsg);
}


BEGIN_MESSAGE_MAP(CShowDlg, CDialogEx)
END_MESSAGE_MAP()


// CShowDlg ��Ϣ�������

void CShowDlg::setShow(const char *pszMsg)
{
    m_strVal = pszMsg;    
}

void CShowDlg::setFont(CFont *pFont)
{
    m_pFont = pFont;
}
