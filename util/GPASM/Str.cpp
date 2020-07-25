#include "Str.h"
#include "ctype.h"

CStr::CStr(const char* str)
{
	m_pLastToken		= NULL;
	m_dwLastTokenType	= STRTYPE_NONE;
	m_pStr				= NULL;
	Set(str);
}

CStr::~CStr(void)
{
	Clear();
}

void CStr::Set(const char* str)
{
	Clear();
	if(!str) return;
	m_pStr = new char[strlen(str)+1];
	strcpy(m_pStr, str);
	m_pCur = m_pStr;
}

void CStr::Clear(void){
	SAFE_DELETE_ARRAY(m_pStr);
	m_pCur	= NULL;
}
BOOL CStr::IsTokenable(BOOL bContinueString){
	// space 구간 지나기
	if(!bContinueString) while(*m_pCur==' ') m_pCur++;
	if(*m_pCur && *m_pCur!=' ') return TRUE;
	return FALSE;
}

DWORD CStr::GetToken(char* tok){
	int			i		= 0;
	char		ch		= NULL;
	DWORD		type	= STRTYPE_NONE;

	if(!IsTokenable()) return type;

	//// 분류하기
	ch	= *m_pCur;
	// Hex 값일 때
	if(ch=='0' && m_pCur[1]=='x'){
		m_pCur += 2;
		do{
			ch = *m_pCur;
			if(isdigit(ch) || (ch>='a' && ch<='f') || (ch>='A' && ch<='F')){
				tok[i] = ch;i++;
				m_pCur++;
			}else{
				if(i>0) type = STRTYPE_HEX;
				tok[i]=NULL;
				break;
			}
		}while(1);
	}else
	// Binary 값일 때
	if(ch=='0' && m_pCur[1]=='b'){
		m_pCur += 2;
		do{
			ch = *m_pCur;
			if(ch=='0' || ch =='1'){
				tok[i] = ch;i++;
				m_pCur++;
			}else
			if(ch=='_' && (m_pCur[1]=='0' || m_pCur[1]=='1')) m_pCur++;
			else{
				if(i>0) type = STRTYPE_BINARY;
				tok[i]=NULL;
				break;
			}
		}while(1);
	}else
	// 숫자일 경우
	if(isdigit(ch) || (ch=='-' || ch=='+')){
		int comma = 0, digit = 0;
		do{
			ch	= *m_pCur;
			if(isdigit(ch)) digit++;
			else
			if(i==0 && (ch=='-' || ch=='+')) {}
			else
			if(ch=='.' && comma==0) comma++;
			else{
				if(digit) {
					if(comma) type = STRTYPE_FLOAT;
					else type = STRTYPE_INT;
				}else{
					if(i==1) type = STRTYPE_SPECIAL;
				}
				tok[i]=NULL;
				break;
			}
			tok[i] = ch;i++;
			m_pCur++;
		}while(1);
	}else
	// 첫 값이 문자일 경우
	if(isalpha(ch) || (ch=='_')){
		do{
			ch	= *m_pCur;

			if(i && ch == ':'){
				m_pCur++;
				type = STRTYPE_ADDRESSTAG;
				tok[i]=NULL;
				break;
			}else
			if(isalpha(ch) || isdigit(ch) || ch=='_'){
				tok[i] = ch;i++;
				m_pCur++;
			}else{
				type = STRTYPE_NAME;
				tok[i]=NULL;
				break;
			}
		}while(1);
	}else
	// 첫 값이 string 일 경우
	if(ch=='"'){
		m_pCur++;
		do{
			ch	= *m_pCur;
			if(ch==NULL) break;
			else
			if(ch=='\\'){
				m_pCur++;
				ch	= *m_pCur;
				switch(ch){
				case '\\': ch = '\\';	break;
				case 't': ch = '\t';	break;
				case 'r': ch = '\r';	break;
				case 'n': ch = '\n';	break;
				case '"': ch = '"';		break;
				default:	return STRTYPE_NONE;
				}
			}else
			if(ch=='"'){
				m_pCur++;
				type = STRTYPE_STRING;
				tok[i]=NULL;
				break;
			}
			tok[i] = ch;i++;
			m_pCur++;
		}while(1);
	}else
	// 특수 문자들
	if(ch != NULL){
		tok[0]=ch;
		tok[1]=NULL;
		m_pCur++;
		type = STRTYPE_SPECIAL;
	}

	m_pLastToken		= tok;
	m_dwLastTokenType	= type;

	return type;
}

BOOL CStr::TokenOut(const char* tok)
{
	if(!tok || !m_pCur) return FALSE;
	while(*tok){
		if(*tok!=' '){
			while(*m_pCur){
				if(*m_pCur==NULL) return FALSE;
				if(*m_pCur!=' ') break;
				m_pCur++;
			}
			if(*tok != *m_pCur) return FALSE;
			m_pCur++;
		}
		tok++;
	}
	return TRUE;
}

BOOL CStr::TokenOutable(const char* tok)
{
	while(*m_pCur==' ') m_pCur++;	// space
	if(strstr(m_pCur, tok) == m_pCur) return TRUE;
	return FALSE;
}

int CStr::RetrieveVariable(void)
{
	char ch;
	int i=0, dat=0;
	switch(m_dwLastTokenType){
	case STRTYPE_INT:
		return atoi(m_pLastToken);
	case STRTYPE_BINARY:
		while((ch = m_pLastToken[i])){
			dat <<= 1;
			if(ch=='1') dat |= 1;
			i++;
		}
		break;
	case STRTYPE_HEX:
		sscanf(m_pLastToken, "%x", &dat);
		break;
	case STRTYPE_FLOAT:
		{
			float fdata = RetrieveFloat();
			return *(int*)&fdata;
		}
		break;
	}
	return dat;
}

float CStr::RetrieveFloat(void)
{
	float dat;
	if(m_dwLastTokenType != STRTYPE_FLOAT ||
	   (m_pLastToken[0]=='0' && (m_pLastToken[1]=='b' || m_pLastToken[1]=='x'))) return (float)RetrieveVariable();
	sscanf(m_pLastToken, "%f", &dat);
	return dat;
}