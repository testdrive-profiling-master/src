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
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()

// CTestDriveApp 생성

CTestDriveApp::CTestDriveApp(void)
{
	m_pTestDrive	= NULL;
	m_bHiColorIcons = TRUE;

	// TODO: 아래 응용 프로그램 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("CloneX.TestDrive.AppID.NoVersion"));
}

CTestDriveApp::~CTestDriveApp(void)
{
	SAFE_DELETE(m_pTestDrive);
}

// 유일한 CTestDriveApp 개체입니다.

CTestDriveApp theApp;


// CTestDriveApp 초기화

BOOL CTestDriveApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	//CoInitialize(NULL);
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);	// for WebView2

	CWinAppEx::InitInstance();
	SetRegistryKey(_T("CloneX"));

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	EnableTaskbarInteraction();

	// RichEdit 컨트롤을 사용하려면  AfxInitRichEdit2()가 있어야 합니다.	
	//AfxInitRichEdit2();

	if (!CTestDrive::Initialize() || !InitRegistries())
		return FALSE;

	m_pTestDrive	= new CTestDrive;

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.

	LoadStdProfileSettings(8);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.
	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_TestDrive_PMTYPE,
		RUNTIME_CLASS(CTestDriveDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CTestDriveView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 주 MDI 프레임 창을 만듭니다.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  MDI 응용 프로그램에서는 m_pMainWnd를 설정한 후 바로 이러한 호출이 발생해야 합니다.
	// 끌어서 놓기에 대한 열기를 활성화합니다.
	m_pMainWnd->DragAcceptFiles();

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if(cmdInfo.m_nShellCommand != CCommandLineInfo::FileOpen){
		cmdInfo.m_nShellCommand	= CCommandLineInfo::FileNothing;
	}

	// DDE Execute 열기를 활성화합니다.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 주 창이 초기화되었으므로 이를 표시하고 업데이트합니다.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

BOOL CTestDriveApp::InitRegistries(void) {

	CString	sTESTDRIVE_DIR;

	{	// TestDrive 환경 변수 경로 확인
		sTESTDRIVE_DIR = InstalledPath();
		sTESTDRIVE_DIR.Replace(_T('\\'), _T('/'));
	}
	// 새로운 변경사항 체크
	if (GetGlobalEnvironmentVariable(_T("TESTDRIVE_DIR")).Compare(sTESTDRIVE_DIR)) {
		// UAC 획득되어야 함.
		BOOL bElevated = FALSE;
		UACElevate UAC;
		if (UAC.IsVistaOrHigher() && UAC.IsElevated(&bElevated) != E_FAIL) {
			// UAC 가 존재하고 관리자 권한이 아님.
			if (!bElevated) {
				if (MessageBox(NULL, _S(ACQUIRE_ADMIN), _S(TESTDRIVE), MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
					return FALSE;
				UAC.SelfExecuteWithElevation();
				return FALSE;
			}

		}
	}

	SetGlobalEnvironmentVariable(_T("TESTDRIVE_DIR"), sTESTDRIVE_DIR);	// TestDrive path 환경 변수를 등록
	{	// 도큐먼트 레지스트리 등록
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
	{	// NotePad++ 환경 추가
		CString sCommand;
		sCommand.Format(_T("\"%s\\bin\\notepad\\notepad++.exe\" \"%%1\""), InstalledPath());
		SetRegistryString(HKEY_CLASSES_ROOT, _T("*\\shell\\notepad++"), NULL, _T("Open with NotePad++"));
		SetRegistryString(HKEY_CLASSES_ROOT, _T("*\\shell\\notepad++\\command"), NULL, sCommand);
		sCommand.Format(_T("\"%s\\bin\\HexEdit.exe\" \"%%1\""), InstalledPath());
		SetRegistryString(HKEY_CLASSES_ROOT, _T("*\\shell\\hexedit"), NULL, _T("Open with HexEdit"));
		SetRegistryString(HKEY_CLASSES_ROOT, _T("*\\shell\\hexedit\\command"), NULL, sCommand);

		SetRegistryDWORD(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION"), _T("TestDrive.exe"), 10000);
	}
	// MinGW 환경 추가
	ModifyGlobalEnvironmentPath(CString(InstalledPath()) + _T("bin\\msys64\\usr\\bin"), _T("\\msys64\\usr\\bin"));
	ModifyGlobalEnvironmentPath(CString(InstalledPath()) + _T("bin\\msys64\\mingw64\\bin"), _T("\\msys64\\mingw64\\bin"));
	ModifyGlobalEnvironmentPath(CString(InstalledPath()) + _T("bin\\msys64\\ucrt64\\bin"), _T("\\msys64\\ucrt64\\bin"));

	// 이전 deprecated 제거
	//ModifyGlobalEnvironmentPath(NULL, CString(InstalledPath()) + _T("bin\\MinGW\\bin"));

	return TRUE;
}

int CTestDriveApp::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	CTestDrive::Release();
	AfxOleTerm(FALSE);
	CoUninitialize();
	return CWinAppEx::ExitInstance();
}

// CTestDriveApp 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CTestDriveApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CTestDriveApp 사용자 지정 로드/저장 메서드

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
		// MFC에서는 이걸로 사용한다.
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

