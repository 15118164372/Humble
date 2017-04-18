
// ToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Tool.h"
#include "ToolDlg.h"
#include "afxdialogex.h"
#include "ShowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_MESSAGE_SHOWMFCMSG WM_USER + 100
#define WM_MESSAGE_ENBLELINKBUTT WM_MESSAGE_SHOWMFCMSG + 1

#define TIMERID 1
#define COMMSELECT "--请选择--"
#define COMMFOLDER "command"

HWND g_hWnd = NULL;

void showMFCMsg(const char *pszMsg, const unsigned int iLens)
{
    char *pBuff = new char[iLens + 1];
    memset(pBuff, 0, iLens + 1);
    memcpy(pBuff, pszMsg, iLens);

    SendMessage(g_hWnd, WM_MESSAGE_SHOWMFCMSG, (WPARAM)pBuff, iLens);
}

void enableLinkButt(void)
{
    SendMessage(g_hWnd, WM_MESSAGE_ENBLELINKBUTT, NULL, NULL);
}

H_BNAMSP
void H_SetPackPath(struct lua_State *pLState)
{
    std::list<std::string> lstDirs;

    H_GetSubDirName(g_strScriptPath.c_str(), lstDirs);
    luabridge::LuaRef objPack = luabridge::getGlobal(pLState, "package");
    std::string strPack = objPack["path"];
    std::string strVal = H_FormatStr("%s?.lua", g_strScriptPath.c_str());

    std::list<std::string>::iterator itDir;
    for (itDir = lstDirs.begin(); lstDirs.end() != itDir; ++itDir)
    {
        strVal = H_FormatStr("%s;%s%s/?.lua", strVal.c_str(), g_strScriptPath.c_str(), itDir->c_str());
    }

    strVal = H_FormatStr("%s;%sscript/public/?.lua", strVal.c_str(), g_strProPath.c_str());
    objPack["path"] = H_FormatStr("%s;%s", strPack.c_str(), strVal.c_str());
}
void H_RegOther(struct lua_State *pLState)
{
    luabridge::getGlobalNamespace(pLState)
        .addFunction("showMsg", showMFCMsg)
        .addFunction("enableLinkButt", enableLinkButt);
}
H_ENAMSP

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CToolDlg 对话框



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
    ON_COMMAND_RANGE(ID_MenuSave, ID_Show, &CToolDlg::OnMenuClick)
    ON_BN_CLICKED(IDC_BUTTON4, &CToolDlg::OnBnClickedButton4)
    ON_CBN_SELCHANGE(IDC_COMBO2, &CToolDlg::OnCbnSelchangeCombo2)
    ON_BN_CLICKED(IDC_BUTTON3, &CToolDlg::OnBnClickedButton3)
    ON_BN_CLICKED(IDC_BUTTON1, &CToolDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CToolDlg::OnBnClickedButton2)
    ON_MESSAGE(WM_MESSAGE_SHOWMFCMSG, ShowMsg)
    ON_MESSAGE(WM_MESSAGE_ENBLELINKBUTT, EnableLinkButt)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CToolDlg 消息处理程序

