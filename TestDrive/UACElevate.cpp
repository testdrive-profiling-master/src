#include "StdAfx.h"
#include "UACElevate.h"
#include "VersionHelpers.h"

/*
typedef struct {
	DWORD TokenIsElevated;
} TOKEN_ELEVATION, *PTOKEN_ELEVATION;*/

#ifndef CSIDL_PROGRAM_FILES
#define CSIDL_PROGRAM_FILES 0x0026
#endif // CSIDL_PROGRAM_FILES

UACElevate::UACElevate()
{

}

UACElevate::~UACElevate()
{

}

int UACElevate::GetNeedElevate()
{
	if (IsVistaOrHigher())
	{
		BOOL bResult = FALSE;

		if (SUCCEEDED(IsElevated(&bResult)))
		{
			if (bResult == TRUE)
				return 4; // 이미 Elevation이 완료됨
			else
				return 3; // Elevation이 필요함
		}
		else
			return 2; // 상태 정보를 조회할 수 없음
	}

	return 1; // UAC가 지원되지 않는 운영체제로 판단함
}

BOOL UACElevate::IsVistaOrHigher(void)
{
	return IsWindowsVistaOrGreater();
}

HRESULT UACElevate::IsElevated(BOOL *pElevated)
{
	HRESULT hResult = E_FAIL;
	HANDLE hToken = NULL;

	if (!IsVistaOrHigher())
		return hResult;

	if (!OpenProcessToken(
		GetCurrentProcess(),
		TOKEN_QUERY,
		&hToken))
		return hResult;

	TOKEN_ELEVATION te = { 0 };
	DWORD dwReturnLength = 0;
	const int TokenElevation = 20;

	if (GetTokenInformation(
		hToken,
		(TOKEN_INFORMATION_CLASS)TokenElevation,
		&te,
		sizeof(te),
		&dwReturnLength))
	{
		hResult = te.TokenIsElevated ? S_OK : S_FALSE;

		if (pElevated)
			*pElevated = (te.TokenIsElevated != 0);
	}

	CloseHandle(hToken);
	return hResult;
}

BOOL UACElevate::ShellExecWithVerb(HWND hWnd, LPCTSTR lpVerb, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory)
{
	SHELLEXECUTEINFO executeInfo;
	memset(&executeInfo, 0, sizeof(executeInfo));

	executeInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	executeInfo.fMask = 0;
	executeInfo.hwnd = hWnd;
	executeInfo.lpVerb = lpVerb;
	executeInfo.lpFile = lpPath;
	executeInfo.lpParameters = lpParameters;
	executeInfo.lpDirectory = lpDirectory;
	executeInfo.nShow = SW_NORMAL;

	return ShellExecuteEx(&executeInfo);
}

BOOL UACElevate::ShellExecWithElevation(HWND hWnd, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory)
{
	return ShellExecWithVerb(hWnd, _T("runas"), lpPath, lpParameters, lpDirectory);
}

BOOL UACElevate::SelfExecuteWithElevation()
{
	TCHAR pszPathName[_MAX_PATH] = _T("");

	//TCHAR lpParameter[1024] = _T("");
	CString sParam	= GetCommandLine();

	/*if(__argc > 1)
	{
		for(int i=1; i<__argc; i++)
		{           
			_tcscat(lpParameter, (LPCTSTR)CString(__argv[i]));
			_tcscat(lpParameter, _T(" "));
		}
	}*/

	::GetModuleFileName(::AfxGetInstanceHandle(), pszPathName, _MAX_PATH);

	sParam.Replace(pszPathName, _T(""));
	sParam.Replace(_T("\"\""), _T(""));

	//PathRemoveFileSpec(pszPathName);
	return ShellExecWithElevation(NULL, pszPathName, sParam, _T(""));
}

BOOL UACElevate::OpenUrlWithElevation(HWND hWnd, LPCTSTR lpUrl)
{
	_TCHAR lpBuffer[MAX_PATH + 1];

	if (!SHGetSpecialFolderPath(hWnd, lpBuffer, CSIDL_PROGRAM_FILES, 0))
		return FALSE;

	_tcscat(lpBuffer, _T("\\Internet Explorer\\iexplore.exe"));
	return ShellExecWithElevation(hWnd, lpBuffer, lpUrl, _T(""));
}
