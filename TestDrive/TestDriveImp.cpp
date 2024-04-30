#include "stdafx.h"
#include "TestDriveImp.h"
#include "Main.h"
#include "Paser.h"
#include "Output.h"
#include "ProfileData.h"
#include "DocumentCtrl.h"
#include "AVIGenerator.h"
#include "TDSemaphore.h"
#include "TestDriveResource.h"
#include "SerialCom/serialport.h"
#include "SplashScreenEx.h"

static const TCHAR *MMFileName		= _T("TESTDRIVE_MEMORY_MAPPED");	// default paged memory name

CTestDrive*		g_pTestDrive	= NULL;
CSemaphore		g_Semaphore;

CTestDrive::CTestDrive(void){
	g_pTestDrive	= this;
	m_bProjectOpen	= FALSE;
	m_bRunning		= FALSE;

	{	// 현재 폴더를 프로젝트 폴더로 지정
		TCHAR szTemp[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, szTemp);
		m_sProjectDir	= szTemp;
	}
	CloseProject();
}

CTestDrive::~CTestDrive(void){
	CloseProject();
	g_pTestDrive	= NULL;
	Release();
}

BOOL CTestDrive::Initialize(void){
	if (!g_Localization.Initialize(InstalledPath())) {
		MessageBox(NULL, _T("Initialization of localization is failed.\n"), _T("Error"), MB_OK);
		return FALSE;
	}
	if (!CSpeech::Initialize()){
		MessageBox(NULL, _T("Speech system is not existed.\n"), _T("Error"), MB_OK);
		return FALSE;
	}
	return TRUE;
}

void CTestDrive::Release(void){
	CSpeech::Relase();
	g_Localization.Release();
}

void CTestDrive::ClearLog(void){
	g_Output[COutput::TD_OUTPUT_SYSTEM].Clear();
	g_Output[COutput::TD_OUTPUT_APPLICATION].Clear();
	g_Output[COutput::TD_OUTPUT_NOTIFY].Clear();
}

void CTestDrive::LogOut(LPCTSTR szMsg, SYSMSG_ID id){
	static CSemaphore		LogSema;
	COLORREF	color	= 0;
	DWORD		effect	= 0;
	CString		msg;

	if(id==SYSMSG_INFO) msg.Format(_T("%s\n"), szMsg);
	else msg.Format(_T("%s : %s\n"), (id==SYSMSG_WARNING) ? _S(WARNING) : _S(ERROR), szMsg);

	switch(id){
	case SYSMSG_INFO:
		color = RGB(0,0,255);
		break;
	case SYSMSG_WARNING:
		color	= RGB(207,73,0);
		effect	= CFE_BOLD;
		break;
	case SYSMSG_ERROR:
		color	= RGB(255,0,0);
		effect	= CFE_BOLD|CFE_UNDERLINE;
		break;
	}

	LogSema.Lock();
	g_Output[COutput::TD_OUTPUT_SYSTEM].LogOut(msg, color, effect);
	if(effect) g_Output[COutput::TD_OUTPUT_NOTIFY].LogOut(msg, color, effect);
	LogSema.Unlock();
}

void CTestDrive::LogInfo(LPCTSTR szFormat, ...){
	CString str;
	va_list args;
	va_start(args, szFormat);
	str.FormatV(szFormat,args);
	va_end(args);
	LogOut(str, SYSMSG_INFO);
}

void CTestDrive::LogWarning(LPCTSTR szFormat, ...){
	CString str;
	va_list args;
	va_start(args, szFormat);
	str.FormatV(szFormat,args);
	va_end(args);
	LogOut(str, SYSMSG_WARNING);
}

void CTestDrive::LogError(LPCTSTR szFormat, ...){
	CString str;
	va_list args;
	va_start(args, szFormat);
	str.FormatV(szFormat,args);
	va_end(args);
	LogOut(str, SYSMSG_ERROR);
}

void ShowDescriptionCode(CPaser* pPaser){
	g_Output[COutput::TD_OUTPUT_NOTIFY].LogOut(_TEXT_(CString(_S(LINE)) + _T("(%d) : %s\n"), pPaser->GetLineCount(), pPaser->GetCurLine()));
}

CString& CTestDrive::UpdateTite(void){
	m_sTitle = m_sTitleDB[TITLE_MAIN];
	if (m_bProjectOpen) {
		if (m_sTitleDB[TITLE_WORK].GetLength()) m_sTitle += _T(" : ") + m_sTitleDB[TITLE_WORK];
		if (m_sTitleDB[TITLE_DOC].GetLength()) m_sTitle += _T("(") + m_sTitleDB[TITLE_DOC] + _T("(");
	}
	return m_sTitle;
}

void CTestDrive::SetTitle(TITLE_ID id, LPCTSTR szTitle){
	m_sTitleDB[id] = szTitle;
	UpdateTite();
}

LPCTSTR CTestDrive::GetTitle(void){
	return m_sTitle;
}

DWORD CTestDrive::GetVersion(void){
	return TESTDRIVE_VERSION;
}

DWORD CTestDrive::GetLocale(void){
	return g_Localization.CurrentLocale()->dwLangID;
}

void CTestDrive::SetProfile(ITDSystem::PROFILE_ID id, LPCTSTR lpszFileName){
	SetProfilePath((TESTDRIVE_PROFILE)id, lpszFileName);
}

BOOL CTestDrive::OpenProject(LPCTSTR szFileName)
{
	// Open file dialog
	if(!CloseProject()) {
		LogOut(_S(PROJECT_BUSY));
		return FALSE;
	}

	// set project path
	if (!szFileName) {
		szFileName = m_sProjectFile;
	} else {
		if (!SetProjectFile(szFileName))
			return FALSE;
	}
	SetProjectDirectroyToCurrent();

	m_Speech.SetNotifyEvent(AfxGetMainWnd()->GetSafeHwnd());

	// Clear message windows
	for(int i=0;i<COutput::TD_OUTPUT_SIZE;i++) g_Output[i].Clear();

	SetProfilePath(TESTDRIVE_PROFILE_INITIALIZE, szFileName);
	
	// set open state
	m_bProjectOpen	= TRUE;

	m_Lua.Initialize();

	// TestDrive project initialization
	g_pTestDrive->Build((LPCTSTR)(CString(InstalledPath()) + _T("bin\\testdrive_initialize.profile")), &g_Output[COutput::TD_OUTPUT_SYSTEM]);

	// run initialization profile
	if(!Build(TESTDRIVE_PROFILE_INITIALIZE)) CloseProject(TRUE);

	UpdateTite();
	return m_bProjectOpen;
}

BOOL CTestDrive::CloseProject(BOOL bPreserveMessage)
{
	if(IsBusy()) return FALSE;
	//if(!m_bProjectOpen) return TRUE;

	for(int i=0;i<TESTDRIVE_PROFILE_SIZE;i++) m_sProfileMenu[i].Empty();
	for(int i=0;i<TITLE_MAX;i++) m_sTitleDB[i].Empty();
	m_sTitleDB[TITLE_MAIN] = _S(NO_PROJECT);
	UpdateTite();

	m_dwCountError		= 0;
	m_dwCountWarning	= 0;

	if(g_pDocumentList)
		g_pDocumentList->RemoveAll(TRUE);

	g_ProfileTree.DeleteAllItems();

	// Clear all output message
	if(!bPreserveMessage && m_bProjectOpen) ClearLog();
	m_bProjectOpen	= FALSE;

	CMemory::DeleteAll();
	m_Lua.Initialize();

	return TRUE;
}

BOOL CTestDrive::IsProjectOpen(void){
	return m_bProjectOpen;
}

BOOL CTestDrive::IsBusy(void){
	if(CRedirectExecute::GetCurrent()) return TRUE;
	return FALSE;
}

BOOL CTestDrive::IsValidProfile(TESTDRIVE_PROFILE id){
	BOOL bValid = m_bProjectOpen && (!m_sProfileMenu[id].IsEmpty());
	return bValid;
}

typedef enum {
	SEARCH_COMMAND_DESCRIPTION,
	SEARCH_COMMAND_TYPE,
	SEARCH_COMMAND_ERROR_TOKEN,
	SEARCH_COMMAND_WORK_FOLDER,
	SEARCH_COMMAND_FILE_NAME,
	SEARCH_COMMAND_OPERAND,
	SEARCH_COMMAND_SCRIPT,
	SEARCH_COMMAND_SIZE
}SEARCH_COMMAND;

static const LPCTSTR __sSearchCommand[SEARCH_COMMAND_SIZE] = {
	_T("DESCRIPTION"),
	_T("TYPE"),
	_T("ERROR_TOKEN"),
	_T("WORK_FOLDER"),
	_T("FILE_NAME"),
	_T("OPERAND"),
	_T("SCRIPT"),
};


