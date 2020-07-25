#include "stdafx.h"
#include "RegistryCtrl.h"
#include "Main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

RegistryCtrl			g_RegistryCtrl;

static const TCHAR*		__SESSION_NAME	= _T("Settings");

RegistryCtrl::RegistryCtrl(void){
}

RegistryCtrl::~RegistryCtrl(void){
}

UINT RegistryCtrl::GetProfileInt(LPCTSTR lpKeyName, INT nDefault){
	return theApp.GetProfileInt(__SESSION_NAME, lpKeyName, nDefault);
}

CString RegistryCtrl::GetProfileString(LPCTSTR lpKeyName, LPCTSTR lpDefault){
	return theApp.GetProfileString(__SESSION_NAME, lpKeyName, lpDefault);
}

BOOL RegistryCtrl::SetProfileInt(LPCTSTR lpKeyName, int nValue){
	return theApp.WriteProfileInt(__SESSION_NAME, lpKeyName, nValue);
}

BOOL RegistryCtrl::SetProfileString(LPCTSTR lpKeyName, LPCTSTR lpString){
	return theApp.WriteProfileString(__SESSION_NAME, lpKeyName, lpString);
}
