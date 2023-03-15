#pragma once
#include <stdio.h>

typedef enum{
	TD_TOKEN_NULL	 = 0,
	TD_TOKEN_INT,
	TD_TOKEN_FLOAT,
	TD_TOKEN_HEX,
	TD_TOKEN_BINARY,
	TD_TOKEN_NAME,
	TD_TOKEN_STRING,
	TD_TOKEN_DELIMITER,
	TD_TOKEN_ERROR
}TD_TOKEN_TYPE;

typedef enum{
	TD_DELIMITER_COLON,		// ":"
	TD_DELIMITER_SEMICOLON,	// ";"
	TD_DELIMITER_PERIOD,	// "."
	TD_DELIMITER_COMMA,		// ","
	TD_DELIMITER_SOPEN,		// "("
	TD_DELIMITER_SCLOSE,	// ")"
	TD_DELIMITER_MOPEN,		// "["
	TD_DELIMITER_MCLOSE,	// "]"
	TD_DELIMITER_LOPEN,		// "{"
	TD_DELIMITER_LCLOSE,	// "}"
	TD_DELIMITER_SIZE
}TD_DELIMITER_TYPE;

extern const TCHAR *g_PaserDelimiter[TD_DELIMITER_SIZE];

class CPaser
{
protected:
	CString		m_sFileName;
	CStdioFile	m_File;
	BOOL		m_bIsComment;
	BOOL		m_bMultilineTokenable;
	BOOL		m_bNewLine;
	DWORD		m_dwCurrentLineCount;
	CString		m_sLine;
	LPTSTR		m_pCur;

	
	BOOL TrimSpace(LPTSTR pLine, BOOL bTrimStringChar = FALSE);
	void TrimComment(LPTSTR pLine);
	/*void TrimToken(const TCHAR* token_ring);
	TCHAR* SearchToken(const TCHAR* token_ring);
	*/
	
public:
	CPaser(void);
	virtual ~CPaser(void);
	BOOL Create(LPCTSTR szFileName, UINT nOpenFlags = CFile::modeRead|CFile::typeText);
	void Release(void);
	LPCTSTR GetFilePath(void);
	BOOL NewLine(void);
	const LPCTSTR GetCurLine(void);
	
	void SetMultilineTokenable(BOOL bEnable){m_bMultilineTokenable = bEnable;}
	DWORD GetLineCount(void){return m_dwCurrentLineCount;}
	/*BOOL CheckDelimiter(const TCHAR cDelimiter);*/
	BOOL IsTokenable(void);
	TD_TOKEN_TYPE GetToken(TCHAR* tok);
	BOOL TokenOut(const TCHAR* tok);
	BOOL TokenOut(TD_DELIMITER_TYPE type);
	/*BOOL TokenOutable(const TCHAR* tok);*/
	BOOL GetTokenName(TCHAR* token_name);
	BOOL GetTokenString(TCHAR* token_str);
	BOOL GetTokenInt(int* token_int);
	BOOL GetTokenInt64(int64_t* token_int);
	BOOL GetTokenFloat(float* token_float);
	BOOL GetTokenDouble(double* token_double);
	BOOL GetTokenText(PVOID pMEM, int& address);
	BOOL Seek(size_t offset, int origin = SEEK_SET);
	uint64_t GetFileSize(void);
	DWORD Read(void* pBuff, DWORD max_size);
	void Write(const void* pBuff, DWORD max_size);
	DWORD AppendText(LPCTSTR fmt, ...);
};
