#include <stdafx.h>
#include <stdio.h>
//#include "egl_Util.h"
#include "MemDebug.h"

extern "C" {
#if (defined(DEBUG) || defined(_DEBUG)) && defined(WIN32)

static HANDLE	g_hLogFile[3];
static LPCTSTR	g_LogFileName[3] = {
	//#ifdef UNICODE
	TEXT("memory_warn.txt"),
	TEXT("memory_error.txt"),
	TEXT("memory_assert.txt"),
};

int __cdecl my_crt_report_hook(int reportType, char* message, int *returnValue)
{
	switch(reportType){
	case _CRT_WARN:
		// 		if(MessageBox(NULL, message, "에러발생", MB_YESNO)==IDNO)
		// 			_asm int 3;
		printf("VR5_CRT_ERROR : %s\n", message);
		//flushall();
		break;
	case _CRT_ERROR:
		printf("VR5_CRT_ERROR : %s\n", message);
		//flushall();
		break;
	case _CRT_ASSERT:
		printf("VR5_CRT_ERROR : %s\n", message);
		//flushall();
		break;
	}
	return *returnValue;
}

void MemoryDebug_Initialize(void){
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF );

	for(int i=0;i<3;i++)
		g_hLogFile[i] = CreateFile(	g_LogFileName[i], GENERIC_WRITE, 
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, NULL);

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);

	//_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportFile(_CRT_WARN,	g_hLogFile[0]);
	_CrtSetReportFile(_CRT_ERROR,	g_hLogFile[1]);
	_CrtSetReportFile(_CRT_ASSERT,	g_hLogFile[2]);

	_CrtSetReportHook(my_crt_report_hook);
}

void MemoryDebug_Uninitialize(void){
	CHECK_MEMORY_LEAK
	_CrtDumpMemoryLeaks();
	_CrtSetReportFile(_CRT_WARN,	_CRTDBG_FILE_STDERR);
	_CrtSetReportFile(_CRT_ERROR,	_CRTDBG_FILE_STDERR);
	_CrtSetReportFile(_CRT_ASSERT,	_CRTDBG_FILE_STDERR);
	_CrtSetReportHook(NULL);
	for(int i=0;i<3;i++) CloseHandle(g_hLogFile[i]);
}

#endif

};