static BOOL SearchTreeFile(LPCTSTR sPath, LPVOID pData){
	BOOL	bRet			= TRUE;
	LPCTSTR sHead			= (LPCTSTR)pData;
	int		iType			= TREE_ITEM_PROFILE;
	CString	sRunFile		= _T(".TestDrive.run.sp");
	CString sWorkFolder		= sPath;
	CString sDescription;
	CString sFileName;
	CString sOperand;
	CString sErrorToken;

	{	// eliminate file part
		int iPos = sWorkFolder.ReverseFind(_T('\\'));
		if(iPos>0) sWorkFolder.Delete(iPos, -1);
	}

	FILE*	fp				= _tfopen(sPath, _T("rt"));
	if(fp){
		TCHAR			sLine[4096];
		static	LPCTSTR	sTrim	= _T(" \t\r\n");
		int		iCom	= (int)_tcslen(sHead);

		while(_fgetts(sLine, 4095, fp) && bRet){
			if(_tcsstr(sLine, sHead) != sLine) continue;
			{
				CString	sTok	= &sLine[iCom];
				CString	sData;
				int		iID		= -1;
				// get command
				for(int i=0;i<SEARCH_COMMAND_SIZE;i++){
					if(!sTok.Find(__sSearchCommand[i])){
						sTok.Delete(0, (int)_tcslen(__sSearchCommand[i]));
						iID		= i;
						break;
					}
				}
				sTok.TrimLeft(sTrim);
				if(sTok[0] == _T('=')){	// direct assignment
					sTok.Delete(0);
					sTok.TrimLeft(sTrim);
					sTok.TrimRight(sTrim);
					sData	= sTok;
					sTok.Empty();
				}else
				if(sTok[0] == _T(':')){	// indirect assignment
					sTok.Delete(0);
					sTok.TrimLeft(sTrim);
					if(!_fgetts(sLine, 4095, fp)){
					ERROR_CASE:
						bRet	= FALSE;
						break;
					}
					sData		= sLine;
					int iPos	= sData.Find(sTok[0]);
					if(iPos<0) goto ERROR_CASE;
					sTok.Delete(0);
					sTok.TrimLeft(sTrim);
					sData.Delete(0, iPos+1);
					sData.TrimLeft(sTrim);
					sData.TrimRight(sTrim);
				}else{	// error case
					bRet	= FALSE;
					break;
				}
				sTok.TrimRight(sTrim);

				// decode command
				switch(iID){
				case SEARCH_COMMAND_DESCRIPTION:
					{
						sDescription.Format(_T("%s %s"), sTok, sData);
						sDescription.TrimLeft(sTrim);
					}
					break;
				case SEARCH_COMMAND_TYPE:
					{
						int iID	= -1;
						for(int i=0;i<=TREE_ITEM_PROFILE;i++)
							if(!sData.Compare(g_sProfileTree[i])){
								iID	= i;
								break;
							}
						if(iID==-1){
							bRet	= FALSE;
							break;
						}else{
							iType	= iID;
						}	
					}
					break;
				case SEARCH_COMMAND_ERROR_TOKEN:
					{
						sErrorToken.Format(_T("%s%s"), sTok, sData);
					}
					break;
				case SEARCH_COMMAND_WORK_FOLDER:
					{
						CWorkDirectory dir(sWorkFolder);
						sWorkFolder.Format(_T("%s%s\\"), sTok, sData);
						CFullPath full_path(sWorkFolder);
						sWorkFolder	= full_path.Path();
					}
					break;
				case SEARCH_COMMAND_FILE_NAME:
					{
						if(sTok[0])	sFileName.Format(sTok, sData);
						else		sFileName	= sData;
					}
					break;
				case SEARCH_COMMAND_OPERAND:
					sOperand	= sData;
					break;
				case SEARCH_COMMAND_SCRIPT:
					sRunFile.Format(_T("%s%s"), sTok, sData);
					break;
				default:
					bRet	= FALSE;
					break;
				}
			}
		}
		if(!bRet){
			g_pTestDrive->LogError(_T("Invalid command : \"%s\""), sLine);
		}else if(sDescription[0]){
			CString sFilePath(sPath);
			{	// del file part & replace runfile name
				int iPos = sFilePath.ReverseFind(_T('\\'));
				if(iPos>0){
					sFilePath.Delete(iPos+1, -1);
				}
				sFilePath	+= sRunFile;
			}
			g_ProfileTree.AddItem((TD_TREE_ITEM)iType, sDescription);
			g_ProfileTree.SetCurrentItemData(new CProfileData(sFilePath));
			{
				FILE* fp = _tfopen(sFilePath, _T("wt"));
				if(fp){
					CString sCommand;
					sFileName.Replace(_T("\\"), _T("\\\\"));
					sWorkFolder.Replace(_T("\\"), _T("\\\\"));
					sErrorToken.Replace(_T("\\"), _T("\\\\"));
					sOperand.Replace(_T("\\"), _T("\\\\"));
					sRunFile.Replace(_T("\\"), _T("\\\\"));
					_ftprintf(fp, _T("// Created by TestDrive Profiling master.\nsystem.clear\n"));
					sCommand.Format(_T("system.msg \"%s\"\n"), _S(PROFILE_PROGRAM_IS_RUNNING));
					_ftprintf(fp, sCommand, sDescription, sFileName);
					_ftprintf(fp, _T("system.call \"%s\", \"%s\", \"%s\"%s%s\n"),
						(LPCTSTR)sFileName,
						(LPCTSTR)sOperand,
						(LPCTSTR)sWorkFolder,
						sErrorToken[0] ? _T(", ") : _T(""),
						(LPCTSTR)sErrorToken);
					sCommand.Format(_T("system.msg \"%s\"\n"), _S(PROFILE_PROGRAM_IS_DONE));
					_ftprintf(fp, sCommand, sDescription);
					fclose(fp);
				}
			}
		}
		fclose(fp);
	}

	return bRet;
}

BOOL SetProfileTreeFromPaser(CPaser* pPaser)
{
	HTREEITEM		hItem	= NULL;		
	BOOL			bRet	= FALSE;
	int				iLoop	= 1;
	TCHAR			token[MAX_PATH];
	TD_TOKEN_TYPE	type;
	int				i;

	if(!pPaser) return bRet;

	pPaser->SetMultilineTokenable(TRUE);

	if(pPaser->TokenOut(TD_DELIMITER_LOPEN)){
		g_ProfileTree.DeleteAllItems();
	}else{
		if(pPaser->GetToken(token)!=TD_TOKEN_STRING) goto END_RETRIEVE_PROFILE_TREE;
		if(*token){	// 이름이 '*' 로 시작하면 하위 아이템들 모두 지움.
			BOOL bDeleteSubItems	= *token==_T('*');
			hItem = g_ProfileTree.SetCurrentRootItem(bDeleteSubItems ? (token + 1): token);	// 하위 아이템을 찾으려면 '\' 기호로 분리한다. 예)"시스템\비디오"
			if(!hItem) goto END_RETRIEVE_PROFILE_TREE;
			if(bDeleteSubItems) g_ProfileTree.DeleteSubItems(hItem);
		}else{
			g_ProfileTree.SetRootItem();
		}
		
		if(!pPaser->TokenOut(TD_DELIMITER_LOPEN)) goto END_RETRIEVE_PROFILE_TREE;
	}

	while(iLoop){
		if(!pPaser->IsTokenable()) break;
		type = pPaser->GetToken(token);
		switch(type){
		case TD_TOKEN_NAME:
			i = CheckCommand(token, g_sProfileTree, PROFILE_ITEM_SIZE);
			switch(i){
			case TREE_ITEM_TREE_OPEN:
				if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) goto END_RETRIEVE_PROFILE_TREE;		// (
				if(pPaser->GetToken(token) != TD_TOKEN_STRING) goto END_RETRIEVE_PROFILE_TREE;	// name
				if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) goto END_RETRIEVE_PROFILE_TREE;		// )
				if(!pPaser->TokenOut(TD_DELIMITER_LOPEN)) goto END_RETRIEVE_PROFILE_TREE;		// {
				iLoop++;
				// add new profile tree
				g_ProfileTree.AddItem(TREE_ITEM_TREE_OPEN, token);
				break;
			case TREE_ITEM_TREE_EXPAND:
				g_ProfileTree.ExpandCurrent();
				if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) goto END_RETRIEVE_PROFILE_TREE;		// (
				if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) goto END_RETRIEVE_PROFILE_TREE;		// )
				if(!pPaser->TokenOut(TD_DELIMITER_SEMICOLON)) goto END_RETRIEVE_PROFILE_TREE;	// ;
				break;
			case TREE_ITEM_SCREEN:
			case TREE_ITEM_CHART:
			case TREE_ITEM_REPORT:
			case TREE_ITEM_NEWORK:
			case TREE_ITEM_PROFILE:
				if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) goto END_RETRIEVE_PROFILE_TREE;		// (
				if(pPaser->GetToken(token) != TD_TOKEN_STRING) goto END_RETRIEVE_PROFILE_TREE;	// name
				// add new item
				g_ProfileTree.AddItem((TD_TREE_ITEM)i, token);
				if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto END_RETRIEVE_PROFILE_TREE;		// ,
				if(pPaser->GetToken(token) != TD_TOKEN_STRING) goto END_RETRIEVE_PROFILE_TREE;	// filename
				g_ProfileTree.SetCurrentItemData(new CProfileData(token));
				if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) goto END_RETRIEVE_PROFILE_TREE;		// )
				if(!pPaser->TokenOut(TD_DELIMITER_SEMICOLON)) goto END_RETRIEVE_PROFILE_TREE;	// ;
				// link new profiles
				break;
			case TREE_ITEM_SEARCH:
				if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) goto END_RETRIEVE_PROFILE_TREE;		// (
				{
					CString sSearchPath, sFileName, sHead;
					if(pPaser->GetToken(token) != TD_TOKEN_STRING) goto END_RETRIEVE_PROFILE_TREE;	// search path
					sSearchPath	= token;
					if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto END_RETRIEVE_PROFILE_TREE;		// ,
					if(pPaser->GetToken(token) != TD_TOKEN_STRING) goto END_RETRIEVE_PROFILE_TREE;	// filename
					sFileName	= token;
					if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto END_RETRIEVE_PROFILE_TREE;		// ,
					if(pPaser->GetToken(token) != TD_TOKEN_STRING) goto END_RETRIEVE_PROFILE_TREE;	// header
					sHead		= token;
					if(!g_pTestDrive->SearchSubPathFile(sSearchPath, sFileName, SearchTreeFile, (LPVOID)(LPCTSTR)sHead))
						goto END_RETRIEVE_PROFILE_TREE;
				}
				if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) goto END_RETRIEVE_PROFILE_TREE;		// )
				if(!pPaser->TokenOut(TD_DELIMITER_SEMICOLON)) goto END_RETRIEVE_PROFILE_TREE;	// ;
				break;
			default:
				goto END_RETRIEVE_PROFILE_TREE;
			}
			//token
			break;
		case TD_TOKEN_DELIMITER:
			if(*token != *g_PaserDelimiter[TD_DELIMITER_LCLOSE]) goto END_RETRIEVE_PROFILE_TREE;
			// close profile tree
			g_ProfileTree.AddItem(TREE_ITEM_TREE_CLOSE);
			iLoop--;
			if(!iLoop) {
				bRet = TRUE;
				goto END_RETRIEVE_PROFILE_TREE;
			}
			break;
		}
	}

