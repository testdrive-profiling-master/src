#pragma once
class CFullPath
{
public:
	CFullPath(LPCTSTR sPathName = NULL);
	CFullPath(const char* sPathName);
	~CFullPath(void);

	BOOL SetPath(LPCTSTR sPathName);
	BOOL SetPath(const char* sPathName);

	operator LPCTSTR (void) const {
		return (LPCTSTR)m_sPath;
	}

	CString&	Path(void);

protected:
	CString		m_sPath;
};

