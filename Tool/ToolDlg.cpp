
// ToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Tool.h"
#include "ToolDlg.h"
#include "afxdialogex.h"
#include <conio.h>
#include "../HLBind/LTask.h"

using namespace Humble;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "libevent.lib")
#pragma comment(lib, "libevent_core.lib")
#pragma comment(lib, "libevent_extras.lib")
#ifdef H_MYSQL
#pragma comment(lib, "libmysql.lib")
#endif
#pragma comment(lib, "HBase.lib")
#pragma comment(lib, "HLBind.lib")

#define COMMSELECT "--��ѡ��--"

HANDLE g_hOutput;
unsigned short m_usCmdType;
intptr_t m_CmdSock;

//�����ֽ�char*ת��Ϊ���ֽ�wchar_t*  
CString ansiToUnicode(const char* pszStr)
{
    CString cstrVal;
    int iLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszStr, -1, NULL, 0);
    if (0 == iLen)
    {
        return cstrVal;
    }

    wchar_t* pResult = new wchar_t[iLen];
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszStr, -1, pResult, iLen);
    cstrVal = pResult;
    delete pResult;

    return cstrVal;
}

//�����ֽ�wchar_t*ת��Ϊ���ֽ�char*  
std::string unicodeToAnsi(const wchar_t* pszStr)
{
    std::string strVal;
    int iLen = WideCharToMultiByte(CP_ACP, 0, pszStr, -1, NULL, 0, NULL, NULL);
    if (0 == iLen)
    {
        return strVal;
    }

    char* pResult = new char[iLen];
    WideCharToMultiByte(CP_ACP, 0, pszStr, -1, pResult, iLen, NULL, NULL);
    strVal = pResult;
    delete pResult;

    return strVal;
}

void consoleWrite(const char *pszBuf)
{
    CString cstrText = ansiToUnicode(pszBuf);
    if (0 == cstrText.GetLength())
    {
        return;
    }

    CString strNow = L"[" + ansiToUnicode(H_Now().c_str()) + L"] ";
    cstrText = strNow + cstrText + L"\n";
    WriteConsole(g_hOutput, cstrText, cstrText.GetLength(), NULL, NULL);
}

intptr_t getLinkSock(void)
{
    return m_CmdSock;
}

unsigned short getLinkType(void)
{
    return m_usCmdType;
}

H_BNAMSP
void H_SetPackPath(struct lua_State *pLState)
{    
    std::list<std::string> lstDirs;

    H_GetSubDirName(g_strScriptPath.c_str(), lstDirs);
    luabridge::LuaRef objPack = luabridge::getGlobal(pLState, "package");
    std::string strPack = objPack["path"];
    std::string strPath = H_FormatStr("%s%s%s", g_strProPath.c_str(), "script", H_PATH_SEPARATOR);
    std::string strPubPath = H_FormatStr("%s%s%s%s%s", g_strProPath.c_str(), "script", H_PATH_SEPARATOR, "public", H_PATH_SEPARATOR);
    std::string strVal = H_FormatStr("%s?.lua;%s?.lua;%s?.lua", g_strScriptPath.c_str(), strPath.c_str(), strPubPath.c_str());

    std::list<std::string>::iterator itDir;
    for (itDir = lstDirs.begin(); lstDirs.end() != itDir; ++itDir)
    {
        strVal = H_FormatStr("%s;%s%s/?.lua", strVal.c_str(), g_strScriptPath.c_str(), itDir->c_str());
    }

    objPack["path"] = H_FormatStr("%s;%s", strPack.c_str(), strVal.c_str());
}

void H_RegOther(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .addFunction("getLinkSock", getLinkSock)
        .addFunction("getLinkType", getLinkType)
        .addFunction("consoleWrite", consoleWrite);
}
H_ENAMSP

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
    DDX_Control(pDX, IDC_IPADDRESS1, m_input_ip);
    DDX_Control(pDX, IDC_EDIT1, m_input_port);
    DDX_Control(pDX, IDC_COMBO1, m_lstParser);
    DDX_Control(pDX, IDC_COMBO2, m_input_cmd);
    DDX_Control(pDX, IDC_EDIT2, m_input_msg);
    DDX_Control(pDX, IDC_LINK_BTN, m_link_btn);
    DDX_Control(pDX, IDC_EDIT3, m_input_socktype);
}

