#pragma once

void WatchProcess(DWORD dwPID);

typedef struct{
	DWORD		iID;
	DWORD		dwPID;
	DWORD		dwParentPID;
	CString		sName;
	ULONG64		ulTime;
}PROCESS_DESC;
