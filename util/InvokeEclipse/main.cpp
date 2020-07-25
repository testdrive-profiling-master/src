#include "stdafx.h"
#include <time.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <io.h>

LPCTSTR __ECLIPSE_PATH	= _T("%TESTDRIVE_PROFILE%common\\common\\eclipse.bat");

struct{
	TCHAR		sName[4096];
	TCHAR		sWorkspacePath[4096];
	TCHAR		sCurrentPath[4096];
}__env;

void GetRoot(LPCTSTR sRoot){
	CString sFullPath;
	{	// get current full path
		TCHAR	sPath[4096];
		GetFullPathName(sRoot, 4096, sPath, NULL);
		sFullPath	= sPath;
		sFullPath.TrimRight(_T('\\'));
		strcpy(__env.sCurrentPath, sFullPath);
	}
	{	// find .workspace folder
		int iPos	= 0;
		BOOL bFound	= FALSE;
		CString	sWorkPath(sFullPath);
		// search sub folder ".workspace"
		do{
			iPos	= sWorkPath.ReverseFind(_T('\\'));
			if(iPos>0){
				sWorkPath.Delete(iPos, sWorkPath.GetLength());
				CString	sAccessPath;
				sAccessPath.Format(_T("%s\\.workspace"), sWorkPath);
				if(!_access(sAccessPath, 0)){
					bFound	= TRUE;
					break;
				}
			}
		}while(iPos>0);
		// if not found ".workspace"
		if(!bFound) {
			sWorkPath	= sFullPath;
			iPos	= sWorkPath.ReverseFind(_T('\\'));
			sWorkPath.Delete(iPos, sWorkPath.GetLength());
		}
		// add workspace
		{
			CString sAddPath(sFullPath);
			iPos		= sWorkPath.GetLength();
			sWorkPath	+= _T("\\.workspace");
			sAddPath.Delete(0, iPos);
			sWorkPath	+= sAddPath;
		}
		
		strcpy(__env.sWorkspacePath, sWorkPath);
	}
	{	// get folder name
		CString sName(sFullPath);
		int iPos	= sName.ReverseFind(_T('\\'));
		if(iPos>=0) sName.Delete(0, iPos+1);
		strcpy(__env.sName, sName);
	}
}

void ShowHelp(void){
	printf("Invoke Eclipse (" __DATE__ ", Q&A : clonextop@gmail.com)\n\n");
	printf("Usage : InvokeEclipse.exe [options]\n" \
		"\t-help               Display this information\n");
}

BOOL CheckParams(int iSize, _TCHAR* sOpt[]){
	memset(&__env, 0, sizeof(__env));

	while(iSize>0){
		_TCHAR*	sCurOpt	= *sOpt;
		if(sCurOpt[0] != '-'){
			if(!*__env.sWorkspacePath){
				// get root path
				GetRoot(sCurOpt);
				iSize--;
				sOpt++;
				continue;
			}else{
				printf("*E : Root directory must be specified once.\n");
				return FALSE;
			}
		}
		iSize--;
		sCurOpt++;
		sOpt++;

#define CHECK_PARAM(s)	if(!strcmp(sCurOpt, s))
#define PROCEED_NEXT_PARAM	\
	if(!iSize) return FALSE; \
	sCurOpt	= *sOpt; \
	iSize--; \
	sOpt++;
		CHECK_PARAM("help"){
			ShowHelp();
			exit(0);
		}
	}
	return TRUE;
}


int _tmain(int argc, _TCHAR* argv[])
{
	if(argc==1){
		ShowHelp();
		return 0;
	}
	if(!CheckParams(argc-1, &argv[1])){
		printf("*E : Option check is failed!\n");
		return 0;
	}

	if(__env.sWorkspacePath[0]){
		TCHAR sTestDriveDir[4096];
		GetEnvironmentVariable("TESTDRIVE_PROFILE", sTestDriveDir, 4096);

		CString sArg;
		CString sCommand;
		sCommand.Format("%scommon\\common\\eclipse.bat", sTestDriveDir);
		sArg.Format(_T("\"%s\" \"%s\""), __env.sCurrentPath, __env.sWorkspacePath);
		ShellExecute(NULL, NULL, sCommand, sArg, NULL, SW_SHOW);
	}else{
		printf("*E: workspace is not existed.\n");
	}

	return 0;
}
