#pragma once
#include "TestDrive.h"

class CMemory :
	public ITDMemory
{
public:
	CMemory(void);
	virtual ~CMemory(void);

	STDMETHOD_(BOOL, Create)(uint64_t dwByteSize, LPCTSTR lpszName);
	STDMETHOD_(void, Release)(void);
	STDMETHOD_(BOOL, IsInitialized)(void);
	STDMETHOD_(uint64_t, GetSize)(void);
	STDMETHOD_(BOOL, IsValidAddress)(uint64_t dwAddress);
	STDMETHOD_(BYTE*, GetPointer)(uint64_t dwAddress = 0, uint64_t dwSize = 0);
	STDMETHOD_(BOOL, Load)(MEM_DISC disc, LPCTSTR lpszFileName, uint64_t offset);
	STDMETHOD_(BOOL, Save)(MEM_DISC disc, LPCTSTR lpszFileName, uint64_t offset, uint64_t size, DWORD stride = 0);
	STDMETHOD_(BOOL, LoadImage)(LPCTSTR lpszFileName, uint64_t dwOffset, COLORFORMAT iColorType, DWORD stride = 0);
	STDMETHOD_(BOOL, StoreImage)(LPCTSTR lpszFileName, uint64_t dwOffset,  DWORD dwWidth, DWORD dwHeight, COLORFORMAT iColorType, BOOL bStoreAlpha = FALSE, DWORD stride = 0);
	STDMETHOD_(TESTDRIVE_CONFIG*, GetConfig)(void);
	STDMETHOD_(LPCTSTR, GetName)(void);

	static CMemory* GetHead(void)	{return m_pHead;}
	static CMemory* Find(LPCTSTR sName);
	static void DeleteAll(void);

protected:
	BYTE*				m_pMemory;
	uint64_t				m_dwMemorySize;

private:
	HANDLE				m_MemHandle;
	BYTE*				m_pSharedMemory;
	static CMemory*		m_pHead;
	CMemory*				m_pNext;
	CString				m_sName;

	void SetNext(CMemory* pMemory);
};
