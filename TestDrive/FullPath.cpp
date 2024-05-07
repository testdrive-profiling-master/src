#include "StdAfx.h"
#include "FullPath.h"
#include "TestDriveImp.h"

CFullPath::CFullPath(LPCTSTR sPathName)
{
	SetPath(sPathName);
}

CFullPath::CFullPath(const char* sPathName)
{
	SetPath(sPathName);
}


CFullPath::~CFullPath(void)
{
}

BOOL CFullPath::SetPath(const char* sPathName) {
	CString	sPath(sPathName);
	return SetPath(sPath);
}

BOOL CFullPath::SetPath(LPCTSTR sPathName){
	if(!sPathName) m_sPath.Empty();
	else{
		CString	sPath;
		TCHAR	*pPart;
		CString	sFilePath;

		TCHAR*	pPath	= sPath.GetBuffer(1024 * 8);

		if(*sPathName == _T('%')){				// '%~%' �� �����ϸ� "~" �̸��� �˻��Ͽ� �˸��� ��θ� �ִ´�.
			CString		global_path, user_path;
			sPathName++;
			_tcscpy(pPath, sPathName);
			pPart = _tcsstr(pPath, _T("%"));
			if(!pPart){
				g_pTestDrive->LogError(_T("'%%' is must be paired with '%%'."));
				return FALSE;
			}
			*pPart		= NULL;
			user_path	= (pPart + 1);
			global_path	= pPath;

			if(global_path == _T("CURRENT")){				// ���� ��ġ
				GetCurrentDirectory(1024 * 8, pPath);
				global_path = (LPCTSTR)sPath;
			} else if(global_path == _T("PROJECT")){		// ���� ������Ʈ ��ġ
				global_path = g_pTestDrive->GetProjectPath();
			} else {
				if(!GetEnvironmentVariable(global_path, pPath, 1024 * 8)){
					global_path.Empty();
					g_pTestDrive->LogWarning(_T("Can't find environment variable(%s) with \"%s\""), (LPCTSTR)global_path, sPathName);
				}else{
					global_path = (LPCTSTR)sPath;
				}
			}

			sFilePath	= global_path + user_path;
		}else{
			sFilePath	= sPathName;
		}

		// exchange environment string table
		for (;;) {
			int iStart = sFilePath.Find(_T("$("));
			if (iStart >= 0) {
				int iEnd = sFilePath.Find(_T(")"), iStart + 2);
				if (iEnd >= 0) {	// find variable name
					CString sEnvironment;
					CString sName(((LPCTSTR)sFilePath) + iStart + 2);
					sName.Delete(iEnd - iStart - 2, sFilePath.GetLength() - iEnd);
					GetEnvironmentVariable(sName, sEnvironment.GetBuffer(1024 * 8), 1024 * 8);
					sFilePath.Delete(iStart, iEnd - iStart + 1);
					sFilePath.Insert(iStart, sEnvironment);
				} else g_pTestDrive->LogError(_T("invalid environment string : %s"), sPathName);
			}else break;
		}

		if(GetFullPathName(sFilePath, 1024 * 8, pPath, &pPart)) {
			m_sPath = (LPCTSTR)sPath;
		} else m_sPath = sFilePath;
	}
	return TRUE;
}

CString& CFullPath::Path(void){
	return	m_sPath;
}
