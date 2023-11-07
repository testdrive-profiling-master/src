#include "stdafx.h"
#include "Memory.h"
#include "Paser.h"
#include "Buffer.h"

#include "TestDriveImp.h"

CMemory* CMemory::m_pHead	= NULL;

CMemory::CMemory(void){
	m_pMemory		= NULL;
	m_dwMemorySize	= 0;
	m_MemHandle		= NULL;
	m_pSharedMemory	= NULL;
	m_pNext			= NULL;

	// add linked list
	if (!m_pHead) {
		m_pHead = this;
	} else {
		CMemory* pMemory = m_pHead;
		while(pMemory){
			if(!pMemory->m_pNext){
				pMemory->m_pNext	= this;
				break;
			}
			pMemory		= pMemory->m_pNext;
		}
	}
}

CMemory::~CMemory(void){
	Release();
	// eject linked list
	if (m_pHead == this) {
		m_pHead = m_pNext;
		SetEnvironmentVariable(_T("TESTDRIVE_MEMORY"), NULL);
	} else {
		CMemory* pMemory = m_pHead;
		while(pMemory){
			if(pMemory->m_pNext == this){
				pMemory->m_pNext = m_pNext;
				break;
			}
		}
	}
}

CMemory* CMemory::Find(LPCTSTR sName){
	if(!sName) return m_pHead;
	else
	if(m_pHead){
		CMemory* pMemory = m_pHead;
		while(pMemory){
			if(!pMemory->m_sName.Compare(sName)) return pMemory;
			pMemory = pMemory->m_pNext;
		}
	}
	return NULL;
}

void CMemory::DeleteAll(void){
	CMemory* pMemory;
	while((pMemory = Find(NULL))) delete pMemory;
}

BOOL CMemory::Create(uint64_t dwByteSize, LPCTSTR lpszName)
{
	if(!dwByteSize || (!lpszName && !m_sName.GetLength())) return FALSE;
	
	Release();
	m_dwMemorySize	= dwByteSize;
	if (lpszName) {
		m_sName = lpszName;
		if (m_pHead == this) {	// first memory
			SetEnvironmentVariable(_T("TESTDRIVE_MEMORY"), lpszName);
		}
	}

	{
//RETRY_CREATE_MAP:
		// 공유 메모리 설정
		{
			uint64_t		mapped_size = dwByteSize + sizeof(TESTDRIVE_CONFIG);
			m_MemHandle = CreateFileMapping(INVALID_HANDLE_VALUE,
				NULL,
				PAGE_READWRITE,
				(mapped_size >> 32) & 0xFFFFFFFF,
				mapped_size & 0xFFFFFFFF,
				lpszName);
		}

		if (GetLastError() == ERROR_ALREADY_EXISTS) {	// 이미 열려 있었다면, 프로젝트가 중복 열림으로 인식하고 종료.
			if(m_MemHandle) CloseHandle(m_MemHandle);
			g_pTestDrive->LogError(_T("Already mapping device is created! : %s"), lpszName);
			//MessageBox(NULL, _T("Already mapping device is created!"), _T("Error"), MB_ICONEXCLAMATION|MB_OK);
			return FALSE;
		}
	}

	if ( m_MemHandle == NULL ) {
		g_pTestDrive->LogError(_T("Unable to create memory mapping device! : %s"), lpszName);
		//MessageBox(NULL, _T("Unable to create memory mapping device!"), _T("Error"), MB_ICONEXCLAMATION|MB_OK);
		return FALSE;
	}

	m_pSharedMemory = (BYTE *)MapViewOfFile(m_MemHandle, FILE_MAP_ALL_ACCESS, 0, 0, dwByteSize + sizeof(TESTDRIVE_CONFIG)); 

	if ( m_pSharedMemory == NULL )
	{
		CloseHandle(m_MemHandle);
		g_pTestDrive->LogError(_T("Unable to create map view. Error. : %s"), lpszName);
		//MessageBox(NULL, _T("Unable to create map view. Error."), _T("Error"), MB_ICONEXCLAMATION|MB_OK);
		return FALSE;
	}

	m_pMemory = m_pSharedMemory + sizeof(TESTDRIVE_CONFIG);
	ZeroMemory(GetConfig(), sizeof(TESTDRIVE_CONFIG));

	GetConfig()->dwMemorySize	= dwByteSize;

	return TRUE;
}

