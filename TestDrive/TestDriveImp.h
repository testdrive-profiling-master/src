#pragma once
#include "commandlist.h"
#include "Memory.h"
#include "Output.h"
#include "RedirectExecute.h"
#include "TestDrive.h"
#include "Config.h"
#include "Mail.h"
#include "Speech.h"
#include "TestDriveLua.h"
#include "TestDriveResource.h"
#include "Localization.h"

class CTestDrive : public ITDSystem, public TestDriveResource
{
public:
	CTestDrive(void);
	virtual ~CTestDrive(void);

	static BOOL Initialize(void);
	static void Release(void);

// 	// Lua dummy call
	void LUA_Clearlog(void)				{ClearLog();}
	void LUA_LogOut(const char* sMsg)	{LogOut(CString(sMsg));}
// 
// 	// title
	STDMETHOD_(DWORD, GetVersion)(void);
	STDMETHOD_(DWORD, GetLocale)(void);
	virtual CString& UpdateTite(void);
	STDMETHOD_(void, SetTitle)(TITLE_ID id, LPCTSTR szTitle);
	LPCTSTR GetTitle(void);
	STDMETHOD_(void, SetProfile)(PROFILE_ID id, LPCTSTR lpszFileName);

	// Output
	STDMETHOD_(void, ClearLog)(void);
	STDMETHOD_(void, LogOut)(LPCTSTR szMsg, SYSMSG_ID id = SYSMSG_INFO);
	STDMETHOD_(void, LogInfo)(LPCTSTR szFormat, ...);
	STDMETHOD_(void, LogWarning)(LPCTSTR szFormat, ...);
	STDMETHOD_(void, LogError)(LPCTSTR szFormat, ...);
	STDMETHOD_(BOOL, RunProfile)(LPCTSTR szFileName, BOOL bSystem = FALSE);
	STDMETHOD_(int, ExecuteFile)(LPCTSTR lpszFileName, LPCTSTR lpszArg, BOOL bSystemLog, TD_REDIRECT_LOG func, LPCTSTR lpszWorkDir, ...);
	STDMETHOD_(ITDImplSystem*, GetImplementation)(void);
	STDMETHOD_(ITDDocument*, GetDocument)(LPCTSTR lpszName);
	STDMETHOD_(ITDMemory*, GetMemory)(LPCTSTR lpszName = NULL, BOOL bForced = FALSE);
	STDMETHOD_(double, GetElapsedTime)(LONGLONG& llPrevTime);
	STDMETHOD_(ITDMail*, CreateMail)(void);
	STDMETHOD_(ITDMovie*, CreateMovieInterface)(void);
	STDMETHOD_(ITDSerialCom*, CreateSerialComInterface)(void);
	STDMETHOD_(PVOID, CreateInterface)(TD_INTERFACE td_inf);
// 
// 	// Tools
	STDMETHOD_(BOOL, VerilatorCompile)(LPCTSTR lpszFileName, LPCTSTR lpszWorkDir, LPCTSTR lpszOption = NULL);
	STDMETHOD_(BOOL, GTKWave)(LPCTSTR lpszCommandLine, LPCTSTR lpszWorkDir = NULL);
	STDMETHOD_(BOOL, CppCheck)(LPCTSTR lpszPath, LPCTSTR lpszArgs = NULL);

	STDMETHOD_(void, PeekAndPumpMessage)(void);
	STDMETHOD_(LPCTSTR, GetProjectPath)(void);
	STDMETHOD_(void, Speak)(LPCTSTR msg, DWORD flags = SPEAK_DEFAULT);
	STDMETHOD_(ITDSemaphore*, CreateSemaphore)(int iInitValue);
	STDMETHOD_(void, SendCommand)(ITDDocument* pDoc, DWORD command, WPARAM wParam = NULL, LPARAM lParam = NULL);
	STDMETHOD_(void, SetForeground)(void);
	STDMETHOD_(LPCTSTR, RetrieveFullPath)(LPCTSTR szPath = NULL);
	STDMETHOD_(void, TryQuit)(void);
	STDMETHOD_(BOOL, InsertCodeInception)(LPCTSTR lpszTargetFile, LPCTSTR lpszInceptionFile, LPCTSTR lpszTitle = NULL, LPCTSTR lpszAuthor = NULL, LPCTSTR lpszVersion = NULL);
	STDMETHOD_(BOOL, ReplaceText)(LPCTSTR lpszTargetFile, LPCTSTR lpszReplaceDescFile);
	STDMETHOD_(BOOL, SearchSubPathFile)(LPCTSTR sSearchPath, LPCTSTR sFileName, SEARCH_FILE_FUNCTION SearchFunc, LPVOID pData);

	// project
	BOOL OpenProject(LPCTSTR szFileName = NULL);
	BOOL CloseProject(BOOL bPreserveMessage = FALSE);
	BOOL IsProjectOpen(void);
	BOOL IsBusy(void);
	BOOL IsValidProfile(TESTDRIVE_PROFILE id);
	BOOL Build(TESTDRIVE_PROFILE id = TESTDRIVE_PROFILE_INITIALIZE);
	BOOL Build(LPCTSTR szFileName, COutput* msg = NULL, BOOL bThreaded = FALSE);
	BOOL SetProfilePath(TESTDRIVE_PROFILE id, LPCTSTR szFileName = NULL);
	CString GetProfilePath(TESTDRIVE_PROFILE id){return m_sProfileMenu[id];}
	void SetProjectDirectroyToCurrent(void);
	BOOL SetProjectFile(LPCTSTR lpszPath);


	// Project Configuration
 	CConfig				m_Config;
	CSpeech				m_Speech;
	CTestDriveLua		m_Lua;

protected:	// local
	CString				m_sTitle;
	CString				m_sTitleDB[TITLE_MAX];
	CString				m_sProfileMenu[TESTDRIVE_PROFILE_SIZE];

	DWORD				m_dwCountError;
	DWORD				m_dwCountWarning;

	BOOL				m_bRunning;
	BOOL				m_bProjectOpen;
};

// Single TestDrive object
extern CTestDrive*		g_pTestDrive;
extern CSemaphore		g_Semaphore;