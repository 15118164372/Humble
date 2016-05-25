
// ToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Tool.h"
#include "ToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMERID 1
#define COMMSELECT "--��ѡ��--"
#define COMMFOLDER "command"

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CToolDlg �Ի���



CToolDlg::CToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CToolDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT2, m_CtrInput);
    DDX_Control(pDX, IDC_LIST1, m_CtrOutput);
    DDX_Control(pDX, IDC_IPADDRESS1, m_CtrIp);
    DDX_Control(pDX, IDC_EDIT1, m_CtrPort);
    DDX_Control(pDX, IDC_COMBO1, m_CtrParser);
    DDX_Control(pDX, IDC_COMBO2, m_CtrCommand);
    DDX_Control(pDX, IDC_BUTTON1, m_CtrLink);
    DDX_Control(pDX, IDC_BUTTON2, m_CtrClose);
    DDX_Control(pDX, IDC_OutputNum, m_CtrTotalNum);
}

BEGIN_MESSAGE_MAP(CToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CToolDlg::OnNMRClickList1)
    ON_COMMAND_RANGE(ID_MenuSave, ID_Clear, &CToolDlg::OnMenuClick)
    ON_BN_CLICKED(IDC_BUTTON4, &CToolDlg::OnBnClickedButton4)
    ON_CBN_SELCHANGE(IDC_COMBO2, &CToolDlg::OnCbnSelchangeCombo2)
    ON_BN_CLICKED(IDC_BUTTON3, &CToolDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON1, &CToolDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CToolDlg::OnBnClickedButton2)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CToolDlg ��Ϣ�������

BOOL CToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    g_strProPath = H_GetProPath();
    g_strScriptPath = H_FormatStr("%s%s%s", g_strProPath.c_str(), "script", H_PATH_SEPARATOR);

    m_CtrOutput.InsertColumn(0, "ʱ��");//�����
    m_CtrOutput.InsertColumn(1, "��Ϣ");

    setStyle();

    m_CtrOutput.SetColumnWidth(0, 200);//�����п�
    m_CtrOutput.SetColumnWidth(1, 480);
    m_CtrIp.SetWindowTextA("127.0.0.1");
    m_CtrPort.SetWindowTextA("15000");

    initParser();
    setCommand();

    SetTimer(TIMERID, 1000, 0);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CToolDlg::OnTimer(UINT_PTR nIDEvent)
{
    int iCount = m_CtrOutput.GetItemCount();
    std::string strNum = H_FormatStr("%d", iCount);
    m_CtrTotalNum.SetWindowTextA(strNum.c_str());
}

void CToolDlg::setStyle(void)
{
    m_objFont.CreateFontA(20,             //����߶�
        0,           // ������ 
        0,           // �ı��е���б�� 
        0,           // �ַ����ߵ���б�� 
        FW_DONTCARE,     // ����Ĵ�ϸ 
        FALSE,        // �����Ƿ�Ϊб�� 
        FALSE,       // �����Ƿ���»��� 
        0,           // �����Ƿ��ɾ���� 
        ANSI_CHARSET,              // ������ַ��� 
        OUT_DEFAULT_PRECIS,        // �ַ���������� 
        CLIP_DEFAULT_PRECIS,       // �ַ��ü��ľ��� 
        DEFAULT_QUALITY,           // �ַ���������� 
        DEFAULT_PITCH | FF_SWISS, // �ַ�����������
        _T("������"));//��������

    m_CtrInput.SetFont(&m_objFont);

    LONG lStyle;
    lStyle = GetWindowLong(m_CtrOutput.m_hWnd, GWL_STYLE);// ��ȡ��ǰ����style 
    lStyle &= ~LVS_TYPEMASK; // �����ʾ��ʽλ 
    lStyle |= LVS_REPORT; // ����style 
    SetWindowLong(m_CtrOutput.m_hWnd, GWL_STYLE, lStyle);// ����style 
    DWORD dwStyle = m_CtrOutput.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT;// ѡ��ĳ��ʹ���и�����ֻ������report ����listctrl �� 
    dwStyle |= LVS_EX_GRIDLINES;// �����ߣ�ֻ������report ����listctrl �� 
                                //dwStyle |= LVS_EX_CHECKBOXES;//item ǰ����checkbox �ؼ� 
    m_CtrOutput.SetExtendedStyle(dwStyle); // ������չ���

    m_CtrOutput.SetFont(&m_objFont);
}

void CToolDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    int iCount = m_CtrOutput.GetItemCount();
    CMenu objMenu;
    objMenu.LoadMenu(IDR_MENU1);

    CPoint stPoint;
    GetCursorPos(&stPoint);
    CMenu* pSubMenu = objMenu.GetSubMenu(0);
    if (0 == iCount)
    {
        pSubMenu->EnableMenuItem(ID_MenuSave, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        pSubMenu->EnableMenuItem(ID_Clear, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }
    pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, stPoint.x, stPoint.y, this);
    objMenu.Detach();

    *pResult = 0;
}

void CToolDlg::OnMenuClick(UINT nID)
{
    switch (nID)
    {
        case ID_Clear:
        {
            m_CtrOutput.DeleteAllItems();
        }
        break;

        case ID_MenuSave:
        {
            CString ctrFileName = "outoput.txt";
            CString ctrFilter = "�ļ� (*.txt)|*.txt||";
            CFileDialog objOpenFileDlg(FALSE, NULL, ctrFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, ctrFilter, NULL);
            objOpenFileDlg.m_ofn.lpstrInitialDir = g_strProPath.c_str();
            INT_PTR iResult = objOpenFileDlg.DoModal();
            if (IDOK != iResult)
            {
                break;
            }

            CString cstrFileName = objOpenFileDlg.GetPathName();
            FILE *pFile = fopen(cstrFileName.GetBuffer(), "w");
            if (NULL == pFile)
            {
                break;
            }
            CString cstrTime;
            CString cstrInfo;
            std::string strBuf;
            strBuf = std::string("ʱ��") +
                std::string("\t") +
                std::string("����") +
                std::string("\n");
            fwrite(strBuf.c_str(), 1, strBuf.size(), pFile);

            int iCount = m_CtrOutput.GetItemCount();
            for (int i = 0; i < iCount; i++)
            {
                cstrTime = m_CtrOutput.GetItemText(i, 0);
                cstrInfo = m_CtrOutput.GetItemText(i, 1);
                strBuf = std::string(cstrTime.GetBuffer()) +
                    std::string("\t") +
                    std::string(cstrInfo.GetBuffer()) +
                    std::string("\n");
                fwrite(strBuf.c_str(), 1, strBuf.size(), pFile);
            }

            fclose(pFile);
            pFile = NULL;
        }
        break;

        default:
            break;
    }
}