void CMemory::Release(void)
{
	if(m_pSharedMemory){
		UnmapViewOfFile(m_pSharedMemory);
		m_pSharedMemory = NULL;
	}
	if(m_MemHandle){
		CloseHandle(m_MemHandle);
		m_MemHandle		= NULL;
	}
	m_dwMemorySize	= 0;
	m_pMemory		= NULL;
}

BOOL CMemory::IsInitialized(void){
	return (m_pSharedMemory != NULL);
}

TESTDRIVE_CONFIG* CMemory::GetConfig(void){
	return (TESTDRIVE_CONFIG*)m_pSharedMemory;
}

LPCTSTR CMemory::GetName(void){
	return (LPCTSTR)m_sName;
}

uint64_t CMemory::GetSize(void){
	return m_dwMemorySize;
}

DWORD g_DitherPattern[]={	0,4,1,5,	//  0  8  2 10
							6,2,7,3,	// 12  4 14  6
							1,5,0,4,	//  3 11  1  9
							6,3,7,2};	// 15  7 13  5

BOOL CMemory::IsValidAddress(uint64_t dwAddress){
	return (dwAddress < m_dwMemorySize);
}

BYTE* CMemory::GetPointer(uint64_t dwAddress, uint64_t dwSize){
	if(dwSize) dwSize--;
	if(!IsValidAddress(dwAddress + dwSize)) return NULL;
	return &m_pMemory[dwAddress];
}

BOOL CMemory::Load(MEM_DISC disc, const TCHAR* lpszFileName, uint64_t offset){
	CPaser src;
	if(!m_pMemory || offset>=m_dwMemorySize) return FALSE;
	if(!src.Create(lpszFileName)) return FALSE;

	BYTE* pMem = &m_pMemory[offset];

	switch(disc) {
	case MEM_DISC_SIMUL:
		{
			LPCTSTR	pLine;
			DWORD	count		= 0;
			BOOL	halfbyte	= FALSE;
			BYTE	data		= 0;
			TCHAR	ch;
			while(src.NewLine()){
				pLine	= src.GetCurLine();
				while(1){
					ch = *pLine; pLine++;
					if(!ch) break;
					if(ch>=_T('a') && ch<=_T('f')) ch += (_T('A')-_T('a'));
					if(ch >= _T('0') && ch <= _T('9'))		ch -= _T('0');
					else if(ch>=_T('A') && ch <=_T('F'))	ch = ch - _T('A') + 10;
					else continue;

					data <<= 4;
					data |= ch;
					if(halfbyte){
						if(offset>=m_dwMemorySize) goto MEMORY_ACCESS_OVERFLOW;
						*pMem = data;offset++;pMem++;
						halfbyte = FALSE;
					}else halfbyte = TRUE;
				}
			}
		}
		break;
	case MEM_DISC_BINARY:
		{
			src.Release();
			if(!src.Create(lpszFileName, CFile::modeRead|CFile::typeBinary)) return FALSE;
			uint64_t read_size = src.GetFileSize();
			if((read_size+offset) > m_dwMemorySize) read_size = m_dwMemorySize -= offset;
			src.Read(pMem, read_size);
		}break;
	case MEM_DISC_FLOAT:
		{
			float fData;
			while(src.NewLine()) while (1){
				if(!src.IsTokenable()) break;
				if(src.GetTokenFloat(&fData)){
					if(offset >= m_dwMemorySize) goto MEMORY_ACCESS_OVERFLOW;
					(*(float*)pMem) = fData;
					pMem	+= 4;
					offset	+= 4;
				}else goto INVALID_SYNTAX;
			}
		}
		break;
	case MEM_DISC_DWORD:
	case MEM_DISC_WORD:
	case MEM_DISC_BYTE:
		{
			int iData;
			while(src.NewLine()) while (1){
				if(!src.IsTokenable()) break;
				if(src.GetTokenInt(&iData)){
					if(offset >= m_dwMemorySize) goto MEMORY_ACCESS_OVERFLOW;
					switch(disc){
					case MEM_DISC_DWORD:	(*(int*)pMem)	= iData;			pMem += 4;	offset += 4; break;
					case MEM_DISC_WORD:		(*(short*)pMem) = (short)iData;		pMem += 2;	offset += 2; break;
					case MEM_DISC_BYTE:		(*(char*)pMem) = (char)iData;		pMem += 1;	offset += 1; break;
					}
				}else break;
			}
		}
		break;
	case MEM_DISC_TEXT:
		{
			int addr = 0;
			while(src.NewLine()) while (1){
				if(!src.IsTokenable()) break;
				if(!src.GetTokenText(&pMem[addr], addr)) {
					goto INVALID_SYNTAX;
					break;
				}
				if(addr+offset > m_dwMemorySize) goto MEMORY_ACCESS_OVERFLOW;
			}
		}
		break;
	default:return FALSE;
	}

	return TRUE;
INVALID_SYNTAX:
	AfxMessageBox(_TEXT_(_T("Invalid syntax at line(%d) : %s"), src.GetLineCount(), src.GetCurLine()));
	return FALSE;
MEMORY_ACCESS_OVERFLOW:
	AfxMessageBox(_T("Memory access overflow is occurred."));
	return FALSE;
}

