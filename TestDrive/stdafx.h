// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// 모든 권리 보유.

// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원

// for SAPI
#include <sapi.h>

// for webview
#include <wrl.h>
#include "WebView2EnvironmentOptions.h"
#include "WebView2.h"



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE(p)			{ if(p) { delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p); (p)=NULL; } }
#define SAFE_FREE(p)			{ if(p) { free(p); (p) = NULL; } }

BOOL SetRegistryDWORD(HKEY hKey, LPCTSTR sKeyPath, LPCTSTR sName, DWORD dwValue);
CString GetRegistryString(HKEY hKey, LPCTSTR sKeyPath, LPCTSTR sName);
BOOL SetRegistryString(HKEY hKey, LPCTSTR sKeyPath, LPCTSTR sName, LPCTSTR sValue);
void SetGlobalEnvironmentVariable(LPCTSTR sName, LPCTSTR sValue);
CString GetGlobalEnvironmentVariable(LPCTSTR sName);
void ModifyGlobalEnvironmentPath(LPCTSTR sPathAdd, LPCTSTR sPathRemove = NULL, BOOL bHeadToAdd = TRUE);
CString GetResourceString(int idResource);
LPCTSTR _TEXT_(LPCTSTR fmt, ...);
int CheckCommand(const TCHAR* token, const TCHAR** cmd_list, DWORD size);
BOOL IsFileExist(LPCTSTR lpPathName);
DWORD64 GetFileModifiedTime(LPCTSTR lpszFilePath);
BOOL IsSourceTreeChanged(LPCTSTR sSearchPath, LPCTSTR* sExtensions, LPCTSTR sTargetPath);
CString GetFileExtension(LPCTSTR sFileName);

// GDI+ 사용
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus")

// STL 사용
#include<iostream>
#include <list>
#include <vector>
#include <afxhtml.h>
using namespace std;

// MAPI 사용
#include <MAPI.h>

// Registry 사용
#include <winreg.h>

UINT GetLastWindowID(CWnd* pWnd);

// resource id
#include "resource.h"

// library
#include "WorkDirectory.h"
#include "FullPath.h"
