#include "stdafx.h"

// �� ������ �ڵ� ��ȣ ���
UINT GetLastWindowID(CWnd* pWnd){
	UINT nID	= AFX_IDW_PANE_FIRST;
	while(pWnd->GetDlgItem(nID)) nID++;
	return nID;
}

// formating �׽�Ʈ ����
LPCTSTR _TEXT_(LPCTSTR fmt, ...){
	static CString str;
	va_list args;
	va_start(args, fmt);
	str.FormatV(fmt,args);
	va_end(args);

	return str;
}

#define XP_COMPATIBILITY

BOOL SetRegistryDWORD(HKEY hKey, LPCTSTR sKeyPath, LPCTSTR sName, DWORD dwValue){
	HKEY key;
	if(RegOpenKeyEx(hKey, sKeyPath, 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS){
		if(RegCreateKeyEx(hKey, sKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL) != ERROR_SUCCESS)
			return FALSE;
	}
	{
		DWORD type = 0;
		BOOL bUpdate	= FALSE;
		DWORD dwData;
		DWORD ret_size	= sizeof(DWORD);

#ifdef XP_COMPATIBILITY
		if(SHRegGetValue(key, NULL, sName, SRRF_RT_REG_DWORD, &type, &dwData, &ret_size) != ERROR_SUCCESS)
#else
		if(RegGetValue(key, NULL, sName, RRF_RT_REG_DWORD, &type, &dwData, &ret_size) != ERROR_SUCCESS)
#endif
			dwData = 0;

		if(dwData != dwValue){
			RegSetValueEx(key, sName, 0, REG_DWORD, (const BYTE*)(&dwValue), sizeof(DWORD));
			bUpdate	= TRUE;
		}

		RegCloseKey(key);
		if(bUpdate){	// ȯ�� ���� ����
			DWORD dwReturnValue = 0;
			SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) _T("Environment"), SMTO_ABORTIFHUNG, 5000, (PDWORD_PTR)&dwReturnValue);
		}
	}
	return TRUE;
}

BOOL SetRegistryString(HKEY hKey, LPCTSTR sKeyPath, LPCTSTR sName, LPCTSTR sValue){
	CString sNewValue(sValue);
	HKEY key;
	if(RegOpenKeyEx(hKey, sKeyPath, 0, KEY_ALL_ACCESS, &key) != ERROR_SUCCESS){
		if(RegCreateKeyEx(hKey, sKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, NULL) != ERROR_SUCCESS)
			return FALSE;
	}
	{
		DWORD type;
		BOOL bUpdate	= FALSE;
		TCHAR path[1024];
		DWORD ret_size	= sizeof(TCHAR)*1024;

#ifdef XP_COMPATIBILITY
		if(SHRegGetValue(key, NULL, sName, SRRF_RT_REG_SZ, &type, path, &ret_size) != ERROR_SUCCESS)
#else
		if(RegGetValue(key, NULL, sName, RRF_RT_REG_SZ, &type, path, &ret_size) != ERROR_SUCCESS)
#endif
			path[0] = NULL;

		if(sNewValue.Compare(path)){
			RegSetValueEx(key, sName, 0, REG_SZ, (const BYTE*)((LPCTSTR)sNewValue), (sNewValue.GetLength()+1)*sizeof(TCHAR));
			bUpdate	= TRUE;
		}

		RegCloseKey(key);
		if(bUpdate){	// ȯ�� ���� ����
			DWORD_PTR dwReturnValue = 0;
			SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) _T("Environment"), SMTO_ABORTIFHUNG, 5000, &dwReturnValue);
		}
	}
	return TRUE;
}

void SetGlobalEnvironmentVariable(LPCTSTR sName, LPCTSTR sValue){
	CString sNewValue(sValue);
	HKEY key;
	//if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"), 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS){
	if(RegOpenKeyEx(HKEY_CURRENT_USER, _T("Environment"), 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS){
		DWORD type;
		BOOL bUpdate	= FALSE;
		TCHAR path[1024];
		DWORD ret_size	= sizeof(TCHAR)*1024;

#ifdef XP_COMPATIBILITY
		if(SHRegGetValue(key, NULL, sName, SRRF_RT_REG_SZ, &type, path, &ret_size) != ERROR_SUCCESS)
#else
		if(RegGetValue(key, NULL, sName, RRF_RT_REG_SZ, &type, path, &ret_size) != ERROR_SUCCESS)
#endif
			path[0] = 0;

		if(sNewValue.Compare(path)){
			RegSetValueEx(key, sName, 0, REG_SZ, (const BYTE*)((LPCTSTR)sNewValue), (sNewValue.GetLength()+1)*sizeof(TCHAR));
			SetEnvironmentVariable(sName, sValue);
			bUpdate	= TRUE;
		}

		RegCloseKey(key);
		if(bUpdate){	// ȯ�� ���� ����
			DWORD_PTR dwReturnValue = 0;
			SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) _T("Environment"), SMTO_ABORTIFHUNG, 5000, &dwReturnValue);
		}
	}
}

