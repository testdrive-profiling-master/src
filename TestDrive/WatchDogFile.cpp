#include "stdafx.h"
#include "WatchDogFile.h"

//-----------------------------------------------------------
// 클라이언트 인터페이스
CWatchDogFileClient::CWatchDogFileClient(){
}

CWatchDogFileClient::~CWatchDogFileClient(){
	ClearWatchingFileAll();
}

void CWatchDogFileClient::AddWatchingFile(LPCTSTR lpszFileName, HWND hWnd, DWORD dwID, BOOL bSearchSubDir){
	if(!lpszFileName) return;
	m_WatchDogFolderList.push_back(new CWatchDogFile(lpszFileName, hWnd, dwID, bSearchSubDir));
}

void CWatchDogFileClient::ClearWatchingFile(DWORD dwID){
RETRY_CLEAR:
	for(list<CWatchDogFile*>::iterator iter = m_WatchDogFolderList.begin();iter!=m_WatchDogFolderList.end();iter++){
		if((*iter)->GetID() == dwID){
			delete (*iter);
			m_WatchDogFolderList.erase(iter);
			goto RETRY_CLEAR;
		}
	}
}

void CWatchDogFileClient::ClearWatchingFileAll(void){
	for(list<CWatchDogFile*>::iterator iter = m_WatchDogFolderList.begin();
		iter!=m_WatchDogFolderList.end();iter++){
			delete (*iter);
	}
	m_WatchDogFolderList.clear();
}

//-----------------------------------------------------------
// 구현부
DWORD WINAPI CWatchDogFile::thFnMonitor(CWatchDogFile* pWatchDog){
	HANDLE					Sema			= NULL;
	LONGLONG				uElpasedTime	= 0;
	Sema = CreateSemaphore(NULL, 1, 1024, NULL);	// for single thread execution.

	while(1){ 
		// Wait for notification.
		DWORD dwWaitStatus = WaitForMultipleObjects(2, pWatchDog->m_hChangeHandles, FALSE, INFINITE);

		if(dwWaitStatus == WAIT_OBJECT_0){
CATCH_MORE:
			FindNextChangeNotification(pWatchDog->m_hChangeHandles[0]);
			dwWaitStatus = WaitForSingleObject(pWatchDog->m_hChangeHandles[0], 300);
			if(dwWaitStatus == WAIT_OBJECT_0) goto CATCH_MORE;
			WaitForSingleObject(Sema, INFINITE);
			pWatchDog->CheckTime();			// single threaded.
			ReleaseSemaphore(Sema, 1, NULL);
		}else break;
	}
	CloseHandle(Sema);

	pWatchDog->m_bWatching	= FALSE;
	return 0;
}

CWatchDogFile::CWatchDogFile(LPCTSTR lpszFileName, HWND hWnd, DWORD dwID, BOOL bSearchSubDir){
	m_bWatching	= FALSE;

	if(!lpszFileName || !hWnd) return;

	m_hWndToNotify	= hWnd;
	m_dwID			= dwID;
	m_bSearchSubDir	= bSearchSubDir;
	m_LatestFileTime.dwHighDateTime		= 0;
	m_LatestFileTime.dwLowDateTime		= 0;
	m_hChangeHandles[0]					= INVALID_HANDLE_VALUE;	// notify for file change
	m_hChangeHandles[1]					= NULL;					// notify for end of this object
	{
		TCHAR	sPath[1024];
		LPTSTR	lpszFilePart	= NULL;

		if(!GetFullPathName(lpszFileName,1024, sPath, &lpszFilePart)) return;
		m_sPath		= sPath;
		
		if(lpszFilePart){
			*lpszFilePart	= NULL;
			m_bSearchSubDir	= FALSE;
			m_bDir			= FALSE;
		}else{
			m_bDir			= TRUE;
		}

		CheckTime(FALSE);	// m_LatestFileTime 을 현재 시간으로 변경

		m_hChangeHandles[0]		= FindFirstChangeNotification(
									sPath,								// directory to watch 
									m_bSearchSubDir,					// watch the subtree 
									FILE_NOTIFY_CHANGE_LAST_WRITE);		// watch file last write

		if (m_hChangeHandles[0] == INVALID_HANDLE_VALUE)
			return;
	}

	m_hChangeHandles[1]		= CreateSemaphore(
									NULL,   // no security attributes
									0,		// initial count
									1,		// maximum count
									NULL);	// unnamed semaphore

	if(m_hChangeHandles[1] == NULL){
		FindCloseChangeNotification(m_hChangeHandles[0]);
		m_hChangeHandles[0]		= INVALID_HANDLE_VALUE;
		return;
	}
	{
		DWORD dwThreadId;
		HANDLE hThread;
		m_bWatching		= TRUE;
		hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thFnMonitor, this, 0, &dwThreadId);
	}
}

