#pragma once
class CFullPath
{
public:
	CFullPath(LPCTSTR sPathName = NULL);
	~CFullPath(void);

	BOOL SetPath(LPCTSTR sPathName);

	CString&	Path(void);

protected:
	CString		m_sPath;
};

