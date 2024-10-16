#include "StdAfx.h"
#include "ProfileData.h"
#include "TestDriveImp.h"
#include "FullPath.h"

CProfileData::CProfileData(LPCTSTR sPathName){
	Path(sPathName);
}

CProfileData::~CProfileData(void){
}

CString& CProfileData::Path(LPCTSTR sPathName){
	if(sPathName){
		CFullPath	full_path(sPathName);
		m_sPathName	= full_path.Path();
	}
	return m_sPathName;
}

void CProfileData::OnSelect(void){
	if(m_sPathName.IsEmpty()) return;

	/*
	if(CRedirectExecute::GetCurrent() && m_sPathName.GetAt(m_sPathName.GetLength()-1) == _T('*'))
	{
		// 이미 프로파일이 실행 중일 경우...
		g_pTestDrive->LogInfo(_S(ANOTHER_PROFILE_IS_RUNNING));
		return;
	}*/
	CString sExt = GetFileExtension(m_sPathName);
	if(!sExt.Compare(_T("sp")) || !sExt.Compare(_T("profile")) || !sExt.Compare(_T("lua")))
		g_pTestDrive->Build(m_sPathName, 0, TRUE);
	else
		ShellExecute(NULL, NULL, m_sPathName, NULL, NULL, SW_SHOW);
}