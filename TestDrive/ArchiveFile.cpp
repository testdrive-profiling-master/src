#include "StdAfx.h"
#include "ArchiveFile.h"

CArchiveFile::CArchiveFile(void)
{
}

CArchiveFile::~CArchiveFile(void)
{
}

BOOL CArchiveFile::Open(LPCTSTR lpszFileName, OPEN_MODE mode){
	if(!CStdioFile::Open(lpszFileName, mode == MODE_LOAD ? (CFile::modeRead|CFile::typeBinary) : (CFile::modeWrite|CFile::typeBinary|CFile::modeCreate))) return FALSE;
	return TRUE;
}
CArchiveFile& CArchiveFile::operator<<(BYTE by){
	Write(&by, sizeof(BYTE));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(WORD w){
	Write(&w, sizeof(WORD));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(LONG l){
	Write(&l, sizeof(LONG));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(DWORD dw){
	Write(&dw, sizeof(DWORD));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(float f){
	Write(&f, sizeof(float));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(double d){
	Write(&d, sizeof(double));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(LONGLONG dwdw){
	Write(&dwdw, sizeof(LONGLONG));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(ULONGLONG dwdw){
	Write(&dwdw, sizeof(ULONGLONG));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(int i){
	Write(&i, sizeof(int));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(short w){
	Write(&w, sizeof(short));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(char ch){
	Write(&ch, sizeof(char));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(BYTE by){
	Read(&by, sizeof(BYTE));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(WORD w){
	Read(&w, sizeof(WORD));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(LONG l){
	Read(&l, sizeof(LONG));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(DWORD& dw){
	Read(&dw, sizeof(DWORD));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(float f){
	Read(&f, sizeof(float));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(double d){
	Read(&d, sizeof(double));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(LONGLONG dwdw){
	Read(&dwdw, sizeof(LONGLONG));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(ULONGLONG dwdw){
	Read(&dwdw, sizeof(ULONGLONG));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(int i){
	Read(&i, sizeof(int));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(short w){
	Read(&w, sizeof(short));
	return *this;
}

CArchiveFile& CArchiveFile::operator>>(char ch){
	Read(&ch, sizeof(char));
	return *this;
}

CArchiveFile& CArchiveFile::operator<<(CString &str){
	int length = str.GetLength();
	*this << length;
	Write(str.GetBuffer(), length * sizeof(TCHAR));

	return *this;
}

CArchiveFile& CArchiveFile::operator>>(CString &str){
	int length = 0;
	*this >> length;
	if(str.GetAllocLength() <= length) str.Preallocate(length+1);
	Read(str.GetBuffer(), length * sizeof(TCHAR));
	str.SetAt(length, NULL);
	return *this;
}