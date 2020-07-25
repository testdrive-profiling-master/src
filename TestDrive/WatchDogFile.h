#pragma once


#define CWM_FILE_CHANGED_NOTIFICATION		(WM_USER+1)

//----------------------------------------------------------
// CWatchDogFolder
class CWatchDogFile;

class CWatchDogFileClient
{
protected:
	list<CWatchDogFile*>	m_WatchDogFolderList;

public:
	void AddWatchingFile(LPCTSTR lpszFileName, HWND hWnd, DWORD dwID, BOOL bSearchSubDir);
	void ClearWatchingFile(DWORD dwID);
	void ClearWatchingFileAll(void);

	CWatchDogFileClient();
	virtual ~CWatchDogFileClient();
};

//----------------------------------------------------------
// CWatchDogFolder
class CWatchDogFile
{
	static DWORD WINAPI thFnMonitor(CWatchDogFile* pWatchDog);
	friend class CWatchDogFileClient;

protected:
	HWND						m_hWndToNotify;
	volatile BOOL				m_bWatching;
	CString						m_sPath;
	HANDLE						m_hChangeHandles[2];
	DWORD						m_dwID;
	BOOL						m_bDir;
	BOOL						m_bSearchSubDir;

	CWatchDogFile(LPCTSTR lpszFileName, HWND hWnd, DWORD dwID, BOOL bSearchSubDir);
	virtual ~CWatchDogFile(void);
	
public:
	LPCTSTR GetPath(void);
	DWORD GetID(void);
	BOOL IsDir(void);

protected:

	FILETIME			m_LatestFileTime;

	void CheckTime(BOOL bReportChange = TRUE, LPCTSTR lpszPath = NULL, BOOL bFirst = TRUE);
};
