#pragma once

class RegistryCtrl{
public:
	RegistryCtrl(void);
	virtual ~RegistryCtrl(void);

	UINT GetProfileInt(LPCTSTR lpKeyName, INT nDefault);
	CString GetProfileString(LPCTSTR lpKeyName, LPCTSTR lpDefault);

	BOOL SetProfileInt(LPCTSTR lpKeyName, int nValue);
	BOOL SetProfileString(LPCTSTR lpKeyName, LPCTSTR lpString);
};

extern RegistryCtrl	g_RegistryCtrl;
