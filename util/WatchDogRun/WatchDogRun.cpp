#include "stdafx.h"
#include "WatchDogRun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWinApp theApp;

HANDLE RunProgram(LPCTSTR sFile, LPCTSTR sParam){
	STARTUPINFO				StartupInfo;
	PROCESS_INFORMATION		ProcessInfo;

	HANDLE	hStdOut	= GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE	hStdErr	= GetStdHandle(STD_ERROR_HANDLE);
	HANDLE	hStdInp	= GetStdHandle(STD_INPUT_HANDLE);

	ZeroMemory( &StartupInfo,			sizeof( StartupInfo ));
	ZeroMemory( &ProcessInfo,			sizeof( ProcessInfo ));

	StartupInfo.cb			= sizeof(STARTUPINFO);
	StartupInfo.dwFlags		= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	StartupInfo.wShowWindow = SW_HIDE;
	StartupInfo.hStdOutput	= hStdOut;
	StartupInfo.hStdError	= hStdErr;
	StartupInfo.hStdInput	= hStdInp;

	CString	sCommand(sFile);

	if(sParam) sCommand.AppendFormat(_T(" %s"), sParam);

	if(CreateProcess
		( 
		NULL,					// pointer to name of executable module
		(LPTSTR)(LPCTSTR)sCommand,// command line 
		NULL,					// pointer to process security attributes
		NULL,					// pointer to thread security attributes (use primary thread security attributes)
		TRUE,					// inherit handles
		0,						// creation flags
		NULL,					// pointer to new environment block (use parent's)
		NULL,					// pointer to current directory name
		&StartupInfo,			// pointer to STARTUPINFO
		&ProcessInfo			// pointer to PROCESS_INFORMATION
		)){
		return ProcessInfo.hProcess;
	}
	return 0;
}

BOOL RetreiveParams(int argc, TCHAR* argv[]){
	for(int i=1;i<argc;i++){
		TCHAR*	sParam = argv[i];

		if(g_sTarget.IsEmpty()){
			if(sParam[0] == _T('-')){
				switch(sParam[1]){
				case _T('r'):	// rerun
					g_Options.bRerun	= TRUE;
					break;
				case _T('d'):	// debug
					g_Options.bDebug	= TRUE;
					break;
				case _T('t'):	// timing
					g_Options.dwSleepTime	= (DWORD)(_tstof(sParam+2) * 1000);
					break;
				default:
					_tprintf(_T("*E : Invalid param : '%s'\n"), sParam);
					return FALSE;
					break;
				}
			}else{
				g_sTarget = sParam;
			}
		}else{
			if(!g_sParam.IsEmpty()) g_sParam.Append(_T(" "));
			g_sParam.Append(sParam);
		}
	}
	return TRUE;
}

static char* __sOptions =
	"	-r  : rerun when processing failed.\n" \
	"	-d  : print tree processes for debug.\n" \
	"	-t# : specify check time. ex) -t5.5\n"
;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			_tprintf(_T("*E : Fatal error - MFC is not initialized.\n"));
			nRetCode = 1;
		}
		else
		{
			if(argc == 1){
				printf("Watchdog Run 0.4 (" __DATE__ " : clonextop@gmail.com)\n");
				printf("Usage : WatchDogRun.exe [options] target [parameters] ...\n");
				printf("Options :\n%s\n", __sOptions);
			}else{
				if(!RetreiveParams(argc, argv))
					return nRetCode;
RETRY_PROCESS:
				if(!g_sTarget.IsEmpty()){
					HANDLE hProcess = RunProgram(g_sTarget, g_sParam);
					g_bFailed	= FALSE;
					nRetCode	= 0;
					if(hProcess){
						WMIConnection connection;
						connection.Open(L"ROOT\\CIMV2");
						WMIProcess	wmi(connection);
						DWORD	dwPID	= GetProcessId(hProcess);
						g_pWMI	= &wmi;
						while(WaitForSingleObject(hProcess, g_Options.dwSleepTime) != WAIT_OBJECT_0){
							WatchProcess(dwPID);
						}
					}else{
						printf("*E : Process is not found.\n");
					}
					if(g_bFailed)
						nRetCode = 2;
				}else{
					printf("*E : Target is not specified.\n");
				}
				if(g_bFailed && g_Options.bRerun){
					printf("\n\nRetrying run target process...\n\n");
					goto RETRY_PROCESS;
				}
			}
		}
	}
	else
	{
		_tprintf(_T("*E : Fatal error - 'GetModuleHandle' is failed.\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
