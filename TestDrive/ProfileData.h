#pragma once
#include "ViewTree.h"

class CProfileData : public CTreeObject
{
	CString		m_sPathName;
public:
	CProfileData(LPCTSTR sPathName);
	virtual ~CProfileData(void);

	CString& Path(LPCTSTR sPathName = NULL);

	virtual void OnSelect(void);
};
