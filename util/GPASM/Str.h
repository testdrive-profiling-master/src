#pragma once
#include "Log.h"

#define	STRTYPE_NONE		0x00000000
#define STRTYPE_INT			0x00000001
#define STRTYPE_FLOAT		0x00000002
#define STRTYPE_HEX			0x00000004
#define STRTYPE_BINARY		0x00000008
#define STRTYPE_NAME		0x00000010
#define STRTYPE_ADDRESSTAG	0x00000020
#define STRTYPE_LINK		0x00000040
#define STRTYPE_STRING		0x00000080
#define STRTYPE_SPECIAL		0x00000100
#define STRTYPE_FUNCTION	0x00000200

class CStr
{
public:
	char*	m_pStr;
	char*	m_pCur;
	char*	m_pLastToken;
	DWORD	m_dwLastTokenType;
	CStr(const char* str = NULL);
	~CStr(void);

	void Set(const char* str);
	char* Get(BOOL bCur = FALSE) {return bCur ? m_pCur : m_pStr;}
	void Clear(void);

	DWORD GetToken(char* tok);
	BOOL IsTokenable(BOOL bContinueString = FALSE);
	BOOL TokenOut(const char* tok);

	BOOL TokenOutable(const char* tok);

	int RetrieveVariable(void);
	float RetrieveFloat(void);
};
