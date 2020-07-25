#include "StdAfx.h"
#include "FullPath.h"
#include "TestDriveImp.h"

CFullPath::CFullPath(LPCTSTR sPathName)
{
	SetPath(sPathName);
}


CFullPath::~CFullPath(void)
{
}

BOOL CFullPath::SetPath(LPCTSTR sPathName){
	if(!sPathName) m_sPath.Empty();
	else{
		TCHAR	path[1024], *pPart;
		CString	sFilePath;

		if(*sPathName == _T('%')){				// '%~%' 로 시작하면 "~" 이름을 검색하여 알맞은 경로를 넣는다.
			CString		global_path, user_path;
			sPathName++;
			_tcscpy(path, sPathName);
			pPart = _tcsstr(path, _T("%"));
			if(!pPart){
				g_pTestDrive->LogError(_T("'%%' is must be paired with '%%'."));
				return FALSE;
				//ASSERT(pPart != NULL);
			}
			*pPart = NULL;
			user_path	= (pPart + 1);
			global_path	= path;

			if(!global_path.Compare(_T("CURRENT"))){		// 현재 위치
				GetCurrentDirectory(1024, path);
				global_path = path;
			}else
			if(!global_path.Compare(_T("PROJECT"))){		// 현재 프로젝트 위치
				global_path = g_pTestDrive->GetProjectPath();
			}else{
				if(!GetEnvironmentVariable(global_path, path, 1024)){
					global_path.Empty();
					g_pTestDrive->LogWarning(_T("Can't find environment variable(%s) with \"%s\""), global_path, sPathName);
				}else{
					global_path = path;
				}
			}

			sFilePath	= global_path + user_path;
		}else{
			sFilePath	= sPathName;
		}

		// exchange environment string table
		for(;;){
			int iStart = sFilePath.Find(_T("$("));
			if (iStart >= 0) {
				int iEnd = sFilePath.Find(_T(")"), iStart+2);
				if (iEnd >= 0) {
					TCHAR sEnvironment[1024] = _T("");
					CString sName(((LPCTSTR)sFilePath) + iStart + 2);
					sName.Delete(iEnd - iStart - 2, sFilePath .GetLength() - iEnd);
					GetEnvironmentVariable(sName, sEnvironment, 1024);
					sFilePath.Delete(iStart, iEnd - iStart + 1);
					sFilePath.Insert(iStart, sEnvironment);
				}else g_pTestDrive->LogError(_T("invalid environment string : %s"), sPathName);
			}else break;
		}

		if(GetFullPathName(sFilePath, MAX_PATH, path, &pPart)){
			m_sPath = path;
		}else m_sPath = sFilePath;
	}
	return TRUE;
}

CString& CFullPath::Path(void){
	return	m_sPath;
}