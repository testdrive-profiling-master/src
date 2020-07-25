#pragma once
#include "TestDrive.h"

class CMemory :
	public ITDMemory
{
public:
	CMemory(void);
	virtual ~CMemory(void);

	STDMETHOD_(BOOL, Create)(DWORD dwByteSize, LPCTSTR lpszName);
	STDMETHOD_(void, Release)(void);
	STDMETHOD_(BOOL, IsInitialized)(void);
	STDMETHOD_(DWORD, GetSize)(void);
	STDMETHOD_(BOOL, IsValidAddress)(DWORD dwAddress);
	STDMETHOD_(BYTE*, GetPointer)(DWORD dwAddress = 0, DWORD dwSize = 0);
	STDMETHOD_(BOOL, Load)(MEM_DISC disc, LPCTSTR lpszFileName, DWORD offset);
	STDMETHOD_(BOOL, Save)(MEM_DISC disc, LPCTSTR lpszFileName, DWORD offset, DWORD size, DWORD stride = 0);
	STDMETHOD_(BOOL, LoadImage)(LPCTSTR lpszFileName, DWORD dwOffset, COLORFORMAT iColorType, DWORD stride = 0);
	STDMETHOD_(BOOL, StoreImage)(LPCTSTR lpszFileName, DWORD dwOffset,  DWORD dwWidth, DWORD dwHeight, COLORFORMAT iColorType, BOOL bStoreAlpha = FALSE, DWORD stride = 0);
	STDMETHOD_(TESTDRIVE_CONFIG*, GetConfig)(void);
	STDMETHOD_(LPCTSTR, GetName)(void);

	static CMemory* GetHead(void)	{return m_pHead;}
	static CMemory* Find(LPCTSTR sName);
	static void DeleteAll(void);

protected:
	BYTE*				m_pMemory;
	DWORD				m_dwMemorySize;

private:
	HANDLE				m_MemHandle;
	BYTE*				m_pSharedMemory;
	static CMemory*		m_pHead;
	CMemory*			m_pNext;
	CString				m_sName;

	void SetNext(CMemory* pMemory);
};