END_RETRIEVE_PROFILE_TREE:
	pPaser->SetMultilineTokenable(FALSE);

	return bRet;
}

BOOL CTestDrive::SetProfilePath(TESTDRIVE_PROFILE id, LPCTSTR szFileName){
	if(id >= TESTDRIVE_PROFILE_SIZE) return FALSE;
	if(!szFileName) m_sProfileMenu[id] = _T("");
	else{
		CFullPath full_path(szFileName);
		m_sProfileMenu[id] = full_path.Path();
	}
	return TRUE;
}

BOOL CTestDrive::SetProjectFile(LPCTSTR lpszPath) {
	if (!lpszPath) return FALSE;
	{	// "profile" 타입 이외의 파일을 읽지 않는다. "sp" 등 서브 프로파일을 프로젝트로써 읽지 못하게 한다.
		TCHAR	sFullPath[4096];
		TCHAR*	sFilePart;
		CString	sConfigFileName;
		int		iExtPos	= 0;
		{
			GetFullPathName(lpszPath, 4096, sFullPath, &sFilePart);
			if (!sFilePart) return FALSE;	// not a file!
		}
		{
			CString	sExt(sFilePart);
			int point = 0, get_point;
			sConfigFileName = sFilePart;
			while ((get_point = sExt.Find(_T('.'), point)) > 0) point = get_point + 1;
			sExt = &(sExt.GetBuffer()[point]);
			sExt.MakeLower();
			if (sExt.Compare(_T("profile"))) return FALSE;
			sConfigFileName.Delete(point, 7);
			sConfigFileName += _T("ini");
		}
		// set project file full path
		m_sProjectFile = sFullPath;
		
		// set directory path
		{
			*sFilePart		= NULL;
			m_sProjectDir	= sFullPath;
		}

		// set config path
		{
			CString sConfigFilePath(m_sProjectDir);
			sConfigFilePath	+= sConfigFileName;
			m_Config.SetConfigFile(sConfigFilePath);
		}

		// set project environment variable
		SetEnvironmentVariable(_T("PROJECT"), m_sProjectDir);
	}
	return TRUE;
}

void CTestDrive::SetProjectDirectroyToCurrent(void){
	SetCurrentDirectory(m_sProjectDir);
}

BOOL CTestDrive::Build(TESTDRIVE_PROFILE id)
{
	COutput* pMsg;
	if(!IsValidProfile(id)) return FALSE;

	pMsg = &g_Output[COutput::TD_OUTPUT_SYSTEM];
	return Build(m_sProfileMenu[id], pMsg);
}

BOOL CTestDrive::RunProfile(LPCTSTR szFileName, BOOL bSystem){
	COutput* pMsg = &g_Output[bSystem ? COutput::TD_OUTPUT_SYSTEM : COutput::TD_OUTPUT_APPLICATION];
	return Build(szFileName, pMsg, TRUE);
}

int CTestDrive::ExecuteFile(LPCTSTR lpszFileName, LPCTSTR lpszArg, BOOL bSystemLog, TD_REDIRECT_LOG func, LPCTSTR lpszWorkDir, ...) {
	COutput* pMsg = &g_Output[bSystemLog ? COutput::TD_OUTPUT_SYSTEM : COutput::TD_OUTPUT_APPLICATION];
	int iRet;


	//g_Semaphore.Lock();
	// run batch file
	CRedirectExecute* pExec = new CRedirectExecute(lpszFileName, pMsg, lpszArg, lpszWorkDir);

	{
		// 에러 문자열 지정
		{
			LPCTSTR		pToken;
			int			iID;
			va_list marker;
			va_start(marker, lpszWorkDir);

			for (;;) {
				if (!(pToken = va_arg(marker, LPCTSTR))) break;
				//@FIXME : NULL 이 없는 경우에는 에러가 발생되므로 주의한다.
				iID = va_arg(marker, int);
				if ((iID&0xFFFF0000) == REDIRECT_ARGUMENT(0)) {
					pExec->AddErrorToken(pToken, va_arg(marker, DWORD_PTR));
				}else
					pExec->AddErrorToken(pToken, (DWORD_PTR)iID);
			}
			va_end(marker);
		}

		pExec->SetLogFunction(func);

		if (!pExec->Run()) iRet = 0x80000000;
		else iRet = pExec->GetErrorCode();
	}

	SAFE_DELETE(pExec);
	//g_Semaphore.Unlock();

	return iRet;
}

ITDImplSystem* CTestDrive::GetImplementation(void){
	return NULL;
}

ITDDocument* CTestDrive::GetDocument(LPCTSTR lpszName){
	return g_pDocumentList->Find(lpszName);
}

ITDMemory* CTestDrive::GetMemory(LPCTSTR lpszName, BOOL bForced){
	CMemory* pMemory = CMemory::Find(lpszName);
	if(!pMemory && bForced) pMemory = new CMemory;
	return pMemory;
}

double CTestDrive::GetElapsedTime(LONGLONG& llPrevTime){
	static BOOL		bInitialize = FALSE;
	static LONGLONG llQPFTicksPerSec  = 0;
	LONGLONG		CurTime;
	double			ElapsedTime;
	if(!bInitialize){
		QueryPerformanceFrequency((LARGE_INTEGER*)&llQPFTicksPerSec);
		bInitialize	= TRUE;		
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&CurTime);
	ElapsedTime	= (double)(CurTime - llPrevTime) / llQPFTicksPerSec;
	llPrevTime	= CurTime;
	return ElapsedTime;
}

ITDMail* CTestDrive::CreateMail(void){
	//CoInitialize(NULL);
	ITDMail*	pMail = new CMail;
	//CoUninitialize();
	return pMail;
}

ITDMovie* CTestDrive::CreateMovieInterface(void){
	CAVIGenerator* pAviGenerator = new CAVIGenerator;
	return pAviGenerator;
}

ITDSerialCom* CTestDrive::CreateSerialComInterface(void){
	return new CSerialPort;
}

PVOID CTestDrive::CreateInterface(TD_INTERFACE td_inf){
	switch(td_inf){
	case TD_INTERFACE_MAIL:		return new CMail;
	case TD_INTERFACE_MOVIE:	return new CAVIGenerator;
	case TD_INTERFACE_SERIAL:	return new CSerialPort;
	}
	return NULL;
}

BOOL CTestDrive::VerilatorCompile(LPCTSTR lpszFileName, LPCTSTR lpszWorkDir, LPCTSTR lpszOption){
	CString	sRunPath;
	CString sWorkPath(RetrieveFullPath(lpszWorkDir));
	CString sArgs(_T("-sv -cc --compiler gcc "));

	sRunPath.Format(_T("\"%sbin\\verilator.exe\""), InstalledPath());
	if(lpszOption){
		sArgs.Append(_T(" "));
		sArgs.Append(lpszOption);
	}
	sArgs.Append(_T(" "));
	sArgs.Append(lpszFileName);

	{
		if(ExecuteFile(	sRunPath, sArgs, TRUE, NULL, sWorkPath,
						_T("Error:"), -1,
						NULL) == -1)
			return FALSE;
	}

	return TRUE;
}

BOOL CTestDrive::GTKWave(LPCTSTR lpszCommandLine, LPCTSTR lpszWorkDir){
	CString	sRunPath;
	CString sWorkPath(RetrieveFullPath(lpszWorkDir));
	BOOL bRet = FALSE;

	sRunPath.Format(_T("\"%sbin\\gtkwave.exe\""), InstalledPath());

	ShellExecute(NULL, _T("open"), sRunPath, lpszCommandLine, sWorkPath, SW_SHOWMAXIMIZED);

	return bRet;
}

BOOL CTestDrive::CppCheck(LPCTSTR lpszPath, LPCTSTR lpszArgs){
	CString	sRunPath;
	CString sArgs;
	
	sRunPath.Format(_T("\"%sbin\\msys64\\mingw64\\bin\\cppcheck.exe\""), InstalledPath());
	{	// full path 지정
		sArgs.Format(_T("\"%s\""), RetrieveFullPath(lpszPath));
		if(lpszArgs) sArgs.AppendFormat(_T(" %s"), lpszArgs);
	}
	{
		if(ExecuteFile(	sRunPath, sArgs, TRUE, NULL, InstalledPath(),
			_T(": (error)"), -1,
			NULL) == -1)
			return FALSE;
	}
	return TRUE;
}

#include "MainFrm.h"
void CTestDrive::PeekAndPumpMessage(void){
	MSG Msg;

	// mfc 용
	while(::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE)) {
	//while(::PeekMessage(&Msg, AfxGetApp()->GetMainWnd()->m_hWnd, 0, 0, PM_NOREMOVE)) {
		if(!g_pMainFrame->PreTranslateMessage(&Msg)) {
			//::PostQuitMessage(0);
			return;
		}
	}
	LONG idle = 0;
	while(AfxGetApp()->OnIdle(idle++));

