#include "stdafx.h"
#include <time.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <io.h>

struct{
	TCHAR		sRootPath[4096];
	TCHAR		sArg[1024*32];
	int			iErrorCode;
	BOOL		bFoundMakefile;
}__env;

void ShowHelp(void){
	printf("Hierarchical Make 1.0b (" __DATE__ ", Q&A : clonextop@gmail.com)\n\n");
	printf("Usage : HierarchicalMake.exe [options] root_directory\n" \
		"\t-arg <arguments>    set build arguments\n" \
		"\t                    ex) -arg \"-j clean\"\n"
	);
}

BOOL CheckParams(int iSize, _TCHAR* sOpt[]){
	memset(&__env, 0, sizeof(__env));

	while(iSize>0){
		_TCHAR*	sCurOpt	= *sOpt;
		if(sCurOpt[0] != '-'){
			if(!*__env.sRootPath){
				// get root path
				GetFullPathName(sCurOpt, 4096, __env.sRootPath, NULL);
				iSize--;
				sOpt++;
				continue;
			}else{
				printf("*E: Root directory must be specified once.\n");
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
		CHECK_PARAM("arg"){
			PROCEED_NEXT_PARAM
			strcpy(__env.sArg, sCurOpt);
		}else
		CHECK_PARAM("help"){
			ShowHelp();
			exit(0);
		}
	}
	return TRUE;
}

BOOL LoopSearchPath(LPCTSTR sPath){
	BOOL				bRet	= FALSE;
	HANDLE				hFind;
	WIN32_FIND_DATA		FindFileData;
	{	// no search file definition
		CString sNoSearchPath;
		sNoSearchPath.Format(_T("%s\\.TestDrive.nosearch"), sPath);
		if(_access(sNoSearchPath, 0)  != -1) return FALSE;
	}
	{
		CString			sFindPath;
		sFindPath.Format(_T("%s\\*.*"), sPath);
		hFind			= FindFirstFile(sFindPath, &FindFileData);
	}
	if(hFind != INVALID_HANDLE_VALUE){
		do {
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && FindFileData.cFileName[0]!=_T('.')) {
				CString	sNextSearchPath;
				sNextSearchPath.Format(_T("%s\\%s"), sPath, FindFileData.cFileName);
				bRet	|= LoopSearchPath(sNextSearchPath);
			}
		} while(FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}

	{	// check makefile exist...
		CString sMakefilePath;
		sMakefilePath.Format(_T("%s\\makefile"), sPath);
		if(_access(sMakefilePath, 0)  == -1) return FALSE;
	}

	__env.bFoundMakefile	= TRUE;

	if(!__env.iErrorCode){
		TCHAR	sCurPath[4096];
		printf("Hierarchical Make : \"%s\\makefile\" %s\n", sPath, __env.sArg);
		GetCurrentDirectory(4096, sCurPath);
		SetCurrentDirectory(sPath);
		bRet	= TRUE;
		{
			CString	sCommand;
			sCommand.Format(_T("mingw32-make %s"), __env.sArg);
			fflush(stdout);
			__env.iErrorCode = system(sCommand);
			fflush(stdout);
			if(__env.iErrorCode){
				printf("*E: build is failed.\n");
			}
		}
		SetCurrentDirectory(sCurPath);
		printf("\n");
	}
	return bRet;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc==1){
		ShowHelp();
		return 0;
	}
	if(!CheckParams(argc-1, &argv[1])){
		printf("*E: Option check is failed!\n");
		return 0;
	}

	if(!*__env.sRootPath){
		printf("*E: Root path must be specified.\n");
	}else{
		LoopSearchPath(__env.sRootPath);
		if(!__env.bFoundMakefile)
			printf("*E: Root directory is not exist.\n");
	}

	return __env.iErrorCode;
}
