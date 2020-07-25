#pragma once

class CArchiveFile :
	public CStdioFile
{
public:
	typedef enum{
		MODE_LOAD,
		MODE_SAVE
	}OPEN_MODE;
	BOOL Open(LPCTSTR lpszFileName, OPEN_MODE mode);
	CArchiveFile& operator<<(BYTE by);
	CArchiveFile& operator<<(WORD w);
	CArchiveFile& operator<<(LONG l);
	CArchiveFile& operator<<(DWORD dw);
	CArchiveFile& operator<<(float f);
	CArchiveFile& operator<<(double d);
	CArchiveFile& operator<<(LONGLONG dwdw);
	CArchiveFile& operator<<(ULONGLONG dwdw);
	CArchiveFile& operator<<(int i);
	CArchiveFile& operator<<(short w);
	CArchiveFile& operator<<(char ch);

	CArchiveFile& operator>>(BYTE by);
	CArchiveFile& operator>>(WORD w);
	CArchiveFile& operator>>(LONG l);
	CArchiveFile& operator>>(DWORD& dw);
	CArchiveFile& operator>>(float f);
	CArchiveFile& operator>>(double d);
	CArchiveFile& operator>>(LONGLONG dwdw);
	CArchiveFile& operator>>(ULONGLONG dwdw);
	CArchiveFile& operator>>(int i);
	CArchiveFile& operator>>(short w);
	CArchiveFile& operator>>(char ch);

	CArchiveFile& operator<<(CString &str);
	CArchiveFile& operator>>(CString &str);

	CArchiveFile(void);
	~CArchiveFile(void);
};