/*	// win32 용
	while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE)) 
	{
	
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
	}*/
}

LPCTSTR CTestDrive::GetProjectPath(void){
	return ProjectPath();
}

void CTestDrive::Speak(LPCTSTR msg, DWORD flags){
	m_Speech.Speak(msg, flags);
}

ITDSemaphore* CTestDrive::CreateSemaphore(int iInitValue){
	return new TDSemaphore(iInitValue);
}

void CTestDrive::SendCommand(ITDDocument* pDoc, DWORD command, WPARAM wParam, LPARAM lParam){
	if(!pDoc){
		AfxGetMainWnd()->PostMessageW(command, wParam, lParam);
	}else{
		CDocumentView* pViewDoc = (CDocumentView*)pDoc;
		USER_COMMAND	user_command = {command, wParam, lParam};
		pViewDoc->Lock();
		pViewDoc->SendMessage(WM_USER_COMMAND, NULL, (LPARAM)&user_command);
		pViewDoc->UnLock();
	}
}

void CTestDrive::SetForeground(void){
	AfxGetMainWnd()->SetForegroundWindow();
}

LPCTSTR CTestDrive::RetrieveFullPath(LPCTSTR szPath){
	static CFullPath	sFullPath;
	if(!szPath){
		TCHAR sDir[1024];
		GetCurrentDirectory(1024, sDir);
		sFullPath.SetPath(sDir);
	}else{
		sFullPath.SetPath(szPath);
	}
	return sFullPath.Path();
}

void CTestDrive::TryQuit(void){
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}

BOOL CTestDrive::InsertCodeInception(LPCTSTR lpszTargetFile, LPCTSTR lpszInceptionFile, LPCTSTR lpszTitle, LPCTSTR lpszAuthor, LPCTSTR lpszVersion){
	CString	sExecutePath;
	CString args;

	if(!lpszInceptionFile || !lpszTargetFile) return FALSE;

	sExecutePath.Format(_T("\"%sbin\\CheckInception.exe\""), InstalledPath());

	args.Format(_T("-inception \"%s\" "), RetrieveFullPath(lpszInceptionFile));
	if(lpszTitle) args.AppendFormat(_T("-title \"%s\" "), lpszTitle);
	if(lpszAuthor) args.AppendFormat(_T("-author \"%s\" "), lpszAuthor);
	if(lpszVersion) args.AppendFormat(_T("-version \"%s\" "), lpszVersion);
	args.AppendFormat(_T("\"%s\""), RetrieveFullPath(lpszTargetFile));

	if(ExecuteFile(sExecutePath, args, TRUE, NULL, m_sProjectDir,
		_T("*E"), -1,
		NULL)<0) return FALSE;
	return TRUE;
}

BOOL CTestDrive::ReplaceText(LPCTSTR lpszTargetFile, LPCTSTR lpszReplaceDescFile){
	CString	sExecutePath;
	CString args;
	CString sTargetFile(RetrieveFullPath(lpszTargetFile));
	CString sReplaceDescFile(RetrieveFullPath(lpszReplaceDescFile));

	sExecutePath.Format(_T("\"%sbin\\TextReplace.exe\""), InstalledPath());

	args.Format(_T("\"%s\" \"%s\""), sReplaceDescFile, sTargetFile);
	if(ExecuteFile(sExecutePath, args, TRUE, NULL,
		_T("*E"), -1,
		NULL)<0) return FALSE;
	return TRUE;
}

BOOL CTestDrive::SearchSubPathFile(LPCTSTR sSearchPath, LPCTSTR sFileName, SEARCH_FILE_FUNCTION SearchFunc, LPVOID pData){
	CFullPath full_path(sSearchPath);
	{	// no search file definition
		CString sPath;
		sPath.Format(_T("%s\\.TestDrive.nosearch"), full_path.Path());
		if(_taccess(sPath, 0)  != -1) return TRUE;
	}
	{	// search sub-folder tree
		HANDLE				hFind;
		WIN32_FIND_DATA		FindFileData;
		CString				sPath;
		sPath.Format(_T("%s\\*.*"), full_path.Path());
		hFind				= FindFirstFile(sPath, &FindFileData);
		if(hFind != INVALID_HANDLE_VALUE){
			do {
				if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && FindFileData.cFileName[0]!=_T('.')) {
					CString	sNextSearchPath;
					sNextSearchPath.Format(_T("%s\\%s"), full_path.Path(), FindFileData.cFileName);
					if(!SearchSubPathFile(sNextSearchPath, sFileName, SearchFunc, pData)){
						FindClose(hFind);
						return FALSE;
					}
				}
			} while(FindNextFile(hFind, &FindFileData));
			FindClose(hFind);
		}
	}
	{
		// search file
		HANDLE				hFind;
		WIN32_FIND_DATA		FindFileData;
		{
			CString			sPath;
			sPath.Format(_T("%s\\%s"), full_path.Path(), sFileName);
			hFind				= FindFirstFile(sPath, &FindFileData);
		}
		if(hFind != INVALID_HANDLE_VALUE){
			do {
				if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					CString	sPath;
					sPath.Format(_T("%s\\%s"), full_path, FindFileData.cFileName);
					if(!SearchFunc(sPath, pData)){
						FindClose(hFind);
						return FALSE;
					}
				}
			} while(FindNextFile(hFind, &FindFileData));
			FindClose(hFind);
		}
	}
	return TRUE;
}

//--------------------------------------------------------------------------------------------------------
// lua script engine
//--------------------------------------------------------------------------------------------------------

