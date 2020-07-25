//- -----------------------------------------------------------------
//- (C) Copyright 2000 Blake V. Miller
//- All Rights Reserved.
//- -----------------------------------------------------------------
//- File	:	CGFILTYP.CPP
//-	Author	:	Blake Miller
//-	Version	:	June 16, 2000
//-	Purpose	:	File Type Access
//- -----------------------------------------------------------------

#include "stdafx.h"
#include "winerror.h"

#ifndef __FILE_TYPE_ACCESS_H__
#include "CFileTypeAccess.h"	//	CGCFileTypeAccess
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFileTypeAccess::CFileTypeAccess()
{
}

// virtual 
CFileTypeAccess::~CFileTypeAccess()
{
}

//-	*****************************************************************
//- Functions Affecting Registry
//-	*****************************************************************

// virtual
BOOL CFileTypeAccess::RegSetAllInfo(void)
{
	RegSetExtension();
	RegSetDocumentType();
	RegSetCLSID();
	RegSetShellInfo();

	return TRUE;
}

// HKEY_CLASSES_ROOT\.<Extension>
// virtual
BOOL CFileTypeAccess::RegSetExtension(void)
{
	if( m_csExtension.IsEmpty() ){
		return TRUE;
	}

	CString csKey = _T(".") + m_csExtension;

	SetRegistryValue(HKEY_CLASSES_ROOT, csKey, _T(""), m_csDocumentClassName);

	if( !m_csShellOpenCommand.IsEmpty() ){
		csKey += _T("\\shell\\open\\command");
		SetRegistryValue(HKEY_CLASSES_ROOT, csKey, _T(""), m_csShellOpenCommand);
	}

	return TRUE;
}

// HKEY_CLASSES_ROOT\.<Document Type>
// virtual
BOOL CFileTypeAccess::RegSetDocumentType(void)
{
	if( m_csDocumentClassName.IsEmpty()){
		return TRUE;
	}

	CString csKey = m_csDocumentClassName;

	SetRegistryValue(HKEY_CLASSES_ROOT, csKey, _T(""), m_csDocumentDescription);

	// DefaultIcon
	if( !m_csDocumentDefaultIcon.IsEmpty() ){
		csKey  = m_csDocumentClassName;
		csKey += _T("\\DefaultIcon");
		SetRegistryValue(HKEY_CLASSES_ROOT, csKey, _T(""), m_csDocumentDefaultIcon);
	}

	// shell\open\command
	if( !m_csShellOpenCommand.IsEmpty() ){
		csKey  = m_csDocumentClassName;
		csKey += _T("\\shell\\open\\command");
		SetRegistryValue(HKEY_CLASSES_ROOT, csKey, _T(""), m_csShellOpenCommand);
	}

	return TRUE;
}

BOOL CFileTypeAccess::RegSetCLSID(void){
	return TRUE;
}

// 
// virtual
BOOL CFileTypeAccess::RegSetShellInfo(void){
	return TRUE;
}

//-	*****************************************************************
//- Member Variable Get/Set Methods
//-	*****************************************************************

void CFileTypeAccess::SetExtension(LPCTSTR szExtension){
	m_csExtension = szExtension;
}
void CFileTypeAccess::GetExtension(CString& csExtension) const {
	csExtension = m_csExtension;
}

void CFileTypeAccess::SetContentType(LPCTSTR szContentType){
	m_csContentType = szContentType;
}
void CFileTypeAccess::GetContentType(CString& csContentType) const {
	csContentType = m_csContentType;
}

void CFileTypeAccess::SetShellOpenCommand(LPCTSTR szShellOpenCommand){
	m_csShellOpenCommand = szShellOpenCommand;
}
void CFileTypeAccess::GetShellOpenCommand(CString& csShellOpenCommand) const {
	csShellOpenCommand = m_csShellOpenCommand;
}

void CFileTypeAccess::SetShellNewCommand(LPCTSTR szShellNewCommand){
	m_csShellNewCommand = szShellNewCommand;
}
void CFileTypeAccess::GetShellNewCommand(CString& csShellNewCommand) const {
	csShellNewCommand = m_csShellNewCommand;
}

void CFileTypeAccess::SetShellNewFileName(LPCTSTR szShellNewFileName){
	m_csShellNewFileName = szShellNewFileName;
}
void CFileTypeAccess::GetShellNewFileName(CString& csShellNewFileName) const {
	csShellNewFileName = m_csShellNewFileName;
}

