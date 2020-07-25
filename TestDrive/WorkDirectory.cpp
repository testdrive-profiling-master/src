#include "StdAfx.h"
#include "WorkDirectory.h"

CWorkDirectory::CWorkDirectory(LPCTSTR pPath){
	GetCurrentDirectory(MAX_PATH, m_sPrevPath);
	*m_sFilePart = NULL;
	if(pPath){
		TCHAR	*pFilePart;
		if(!GetFullPathName(pPath, MAX_PATH, m_sCurrentPath, &pFilePart)) return;
		if(pFilePart){
			_tcscpy(m_sFilePart, pFilePart);
			*pFilePart = NULL;
		}
		SetCurrentDirectory(m_sCurrentPath);
	}
}

CWorkDirectory::~CWorkDirectory(void){
	SetCurrentDirectory(m_sPrevPath);
}