/*
void TB_LUA_Alert(const char* err){
	g_pTestDrive->LogError(_T("%s"), CString(err));
}

void TB_LUA_SetForeground(void){
	g_pTestDrive->SetForeground();
}

void TB_LUA_LogInfo(const char* msg){
	g_pTestDrive->LogInfo(_T("%s"), CString(msg));
}

void TB_LUA_LogWarning(const char* msg){
	g_pTestDrive->LogWarning(_T("%s"), CString(msg));
}

void TB_LUA_LogError(const char* msg){
	g_pTestDrive->LogError(_T("%s"), CString(msg));
}

void TB_LUA_system_clear(void){
	g_pTestDrive->ClearLog();
}

void TB_LUA_system_title(const char* title){
	CString title_name(title);
	g_pTestDrive->SetTitle(ITDSystem::TITLE_MAIN, title_name);
	g_pTestDrive->m_pMsg->LogOut(CString(_S(MAIN_TITLE)) + _T(" : "), RGB(0,0,255));
	g_pTestDrive->m_pMsg->LogOut(_TEXT_(_T("\"%s\"\n"), title_name), RGB(0,0,255), CFE_BOLD);
	g_pTestDrive->UpdateTite();
}

void TB_LUA_system_subtitle(const char* title){
	CString title_name(title);
	g_pTestDrive->SetTitle(ITDSystem::TITLE_WORK, title_name);
	//SetTitle(TITLE_MAIN, temp);
	g_pTestDrive->m_pMsg->LogOut(CString(_S(MAIN_TITLE)) + _T(" : "), RGB(0,0,255));
	g_pTestDrive->m_pMsg->LogOut(_TEXT_(_T("\"%s\"\n"), title_name), RGB(0,0,255), CFE_BOLD);
	g_pTestDrive->UpdateTite();
}

void TB_LUA_system_call(const char* command, const char* work_path = NULL, const char* argment = NULL, const char* error_token = NULL){
	BOOL		bError = FALSE;
	CString		sCommand(command);
	CString		sArg(argment);
	CString		sWorkPath(work_path);
	if(!work_path){
		TCHAR	FullPath[1024], *pFilePart;
		//if(!command) // error
		GetFullPathName(sCommand, 1024, FullPath, &pFilePart);
		//if(!pFilePart) // error
		sCommand	= pFilePart;
		*pFilePart	= NULL;
		sWorkPath	= FullPath;
	}
	
	{	// run batch file
		CRedirectExecute* pExec = new CRedirectExecute(sCommand, g_pTestDrive->m_pMsg, argment ? sArg : NULL, sWorkPath);
		// 에러 문자열 지정
		if(error_token){
			pExec->AddErrorToken(CString(error_token), -1);
		}
		pExec->AddErrorToken(_T("##"), 0);
		if(!pExec->Run()) {
			SAFE_DELETE(pExec);
			// goto ERROR_OCCUR;	// error
			return;
		}
		bError = pExec->GetErrorCode();
		SAFE_DELETE(pExec);

		if(bError){
			g_pTestDrive->LogOut(_S(ERROR_IN_PROGRESS), ITDSystem::SYSMSG_ERROR);
			// do something for error...
		}
	}
}

void TB_LUA_system_shell(const char* file_name, const char* argument){
	CString sPath(file_name);
	CString sArg(argument);
	//if(!file_name) goto ERROR_OCCUR;	// error
	{
		TCHAR cur_path[MAX_PATH], *pFile, sArg[MAX_PATH];
		GetFullPathName(sPath, MAX_PATH, cur_path, &pFile);
		*(pFile-1) = NULL;
		ShellExecute(NULL, _T("open"), pFile, argument ? sArg : NULL, cur_path, SW_SHOWNORMAL);
	}
}

void TB_LUA_memory_create(unsigned int mem_size, const char* name){
	//if(mem_size<0) // error
	CString	mem_name(name);
	if(!name){
		mem_name = MMFileName;
	}
	g_pTestDrive->m_pMsg->LogOut(_S(CREATE_MEMORY_MODEL), RGB(0,0,255));
	g_pTestDrive->m_pMsg->LogOut(_TEXT_(_T("'%s' : %d Bytes "), mem_name, mem_size), RGB(0,0,255), CFE_BOLD);
	if((mem_size/0x100000))		g_pTestDrive->m_pMsg->LogOut(_TEXT_(_T("(%.1fMB)\n"), (float)mem_size/0x100000), RGB(0,0,255), CFE_BOLD);
	else if((mem_size/0x400))	g_pTestDrive->m_pMsg->LogOut(_TEXT_(_T("(%.1fKB)\n"), (float)mem_size/0x400), RGB(0,0,255), CFE_BOLD);
	else g_pTestDrive->m_pMsg->LogOut(_T("\n"));
	if(mem_size >= 0x40000000){
		g_pTestDrive->LogOut(_S(TOO_BIG_MEMORY_REQUEST), ITDSystem::SYSMSG_WARNING);
		// error
	}
	if(CMemory::Find(mem_name)){
		g_pTestDrive->LogOut(_S(ALREADY_MEMORY_EXIST), ITDSystem::SYSMSG_WARNING);
		g_pTestDrive->LogError(_T("%s : %s"), _S(ALREADY_MEMORY_EXIST), mem_name);
		// goto ERROR_OCCUR;
		// error
	}
	{
		CMemory* pMemory = new CMemory;
		if(!pMemory->Create(mem_size, mem_name)){
			delete pMemory;
			//goto ERROR_OCCUR;
			// error
		}
	}
}

BOOL TB_LUA_file_exist(const char* file_name){
	return PathFileExists(CString(file_name));
}

void TB_LUA_profile_set(TESTDRIVE_PROFILE id, LPCTSTR sFileName){
	if(!g_pTestDrive->SetProfilePath(id, sFileName)){
		g_pTestDrive->LogOut(_TEXT_(CString(_S(INVALID_FILE_PATH)) + _T(" : %s"), sFileName), ITDSystem::SYSMSG_ERROR);
	}
}
void TB_LUA_profile_set_initialize(const char* file_name){
	TB_LUA_profile_set(TESTDRIVE_PROFILE_INITIALIZE, file_name ? CString(file_name) : NULL);
}

void TB_LUA_profile_set_check(const char* file_name){
	TB_LUA_profile_set(TESTDRIVE_PROFILE_CHECK, file_name ? CString(file_name) : NULL);
}

void TB_LUA_profile_set_cleanup(const char* file_name){
	TB_LUA_profile_set(TESTDRIVE_PROFILE_CLEANUP, file_name ? CString(file_name) : NULL);
}

void TB_LUA_profile_set_sw_compile(const char* file_name){
	TB_LUA_profile_set(TESTDRIVE_PROFILE_SW_COMPILE, file_name ? CString(file_name) : NULL);
}

void TB_LUA_profile_set_hw_compile(const char* file_name){
	TB_LUA_profile_set(TESTDRIVE_PROFILE_HW_COMPILE, file_name ? CString(file_name) : NULL);
}

void TB_LUA_profile_call(const char* file_name, const char* log_name = NULL){
	TCHAR cur_path[MAX_PATH];
	BOOL bError = FALSE;
	COutput* pNewMsg = g_pTestDrive->m_pMsg;

	//if(!file_name) // error
	// 현재 폴더위치 저장
	GetCurrentDirectory(MAX_PATH, cur_path);

	if(log_name){
		// 로그 이름 얻기
		if(!strcmp(log_name,"sys"))			pNewMsg = &g_Output[COutput::TD_OUTPUT_SYSTEM];
		else if(!strcmp(log_name,"app"))	pNewMsg = &g_Output[COutput::TD_OUTPUT_APPLICATION];
		else goto ERROR_OCCUR;
	}
	// run profile
	if(!g_pTestDrive->Build(CString(file_name), pNewMsg, TRUE)) goto ERROR_OCCUR;
ERROR_OCCUR:
	// 현재 폴더 위치 복구
	SetCurrentDirectory(cur_path);

	if(bError){
		g_pTestDrive->LogOut(_S(ERROR_IN_PROGRESS), ITDSystem::SYSMSG_ERROR);
		//goto ERROR_OCCUR;
	}
}*/

/*
BOOL CTestDrive::Build(LPCTSTR szFileName, COutput* pMsg, BOOL bThreaded)
{
	CString		sFullPath;
	COutput*	pPrevMsg	= m_pMsg;
	LPCTSTR		sErrorMsg	= _S(ERROR_IN_PROGRESS);

	{	//// check environment
		// IDS_TESTDRIVE_TOO_MANY_RECURSIVE_CALL : eliminated
		// present project check
		if(!IsProjectOpen()){
			LogOut(_S(OPEN_PROJECT_FIRST), SYSMSG_ERROR);	// _T("First, open your project!")
			return FALSE;
		}

		// another process check
		if(!bThreaded)
		if(IsBusy()){
			LogOut(_S(ANOTHER_PROFILE_IS_RUNNING), SYSMSG_ERROR);	// _T("Now, another profile is running!")
			return FALSE;
		}

		// check file name
		if(!szFileName || !*szFileName) {
			LogOut(_S(NO_PROFILE_NAME), SYSMSG_ERROR); // _T("Profile name is not assigned!")
			return FALSE;
		}

		// set current message
		if(pMsg) m_pMsg = pMsg;
	}
	lua_State* L = NULL;

	
	// Set current directory
	{
		TCHAR path[MAX_PATH], *pFilePart;

		if(!GetFullPathName(szFileName, MAX_PATH, path, &pFilePart)){
			LogOut(_TEXT_(CString(_S(INVALID_FILE_PATH)) + _T(" : \"%s\""), szFileName), SYSMSG_ERROR);
			return FALSE;
		}
		sFullPath	= path;

		// Paser the profile
// 		if(!paser.Create(sFullPath)){
// 			LogOut(_TEXT_(CString(_S(CANT_OPEN_PROFILE)) + _T(" : \"%s\""), sFullPath), SYSMSG_ERROR); // Can't open the profile
// 			return FALSE;
// 		}

		*pFilePart	= NULL;
		if(bThreaded) SetCurrentDirectory(path);
		else m_bRunning = TRUE;
	}
	// 루아 실행
	{
		L = lua_open();
		// Lua 기본 함수들을 로드한다.- print() 사용
		luaopen_base(L);
		// Lua 문자열 함수들을 로드한다.- string 사용
		luaopen_string(L);

		{
			lua_tinker::def(L, "_ALERT", TB_LUA_Alert);
			lua_tinker::def(L, "LogInfo", TB_LUA_LogInfo);
			lua_tinker::def(L, "LogWarning", TB_LUA_LogWarning);
			lua_tinker::def(L, "LogError", TB_LUA_LogError);

			lua_tinker::def(L, "system_clear", TB_LUA_system_clear);
			lua_tinker::def(L, "system_title", TB_LUA_system_title);
			lua_tinker::def(L, "system_subtitle", TB_LUA_system_subtitle);
			lua_tinker::def(L, "system_call", TB_LUA_system_call);
			lua_tinker::def(L, "system_shell", TB_LUA_system_shell);
			//lua_tinker::def(L, "system_application", TB_LUA_system_application);	// 삭제함 : 더이상 사용하지 않음.
// 				case CMD_SYSTEM_APPLICATION:
// 						if(!paser.GetTokenString(token)) g_Application.Release(TRUE);
// 						else{
// 							if(!g_Application.Create(token)){
// 								LogOut(_TEXT_(CString(_S(APPLICATION_CREATE_FAILED)) + _T(" : %s"), token), SYSMSG_ERROR);
// 								goto ERROR_OCCUR;
// 							}
// 						}break;

			lua_tinker::def(L, "memory_create", TB_LUA_memory_create);

			lua_tinker::def(L, "file_exist", TB_LUA_file_exist);

			lua_tinker::def(L, "profile_set_initialize",	TB_LUA_profile_set_initialize);
			lua_tinker::def(L, "profile_set_check",			TB_LUA_profile_set_check);
			lua_tinker::def(L, "profile_set_cleanup",		TB_LUA_profile_set_cleanup);
			lua_tinker::def(L, "profile_set_sw_compile",	TB_LUA_profile_set_sw_compile);
			lua_tinker::def(L, "profile_set_hw_compile",	TB_LUA_profile_set_hw_compile);
			lua_tinker::def(L, "profile_call",				TB_LUA_profile_call);
		}
		lua_tinker::dofile(L, CStringA(sFullPath));
		lua_close(L);
	}

	m_pMsg	= pPrevMsg;

	if(bThreaded){
		m_bRunning	= FALSE;
		SetProjectDirectroyToCurrent();
	}
	return TRUE;

// ERROR_OCCUR:
// 
// 	m_bRunning	= FALSE;
// 	SetProjectDirectroyToCurrent();
// 
// 	LogOut(GetResourceString(ErrorCode), SYSMSG_ERROR);
// 
// 	return FALSE;
}*/