CString GetGlobalEnvironmentVariable(LPCTSTR sName){

	HKEY key;
	TCHAR sValue[4096];
	DWORD ret_size	= sizeof(TCHAR)*4096;
	memset(sValue, 0, ret_size);

	if(RegOpenKeyEx(HKEY_CURRENT_USER, _T("Environment"), 0, KEY_READ, &key) == ERROR_SUCCESS){
		DWORD type;
		BOOL bUpdate	= FALSE;

#ifdef XP_COMPATIBILITY
		if(SHRegGetValue(key, NULL, sName, SRRF_RT_REG_SZ, &type, sValue, &ret_size) != ERROR_SUCCESS)
#else
		if(RegGetValue(key, NULL, sName, RRF_RT_REG_SZ, &type, sValue, &ret_size) != ERROR_SUCCESS)
#endif

			RegCloseKey(key);
	}
	return CString(sValue);
}

void ModifyGlobalEnvironmentPath(LPCTSTR sPathAdd, LPCTSTR sPathRemove, BOOL bHeadToAdd){
	static LPCTSTR	sPathEnvironment	= _T("Path");
	BOOL bChanged						= FALSE;
	CString sEnv						= GetGlobalEnvironmentVariable(sPathEnvironment);

	if(!sPathAdd || sEnv.Find(sPathAdd)<0){
		// �ߺ� ��� ����
		if(sPathRemove){
			CString sFind(sPathRemove);
			sFind.MakeLower();
			sFind.Replace(_T("/"), _T("\\"));

			for(;;){
				CString sTemp = sEnv;
				int iFirst, iEnd;
				sTemp.MakeLower();
				sTemp.Replace(_T("/"), _T("\\"));
				int iTag = sTemp.Find(sFind);
				if(iTag<0) break;
				for(iFirst=iTag;iFirst>0;iFirst--){
					if(sTemp.GetAt(iFirst) == _T(';')) break;
				}
				for(iEnd=iTag;sTemp.GetAt(iEnd+1) != _T('\0');iEnd++){
					if(sTemp.GetAt(iEnd) == _T(';')){
						if(iFirst) iEnd--;
						break;
					}
				}
				sEnv.Delete(iFirst, iEnd-iFirst+1);
				bChanged	= TRUE;
			}
		}

		// ��� �߰�
		if(sPathAdd){
			if(sEnv.GetLength()){
				if(bHeadToAdd){
					CString sOld	= sEnv;
					sEnv.Format(_T("%s;%s"), sPathAdd, (LPCTSTR)sOld);
				}else{
					sEnv.AppendFormat(_T(";%s"), sPathAdd);
				}
			}else{
				sEnv	= sPathAdd;
			}
			bChanged	= TRUE;
		}

		// �۷ι� ��κ����� ����
		if(bChanged){
			HKEY key;
			if(RegOpenKeyEx(HKEY_CURRENT_USER, _T("Environment"), 0, KEY_WRITE, &key) == ERROR_SUCCESS){
				RegSetValueEx(key, sPathEnvironment, 0, REG_SZ, (const BYTE*)((LPCTSTR)sEnv), (sEnv.GetLength()+1)*sizeof(TCHAR));
				RegCloseKey(key);
				{	// ����� ȯ�� ���� broadcast
					DWORD_PTR dwReturnValue = 0;
					SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM) _T("Environment"), SMTO_ABORTIFHUNG, 5000, &dwReturnValue);
				}
			}
			// ���� ȯ�溯��
			SetEnvironmentVariable(sPathEnvironment, sEnv);
		}
	}
}

int CheckCommand(const TCHAR* token, const TCHAR** cmd_list, DWORD size){
	DWORD i;
	for(i=0;i<size;i++) if(!_tcscmp(token, cmd_list[i])) break;
	return i;
}

