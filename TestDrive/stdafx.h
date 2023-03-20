// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")�� 
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե� 
// Microsoft Foundation Classes Reference �� ���� ���� ������ ���� 
// �߰������� �����Ǵ� �����Դϴ�.  
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.  
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������ 
// http://msdn.microsoft.com/officeui�� �����Ͻʽÿ�.
//
// Copyright (C) Microsoft Corporation
// ��� �Ǹ� ����.

// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����

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

// GDI+ ���
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus")

// STL ���
#include<iostream>
#include <list>
#include <vector>
#include <afxhtml.h>
using namespace std;

// MAPI ���
#include <MAPI.h>

// Registry ���
#include <winreg.h>

UINT GetLastWindowID(CWnd* pWnd);

// resource id
#include "resource.h"

// library
#include "WorkDirectory.h"
#include "FullPath.h"