BOOL CTestDrive::Build(LPCTSTR szFileName, COutput* pMsg, BOOL bThreaded)
{
	CString		sFullPath;
	static DWORD RefCount = 0;
	int i;
	TCHAR token[MAX_PATH], temp[MAX_PATH];
	BOOL bTest = TRUE;
	LPCTSTR		sErrorMsg	= _S(ERROR_IN_PROGRESS);

	CPaser paser;

	// Reference count check
	if(RefCount>50){
		LogOut(_S(TOO_MANY_RECURSIVE_CALL), SYSMSG_ERROR);	// _T("Too many recursive call!")
		return FALSE;
	}

	// present project check
	if(!IsProjectOpen()){
		LogOut(_S(OPEN_PROJECT_FIRST), SYSMSG_ERROR);	// _T("First, open your project!")
		return FALSE;
	}

	// another process check
	if(!bThreaded)
		if(IsBusy()){
			LogOut(_S(ANOTHER_PROFILE_IS_RUNNING), SYSMSG_ERROR);	// _T("Now, another profile is running!")
			return FALSE;
		}

		// check file name
		if(!szFileName) {
NO_PROFILE_NAME:
			LogOut(_S(NO_PROFILE_NAME), SYSMSG_ERROR); // _T("Profile name is not assigned!")
			return FALSE;
		}
		if(!*szFileName) goto NO_PROFILE_NAME;

		// provide default message out interface
		if(!pMsg) pMsg = &g_Output[COutput::TD_OUTPUT_SYSTEM];

		// Set current directory
		{
			CFullPath	full_path(szFileName);
			TCHAR path[MAX_PATH], *pFilePart;

			if(!GetFullPathName(full_path.Path(), MAX_PATH, path, &pFilePart)){
				LogOut(_TEXT_(CString(_S(INVALID_FILE_PATH)) + _T(" : \"%s\""), full_path.Path()), SYSMSG_ERROR);
				return FALSE;
			}
			sFullPath	= path;

			// Paser the profile
			if(!paser.Create(sFullPath)){
				LogOut(_TEXT_(CString(_S(CANT_OPEN_PROFILE)) + _T(" : \"%s\""), sFullPath), SYSMSG_ERROR); // Can't open the profile
				return FALSE;
			}

			*pFilePart	= NULL;
			SetCurrentDirectory(path);
		}

		// reset
		RefCount++;
		m_bRunning	= TRUE;

		while(paser.NewLine() && m_bRunning){
			//LogOut(paser.GetCurLine());	// test
			if(!paser.GetTokenName(token)) continue;

			if(!paser.TokenOut(TD_DELIMITER_PERIOD)) goto ERROR_OCCUR;

			i = CheckCommand(token, g_sCmd, CMD_SIZE);
			if(!bTest && i!=CMD_IF) continue;

			switch(i){
			case CMD_SYSTEM:
				{
					if(!paser.GetTokenName(token)) goto ERROR_OCCUR;
					i = CheckCommand(token, g_sCmdSystem, CMD_SYSTEM_SIZE);

					switch(i){
					case CMD_SYSTEM_TITLE:
						if(!paser.GetTokenString(temp)) goto ERROR_OCCUR;
						SetTitle(TITLE_MAIN, temp);
						pMsg->LogOut(CString(_S(MAIN_TITLE)) + _T(" : "), RGB(0,0,255));
						pMsg->LogOut(_TEXT_(_T("\"%s\"\n"), temp), RGB(0,0,255), CFE_BOLD);
						break;
					case CMD_SYSTEM_SUBTITLE:
						if(!paser.GetTokenString(temp)) goto ERROR_OCCUR;
						SetTitle(TITLE_WORK, temp);
						pMsg->LogOut(CString(_S(SUB_TITLE)) + _T(" : "), RGB(0,0,255));
						pMsg->LogOut(_TEXT_(_T("\"%s\"\n"), temp), RGB(0,0,255), CFE_BOLD);
						break;
					case CMD_SYSTEM_SPLASH:
						if(!paser.GetTokenString(temp)) goto ERROR_OCCUR;
						{
							int iDelayTime	= 2000;
							int iVelocity	= 15;
							if(paser.TokenOut(TD_DELIMITER_COMMA)){
								if(!paser.GetTokenInt(&iDelayTime)) goto ERROR_OCCUR;
								if(paser.TokenOut(TD_DELIMITER_COMMA)){
									if(!paser.GetTokenInt(&iVelocity)) goto ERROR_OCCUR;
									if(iVelocity<=0) iVelocity = 1;
								}
							}
							CSplashScreenEx *pSplash=new CSplashScreenEx();
							if(pSplash->Create(AfxGetMainWnd()->m_hWnd, temp)){
								pSplash->Start(iDelayTime, iVelocity);
							}else{
								delete pSplash;
								pMsg->LogOut(CString(_S(LOAD_FILE_IS_FAILED)) + _T(" : "), RGB(255,0,0));
								pMsg->LogOut(_TEXT_(_T("\"%s\"\n"), temp), RGB(255,0,0), CFE_BOLD);
							}
						}
						break;
					case CMD_SYSTEM_CLEAR:
						for(i=0;i<COutput::TD_OUTPUT_SIZE;i++) g_Output[i].Clear();
						break;
					case CMD_SYSTEM_CALL:
						{
							TCHAR sCommand[1024];
							TCHAR sWorkPath[1024];
							TCHAR sArg[1024];
							BOOL bError = FALSE;

							// 파일 이름 얻기
							if(!paser.GetTokenString(sCommand)){
								goto ERROR_OCCUR;
							}
							// argument 얻기
							if(paser.TokenOut(TD_DELIMITER_COMMA)){
								if(!paser.GetTokenString(sArg)) goto ERROR_OCCUR;
							}else *sArg = NULL;
							// work dir 얻기
							if(paser.TokenOut(TD_DELIMITER_COMMA)){
								if(!paser.GetTokenString(sWorkPath)) goto ERROR_OCCUR;
								CFullPath	full_path(sWorkPath);
								_tcscpy(sWorkPath, full_path.Path());
							}else *sWorkPath = NULL;
							// run batch file
							{
								CRedirectExecute* pExec = new CRedirectExecute(sCommand, pMsg, *sArg ? sArg : NULL, *sWorkPath ? sWorkPath : NULL);
								// 에러 문자열 지정
								while(paser.TokenOut(TD_DELIMITER_COMMA)){
									int error_code = -1;
									if(!paser.GetTokenString(temp)){
										SAFE_DELETE(pExec);
										goto ERROR_OCCUR;
									}
									if(paser.TokenOut(TD_DELIMITER_COLON)){
										TCHAR sCode[MAX_PATH];
										switch (paser.GetToken(sCode)) {
										case TD_TOKEN_INT:
											error_code	= StrToInt(temp);
											break;
										case TD_TOKEN_NAME:
											if (!_tcscmp(sCode, _T("W"))) {
												error_code = INT_MAX;
											} else goto ERROR_OCCUR;
											break;
										default:
											goto ERROR_OCCUR;
										}
									}
									pExec->AddErrorToken(temp, error_code);
								}
								pExec->AddErrorToken(_T("##"), 0);
								if(!pExec->Run()) {
									SAFE_DELETE(pExec);
									goto ERROR_OCCUR;
								}
								bError = (pExec->GetErrorCode() < 0);
								SAFE_DELETE(pExec);
							}

							if(bError){
								LogOut(_S(ERROR_IN_PROGRESS), SYSMSG_ERROR);
								goto ERROR_OCCUR;
							}
						}break;
					case CMD_SYSTEM_SHELL:
						{
							// 파일 이름 얻기
							if(!paser.GetTokenString(temp)) goto ERROR_OCCUR;
							{
								TCHAR cur_path[MAX_PATH], *pFile, sArg[MAX_PATH];
								CFullPath full_path(temp);
								GetFullPathName(full_path.Path(), MAX_PATH, cur_path, &pFile);
								if(!pFile) goto ERROR_OCCUR;
								*(pFile-1) = NULL;
								*sArg	= NULL;
								if(paser.TokenOut(TD_DELIMITER_COMMA)){
									if(!paser.GetTokenString(sArg)) goto ERROR_OCCUR;
								}
								ShellExecute(NULL, _T("open"), pFile, sArg, cur_path, SW_SHOWNORMAL);
							}
						}
						break;
					case CMD_SYSTEM_LUA:
						{
							// 파일 이름 얻기
							if(!paser.GetTokenString(temp)) goto ERROR_OCCUR;
							CFullPath full_path(temp);
							m_Lua.Run(CStringA(full_path.Path()));
						}
						break;
					case CMD_SYSTEM_DOCUMENT:
						if(!paser.TokenOut(TD_DELIMITER_PERIOD)) goto ERROR_OCCUR;
						if(!paser.GetTokenName(token)) goto ERROR_OCCUR;
						i = CheckCommand(token, g_sCmdSystemDocument, CMD_SYSTEM_VIEW_SIZE);
						if(i>=CMD_SYSTEM_VIEW_SIZE) goto ERROR_OCCUR;

						switch(i){
						case CMD_SYSTEM_DOCUMENT_ADD:
							{
								if(!paser.GetTokenString(token)) goto ERROR_OCCUR;
								paser.SetMultilineTokenable(TRUE);
								{
									// 이미 존재하는 도큐먼트일 경우 활성화 한다.
									CDocumentView* pVDoc = g_pDocumentList->Find(token);
									if(pVDoc){
										if(!pVDoc->IsVisible()){
											LogInfo(_S(DOCUEMT_IS_HIDE), pVDoc->DocumentTitle());
										}else{

											g_pDocumentList->SetActive(pVDoc);
										}
										goto QUICK_END;
									}
								}
								if(!g_pDocumentList->Add(token, &paser)){
									sErrorMsg = _S(DOCUEMT_DENIED);
									goto ERROR_OCCUR;
								}
								paser.SetMultilineTokenable(FALSE);

							}break;
						case CMD_SYSTEM_DOCUMENT_CLOSE:
							if(!paser.IsTokenable()){
								g_pDocumentList->RemoveAll();
							}else{
								if(!paser.GetTokenString(token)) goto ERROR_OCCUR;
								g_pDocumentList->Remove(token);
							}break;
						case CMD_SYSTEM_DOCUMENT_CALL:
							{
								ITDImplDocument* pDocImp;

								{	// 구현 찾기
									CDocumentView* pDoc;
									if(paser.GetTokenString(token)){
										pDoc = g_pDocumentList->Find(token);
										if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
									}
									if(!pDoc) break;
									pDocImp = pDoc->GetImplementation();
									if(!pDocImp) break;
								}
								{
									int iCommand;
									TCHAR sArg0[MAX_PATH], sArg1[MAX_PATH];
									*sArg0	= NULL;
									*sArg1	= NULL;

									paser.GetTokenInt(&iCommand);

									if(paser.TokenOut(TD_DELIMITER_COMMA)){
										if(!paser.GetTokenString(sArg0)) goto ERROR_OCCUR;
										if(paser.TokenOut(TD_DELIMITER_COMMA))
											if(!paser.GetTokenString(sArg1)) goto ERROR_OCCUR;
									}
									pDocImp->OnCommand(iCommand, (WPARAM)sArg0, (LPARAM)sArg1);
								}
							}break;
						default:
							goto ERROR_OCCUR;
						}
						break;
					case CMD_SYSTEM_MSG:
						if(!paser.GetTokenString(temp)) goto ERROR_OCCUR;
						pMsg->LogOut(temp, RGB(0,0,255));
						break;
					case CMD_SYSTEM_ERROR:
						if(!paser.GetTokenString(temp)) goto ERROR_OCCUR;
						pMsg->LogOut(_TEXT_(CString(_S(ASERTED_BY_USER)) + _T(" : %s\n"), temp), RGB(255,0,0));	// Assert error by user
					default:
						goto ERROR_OCCUR;
					}
				}break;
			case CMD_MEMORY:
				{
					if(!paser.GetTokenName(token)) goto ERROR_OCCUR;
					i = CheckCommand(token, g_sCmdMemory, CMD_MEMORY_SIZE);
					switch(i){
					case CMD_MEMORY_CREATE:
						{
							uint64_t mem_size;
							TCHAR mem_name[MAX_PATH];
							*mem_name = 0;
							if(!paser.GetTokenInt64((int64_t*)&mem_size)) goto ERROR_OCCUR;
							if(paser.TokenOut(TD_DELIMITER_COMMA)){
								if(!paser.GetTokenString(mem_name)) goto ERROR_OCCUR;
							}

							{
								CMemory* pMemory = CMemory::Find(mem_name);
								if(pMemory){
									if(pMemory->GetSize() != mem_size){	// 메모리가 존재하면서 사이즈가 다를 때...
										LogOut(_S(ALREADY_MEMORY_EXIST), SYSMSG_WARNING);
										LogError(_T("%s : %s"), _S(ALREADY_MEMORY_EXIST), mem_name);
										goto ERROR_OCCUR;
									}else{
										break;
									}
								}
							}

							if(!*mem_name) _tcscpy(mem_name, MMFileName);

							pMsg->LogOut(_S(CREATE_MEMORY_MODEL), RGB(0,0,255));
							{
								CString sBytes;	// 1000 자리 구분자 ',' 추가
								sBytes.Format(_T("%lld"), mem_size);
								for (int i = sBytes.GetLength() - 3; i > 0; i -= 3) {
									sBytes.Insert(i, _T(','));
								}
								pMsg->LogOut(_TEXT_(_T("'%s' : %s Bytes "), mem_name, (LPCTSTR)sBytes), RGB(0, 0, 255), CFE_BOLD);
							}

							if((mem_size/0x100000))		pMsg->LogOut(_TEXT_(_T("(%.1fMB)\n"), (float)mem_size/0x100000), RGB(0,0,255), CFE_BOLD);
							else if((mem_size/0x400))	pMsg->LogOut(_TEXT_(_T("(%.1fKB)\n"), (float)mem_size/0x400), RGB(0,0,255), CFE_BOLD);
							else pMsg->LogOut(_T("\n"));

							{
								CMemory* pMemory = new CMemory;
								if(!pMemory->Create(mem_size, mem_name)){
									delete pMemory;
									goto ERROR_OCCUR;
								}
							}
						}break;
					case CMD_MEMORY_LOAD:
						{
							if(!paser.TokenOut(TD_DELIMITER_PERIOD)) goto ERROR_OCCUR;
							if(!paser.GetTokenName(token)) goto ERROR_OCCUR;
							i = CheckCommand(token, g_sCmdMemoryLoadStore, CMD_MEMORY_LOAD_SIZE);

							if(i>=CMD_MEMORY_LOAD_SIZE) goto ERROR_OCCUR;

							if(i==CMD_MEMORY_LOAD_IMAGE){	// load image
								TCHAR filename[MAX_PATH];
								DWORD offset, stride = 0;
								if(!paser.GetTokenString(filename)) goto ERROR_OCCUR;
								if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
								if(!paser.GetTokenInt((int*)&offset)) goto ERROR_OCCUR;
								if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
								if(!paser.GetTokenName(token)) goto ERROR_OCCUR;
								if(paser.TokenOut(TD_DELIMITER_COMMA)){
									if(!paser.GetTokenInt((int*)&stride)) goto ERROR_OCCUR;
								}
								i = CheckCommand(token, g_sColorTypes, COLORTYPE_SIZE);
								if(i>=COLORTYPE_SIZE){
									LogOut(_S(INVALID_COLOR_TYPE), SYSMSG_ERROR);
									goto ERROR_OCCUR;
								}
								{
									CMemory* pMemory = CMemory::Find(NULL);
									if(!pMemory) {
										LogError(_S(NONE_MEMORY_MODEL));
										goto ERROR_OCCUR;
									}
									if(!pMemory->LoadImage(filename, offset, (COLORFORMAT)i, stride)) goto ERROR_OCCUR;
								}

								pMsg->LogOut(_TEXT_(CString(_S(LOAD_MEM_FROM_IMAGE)) + _T(" : \"%s\" (offset : %d (0x%X), color type : %s, stride : %d)\n"), filename, offset, offset, g_sColorTypes[i], stride), RGB(0,0,255));	// Load image to memory
							}else{							// load data
								TCHAR filename[MAX_PATH];
								DWORD offset;
								if(!paser.GetTokenString(filename)) goto ERROR_OCCUR;
								if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
								if(!paser.GetTokenInt((int*)&offset)) goto ERROR_OCCUR;
								pMsg->LogOut(_TEXT_(CString(_S(LOAD_MEM_FROM_FILE)) + _T(" : \"%s\" (offset : %d (0x%X))\n"), filename, offset, offset), RGB(0,0,255)); // Load memory from source
								{
									CMemory* pMemory = CMemory::Find(NULL);
									if(!pMemory) {
										LogError(_S(NONE_MEMORY_MODEL));
										goto ERROR_OCCUR;
									}
									if(!pMemory->Load((CMemory::MEM_DISC)(i-1), filename, offset)) goto ERROR_OCCUR;
								}
							}
						}break;
					case CMD_MEMORY_STORE:
						{
							if(!paser.TokenOut(TD_DELIMITER_PERIOD)) goto ERROR_OCCUR;
							if(!paser.GetTokenName(token)) goto ERROR_OCCUR;
							i = CheckCommand(token, g_sCmdMemoryLoadStore, CMD_MEMORY_STORE_SIZE);

							if(i>=CMD_MEMORY_STORE_SIZE) goto ERROR_OCCUR;

							if(i==CMD_MEMORY_STORE_IMAGE){
								TCHAR filename[MAX_PATH];
								DWORD offset, width, height, use_alpha = 0, stride = 0;
								if(!paser.GetTokenString(filename)) goto ERROR_OCCUR;
								if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
								if(!paser.GetTokenInt((int*)&offset)) goto ERROR_OCCUR;
								if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
								if(!paser.GetTokenInt((int*)&width)) goto ERROR_OCCUR;
								if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
								if(!paser.GetTokenInt((int*)&height)) goto ERROR_OCCUR;
								if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
								if(!paser.GetTokenName(token)) goto ERROR_OCCUR;
								if(paser.TokenOut(TD_DELIMITER_COMMA)){
									if(!paser.GetTokenInt((int*)&use_alpha)) goto ERROR_OCCUR;
									if(paser.TokenOut(TD_DELIMITER_COMMA)){
										if(!paser.GetTokenInt((int*)&stride)) goto ERROR_OCCUR;
									}
								}

								i = CheckCommand(token, g_sColorTypes, COLORTYPE_SIZE);
								if(i>=COLORTYPE_SIZE){
									LogOut(_S(INVALID_COLOR_TYPE), SYSMSG_ERROR);
									goto ERROR_OCCUR;
								}
								{
									CMemory* pMemory = CMemory::Find(NULL);
									if(!pMemory) {
										LogError(_S(NONE_MEMORY_MODEL));
										goto ERROR_OCCUR;
									}
									if(!pMemory->StoreImage(filename, offset, width, height, (COLORFORMAT)i, use_alpha, stride)) goto ERROR_OCCUR;
								}
								pMsg->LogOut(_TEXT_(CString(_S(STORE_IMAGE_FROM_MEM)) + _T(" : \"%s\" (offset : %d (0x%X), dimmension : %dx%d, color type : %s, stride : %d)\n"), filename, offset, offset, width, height, g_sColorTypes[i], stride), RGB(0,0,255)); // Store image from memory
							}else{
								TCHAR filename[MAX_PATH];
								DWORD offset,size, stride = 0;
								if(!paser.GetTokenString(filename)) goto ERROR_OCCUR;
								if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
								if(!paser.GetTokenInt((int*)&offset)) goto ERROR_OCCUR;
								if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
								if(!paser.GetTokenInt((int*)&size)) goto ERROR_OCCUR;
								if(i == CMemory::MEM_DISC_SIMUL) {
									if(!paser.TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OCCUR;
									if(!paser.GetTokenInt((int*)&stride)) goto ERROR_OCCUR;
									if(stride<=0) goto ERROR_OCCUR;
								}
								pMsg->LogOut(_TEXT_(CString(_S(STORE_FILE_FROM_MEM)) + _T(" : \"%s\" (offset : %d (0x%X), size  : %d (0x%X))"), filename, offset, offset, size, size), RGB(0,0,255));	// Save memory to file
								if(i == CMemory::MEM_DISC_SIMUL) pMsg->LogOut(_TEXT_(_T(", stride : %d"), stride), RGB(0,0,255));
								pMsg->LogOut(_T("\n"));
								{
									CMemory* pMemory = CMemory::Find(NULL);
									if(!pMemory) {
										LogError(_S(NONE_MEMORY_MODEL));
										goto ERROR_OCCUR;
									}
									if(!pMemory->Save((CMemory::MEM_DISC)(i-1), filename, offset, size, stride)) goto ERROR_OCCUR;
								}
							}
						}break;
					default:
						goto ERROR_OCCUR;
					}
				}break;
			case CMD_IF:
				{
					if(!paser.GetTokenName(token)) goto ERROR_OCCUR;
					i = CheckCommand(token, g_sCmdIf, CMD_IF_SIZE);

					switch(i){
					case CMD_IF_FILE:
						{
							TCHAR filename[MAX_PATH];
							if(!paser.GetTokenString(filename)) goto ERROR_OCCUR;
							{
								CFullPath	FilePath(filename);
								bTest = PathFileExists(FilePath.Path());
							}
						}continue;
					case CMD_IF_DOCUMENT:
						{
							TCHAR document[MAX_PATH];
							if(!paser.GetTokenString(document)) goto ERROR_OCCUR;
							bTest = GetDocument(document) != NULL;
						}continue;
					case CMD_IF_LOCALE:
						{
							TCHAR locale[MAX_PATH];
							if(!paser.GetTokenString(locale)) goto ERROR_OCCUR;
							bTest = (g_Localization.CurrentLocale()->dwLangID == _ttoi(locale));
						}continue;
					case CMD_IF_ELSE:
						bTest = !bTest;
						continue;
					case CMD_IF_END:
						bTest = TRUE;
						continue;
					default:
						goto ERROR_OCCUR;
					}
				}break;
			case CMD_PROFILE:
				{
					if(!paser.GetTokenName(token)) goto ERROR_OCCUR;
					i = CheckCommand(token, g_sCmdProfile, CMD_PROFILE_SIZE);

					switch(i){
					case CMD_PROFILE_TREE:
						if(!SetProfileTreeFromPaser(&paser)) goto ERROR_OCCUR;
						break;
					case CMD_PROFILE_CLEAR:
						g_ProfileTree.DeleteAllItems();
						break;
					case CMD_PROFILE_CALL:
						{
							TCHAR cur_path[MAX_PATH];
							BOOL bError = FALSE;

							// 현재 폴더위치 저장
							GetCurrentDirectory(MAX_PATH, cur_path);
							// 파일 이름 얻기
							if(!paser.GetTokenString(temp)) goto ERROR_OCCUR;

							COutput* pNewMsg = pMsg;
							// 로그 이름 얻기
							if(paser.TokenOut(TD_DELIMITER_COMMA)){
								TCHAR logname[128];
								if(!paser.GetTokenName(logname)) goto ERROR_OCCUR;
								_tcslwr(logname);
								if(!_tcscmp(logname,_T("sys")))		pNewMsg = &g_Output[COutput::TD_OUTPUT_SYSTEM];
								else if(!_tcscmp(logname,_T("app")))	pNewMsg = &g_Output[COutput::TD_OUTPUT_APPLICATION];
								else goto ERROR_OCCUR;
							}
							// run profile
							if(!Build(temp,pNewMsg,bThreaded)) goto ERROR_OCCUR;

							// 현재 폴더 위치 복구
							SetCurrentDirectory(cur_path);
							if(bError){
								LogOut(_S(ERROR_IN_PROGRESS), SYSMSG_ERROR);
								goto ERROR_OCCUR;
							}
						}break;
					case CMD_PROFILE_SET:
						{
							if(!paser.TokenOut(TD_DELIMITER_PERIOD)) goto ERROR_OCCUR;
							if(!paser.GetTokenName(token)) goto ERROR_OCCUR;
							i = CheckCommand(token, g_sCmdProfileSet, TESTDRIVE_PROFILE_SIZE);

							if(i >= TESTDRIVE_PROFILE_SIZE) goto ERROR_OCCUR;

							if(!paser.GetTokenString(token)) goto ERROR_OCCUR;

							if(!SetProfilePath((TESTDRIVE_PROFILE)i, token)) {
								LogOut(_TEXT_(CString(_S(INVALID_FILE_PATH)) + _T(" : %s"), token), SYSMSG_ERROR);
								goto ERROR_OCCUR;
							}
						}break;
					case CMD_PROFILE_PATH:
						{
							if(!paser.GetTokenString(token)) goto ERROR_OCCUR;
							{
								TCHAR fullpath[1024];
								GetFullPathName(token, 1024, fullpath, NULL);
								{	// set root profile path
									CString sPath(fullpath);
									sPath.Replace(_T('\\'), _T('/'));
									SetGlobalEnvironmentVariable(_T("TESTDRIVE_PROFILE"), sPath);
								}
								{	// set profile common binary path
									CString sPath;
									sPath.Format(_T("%scommon\\bin\\"), fullpath);
									GetFullPathName(sPath, 1024, fullpath, NULL);
									{
										int iLen = (int)_tcslen(fullpath);
										if(fullpath[iLen-1]==_T('\\'))
											fullpath[iLen-1]	= _T('\0');
									}
									ModifyGlobalEnvironmentPath(fullpath);
								}
							}
						}break;
					default:
						goto ERROR_OCCUR;
					}
				}break;
			default:
				LogOut(_TEXT_(CString(_S(INVALID_COMMAND)) + _T(" : \"%s\""), token));
				goto ERROR_OCCUR;
			}
		}
QUICK_END:
		RefCount--;

		if(!m_bRunning)
			g_Output[COutput::TD_OUTPUT_SYSTEM].LogOut(_TEXT_(CString(_S(ASERTED_BY_USER)) + _T("(\"%s\")\n"), sFullPath), RGB(255,0,0));

		if(!RefCount){
			m_bRunning	= FALSE;
			SetProjectDirectroyToCurrent();
		}
		return TRUE;

ERROR_OCCUR:
		RefCount--;

		if(!RefCount){
			m_bRunning	= FALSE;
			SetProjectDirectroyToCurrent();
		}
		ShowDescriptionCode(&paser);

		{
			SYSMSG_ID	log_id		= SYSMSG_ERROR;
			CString		message		= sErrorMsg;
			if(message.GetAt(0)==_T('$')){	// 경고
				log_id	= SYSMSG_WARNING;
				message = ((LPCTSTR)message + 1);
			}else
			if(message.GetAt(0)==_T('!')){	// 일반 메시지
				log_id	= SYSMSG_INFO;
				message = ((LPCTSTR)message + 1);
			}
			LogOut(message, log_id);
		}

		return FALSE;
}

static LPCTSTR __sAppName		= _T("TESTDRIVE");
static LPCTSTR __sConfigFile	= _T("%stestdrive.ini");
static LPCTSTR __sEnvDelim		= _T("@");

BOOL CTestDrive::GetEnvString(LPCTSTR sKey, CString& sAppName, CString& sKeyName, CString& sEnvPath) {
	if (!sKey) return FALSE;
	CString sCode(sKey);
	sEnvPath.Format(__sConfigFile, InstalledPath());
	int iPos = 0;
	sKeyName = sCode.Tokenize(__sEnvDelim, iPos);
	sAppName = sCode.Tokenize(__sEnvDelim, iPos);
	if (sKeyName.IsEmpty()) return FALSE;
	if (sAppName.IsEmpty()) sAppName = __sAppName;

	return TRUE;
}

int CTestDrive::GetConfigInt(LPCTSTR sKey, int iDefault) {
	CString sAppName, sKeyName, sEnvPath;
	if (!GetEnvString(sKey, sAppName, sKeyName, sEnvPath)) return iDefault;

	return GetPrivateProfileInt(sAppName, sKeyName, iDefault, sEnvPath);
}

void CTestDrive::SetConfigInt(LPCTSTR sKey, int iData) {
	CString sAppName, sKeyName, sEnvPath;
	if (!GetEnvString(sKey, sAppName, sKeyName, sEnvPath)) return;

	CString sData;
	sData.Format(_T("%d"), iData);

	WritePrivateProfileString(sAppName, sKeyName, sData, sEnvPath);
}

LPCTSTR CTestDrive::GetConfigString(LPCTSTR sKey) {
	CString sAppName, sKeyName, sEnvPath;
	if (sKey && GetEnvString(sKey, sAppName, sKeyName, sEnvPath)) {
		static CString sRet;

		if (GetPrivateProfileString(sAppName, sKeyName, _T(""), sRet.GetBuffer(1024 * 64), 1024 * 64, sEnvPath)) {
			return (LPCTSTR)sRet;
		}
	}

	return _T("");
}

void CTestDrive::SetConfigString(LPCTSTR sKey, LPCTSTR sData) {
	CString sAppName, sKeyName, sEnvPath;
	if (sKey && GetEnvString(sKey, sAppName, sKeyName, sEnvPath)) {
		WritePrivateProfileString(sAppName, sKeyName, sData, sEnvPath);
	}
}