BEGIN_MESSAGE_MAP(CToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_LINK_BTN, &CToolDlg::OnBnClickedLinkBtn)
    ON_BN_CLICKED(IDC_SEND_BTN, &CToolDlg::OnBnClickedSendBtn)
    ON_BN_CLICKED(IDC_BUTTON4, &CToolDlg::OnBnClickedButton4)
    ON_CBN_SELCHANGE(IDC_COMBO2, &CToolDlg::OnCbnSelchangeCombo2)
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
    m_CmdSock = H_INVALID_SOCK;
    m_usCmdType = 0;
    g_strProPath = H_GetProPath();
    g_strScriptPath = H_FormatStr("%s%s%s", g_strProPath.c_str(), "toolscript", H_PATH_SEPARATOR);
    m_input_ip.SetWindowTextW(L"127.0.0.1");
    m_input_port.SetWindowTextW(L"16000");
    m_input_socktype.SetWindowTextW(L"10");

    m_objFont.CreateFontW(20,             //����߶�
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

    m_input_msg.SetFont(&m_objFont);

    setConsole();
    setSVParam();
    runSV();
    initData();
    initCMDLua();
    setCommand();

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

void CToolDlg::setConsole(void)
{
    //����һ������̨����
    AllocConsole();
    g_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    //���ÿ���̨���ڵ�����
    WCHAR title[] = L"Humble Tool Output";
    SetConsoleTitle(title);
    SetConsoleTextAttribute((HANDLE)g_hOutput, FOREGROUND_GREEN);

    //��ȡ����̨���ڵľ��
    HWND hwnd = NULL;
    while (NULL == hwnd)
    {
        hwnd = ::FindWindow(NULL, title);
    }

    //���ε�����̨���ڵĹرհ�ť���Է����ڱ���ɾ��
    HMENU hmenu = ::GetSystemMenu(hwnd, FALSE);
    DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
}

void CToolDlg::setSVParam(void)
{
    CParserMgr::getSingletonPtr()->addParser(CHttp::getSingletonPtr());
    m_lstParser.AddString(TEXT(H_PARSER_HTTP));
    CParserMgr::getSingletonPtr()->addParser(CTcp1::getSingletonPtr());
    m_lstParser.AddString(TEXT(H_PARSER_TCP1));
    CParserMgr::getSingletonPtr()->addParser(CTcp2::getSingletonPtr());
    m_lstParser.AddString(TEXT(H_PARSER_TCP2));
    m_lstParser.SetCurSel(0);

    CLog *pLog = CLog::getSingletonPtr();
    pLog->setPriority(LOGLV_DEBUG);
    std::string strLogFile = H_FormatStr("%s%s%s%s", g_strProPath.c_str(),
        "log", H_PATH_SEPARATOR, "tool.txt");
    pLog->setLogFile(strLogFile.c_str());
    pLog->Open();

    unsigned short usNum = H_GetCoreCount();
    CNetWorkerMgr::getSingletonPtr()->startWorker(usNum);
    CSender::getSingletonPtr()->startSender(usNum);
    CTick::getSingletonPtr()->setTime(20, 20);
    CTaskGlobleQu::getSingletonPtr()->setThreadNum(usNum);
    CTaskMgr::getSingletonPtr()->setThreadNum(usNum);    
}

void CToolDlg::runSV(void)
{
    CThread::Creat(CLog::getSingletonPtr());
    CLog::getSingletonPtr()->waitStart();
    CThread::Creat(CLinker::getSingletonPtr());
    CLinker::getSingletonPtr()->waitStart();
    CThread::Creat(CNetListener::getSingletonPtr());
    CNetListener::getSingletonPtr()->waitStart();
    CThread::Creat(CTick::getSingletonPtr());
    CTick::getSingletonPtr()->waitStart();
}

void CToolDlg::initData(void)
{
    struct lua_State *pLState = luaL_newstate();
    if (NULL == pLState)
    {
        consoleWrite("luaL_newstate error.");
        return;
    }

    luaL_openlibs(pLState);
    H_RegAll(pLState);

    std::string strLuaFile = g_strScriptPath + "toolstart.lua";
    if (H_RTN_OK != luaL_dofile(pLState, strLuaFile.c_str()))
    {
        consoleWrite(lua_tostring(pLState, -1));
        lua_close(pLState);
        return;
    }

    lua_close(pLState);
}

void CToolDlg::initCMDLua(void)
{
    m_pCmdState = luaL_newstate();
    if (NULL == m_pCmdState)
    {
        consoleWrite("luaL_newstate error.");
        return;
    }

    luaL_openlibs(m_pCmdState);
    H_RegAll(m_pCmdState);
}

void CToolDlg::stopSV(void)
{
    CTick::getSingletonPtr()->Join();
    CNetListener::getSingletonPtr()->Join();
    CTaskMgr::getSingletonPtr()->stopWorker();
    CSender::getSingletonPtr()->stopSender();
    CNetWorkerMgr::getSingletonPtr()->stopWorker();
    CLinker::getSingletonPtr()->Join();
    CLog::getSingletonPtr()->Join();
}

void CToolDlg::getAllFileName(const char *pszParentPathName, std::list<std::string> &lstFileName)
{
    WIN32_FIND_DATA fd = { 0 };
    HANDLE hSearch;
    std::string strFilePathName;
    strFilePathName = pszParentPathName + std::string("*");

    hSearch = FindFirstFile(ansiToUnicode(strFilePathName.c_str()), &fd);
    if (INVALID_HANDLE_VALUE == hSearch)
    {
        return;
    }

    if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
        && !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
    {
        if (wcscmp(fd.cFileName, L".")
            && wcscmp(fd.cFileName, L".."))
        {
            if (0 != wcscmp(L".placeholder", fd.cFileName))
            {
                lstFileName.push_back(unicodeToAnsi(fd.cFileName));
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
            if (wcscmp(fd.cFileName, L".")
                && wcscmp(fd.cFileName, L".."))
            {
                if (0 != wcscmp(L".placeholder", fd.cFileName))
                {
                    lstFileName.push_back(unicodeToAnsi(fd.cFileName));
                }
            }
        }
    }

    FindClose(hSearch);

    return;
}

void CToolDlg::setCommand(void)
{
    std::list<std::string>::iterator itName;
    std::string strCommFolder = g_strProPath + "command" + std::string("\\");
    getAllFileName(strCommFolder.c_str(), m_lstCommName);
    for (itName = m_lstCommName.begin(); m_lstCommName.end() != itName; ++itName)
    {
        m_input_cmd.AddString(ansiToUnicode(itName->c_str()));
    }

    m_input_cmd.SetCurSel(m_input_cmd.AddString(TEXT(COMMSELECT)));
}

void CToolDlg::OnClose()
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    stopSV();
    FreeConsole();
    if (NULL != m_pCmdState)
    {
        lua_close(m_pCmdState);
        m_pCmdState = NULL;
    }
    if (H_INVALID_SOCK != m_CmdSock)
    {
        closesocket(m_CmdSock);
        m_CmdSock = H_INVALID_SOCK;
    }
    
    CDialogEx::OnClose();
}


void CToolDlg::OnBnClickedLinkBtn()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CString cstrIp;
    CString cstrPort;
    CString cstrParser;
    CString cstrType;
    m_input_ip.GetWindowTextW(cstrIp);
    m_input_port.GetWindowTextW(cstrPort);
    m_lstParser.GetWindowTextW(cstrParser);
    m_input_socktype.GetWindowTextW(cstrType);
    if (0 == cstrIp.GetLength() || 0 == cstrPort.GetLength() || 0 == cstrType.GetLength())
    {
        return;
    }

    m_CmdSock = H_ClientSock(unicodeToAnsi(cstrIp).c_str(), atoi(unicodeToAnsi(cstrPort).c_str()));
    if (H_INVALID_SOCK == m_CmdSock)
    {
        consoleWrite("connetct error.");
        return;
    }

    CParser *pParser = CParserMgr::getSingletonPtr()->getParser(unicodeToAnsi(cstrParser).c_str());
    if (NULL == pParser)
    {
        consoleWrite("get parser error.");
        closesocket(m_CmdSock);
        m_CmdSock = H_INVALID_SOCK;
        return;
    }

    m_usCmdType = atoi(unicodeToAnsi(cstrType.GetBuffer()).c_str());
    CNetWorkerMgr::getSingletonPtr()->addLink(m_CmdSock, pParser, m_usCmdType, false);
    m_link_btn.EnableWindow(FALSE);
    m_lstParser.EnableWindow(FALSE);
    m_input_socktype.EnableWindow(FALSE);
}