DWORD64 GetFileModifiedTime(LPCTSTR lpszFilePath){
	DWORD64	fTime	= 0;
	HANDLE	hFile	= CreateFile(lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME	ft;
		//SYSTEMTIME	stUTC, stLocal;
		GetFileTime(hFile, NULL, NULL, &ft);
		//FileTimeToSystemTime(&ft, &stUTC);
		//SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);			
		memcpy(&fTime, &ft, sizeof(DWORD64));
		CloseHandle(hFile);
	}

	return fTime;
}

BOOL IsFileExist(LPCTSTR lpPathName){
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	TCHAR	sPath[4096];
	{
		CString sModify(lpPathName);
		sModify.Replace(_T("\""), _T(""));	// @FIXME �����ڵ�
		GetFullPathName(sModify, MAX_PATH, sPath, NULL);
	}
	
	if ((hFind = FindFirstFile (sPath, &fd)) != INVALID_HANDLE_VALUE) {
		FindClose (hFind);
		return TRUE;
	}
	return FALSE;
}

#include "TestDriveImp.h"

BOOL IsSourceTreeChanged(LPCTSTR sSearchPath, LPCTSTR* sExtensions, LPCTSTR sTargetPath){
	ULONG64			file_time	= GetFileModifiedTime(sTargetPath);
	CString			sSearch(sSearchPath);
	BOOL			bModified	= FALSE;
	WIN32_FIND_DATA	FindFileData;
	HANDLE			hFind;

	// check dependancy script 
	{
		CString	sScriptPath(sSearch + _T(".TestDrive"));
		TCHAR sDependencies[4096];
		GetPrivateProfileString(_T("TestDrive"), _T("dependency_include"), _T(""), sDependencies, 4096, sScriptPath);
		if (*sDependencies) {
			CString	sList(sDependencies);
			for(int iPos = 0;;) {
				CString ExtraDependencyPath;
				{	// get tokenize from dependency include path list
					CString sToken = sList.Tokenize(_T(";"), iPos);
					if (iPos < 0) break;
					sToken.TrimLeft(_T(" "));
					sToken.TrimRight(_T(" "));
					CWorkDirectory work_dir(sSearch);
					CFullPath	FullPath(sToken);
					ExtraDependencyPath = FullPath.Path();
				}
				if (IsSourceTreeChanged(ExtraDependencyPath, sExtensions, sTargetPath))
					return TRUE;
			}
		}
	}

	// Check modification on current folder
	for(int i=0;sExtensions[i] && !bModified;i++){
		hFind	= FindFirstFile(sSearch + _T("*.") + sExtensions[i], &FindFileData);
		if(hFind != INVALID_HANDLE_VALUE){
			do{
				if(GetFileModifiedTime(sSearch + FindFileData.cFileName) > file_time){
					bModified	= TRUE;
					break;
				}
			}while(FindNextFile(hFind, &FindFileData));
			FindClose(hFind);
		}
	}

	return bModified;
}

CString GetFileExtension(LPCTSTR sFileName){
	CString sExt(sFileName);
	int iPoint;

	if((iPoint = sExt.ReverseFind(_T('/'))) != -1)	sExt.Delete(0, iPoint+1);
	if((iPoint = sExt.ReverseFind(_T('\\'))) != -1)	sExt.Delete(0, iPoint+1);

	if((iPoint = sExt.ReverseFind(_T('.'))) != -1){
		sExt.Delete(0, iPoint+1);
		sExt.MakeLower();
	}else{
		sExt.Empty();
	}
	return sExt;
}

// ����/��� ��� ���� ����
void PreventMonitorPowerdown(void)
{
	SetThreadExecutionState(/*ES_DISPLAY_REQUIRED |*/ ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
}

// ����/��� ��� ���� ���
void AllowMonitorPowerdown(void)
{
	SetThreadExecutionState(/*~ES_DISPLAY_REQUIRED &*/ ES_CONTINUOUS & ~ES_SYSTEM_REQUIRED);
}

// GDI+ Ȱ��ȭ �ϱ� ���� Ŭ����
class CGdiPlusStarter
{
private:
	ULONG_PTR m_gpToken;

public:
	bool m_bSuccess;
	CGdiPlusStarter() {
		GdiplusStartupInput gpsi;
		m_bSuccess=(GdiplusStartup(&m_gpToken,&gpsi,NULL) == Ok);
	}
	~CGdiPlusStarter() {
		GdiplusShutdown(m_gpToken);
	}
};

CGdiPlusStarter g_GDI_Plus;