CWatchDogFile::~CWatchDogFile(void){
	if(m_bWatching){
		ReleaseSemaphore(m_hChangeHandles[1], 1, NULL);
		while(m_bWatching){
			Sleep(0);
		}
		if (m_hChangeHandles[0] != INVALID_HANDLE_VALUE) {
			FindCloseChangeNotification(m_hChangeHandles[0]);
			m_hChangeHandles[0]	= INVALID_HANDLE_VALUE;
		}
		if (m_hChangeHandles[1]) {
			CloseHandle(m_hChangeHandles[1]);
			m_hChangeHandles[1]	= NULL;
		}
	}
}

LPCTSTR CWatchDogFile::GetPath(void){
	return (LPCTSTR)m_sPath;
}

BOOL CWatchDogFile::IsDir(void){
	return m_bDir;
}

void CWatchDogFile::CheckTime(BOOL bReportChange, LPCTSTR lpszPath, BOOL bFirst){
	if(!lpszPath) lpszPath			= m_sPath;

	if(!m_bDir){	// single file
		_WIN32_FILE_ATTRIBUTE_DATA	fileAttData;
		if(GetFileAttributesEx(lpszPath, GetFileExInfoStandard, (void*)&fileAttData)){
			FILETIME FileTime	= fileAttData.ftLastWriteTime;

			if(CompareFileTime(&FileTime, &m_LatestFileTime)==1){
				if(bReportChange)
					::SendMessage(m_hWndToNotify, CWM_FILE_CHANGED_NOTIFICATION, (WPARAM)this, (LPARAM)((LPCTSTR)lpszPath));
				
			}
		}
		return;
	}else{			// directory
		HANDLE				hFind;
		WIN32_FIND_DATA		FindFileData;
		TCHAR	sFindPath[MAX_PATH];


		_stprintf(sFindPath, _T("%s*.*"), lpszPath);
		hFind = FindFirstFile(sFindPath, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) return;

		do{
			if(FindFileData.cFileName[0]==_T('.')){
			}else
			if((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)){
				if(m_bSearchSubDir){
					CString sNextPath;
					sNextPath.Format(_T("%s%s\\"), lpszPath, FindFileData.cFileName);
					CheckTime(bReportChange, sNextPath, FALSE);
				}
			}else
			if(!(FindFileData.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN))){
				if(bReportChange)
				if(CompareFileTime(&FindFileData.ftLastWriteTime, &m_LatestFileTime)==1){
					CString sFullPath;
					sFullPath.Format(_T("%s%s"), lpszPath, FindFileData.cFileName);
					::SendMessage(m_hWndToNotify, CWM_FILE_CHANGED_NOTIFICATION, (WPARAM)this, (LPARAM)((LPCTSTR)sFullPath));
				}
			}
		}while(FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
	if(bFirst){	// 최상위 함수
		SYSTEMTIME	sysTime;
		GetSystemTime(&sysTime);	// 파일에는 localtime 이 쓰이지 않고 UTC 타임이 사용된다.
		//GetLocalTime(&sysTime);
		SystemTimeToFileTime(&sysTime, &m_LatestFileTime);
	}
}

DWORD CWatchDogFile::GetID(void){
	return m_dwID;
}
