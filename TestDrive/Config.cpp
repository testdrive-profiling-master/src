#include "StdAfx.h"
#include "Config.h"


CConfig::CConfig(void){
}

CConfig::~CConfig(void){
}

void CConfig::SetConfigFile(LPCTSTR lpszFileName){
	m_sConfigFileFullName = lpszFileName;
}

int CConfig::GetInt(LPCTSTR lpszAppName, LPCTSTR lpszKeyName, int default_value){
	if(!lpszAppName || !lpszKeyName) return default_value;
	return GetPrivateProfileInt(lpszAppName, lpszKeyName, default_value, m_sConfigFileFullName);
}

void CConfig::SetInt(LPCTSTR lpszAppName, LPCTSTR lpszKeyName, int set_value){
	if(!lpszAppName || !lpszKeyName) return;
	CString str;
	str.Format(_T("%d"), set_value);
	WritePrivateProfileString(lpszAppName, lpszKeyName, str, m_sConfigFileFullName);
}

void CConfig::GetString(LPCTSTR lpszAppName, LPCTSTR lpszKeyName, LPTSTR lpszStr, DWORD dwSize, LPCTSTR lpszDefault){
	if(!lpszStr) return;
	if(!lpszKeyName){
		if(lpszDefault) _tcscpy_s(lpszStr, dwSize, lpszDefault);
	}else{
		GetPrivateProfileString(lpszAppName, lpszKeyName, lpszDefault, lpszStr, dwSize, m_sConfigFileFullName);
	}
}

void CConfig::SetString(LPCTSTR lpszAppName, LPCTSTR lpszKeyName, LPCTSTR lpszStr){
	if(!lpszKeyName || !lpszStr) return;
	WritePrivateProfileString(lpszAppName, lpszKeyName, lpszStr, m_sConfigFileFullName);
}