void CToolDlg::getAllFileName(const char *pszParentPathName, std::list<std::string> &lstFileName)
{
    WIN32_FIND_DATA fd = { 0 };
    HANDLE hSearch;
    std::string strFilePathName;
    strFilePathName = pszParentPathName + std::string("\\*");

    hSearch = FindFirstFile(strFilePathName.c_str(), &fd);
    if (INVALID_HANDLE_VALUE == hSearch)
    {
        return;
    }

    if (!fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
    {
        if (strcmp(fd.cFileName, ".")
            && strcmp(fd.cFileName, ".."))
        {
            if (0 != strcmp(".placeholder", fd.cFileName))
            {
                lstFileName.push_back(fd.cFileName);
            }            
        }
    }

    for (;;)
    {
        memset(&fd, 0, sizeof(fd));
        if (!FindNextFile(hSearch, &fd))
        {
            if (ERROR_NO_MORE_FILES == GetLastError())
            {
                break;
            }

            FindClose(hSearch);

            return;
        }

        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
            && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
        {
            if (strcmp(fd.cFileName, ".")
                && strcmp(fd.cFileName, ".."))
            {
                if (0 != strcmp(".placeholder", fd.cFileName))
                {
                    lstFileName.push_back(fd.cFileName);
                }
            }
        }
    }

    FindClose(hSearch);

    return;
}

void CToolDlg::initParser(void)
{
    CNetParser *pParser = CNetParser::getSingletonPtr();

    pParser->addParser(CDefParser::getSingletonPtr());
    m_CtrParser.AddString(CDefParser::getSingletonPtr()->getName());
    pParser->addParser(CTcp1::getSingletonPtr());
    m_CtrParser.AddString(CTcp1::getSingletonPtr()->getName());
    pParser->addParser(CTcp2::getSingletonPtr());
    m_CtrParser.AddString(CTcp2::getSingletonPtr()->getName());
    pParser->addParser(CHttp::getSingletonPtr());
    m_CtrParser.AddString(CHttp::getSingletonPtr()->getName());
    pParser->addParser(CWebSock::getSingletonPtr());
    m_CtrParser.AddString(CWebSock::getSingletonPtr()->getName());
    pParser->addParser(CMQTT::getSingletonPtr());
    m_CtrParser.AddString(CMQTT::getSingletonPtr()->getName());

    m_CtrParser.SetCurSel(0);
}

void CToolDlg::setCommand(void)
{
    std::list<std::string>::iterator itName;
    std::string strCommFolder = g_strProPath + COMMFOLDER + std::string("\\");
    getAllFileName(strCommFolder.c_str(), m_lstCommName);
    for (itName = m_lstCommName.begin(); m_lstCommName.end() != itName; ++itName)
    {
        m_CtrCommand.AddString(itName->c_str());
    }

    int iIndex = m_CtrCommand.AddString(COMMSELECT);
    m_CtrCommand.SetCurSel(iIndex);
}

//����
void CToolDlg::OnBnClickedButton4()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CString cstrComm;
    m_CtrInput.GetWindowTextA(cstrComm);
    if (0 == cstrComm.GetLength())
    {
        return;
    }

    CString cstrName;
    m_CtrCommand.GetWindowTextA(cstrName);
    if (0 == cstrName.GetLength())
    {
        return;
    }

    if (COMMSELECT == cstrName)
    {
        return;
    }

    std::string strName = cstrName.GetBuffer();
    std::string strCommFile = g_strProPath + COMMFOLDER + std::string("\\") + strName;
    FILE *pFile = fopen(strCommFile.c_str(), "w");
    if (NULL != pFile)
    {
        std::string strMsg = cstrComm.GetBuffer();
        fwrite(strMsg.c_str(), 1, strMsg.size(), pFile);

        fclose(pFile);
    }

    std::list<std::string>::iterator itComm;
    itComm = std::find(m_lstCommName.begin(), m_lstCommName.end(), strName);
    if (m_lstCommName.end() == itComm)
    {
        m_lstCommName.push_back(strName);
        int iIndex = m_CtrCommand.AddString(strName.c_str());
        m_CtrCommand.SetCurSel(iIndex);
    }
}

void CToolDlg::OnCbnSelchangeCombo2()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CString cstrVal;

    int iIndex = m_CtrCommand.GetCurSel();
    m_CtrCommand.GetLBText(iIndex, cstrVal);
    if (0 == cstrVal.GetLength())
    {
        return;
    }

    std::string strName = cstrVal.GetBuffer();
    if (COMMSELECT == strName)
    {
        m_CtrInput.SetWindowTextA("");
        return;
    }

    std::string strCommFile = g_strProPath + COMMFOLDER + std::string("\\") + strName;
    FILE *pFile = fopen(strCommFile.c_str(), "r");
    if (NULL != pFile)
    {
        char acTmp[2048] = { 0 };
        fread(acTmp, 1, sizeof(acTmp) - 1, pFile);
        m_CtrInput.SetWindowTextA(acTmp);
        fclose(pFile);
    }
}

//����
void CToolDlg::OnBnClickedButton3()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}

//����
void CToolDlg::OnBnClickedButton1()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}

//����
void CToolDlg::OnBnClickedButton2()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CToolDlg::OnClose()
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    CDialogEx::OnClose();
}
