#include "stdafx.h"
#include <windows.h>

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64(void)
{
#if defined(_WIN64)
	return TRUE;
#elif define(_WIN32)
	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "IsWow64Process");

	if(fnIsWow64Process != NULL)
	{
		if( !fnIsWow64Process(GetCurrentProcess(), &bIsWow64) )
		{
			// handle error
		}
	}

	return bIsWow64;
#else
	return FALSE;
#endif
}


bool GetDownloadPathEclipse(char* sPath, BOOL b64 = TRUE){
	bool	bFound	= false;
	FILE*	fp		= fopen("download.html", "rt");

	if(fp){
		char *sLine = new char[1024*1024];
		int	iStep = 0;
		while(fgets(sLine, 1024*1024, fp) && !bFound){
			switch (iStep) {
			case 0:
				if (!strstr(sLine, "C/C++")) {
					iStep++;
				}
				break;
			case 1:
				if (!strstr(sLine, "cpp")) break;
				if (!strstr(sLine, b64 ? "win32-x86_64.zip" : "win32.zip")) break;
				{
					char* sTok = strstr(sLine, ".zip");
					sTok[4] = NULL;
					sTok = strstr(sLine, "www.eclipse.org");
					if (sTok) {
						strcpy(sPath, sTok);
					}
					else {
						sTok = strstr(sLine, "href=\"");
						sprintf(sPath, "www.eclipse.org/downloads/%s", sTok + 6);
					}

					bFound = true;
				}
				break;
			}
		}
		delete [] sLine;
		fclose(fp);
		system("del /Q download.html");
	}

	return bFound;
}

bool GetDownloadPathNotepad(char* sPath){
	bool	bFound	= false;
	FILE*	fp;

	char *sLine = new char[1024 * 64];
	char *sTok, *sEnd;

	if((fp = fopen("download.html", "rt")) != NULL){
		// find download page
		while(fgets(sLine, 1024*64, fp)){
			sTok = strstr(sLine, "href=\"");
			if (!sTok) continue;
			sTok += 6;
			if(strstr(sTok, "/downloads/") != sTok) continue;
			{
				sEnd = strstr(sTok, "\"");
				if (sEnd) {
					*sEnd = NULL;
					{
						// get download page
						char sCmd[1024*4];
						sprintf(sCmd, "wget --no-check-certificate https://notepad-plus-plus.org%s -O download_2.html", sTok);
						system(sCmd);
						bFound = true;
					}
					break;
				}
				bFound = true;
			}
		}
		fclose(fp);
		system("del /Q download.html");
	}

	if (bFound) {
		bFound = FALSE;
		if ((fp = fopen("download_2.html", "rt")) != NULL) {
			// find download url
			while (fgets(sLine, 1024 * 64, fp)) {
				sTok = strstr(sLine, "href=\"");
				if (!sTok) continue;
				sTok += 6;
				sEnd = strstr(sTok, ".zip\"");
				if (sEnd) {
					sEnd[4] = NULL;
					if (strstr(sTok, "http") == sTok) {
						strcpy(sPath, sTok);
					}
					else {
						sprintf(sPath, "https://notepad-plus-plus.org%s", sTok);
					}
					bFound = true;
					break;
				}
			}
			fclose(fp);
			system("del /Q download_2.html");
		}
	}

	delete[] sLine;
	return bFound;
}

bool GetDownloadHtmlJRE(char* sPath){
	bool	bFound	= false;
	FILE*	fp		= fopen("download.html", "rt");

	if(fp){
		char sLine[1024*64];
		while(fgets(sLine, 1024* 64, fp)){
			char* sURL = strstr(sLine, "-x64.exe</a>");
			if (!sURL) continue;
			sURL[8] = NULL;
			{
				char* sTok = sLine;
				while (sURL = strstr(sTok, "<a href")) {
					sTok = sURL + 1;
				}
				sURL = strstr(sTok, ">");
				if (!sURL) continue;
				strcpy(sLine, sURL + 1);
			}
			{
				sprintf(sPath, "https://enos.itcollege.ee/~jpoial/allalaadimised/jdk8/%s", sLine);
				bFound = true;
				printf("URL found : %s\n", sPath);
				break;
			}
		}
		fclose(fp);
		system("del /Q download.html");
	}
	else false;

	return bFound;
}