void CFileTypeAccess::SetDocumentClassName(LPCTSTR szDocumentClassName){
	m_csDocumentClassName = szDocumentClassName;
}
void CFileTypeAccess::GetDocumentClassName(CString& csDocumentClassName) const {
	csDocumentClassName = m_csDocumentClassName;
}

void CFileTypeAccess::SetDocumentDescription(LPCTSTR szDocumentDescription){
	m_csDocumentDescription = szDocumentDescription;
}
void CFileTypeAccess::GetDocumentDescription(CString& csDocumentDescription) const {
	csDocumentDescription = m_csDocumentDescription;
}

void CFileTypeAccess::SetDocumentCLSID(LPCTSTR szDocumentCLSID){
	m_csDocumentCLSID = szDocumentCLSID;
}
void CFileTypeAccess::GetDocumentCLSID(CString& csDocumentCLSID) const {
	csDocumentCLSID = m_csDocumentCLSID;
}

void CFileTypeAccess::SetDocumentCurrentVersion(LPCTSTR szDocumentCurrentVersion){
	m_csDocumentCurrentVersion = szDocumentCurrentVersion;
}
void CFileTypeAccess::GetDocumentCurrentVersion(CString& csDocumentCurrentVersion) const {
	csDocumentCurrentVersion = m_csDocumentCurrentVersion;
}

void CFileTypeAccess::SetDocumentDefaultIcon(LPCTSTR szDocumentDefaultIcon){
	m_csDocumentDefaultIcon = szDocumentDefaultIcon;
}
void CFileTypeAccess::GetDocumentDefaultIcon(CString& csDocumentDefaultIcon) const {
	csDocumentDefaultIcon = m_csDocumentDefaultIcon;
}

void CFileTypeAccess::SetDocumentShellOpenCommand(LPCTSTR szDocumentShellOpenCommand){
	m_csDocumentShellOpenCommand = szDocumentShellOpenCommand;
}
void CFileTypeAccess::GetDocumentShellOpenCommand(CString& csDocumentShellOpenCommand) const {
	csDocumentShellOpenCommand = m_csDocumentShellOpenCommand;
}

// virtual 
void CFileTypeAccess::ClearAllData(void)
{
	m_csExtension.Empty();
	m_csContentType.Empty();
	m_csShellOpenCommand.Empty();
	m_csShellNewCommand.Empty();
	m_csShellNewFileName.Empty();

	m_csDocumentClassName.Empty();
	m_csDocumentDescription.Empty();
	m_csDocumentCLSID.Empty();
	m_csDocumentCurrentVersion.Empty();
	m_csDocumentDefaultIcon.Empty();
	m_csDocumentShellOpenCommand.Empty();
}

//- -----------------------------------------------------------------
//- set string value into registry

BOOL CFileTypeAccess::SetRegistryValue(HKEY	hOpenKey, LPCTSTR szKey, LPCTSTR szValue, LPCTSTR szData){
	// validate input
	if( !hOpenKey || !szKey || !szKey[0] || 
		!szValue || !szData ){
		::SetLastError(E_INVALIDARG);
		return FALSE;
	}

	BOOL 	bRetVal = FALSE;
	DWORD	dwDisposition;
	DWORD	dwReserved = 0;
	HKEY  	hTempKey = (HKEY)0;

	// length specifier is in bytes, and some TCHAR 
	// are more than 1 byte each
	DWORD	dwBufferLength = lstrlen(szData) * sizeof(TCHAR);

	// Open key of interest
	// Assume all access is okay and that all keys will be stored to file
	// Utilize the default security attributes
	if( ERROR_SUCCESS == ::RegCreateKeyEx(hOpenKey, szKey, dwReserved,
		(LPTSTR)0, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, 0,
		&hTempKey, &dwDisposition) ){
		
		// dwBufferLength must include size of terminating nul 
		// character when using REG_SZ with RegSetValueEx function
		dwBufferLength += sizeof(TCHAR);
		
		if( ERROR_SUCCESS == ::RegSetValueEx(hTempKey, (LPTSTR)szValue,
			dwReserved, REG_SZ, (LPBYTE)szData, dwBufferLength) ){
			bRetVal = TRUE;
		}
	}

	// close opened key
	if( hTempKey ){
		::RegCloseKey(hTempKey);
	}

	return bRetVal;
}
