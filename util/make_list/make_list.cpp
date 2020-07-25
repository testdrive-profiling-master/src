#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <list>
#include <string>

using namespace std;

#define strlwr _strlwr

typedef struct{
	TCHAR	sFormat[1024];
}LIST_DESC;
LIST_DESC	g_desc;

list<string>	path_list;
list<string>	exclude_list;

typedef enum{
	FILE_TYPE_INCLUDE,
	FILE_TYPE_MAKEFILE,
}FILE_TYPE;

static LPCTSTR __sFileTypeList[] = {
	"include",
	"makefile",
	NULL
};

struct{
	FILE_TYPE	FileType;
	TCHAR		sPreFix[4096];
	TCHAR		sPostFix[4096];
	TCHAR		sOutputFileName[MAX_PATH];
	TCHAR		sDescFileName[MAX_PATH];
}__env;

BOOL GetTokenString(TCHAR* sStr, list<string>* pList){
	static const char* __sDelim	= ";";
	TCHAR* sTok;
	sTok	= strtok(sStr, __sDelim);
	while(sTok){
		//string	str(sTok);
		pList->push_back(sTok);
		sTok = strtok(NULL, ";");
	}
	return TRUE;
}

int DecodeTokenList(LPCTSTR* pList, LPCTSTR sToken){
	if(pList && sToken){
		int id = 0;
		while(*pList){
			if(!strcmp(*pList, sToken)) return id;
			pList++;
			id++;
		}
	}
	return -1;
}

BOOL CheckDescription(LPCTSTR sDescFile){
	FILE* fp = fopen(sDescFile, "r");
	TCHAR sLine[4096], *pTok;
	memset(&g_desc, 0, sizeof(LIST_DESC));

	if(!fp) return FALSE;
	
	while(fgets(sLine, 1024, fp)){
		pTok	= strstr(sLine, "\r"); if(pTok) *pTok = NULL;
		pTok	= strstr(sLine, "\n"); if(pTok) *pTok = NULL;

		if(strstr(sLine, "Target:") == sLine){
			TCHAR* lpszList	= &sLine[7];
			GetTokenString(lpszList, &path_list);
		}else
		if(strstr(sLine, "Format:") == sLine){
			strcpy(g_desc.sFormat, &sLine[7]);
			strcat(g_desc.sFormat, "\n");
		}else
		if(strstr(sLine, "Prefix:") == sLine){
			strcat(__env.sPreFix, &sLine[7]);
			strcat(__env.sPreFix, "\n");
		}else
		if(strstr(sLine, "Postfix:") == sLine){
			strcat(__env.sPostFix, &sLine[8]);
			strcat(__env.sPostFix, "\n");
		}else
		if(strstr(sLine, "Exclude:") == sLine){
			GetTokenString(&sLine[8], &exclude_list);
			{
				for(list<string>::iterator iter=exclude_list.begin();iter!=exclude_list.end();iter++){
					string* pStr = (string*)&(*iter);
					strlwr((LPTSTR)pStr);
				}
			}
		}else
		if((strstr(sLine, ";") == sLine) || (strstr(sLine, "//") == sLine)){
			// comment line
		}else
		if(strlen(sLine)){
			printf("Invalid description = %s\n", sLine);
			return FALSE;
		}
	}
	if(	!path_list.size() ||
		!*g_desc.sFormat)
		return FALSE;
	return TRUE;
}

BOOL CheckExcludeFile(LPCTSTR sFileName){
	TCHAR sCmp[1024];
	strcpy(sCmp, sFileName);
	strlwr(sCmp);

	for(list<string>::iterator iter=exclude_list.begin();iter!=exclude_list.end();iter++){
		if(strstr(sCmp, iter->c_str())) return TRUE;
	}

	return FALSE;
}

void RetrievePathName(LPCSTR sPath, DWORD dwSize, LPSTR sFullPath, LPSTR* lpFilePart){
	TCHAR	sRetrivePath[1024], sName[256];
	LPCTSTR	pToken;
	*sRetrivePath	= 0;
	while(1){
		if(pToken = strstr(sPath, "%")){
			strncat(sRetrivePath, sPath, (DWORD)(pToken - sPath) / sizeof(TCHAR));
			sPath = pToken + 1;
			pToken = strstr(sPath, "%");
			*sName = 0;
			strncat(sName, sPath, (DWORD)(pToken - sPath) / sizeof(TCHAR));
			sPath = pToken + 1;
			{
				TCHAR sVariable[1024];
				GetEnvironmentVariable(sName, sVariable, 1024);
				strcat(sRetrivePath, sVariable);
			}
		}else{
			strcat(sRetrivePath, sPath);
			break;
		}
	}

	GetFullPathName(sRetrivePath, dwSize, sFullPath, lpFilePart);
}

