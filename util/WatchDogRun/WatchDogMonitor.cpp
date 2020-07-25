#include "stdafx.h"
#include "WatchDogRun.h"

static ULONG64					__ulPrevTime		= -1;
typedef list<PROCESS_DESC>		PROCESS_LIST;
static PROCESS_LIST				__ProcessList;
static DWORD					__CurrentSession	= -1;

static ULONG64 RefreshProcessList(DWORD dwPID){
	ULONG64	ulTime	= 0;
	// 프로세스 리스트 작성
	__ProcessList.clear();
	g_pWMI->Refresh();
	while(g_pWMI->Next()){
		PROCESS_DESC	desc;
		desc.dwPID			= g_pWMI->PID();

		// 동일 세션만 탐색되도록 함.
		if(__CurrentSession != (DWORD)-1){
			if(g_pWMI->SessionID() != __CurrentSession) continue;
		}else if(desc.dwPID == dwPID){
			__CurrentSession	= g_pWMI->SessionID();
		}

		desc.dwParentPID	= g_pWMI->ParentPID();
		desc.sName			= g_pWMI->Name();
		desc.ulTime			= g_pWMI->KernelModeTime();
		desc.iID			= desc.dwPID == dwPID ? 1 : 0;
		if(desc.dwPID == dwPID){
			ulTime	= desc.ulTime;
		}

		__ProcessList.push_back(desc);
	}

	// 프로세스 순위 만듬
	{
		BOOL bTest	= TRUE;
		int	iCount	= 1;

		for(int iTarget	= 1;iCount != 0;iTarget++){
			iCount	= 0;
			for(PROCESS_LIST::iterator i = __ProcessList.begin();i != __ProcessList.end();i++){
				if(i->iID != iTarget) continue;
				for(PROCESS_LIST::iterator t = __ProcessList.begin();t != __ProcessList.end();t++){
					if(t->iID || (i->dwPID == t->dwPID)) continue;
					if(t->dwParentPID == i->dwPID){
						t->iID	= iTarget + 1;
						ulTime	+= t->ulTime;
						iCount++;
					}
				}
			}
		}
	}
	return ulTime;
}

void __fastcall KillProcessTree(DWORD dwPID){
	// Kill me first
	HANDLE hProc	= OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, dwPID);
	if(hProc){
		if(!TerminateProcess(hProc, 1)){
			char sCmd[1024];
			sprintf(sCmd, "taskkill /F /T /PID %d", dwPID);
			system(sCmd);
		}
		CloseHandle(hProc);
	}else return;
	// Kill childs
	{
		PROCESSENTRY32	pe;
		memset(&pe, 0, sizeof(PROCESSENTRY32));
		pe.dwSize	= sizeof(PROCESSENTRY32);
		HANDLE	hSnap	= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(Process32First(hSnap, &pe))
			do{
				if(pe.th32ParentProcessID == dwPID)
					KillProcessTree(pe.th32ProcessID);
			}while(Process32Next(hSnap, &pe));
	}
}

void WatchProcess(DWORD dwPID){
	ULONG64 ulTime	= RefreshProcessList(dwPID);

	// 프로세스가 정체되고 있다면, 마지막 leaf 프로세스만 죽이고 지켜보기
	if(__ulPrevTime == ulTime){
		printf("\nWatchDogRun : Shutdown processes...\n");
		KillProcessTree(dwPID);
		fflush(stdout);
		g_bFailed	= TRUE;
	}

	if(g_Options.bDebug){
		for(PROCESS_LIST::iterator i = __ProcessList.begin();i != __ProcessList.end();i++)
			if(i->iID){
				_tprintf(_T("%2d : %5d %-20s\n"), i->iID, i->dwPID, i->sName.GetBuffer());
			}
		_tprintf(_T("\n\n"));
	}

	__ulPrevTime	= ulTime;
}
