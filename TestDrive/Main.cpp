#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Main.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "TestDriveDoc.h"
#include "TestDriveView.h"
#include "TestDriveImp.h"
#include "CFileTypeAccess.h"
#include "UACElevate.h"
#include "TestDriveResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDriveApp

BEGIN_MESSAGE_MAP(CTestDriveApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CTestDriveApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

// CTestDriveApp ����

CTestDriveApp::CTestDriveApp(void)
{
	m_pTestDrive	= NULL;
	m_bHiColorIcons = TRUE;

	// TODO: �Ʒ� ���� ���α׷� ID ���ڿ��� ���� ID ���ڿ��� �ٲٽʽÿ�(����).
	// ���ڿ��� ���� ����: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("CloneX.TestDrive.AppID.NoVersion"));
}

CTestDriveApp::~CTestDriveApp(void)
{
	SAFE_DELETE(m_pTestDrive);
}

// ������ CTestDriveApp ��ü�Դϴ�.

CTestDriveApp theApp;


// CTestDriveApp �ʱ�ȭ

BOOL CTestDriveApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�. 
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	//CoInitialize(NULL);
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);	// for WebView2

	CWinAppEx::InitInstance();
	SetRegistryKey(_T("CloneX"));

	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	EnableTaskbarInteraction();

	// RichEdit ��Ʈ���� ����Ϸ���  AfxInitRichEdit2()�� �־�� �մϴ�.	
	//AfxInitRichEdit2();

	if (!CTestDrive::Initialize() || !InitRegistries())
		return FALSE;

	m_pTestDrive	= new CTestDrive;

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.

	LoadStdProfileSettings(8);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.
	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ���� ���α׷��� ���� ���ø��� ����մϴ�. ���� ���ø���
	//  ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_TestDrive_PMTYPE,
		RUNTIME_CLASS(CTestDriveDoc),
		RUNTIME_CLASS(CChildFrame), // ����� ���� MDI �ڽ� �������Դϴ�.
		RUNTIME_CLASS(CTestDriveView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// �� MDI ������ â�� ����ϴ�.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// ���̻簡 ���� ��쿡�� DragAcceptFiles�� ȣ���մϴ�.
	//  MDI ���� ���α׷������� m_pMainWnd�� ������ �� �ٷ� �̷��� ȣ���� �߻��ؾ� �մϴ�.
	// ��� ���⿡ ���� ���⸦ Ȱ��ȭ�մϴ�.
	m_pMainWnd->DragAcceptFiles();

	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if(cmdInfo.m_nShellCommand != CCommandLineInfo::FileOpen){
		cmdInfo.m_nShellCommand	= CCommandLineInfo::FileNothing;
	}

	// DDE Execute ���⸦ Ȱ��ȭ�մϴ�.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// ����ٿ� ������ ����� ����ġ�մϴ�.
	// ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// �� â�� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

BOOL CTestDriveApp::InitRegistries(void) {

	CString	sTESTDRIVE_DIR;

	{	// TestDrive ȯ�� ���� ��� Ȯ��
		sTESTDRIVE_DIR = InstalledPath();
		sTESTDRIVE_DIR.Replace(_T('\\'), _T('/'));
	}
	// ���ο� ������� üũ
	if (GetGlobalEnvironmentVariable(_T("TESTDRIVE_DIR")).Compare(sTESTDRIVE_DIR)) {
		// UAC ȹ��Ǿ�� ��.
		BOOL bElevated = FALSE;
		UACElevate UAC;
		if (UAC.IsVistaOrHigher() && UAC.IsElevated(&bElevated) != E_FAIL) {
			// UAC �� �����ϰ� ������ ������ �ƴ�.
			if (!bElevated) {
				if (MessageBox(NULL, _S(ACQUIRE_ADMIN), _S(TESTDRIVE), MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
					return FALSE;
				UAC.SelfExecuteWithElevation();
				return FALSE;
			}

		}
	}

	SetGlobalEnvironmentVariable(_T("TESTDRIVE_DIR"), sTESTDRIVE_DIR);	// TestDrive path ȯ�� ������ ���
	{	// ��ť��Ʈ ������Ʈ�� ���
		CFileTypeAccess	fAccess;
		CString sCmd;
		fAccess.SetExtension(_T("profile"));
		sCmd.Format(_T("%sTestDrive.exe \"%%1\""), InstalledPath());
		fAccess.SetShellOpenCommand(sCmd);
		fAccess.SetDocumentShellOpenCommand(sCmd);
		fAccess.SetDocumentClassName(_T("TestDrive.Document"));
		sCmd.Format(_T("%sTestDrive.exe,1"), InstalledPath());
		fAccess.SetDocumentDefaultIcon(sCmd);
		fAccess.RegSetAllInfo();
	}
	{	// NotePad++ ȯ�� �߰�
		CString sCommand;
		sCommand.Format(_T("\"%s\\bin\\notepad\\notepad++.exe\" \"%%1\""), InstalledPath());
		SetRegistryString(HKEY_CLASSES_ROOT, _T("*\\shell\\notepad++"), NULL, _T("Open with NotePad++"));
		SetRegistryString(HKEY_CLASSES_ROOT, _T("*\\shell\\notepad++\\command"), NULL, sCommand);
		sCommand.Format(_T("\"%s\\bin\\HexEdit.exe\" \"%%1\""), InstalledPath());
		SetRegistryString(HKEY_CLASSES_ROOT, _T("*\\shell\\hexedit"), NULL, _T("Open with HexEdit"));
		SetRegistryString(HKEY_CLASSES_ROOT, _T("*\\shell\\hexedit\\command"), NULL, sCommand);

		SetRegistryDWORD(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION"), _T("TestDrive.exe"), 10000);
	}
	// MinGW ȯ�� �߰�
	ModifyGlobalEnvironmentPath(CString(InstalledPath()) + _T("bin\\msys64\\usr\\bin"), _T("\\msys64\\usr\\bin"));
	ModifyGlobalEnvironmentPath(CString(InstalledPath()) + _T("bin\\msys64\\mingw64\\bin"), _T("\\msys64\\mingw64\\bin"));
	ModifyGlobalEnvironmentPath(CString(InstalledPath()) + _T("bin\\msys64\\ucrt64\\bin"), _T("\\msys64\\ucrt64\\bin"));

	// ���� deprecated ����
	//ModifyGlobalEnvironmentPath(NULL, CString(InstalledPath()) + _T("bin\\MinGW\\bin"));

	return TRUE;
}

int CTestDriveApp::ExitInstance()
{
	//TODO: �߰��� �߰� ���ҽ��� ó���մϴ�.
	CTestDrive::Release();
	AfxOleTerm(FALSE);
	CoUninitialize();
	return CWinAppEx::ExitInstance();
}

// CTestDriveApp �޽��� ó����


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CTestDriveApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CTestDriveApp ����� ���� �ε�/���� �޼���

void CTestDriveApp::PreLoadState()
{
	//BOOL bNameValid;
	//CString strName;
	//bNameValid = strName.LoadString(IDS_EDIT_MENU);
	//ASSERT(bNameValid);
	//GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	//bNameValid = strName.LoadString(IDS_EXPLORER);
	//ASSERT(bNameValid);
	//GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CTestDriveApp::LoadCustomState()
{
}

void CTestDriveApp::SaveCustomState()
{
}

void PeekAndPumpMessage(void){
	MSG Msg;
	while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE)) 
	{
		// MFC������ �̰ɷ� ����Ѵ�.
		//(void)AfxGetApp()->PumpMessage(); //lint !e1924 (warning about C-style cast)

		// dummy message handler
		BOOL bRet = ::GetMessage(&Msg, NULL, 0, 0);
		if(bRet == -1)
		{
			//TRACE(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
			continue;   // error, don't process
		}
		else if(!bRet)
		{
			//TRACE(atlTraceUI, 0, _T("CMessageLoop::Run - exiting\n"));
			break;	   // WM_QUIT, exit message loop
		}

		if(!theApp.PreTranslateMessage(&Msg))
		{

			::TranslateMessage(&Msg);
			::DispatchMessage(&Msg);
		}
	}
}