BOOL MakeList(FILE* fList){
	int iCount = 0;
	for(list<string>::iterator iter=path_list.begin();iter!=path_list.end();iter++){
		// 파일 검색
		HANDLE			hSrch;
		WIN32_FIND_DATA	wfd;
		BOOL			bRet	= TRUE;
		TCHAR			sFullPath[1024], *sFilePart;

		RetrievePathName(iter->c_str(), 1024, sFullPath, &sFilePart);

		hSrch	= FindFirstFile(sFullPath, &wfd);
		if(sFilePart) *sFilePart = NULL;
		while(sFilePart = strstr(sFullPath, "\\")) *sFilePart = '/';

		if(hSrch == INVALID_HANDLE_VALUE) continue;
		while(bRet){
			if(!(wfd.dwFileAttributes & (FILE_ATTRIBUTE_DEVICE|FILE_ATTRIBUTE_DIRECTORY))){
				if(!CheckExcludeFile(wfd.cFileName)){
					TCHAR	sFilename[1024];
					sprintf(sFilename, "%s%s", sFullPath, wfd.cFileName);
					fprintf(fList, g_desc.sFormat, sFilename);
					iCount++;
				}
			}
			bRet	= FindNextFile(hSrch, &wfd);
		}
		FindClose(hSrch);
	}
	
	return TRUE;
}

void ShowHelp(void){
	printf("Make list (" __DATE__ ", Q&A : clonextop@gmail.com)\n\n");
	printf("Usage : make_list.exe [options] description_file\n" \
		"\t-help               Display this information\n" \
		"\t-output <file>      set output file name [default:\"list.txt\"]\n" \
		"\t-target <paths>     add target path list. ex) '..\\*.cpp;..\\inc\\*.cpp'\n" /* \
		"\t                    'include'    : C/C++ include file type [default]\n" \
		"\t                    'makefile'   : makefile type\n"*/
	);
}

BOOL CheckParams(int iSize, _TCHAR* sOpt[]){
	while(iSize>0){
		_TCHAR*	sCurOpt	= *sOpt;
		if(sCurOpt[0] != '-'){
			if(!*__env.sDescFileName){
				strcpy(__env.sDescFileName, sCurOpt);
				iSize--;
				sOpt++;
				continue;
			}else{
				printf("*E : Description file must be specified once.\n");
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
		/*
		CHECK_PARAM("format"){
			PROCEED_NEXT_PARAM
			__env.FileType	= (FILE_TYPE)DecodeTokenList(__sFileTypeList, sCurOpt);
			if(__env.FileType<0){
				printf("*E : Invalid format type : %s\n", *sOpt);
				return FALSE;
			}
			sOpt++;
			iSize--;
		}else*/
		CHECK_PARAM("output"){
			PROCEED_NEXT_PARAM
			strcpy(__env.sOutputFileName, sCurOpt);
		}else
		CHECK_PARAM("target"){
			PROCEED_NEXT_PARAM
			{
				TCHAR sPath[1024];
				strcpy(sPath, sCurOpt);
				GetTokenString(sPath, &path_list);
			}
		}else
		CHECK_PARAM("help"){
			ShowHelp();
			exit(0);
		}
	}
	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[]){
	if(argc==1){
		ShowHelp();
		return 0;
	}
	// initialize environment
	{
		__env.sDescFileName[0]	= '\0';
		__env.FileType			= FILE_TYPE_INCLUDE;
		__env.sPreFix[0]		= '\0';
		__env.sPostFix[0]		= '\0';
		strcpy(__env.sOutputFileName, "list.txt");
	}

	if(!CheckParams(argc-1, &argv[1])){
		printf("\t*E : Option check is failed!\n");
		return 0;
	}

	if(!CheckDescription(__env.sDescFileName)){
		printf("\t*E : Description file is invalid! : %s\n", __env.sDescFileName);
		return 0;
	}

	{
		FILE* fp = fopen(__env.sOutputFileName, "w");
		if(!fp){
			printf("\t*E : Can't create output file...\n");
			return 0;
		}
		fprintf(fp, __env.sPreFix);
		if(!MakeList(fp)){
			printf("\tMaking list is failed!\n");
		}
		fprintf(fp, __env.sPostFix);
		fclose(fp);
	}
	

	return 0;
}

