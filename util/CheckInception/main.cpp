#include "stdafx.h"
#include <time.h>
#include  <stdio.h>
#include  <stdlib.h>
#include <locale.h>
#include <io.h>

typedef enum{
	TEXT_ENCODING_UTF32_BIG_ENDIAN,
	TEXT_ENCODING_UTF32_LITTLE_ENDIAN,
	TEXT_ENCODING_UTF16_BIG_ENDIAN,
	TEXT_ENCODING_UTF16_LITTLE_ENDIAN,
	TEXT_ENCODING_UTF8,
	TEXT_ENCODING_NONE,
	TEXT_ENCODING_SIZE
}TEXT_ENCODING;

typedef struct{
	int		iSize;
	char	sBOMCodes[8];
}ENCODING_STRUCT;

ENCODING_STRUCT	g_Encoding[TEXT_ENCODING_SIZE]={
	{4, {0x00, 0x00, 0xFE, 0xFF}},	// TEXT_ENCODING_UTF32_BIG_ENDIAN
	{4, {0xFF, 0xFE, 0x00, 0x00}},	// TEXT_ENCODING_UTF32_LITTLE_ENDIAN
	{2, {0xFE, 0xFF}},				// TEXT_ENCODING_UTF16_BIG_ENDIAN
	{2, {0xFF, 0xFE}},				// TEXT_ENCODING_UTF16_LITTLE_ENDIAN
	{3, {0xEF, 0xBB, 0xBF}},		// TEXT_ENCODING_UTF8
	{0, ""}							// TEXT_ENCODING_NONE
};

typedef enum{
	TAG_CHECK,
	TAG_TITLE,
	TAG_YEAR,
	TAG_DATE,
	TAG_TIME,
	TAG_AUTHOR,
	TAG_VERSION,
	TAG_SIZE
}TAG;

char*	__TAG[TAG_SIZE]	= {
	"__CHECK__",
	"__TITLE__",
	"__YEAR__",
	"__DATE__",
	"__TIME__",
	"__AUTHOR__",
	"__VERSION__",
};

typedef struct{
	struct{
		CString		check;
		CString		exchange;
	}line[1024];

	DWORD	dwCount;
}INCEPTION;

struct{
	TCHAR		sTitle[4096];
	TCHAR		sAuthor[4096];
	TCHAR		sVersion[4096];
	TCHAR		sTargetFile[4096];
	TCHAR		sInception[4096];
}__env;

LPCTSTR __APP__ = _T("INCEPTION");

void ShowHelp(void){
	printf("Check Inception (" __DATE__ ", Q&A : clonextop@gmail.com)\n\n");
	printf("Usage : CheckInception.exe [options] target_file\n" \
		"\t-help                      show this help message.\n" \
		"\t-inception <file_path>     set inception file path.\n" \
		"\t-title <title_name>        set title name. default('no title')\n" \
		"\t-author <author_name>      set author name. default('nobody')\n" \
		"\t-version <version_string>  set version string. default('none')\n" \
		"\n\tex) CheckInception.exe -title \"ABC title\" -author \"You and me\" test.c\n"
		);
}

