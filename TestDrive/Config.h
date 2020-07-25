#pragma once

class CConfig
{
	CString		m_sConfigFileFullName;
public:

	CConfig(void);
	~CConfig(void);

	void SetConfigFile(LPCTSTR lpszFileName);
	int GetInt(LPCTSTR lpszAppName, LPCTSTR lpszKeyName, int default_value = 0);
	void SetInt(LPCTSTR lpszAppName, LPCTSTR lpszKeyName, int set_value);
	void GetString(LPCTSTR lpszAppName, LPCTSTR lpszKeyName, LPTSTR lpszStr, DWORD dwSize, LPCTSTR lpszDefault = _T(""));
	void SetString(LPCTSTR lpszAppName, LPCTSTR lpszKeyName, LPCTSTR pStr);
};

extern CConfig	g_Config;