BOOL CMemory::Save(MEM_DISC disc, LPCTSTR lpszFileName, uint64_t offset, uint64_t size, DWORD stride)
{
	CPaser dest;
	if (!m_pMemory || (offset+size) > m_dwMemorySize) return FALSE;

	BYTE* pMem = &m_pMemory[offset];

	switch(disc){
	case MEM_DISC_SIMUL:
		{
			if(!stride) return FALSE;
			if(!dest.Create(lpszFileName, CFile::modeWrite|CFile::typeText|CFile::modeCreate)) return FALSE;
			while(size >= stride){
				for(int i=stride;i>0;i--) {dest.AppendText(_T("%02X"), *pMem);pMem++;}
				size-=stride;
				dest.AppendText(_T("\n"));
			}
		}
		break;
	case MEM_DISC_BINARY:
		{
			if(!dest.Create(lpszFileName, CFile::modeWrite|CFile::typeBinary|CFile::modeCreate)) return FALSE;
			dest.Write(pMem, size);
		}
		break;
	default:return FALSE;
	}
	return TRUE;
}


BOOL CMemory::LoadImage(LPCTSTR lpszFileName, uint64_t dwOffset, COLORFORMAT iColorType, DWORD stride){
	CBuffer	img;
	if(!img.CreateFromFile(lpszFileName, iColorType)) return FALSE;
	
	{	// memory validation check
		DWORD LineBytes		= img.Width() * img.ColorBitCount()/8;
		if(!stride || stride<LineBytes) stride = LineBytes;
		uint64_t LastAddress	= dwOffset + (img.Height()*stride) - 1;
		if(!IsValidAddress(LastAddress)) return FALSE;
	}
	{	// Copy to memory
		BYTE* pMem = &m_pMemory[dwOffset];
		if(!img.CopyToMemory(pMem, stride)) return FALSE;
	}

	return TRUE;
}

BOOL CMemory::StoreImage(LPCTSTR lpszFileName, uint64_t dwOffset, DWORD dwWidth, DWORD dwHeight, COLORFORMAT iColorType, BOOL bStoreAlpha, DWORD stride){
	CBuffer	img;
	if(!dwWidth || !dwHeight) return FALSE;

	if(!img.Create(dwWidth, dwHeight, iColorType)) return FALSE;
	
	{	// memory validation check
		DWORD LineBytes		= dwWidth * img.ColorBitCount()/8;
		if(!stride || stride<LineBytes) stride = LineBytes;
		uint64_t LastAddress	= dwOffset + (dwHeight*stride) - 1;
		if(!IsValidAddress(LastAddress)) return FALSE;
	}
	{	// Copy from memory
		BYTE* pMem = &m_pMemory[dwOffset];
		if(!img.CopyFromMemory(pMem, stride)) return FALSE;
	}
	{
		CString ext;
		{	// get file part
			TCHAR	Path[1024];
			LPTSTR	pFilePart	= NULL;
			GetFullPathName(lpszFileName, 1024, Path, &pFilePart);
			if(!pFilePart) return FALSE;
			ext = pFilePart;
		}
		if(!img.SaveToFile(lpszFileName, IMAGETYPE_AUTO, bStoreAlpha)) return FALSE;
	}

	return TRUE;
}
