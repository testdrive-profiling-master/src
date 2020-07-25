#pragma once

class CWorkDirectory
{	
public:
	CWorkDirectory(LPCTSTR pPath);
	virtual ~CWorkDirectory(void);

	TCHAR	m_sFilePart[MAX_PATH];
	TCHAR	m_sCurrentPath[MAX_PATH];
	TCHAR	m_sPrevPath[MAX_PATH];
};