BOOL CToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    g_hWnd = m_hWnd;
    m_iLinkID = -1;
    m_usType = H_INIT_NUMBER;
    g_strProPath = H_GetProPath();
    g_strScriptPath = H_FormatStr("%s%s%s", g_strProPath.c_str(), "toolscript", H_PATH_SEPARATOR);

    m_CtrOutput.InsertColumn(0, "时间");//添加列
    m_CtrOutput.InsertColumn(1, "信息");
    setStyle();
    m_CtrOutput.SetColumnWidth(0, 200);//设置列宽
    m_CtrOutput.SetColumnWidth(1, 480);
    m_CtrIp.SetWindowTextA("127.0.0.1");
    m_CtrPort.SetWindowTextA("15000");

    m_pLState = luaL_newstate();
    luaL_openlibs(m_pLState);
    H_RegAll(m_pLState);
    std::string strLuaFile = g_strScriptPath + "send.lua";
    if (H_RTN_OK != luaL_dofile(m_pLState, strLuaFile.c_str()))
    {
        const char *pError = lua_tostring(m_pLState, -1);
        AfxMessageBox(pError);
    }

    initParser();
    setCommand();
    startSV();

    SetTimer(TIMERID, 1000, 0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
    m_objFont.CreateFontA(20,             //字体高度
        0,           // 字体宽度 
        0,           // 文本行的倾斜度 
        0,           // 字符基线的倾斜度 
        FW_DONTCARE,     // 字体的粗细 
        FALSE,        // 字体是否为斜体 
        FALSE,       // 字体是否带下划线 
        0,           // 字体是否带删除线 
        ANSI_CHARSET,              // 字体的字符集 
        OUT_DEFAULT_PRECIS,        // 字符的输出精度 
        CLIP_DEFAULT_PRECIS,       // 字符裁剪的精度 
        DEFAULT_QUALITY,           // 字符的输出质量 
        DEFAULT_PITCH | FF_SWISS, // 字符间距和字体族
        _T("新宋体"));//字体名称

    m_CtrInput.SetFont(&m_objFont);

    LONG lStyle;
    lStyle = GetWindowLong(m_CtrOutput.m_hWnd, GWL_STYLE);// 获取当前窗口style 
    lStyle &= ~LVS_TYPEMASK; // 清除显示方式位 
    lStyle |= LVS_REPORT; // 设置style 
    SetWindowLong(m_CtrOutput.m_hWnd, GWL_STYLE, lStyle);// 设置style 
    DWORD dwStyle = m_CtrOutput.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT;// 选中某行使整行高亮（只适用与report 风格的listctrl ） 
    dwStyle |= LVS_EX_GRIDLINES;// 网格线（只适用与report 风格的listctrl ） 
                                //dwStyle |= LVS_EX_CHECKBOXES;//item 前生成checkbox 控件 
    m_CtrOutput.SetExtendedStyle(dwStyle); // 设置扩展风格

    m_CtrOutput.SetFont(&m_objFont);
}

void CToolDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
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
        pSubMenu->EnableMenuItem(ID_Show, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }
    else
    {
        int iCon = m_CtrOutput.GetSelectionMark();
        if (-1 == iCon)
        {
            pSubMenu->EnableMenuItem(ID_Show, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        }
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
            CString ctrFilter = "文件 (*.txt)|*.txt||";
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
            strBuf = std::string("时间") +
                std::string("\t") +
                std::string("内容") +
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

        case ID_Show:
        {
            int iCon = m_CtrOutput.GetSelectionMark();
            if (-1 != iCon)
            {
                CString cstrMsg = m_CtrOutput.GetItemText(iCon, 1);
                CShowDlg objDlg;
                objDlg.setFont(&m_objFont);
                objDlg.setShow(cstrMsg.GetBuffer());
                objDlg.DoModal();
            }
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

    pParser->addParser(CTcp1::getSingletonPtr());
    m_CtrParser.AddString(CTcp1::getSingletonPtr()->getName());     
    pParser->addParser(CTcp2::getSingletonPtr());
    m_CtrParser.AddString(CTcp2::getSingletonPtr()->getName());
    pParser->addParser(CTcp3::getSingletonPtr());
    m_CtrParser.AddString(CTcp3::getSingletonPtr()->getName());
    pParser->addParser(CHttp::getSingletonPtr());
    m_CtrParser.AddString(CHttp::getSingletonPtr()->getName());
    pParser->addParser(CWebSock::getSingletonPtr());
    m_CtrParser.AddString(CWebSock::getSingletonPtr()->getName());
    pParser->addParser(CMQTT::getSingletonPtr());
    m_CtrParser.AddString(CMQTT::getSingletonPtr()->getName());
    pParser->addParser(CDefParser::getSingletonPtr());
    m_CtrParser.AddString(CDefParser::getSingletonPtr()->getName());

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

//保存
void CToolDlg::OnBnClickedButton4()
{
    // TODO: 在此添加控件通知处理程序代码
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
    // TODO: 在此添加控件通知处理程序代码
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

void CToolDlg::startSV(void)
{
    CLog *pLog = CLog::getSingletonPtr();
    CWorkerDisp *pWorker = CWorkerDisp::getSingletonPtr();
    CTick *pTick = CTick::getSingletonPtr();
    CLinker *pLinker = CLinker::getSingletonPtr();
    CNetWorker *pNet = CNetWorker::getSingletonPtr();
    CSender *pSender = CSender::getSingletonPtr();
    unsigned short usThreadNum(1);

    pTick->setTick(20);
    pWorker->setThreadNum(usThreadNum);
    pTick->setThreadNum(usThreadNum);
    pLog->setPriority(LOGLV_DEBUG);
    std::string strLogFile = H_FormatStr("%s%s%s%s", g_strProPath.c_str(),
        "log", H_PATH_SEPARATOR, "tool.log");
    pLog->setLogFile(strLogFile.c_str());
    pLog->Open();

    pNet->setIntf(CLNetDisp::getSingletonPtr());
    pTick->setIntf(CLTick::getSingletonPtr());

    CThread::Creat(pLog);
    pLog->waitStart();
    CThread::Creat(pLinker);
    pLinker->waitStart();
    CThread::Creat(pNet);
    pNet->waitStart();
    CThread::Creat(pSender);
    pSender->waitStart();
    CThread::Creat(pWorker);
    pWorker->waitStart();
    CThread::Creat(pTick);
    pTick->waitStart();
}

LRESULT CToolDlg::ShowMsg(WPARAM wParam, LPARAM lParam)
{
    char *pMsg = (char*)wParam;
    size_t uiLens = lParam;
    std::string strTmp(pMsg, uiLens);
    H_SafeDelete(pMsg);

    int iRow = m_CtrOutput.InsertItem(0, H_Now().c_str());//插入行
    m_CtrOutput.SetItemText(iRow, 1, strTmp.c_str());//设置数据

    return 0;
}

LRESULT CToolDlg::EnableLinkButt(WPARAM wParam, LPARAM lParam)
{
    m_CtrLink.EnableWindow(TRUE);
    m_CtrParser.EnableWindow(TRUE);

    return 0;
}

//发送
void CToolDlg::OnBnClickedButton3()
{
    // TODO: 在此添加控件通知处理程序代码
    if (-1 == m_iLinkID)
    {
        return;
    }

    CString cstrVal;
    m_CtrInput.GetWindowTextA(cstrVal);
    std::string strVal = cstrVal.GetBuffer();
    m_CtrParser.GetWindowTextA(cstrVal);
    std::string strParser = cstrVal.GetBuffer();

    if (strVal.empty())
    {
        return;
    }

    try
    {
        luabridge::getGlobal(m_pLState, "onSend")(strParser.c_str(), strVal.c_str());
    }
    catch (luabridge::LuaException &e)
    {
        showMFCMsg(e.what(), strlen(e.what()));
    }
}

//连接
void CToolDlg::OnBnClickedButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    if (-1 != m_iLinkID)
    {
        return;
    }

    CString cstrVal;

    m_CtrIp.GetWindowTextA(cstrVal);
    std::string strIp = cstrVal.GetBuffer();
    m_CtrPort.GetWindowTextA(cstrVal);
    unsigned short usPort = atoi(cstrVal.GetBuffer());
    m_CtrParser.GetWindowTextA(cstrVal);
    std::string strParser = cstrVal.GetBuffer();

    CNetWorker *pNet = CNetWorker::getSingletonPtr();
    m_iLinkID = pNet->addTcpLink(m_usType, strIp.c_str(), usPort);
    CNetParser::getSingletonPtr()->setParser(m_usType, strParser.c_str());

    m_CtrLink.EnableWindow(FALSE);
    m_CtrParser.EnableWindow(FALSE);
}

//断开
void CToolDlg::OnBnClickedButton2()
{
    // TODO: 在此添加控件通知处理程序代码
    if (-1 == m_iLinkID)
    {
        return;
    }

    CNetWorker::getSingletonPtr()->delTcpLink(m_iLinkID);
    m_iLinkID = -1;
    m_CtrLink.EnableWindow(TRUE);
    m_CtrParser.EnableWindow(TRUE);
}

void CToolDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CLog *pLog = CLog::getSingletonPtr();
    CWorkerDisp *pWorker = CWorkerDisp::getSingletonPtr();
    CTick *pTick = CTick::getSingletonPtr();
    CLinker *pLinker = CLinker::getSingletonPtr();
    CNetWorker *pNet = CNetWorker::getSingletonPtr();
    CSender *pSender = CSender::getSingletonPtr();

    pTick->Join();
    pWorker->Join();
    pSender->Join();
    pNet->Join();
    pLinker->Join();
    pLog->Join();

    if (NULL != m_pLState)
    {
        lua_close(m_pLState);
        m_pLState = NULL;
    }

    CDialogEx::OnClose();
}