void CToolDlg::OnBnClickedSendBtn()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    if (H_INVALID_SOCK == m_CmdSock)
    {
        consoleWrite("link server first.");
        return;
    }
    CString cstrBuf;
    m_input_msg.GetWindowTextW(cstrBuf);
    if (0 == cstrBuf.GetLength())
    {
        return;
    }

    std::string strBuf = unicodeToAnsi(cstrBuf.GetBuffer());
    if (strBuf.empty())
    {
        return;
    }
    int iErr = luaL_loadbuffer(m_pCmdState, strBuf.c_str(), strBuf.size(), strBuf.c_str());
    if (0 != iErr)
    {
        consoleWrite(lua_tostring(m_pCmdState, -1));
        return;
    }
    iErr = lua_pcall(m_pCmdState, 0, 0, 0);
    if (0 != iErr)
    {
        consoleWrite(lua_tostring(m_pCmdState, -1));
    }
}


void CToolDlg::OnBnClickedButton4()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CString cstrComm;
    CString cstrName;
    m_input_msg.GetWindowTextW(cstrComm);
    m_input_cmd.GetWindowTextW(cstrName);
    if (0 == cstrComm.GetLength() || 0 == cstrName.GetLength() || TEXT(COMMSELECT) == cstrName)
    {
        return;
    }

    std::string strName = unicodeToAnsi(cstrName.GetBuffer());
    std::string strInfo = unicodeToAnsi(cstrComm.GetBuffer());
    if (strName.empty() || strInfo.empty())
    {
        return;
    }

    std::string strCommFile = g_strProPath + "command" + std::string("\\") + strName;
    FILE *pFile = fopen(strCommFile.c_str(), "w");
    if (NULL != pFile)
    {
        fwrite(strInfo.c_str(), 1, strInfo.size(), pFile);
        fclose(pFile);
    }

    std::list<std::string>::iterator itComm;
    itComm = std::find(m_lstCommName.begin(), m_lstCommName.end(), strName);
    if (m_lstCommName.end() == itComm)
    {
        m_lstCommName.push_back(strName);
        m_input_cmd.SetCurSel(m_input_cmd.AddString(cstrName));
    }
}


void CToolDlg::OnCbnSelchangeCombo2()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CString cstrVal;

    int iIndex = m_input_cmd.GetCurSel();
    m_input_cmd.GetLBText(iIndex, cstrVal);
    if (0 == cstrVal.GetLength())
    {
        return;
    }

    if (TEXT(COMMSELECT) == cstrVal)
    {
        m_input_msg.SetWindowTextW(L"");
        return;
    }

    std::string strCommFile = g_strProPath + "command" + std::string("\\") + unicodeToAnsi(cstrVal.GetBuffer());
    FILE *pFile = fopen(strCommFile.c_str(), "r");
    if (NULL != pFile)
    {
        char acTmp[2048] = { 0 };
        fread(acTmp, 1, sizeof(acTmp) - 1, pFile);
        m_input_msg.SetWindowTextW(ansiToUnicode(acTmp));
        fclose(pFile);
    }
}