BOOL CheckParams(int iSize, _TCHAR* sOpt[]){
	memset(&__env, 0, sizeof(__env));

	// set default string
	strcpy(__env.sTitle,   "no title");
	strcpy(__env.sAuthor,  "nobody");
	strcpy(__env.sVersion, "none");

	while(iSize>0){
		_TCHAR*	sCurOpt	= *sOpt;
		if(sCurOpt[0] != '-'){
			if(!*__env.sTargetFile){
				// get root path
				GetFullPathName(sCurOpt, 4096, __env.sTargetFile, NULL);
				iSize--;
				sOpt++;
				continue;
			}else{
				printf("*E: Target file must be specified once.\n");
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
		CHECK_PARAM("title"){
			PROCEED_NEXT_PARAM
			strcpy(__env.sTitle, sCurOpt);
		}else
		CHECK_PARAM("author"){
			PROCEED_NEXT_PARAM
			strcpy(__env.sAuthor, sCurOpt);
		}else
		CHECK_PARAM("version"){
			PROCEED_NEXT_PARAM
			strcpy(__env.sVersion, sCurOpt);
		}else
		CHECK_PARAM("inception"){
			PROCEED_NEXT_PARAM
			strcpy(__env.sInception, sCurOpt);
		}else
		CHECK_PARAM("help"){
			ShowHelp();
			exit(0);
		}
	}
	if(!*__env.sTargetFile){
		printf("*E: No target file.\n");
		return 0;
	}
	return TRUE;
}

BOOL CheckSubPath(void){
	BOOL		bRet	= FALSE;
	CString		sSourcePath, sProjectPath;

	{	// get project path
		TCHAR		sEnvPath[4096];
		*sEnvPath	= _T('\0');
		GetEnvironmentVariable(_T("PROJECT"), sEnvPath, 4096);
		if(*sEnvPath){
			TCHAR		sFullPath[4096];
			*sFullPath		= _T('\0');
			GetFullPathName(sEnvPath, 4096, sFullPath, NULL);
			sProjectPath	= sFullPath;
			sProjectPath.TrimRight(_T('\\'));
		}
	}
	{	// get target file path
		TCHAR	sSearchPath[4096];
		TCHAR*	pFilePart;
		GetFullPathName(__env.sTargetFile, 4096, sSearchPath, &pFilePart);
		if(pFilePart) *pFilePart = _T('\0');
		sSourcePath	= sSearchPath;
		sSourcePath.TrimRight(_T('\\'));
	}
	do{
		{	// check inception option files
			CString sPath;
			sPath.Format(_T("%s\\.inception"), sSourcePath);
			if(_taccess(sPath, 0) == 0){	// found inception option files
				TCHAR sEnv[4096];
				GetPrivateProfileString(__APP__, _T("inception"), _T(""), sEnv, 4096, sPath);
				if(*sEnv) strcpy(__env.sInception, sEnv);
				GetPrivateProfileString(__APP__, _T("author"), _T(""), sEnv, 4096, sPath);
				if(*sEnv) strcpy(__env.sAuthor, sEnv);
				GetPrivateProfileString(__APP__, _T("title"), _T(""), sEnv, 4096, sPath);
				if(*sEnv) strcpy(__env.sTitle, sEnv);
				GetPrivateProfileString(__APP__, _T("version"), _T(""), sEnv, 4096, sPath);
				if(*sEnv) strcpy(__env.sVersion, sEnv);

			}
		}
		int iFound	= sSourcePath.ReverseFind(_T('\\'));
		if(iFound>0) sSourcePath.Delete(iFound, sSourcePath.GetLength());
		else break;
		if(!*sProjectPath || sSourcePath.Find(sProjectPath)) break;
	}while(1);

	return bRet;
}

TEXT_ENCODING CheckBOM(FILE* fp){
	for(int i=0;i<TEXT_ENCODING_SIZE;i++){
		BOOL bFound	= TRUE;
		fseek(fp, 0, SEEK_SET);
		for(int t=0;t<g_Encoding[i].iSize;t++){
			char cData;
			fread(&cData, 1, 1,fp);
			if(cData != g_Encoding[i].sBOMCodes[t]){
				bFound	= FALSE;
				break;
			}
		}
		if(bFound){
			fseek(fp, g_Encoding[i].iSize, SEEK_SET);
			return (TEXT_ENCODING)i;
		}
	}
	return TEXT_ENCODING_NONE;
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
	CheckSubPath();
	if(!*__env.sInception){
		printf("*E: No inception file.\n");
		return 0;
	}

	INCEPTION	inception;
	inception.dwCount	= 0;
	setlocale(LC_ALL, "English");
	{	// 문법 읽기
		FILE*	fp	= fopen(__env.sInception, "rt");
		char	line[4096];
		char	sYear[MAX_PATH];
		char	sDate[MAX_PATH];
		char	sTime[MAX_PATH];

		{
			time_t cur;
			struct tm* ptm;

			cur = time(NULL);
			ptm = localtime(&cur);

			sprintf(sYear, "%d", ptm->tm_year + 1900);
			strftime(sDate, MAX_PATH, "%#m/%#d/%Y %a", ptm);
			strftime(sTime, MAX_PATH, "%#I:%M:%#S %p", ptm);
		}

		if(!fp){
			printf("*E: Inception file is not found : %s.\n", __env.sInception);
			return 0;
		}

		while(fgets(line, 4096, fp)){
			CString* sCheck		= &inception.line[inception.dwCount].check;
			CString* sExchange	= &inception.line[inception.dwCount].exchange;
			*sCheck	= line;
			sCheck->Trim("\r\n");
			if(!sCheck->GetLength()) continue;

			*sExchange	= *sCheck;
			
			for(int i=0;i<TAG_SIZE;i++){
				int pos;
				pos	= sCheck->Find(__TAG[i]);
				if(pos>=0) *sCheck	= sCheck->Left(pos);
			}

			while(sExchange->Replace(__TAG[TAG_CHECK], ""));
			while(sExchange->Replace(__TAG[TAG_TITLE], __env.sTitle));			// title
			while(sExchange->Replace(__TAG[TAG_YEAR], sYear));					// year
			while(sExchange->Replace(__TAG[TAG_DATE], sDate));					// date
			while(sExchange->Replace(__TAG[TAG_TIME], sTime));					// time
			while(sExchange->Replace(__TAG[TAG_AUTHOR], __env.sAuthor));		// author
			while(sExchange->Replace(__TAG[TAG_VERSION], __env.sVersion));		// version

			inception.dwCount++;
		}

		fclose(fp);
	}

	if(inception.dwCount)
	{	// 문장 바꾸기
		CString	sText;
		BOOL			bModified	= FALSE;
		TEXT_ENCODING	Enc			= TEXT_ENCODING_NONE;
		{	// 원문 읽기
			FILE*	fp		= fopen(__env.sTargetFile, "rb");
			
			if(!fp){
				printf("*E: Target file is not found : %s.\n", __env.sTargetFile);
				return 0;
			}
			fseek(fp, 0, SEEK_END);
			int				string_size	= ftell(fp);
			Enc				= CheckBOM(fp);

			if (Enc != TEXT_ENCODING_NONE) {
				fseek(fp, g_Encoding[Enc].iSize, SEEK_SET);
			}

			if(string_size){
				{	// 인셉션 비교
					int		cur_pos	= ftell(fp);
					char	line[4096];

					for(DWORD i=0;i<inception.dwCount;i++){
						if(fgets(line, 4096, fp)){
							CString str(line);
							str.Trim("\r\n");

							if(!str.Find(inception.line[i].check)){
								// check string 일치
								cur_pos	= ftell(fp);
								if(str.Compare(inception.line[i].exchange))
									bModified	= TRUE;
							}else{
								// check string 불일치
								bModified	= TRUE;
								break;
							}
						}else{
							bModified	= TRUE;
							break;
						}
					}
					fseek(fp, cur_pos, SEEK_SET);
					string_size	-= cur_pos;
				}

				if(string_size>0)
				{	// inception 을 제외한 본문 읽기
					char* pMem = new char[string_size+1];

					fread(pMem, 1, string_size, fp);
					pMem[string_size]	= 0;
					sText = pMem;
					delete [] pMem;
				}
			}
			
			fclose(fp);
		}

		if(!bModified){
			// 아무런 변경사항이 없음.
			return 0;
		}

		{
			// 저장
			FILE*	fp	= fopen(__env.sTargetFile, "wb");
			if(!fp){
				printf("*E: Can't overwrite this file : %s.\n", __env.sTargetFile);
				return 0;
			}

			if(g_Encoding[Enc].iSize){
				fwrite(g_Encoding[Enc].sBOMCodes, g_Encoding[Enc].iSize, 1, fp);
			}

			// 인셉션 추가
			for(DWORD i=0;i<inception.dwCount;i++)
				fprintf(fp, "%s\r\n", (const char*)inception.line[i].exchange);
			
			// 본문 추가
			if(!sText.IsEmpty())
				fwrite((char*)sText.GetBuffer(), 1, sText.GetLength(), fp);

			fclose(fp);
		}
	}

	return 0;
}