bool GetDownloadPathJRE(char* sPath){
	bool	bFound	= false;
	FILE*	fp		= fopen("download.html", "rt");

	if(fp){
		char sLine[1024 * 64];
		while (fgets(sLine, 1024 * 64, fp)) {
			char* sURL = strstr(sLine, "<a title=\"");
			if (!sURL) continue;
			strcpy(sLine, sURL + 10);
			sURL = strstr(sLine, "\"");
			if (!sURL) continue;
			*sURL = NULL;
			{	// check title
				strlwr(sLine);
				if (!strstr(sLine, "download")) continue;
				if (!strstr(sLine, "java")) continue;
				if (!strstr(sLine, "windows")) continue;
				if (!strstr(sLine, "64")) continue;
			}
			strcpy(sLine, sURL + 1);
			sURL = strstr(sLine, "\"");
			if (!sURL) continue;
			strcpy(sLine, sURL + 1);
			sURL = strstr(sLine, "\"");
			if (!sURL) continue;
			*sURL = NULL;
			{
				strcpy(sPath, sLine);
				bFound = true;
				printf("URL found : %s\n", sPath);
				break;
			}
		}
		fclose(fp);
		system("del /Q download.html");
	}
	return bFound;
}

bool GetDownloadPathMinGW(char* sPath){
	bool	bFound	= false;
	FILE*	fp		= fopen("download.html", "rt");
	if(fp){
		char *sLine = new char[1024*1024];
		while(fgets(sLine, 1024*1024, fp)){
			char* sURL = strstr(sLine, "\"http");
			if(!sURL) continue;
			strcpy(sLine, sURL+1);
			sURL = strstr(sLine, "\"");
			if (!sURL) continue;
			*sURL = NULL;
			{
				sURL = strstr(sLine, "msys2");
				if (!sURL) continue;
				sURL = strstr(sLine, ".tar.xz");
				if(!sURL) continue;
				sURL = strstr(sLine, "x86_64");
				if (!sURL) continue;
				sURL = strstr(sLine, ".sig");
				if (sURL) continue;
				strcpy(sPath, sLine);
				bFound = true;
				printf("URL found : %s\n", sPath);
				break;
			}
		}
		fclose(fp);
		system("del /Q download.html");
	}
	return bFound;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char sPath[2048], sCommand[2048];
	if(argc != 2){
		printf("Eclipse CDT/Notepad++ auto download. (" __DATE__ " : clonextop@gmail.com)\n\n");
		printf(
			"Usage : ToolsAutoDownload [eclipse/notepad++/jre]\n" \
			"\tDownload newest version of tools for TestDrive Profiling Master.\n" \
			"\t    eclipse    : Eclipse CDT\n" \
			"\t    notepad++  : NotePad++\n" \
			"\t    mingw      : MinGWx64 + MSys2\n" \
			"\t    jre        : Java Runtime Environment\n" \
			);
		return 0;
	}

	if(!strcmp(argv[1], "eclipse")){
		system("curl http://www.eclipse.org/downloads/packages/ -o download.html");

		if(GetDownloadPathEclipse(sPath, IsWow64()))
		{
			sprintf(sCommand, "curl -L \"http://%s&r=1\" -o eclipse.zip", sPath);

			printf("Downloading Eclipse for %s\n:'http://%s'\n\n", IsWow64() ? "x64" : "x86", sPath);
			system(sCommand);
		}else{
			printf("*E : Failed to access to Eclipse page...\n");
		}
	}else
	if(!strcmp(argv[1], "notepad++")){
		system("wget --no-check-certificate https://notepad-plus-plus.org -O download.html");
		if(GetDownloadPathNotepad(sPath)){
			printf("path : %s\n", sPath);
			sprintf(sCommand, "wget --no-check-certificate \"%s\" -O notepad.zip", sPath);
			printf("Downloading notepad++... : %s\n\n", sPath);
			system(sCommand);
		}else{
			printf("*E : Failed to access to notepad++ page...\n");
		}
	}else
	if(!strcmp(argv[1], "jre")){
		//system("wget --no-cookies --no-check-certificate --header \"Cookie: oraclelicense = accept-securebackup-cookie\" \"https://www.java.com/download/\" -O download.html");
		system("wget --no-cookies --no-check-certificate \"https://enos.itcollege.ee/~jpoial/allalaadimised/jdk8/\" -O download.html");
		if(GetDownloadHtmlJRE(sPath)){
			/*{
				sprintf(sCommand, "wget --no-cookies --no-check-certificate --header \"Cookie: oraclelicense = accept - securebackup - cookie\" \"%s\" -O download.html", sPath);
				system(sCommand);
			}*/
			//if(GetDownloadPathJRE(sPath))
			{
				//sprintf(sCommand, "wget --no-cookies --no-check-certificate --header \"Cookie: oraclelicense = accept - securebackup - cookie\" \"%s\" -O jre.exe", sPath);
				sprintf(sCommand, "wget --no-cookies --no-check-certificate \"%s\" -O jre.exe", sPath);
				system(sCommand);
			}
		}
	}else
	if(!strcmp(argv[1], "mingw")){
		sprintf(sCommand, "wget --no-check-certificate \"https://github.com/msys2/msys2-installer/releases/download/nightly-x86_64/msys2-base-x86_64-latest.tar.xz\" -O msys2.tar.xz", sPath);
		system(sCommand);
	}
	else{
		printf("*E : not support for download '%s'\n", argv[1]);
	}

	return 0;
}
