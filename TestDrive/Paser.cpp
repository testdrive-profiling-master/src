#include "stdafx.h"
#include "Paser.h"

CPaser::CPaser(void)
{
	m_pCur	= NULL;
	Release();
}

CPaser::~CPaser(void)
{
	Release();
}

BOOL CPaser::Create(LPCTSTR szFileName, UINT nOpenFlags)
{
	Release();
	if(!m_File.Open(szFileName, nOpenFlags)) return FALSE;
	m_sFileName				= szFileName;
	m_pCur					= m_sLine.GetBuffer();
	return TRUE;
}

void CPaser::Release(void)
{
	if(m_File.m_pStream) m_File.Close();
	m_sLine.Empty();
	m_dwCurrentLineCount	= 0;
	m_bMultilineTokenable	= FALSE;
	m_bIsComment			= FALSE;
	m_bNewLine				= FALSE;
	m_pCur					= NULL;
	m_sFileName.Empty();
}

LPCTSTR CPaser::GetFilePath(void){
	return m_sFileName;
}

BOOL CPaser::NewLine(void){
	m_bNewLine	= TRUE;
RE_CHECK_NEW_LINE:
	
	//m_File.
	if(!m_File.ReadString(m_sLine)) return FALSE;
	m_dwCurrentLineCount++;
	TrimComment(m_sLine.GetBuffer());
	{
		int iLength = m_sLine.GetLength();
		if(!iLength) goto RE_CHECK_NEW_LINE;
		while(m_sLine.GetAt(iLength-1) == _T('\\')){
			iLength--;
			
			m_sLine.SetAt(iLength, _T(' '));
			{
				CString newStr;
				if(!m_File.ReadString(newStr)) return FALSE;
				TrimComment(newStr.GetBuffer());
				m_sLine += newStr;
			}
			m_dwCurrentLineCount++;
			{
				int NewLengh = m_sLine.GetLength();
				if(iLength==NewLengh) return FALSE;
				iLength = NewLengh;
			}
		}
	}
	m_pCur		= m_sLine.GetBuffer();
	return TRUE;
}

const LPCTSTR CPaser::GetCurLine(void){
	return (LPCTSTR)m_sLine;
}
/*
void CPaser::TrimToken(const TCHAR* token_ring)
{
	if(token_ring)
	while(strchr(token_ring, *m_sLine)) _tcscpy(m_sLine, &m_sLine[1]);
}

BOOL CPaser::CheckDelimiter(const TCHAR cDelimiter)
{
	//if()
	return TRUE;
}

TCHAR* CPaser::SearchToken(const TCHAR* token_ring)
{
	int i, len = strlen(token_ring);
	TCHAR* pTok;

	for(i=0;i<len;i++){
		pTok = strchr(m_sLine, token_ring[i]);
		if(pTok) return pTok;
	}

	return NULL;
}
*/
BOOL CPaser::IsTokenable(void){
	while(*m_pCur==_T(' ')) m_pCur++;	// space 구간 제외
	if(*m_pCur) return TRUE;
	else if(m_bMultilineTokenable) return NewLine();
	return FALSE;
}

TD_TOKEN_TYPE CPaser::GetToken(TCHAR* tok){
	int				i		= 0;
	TD_TOKEN_TYPE	type	= TD_TOKEN_NULL;
	TCHAR			ch, ch2;
	
	tok[0] = NULL;

	if(!IsTokenable()) return type;
	
	//// 분류하기
	ch	= *m_pCur;
	ch2	= m_pCur[1];

	// HEX 값일 때
	if(ch==_T('0') && ch2==_T('x')){
		m_pCur += 2;
		type = TD_TOKEN_HEX;
		do{
			ch = *m_pCur;
			if(isdigit(ch) || (ch>=_T('a') && ch<=_T('f')) || (ch>=_T('A') && ch<=_T('F'))){
				tok[i] = ch;i++;
				m_pCur++;
			}else{
				if(!i) type = TD_TOKEN_ERROR;
				tok[i]=NULL;
				break;
			}
		}while(1);
	}else
	// Binary 값일 때
	if(ch==_T('0') && m_pCur[1]==_T('b')){
		m_pCur += 2;
		do{
			ch = *m_pCur;
			if(ch==_T('0') || ch ==_T('1')){
				tok[i] = ch;i++;
				m_pCur++;
			}else
				if(ch==_T('_') && (m_pCur[1]==_T('0') || m_pCur[1]==_T('1'))) m_pCur++;
				else{
					if(i>0) type = TD_TOKEN_BINARY;
					tok[i]=NULL;
					break;
				}
		}while(1);
	}else
	// 숫자일 경우
	if(isdigit(ch) || ((ch==_T('-') || ch==_T('+')) && isdigit(ch2))){
		int comma = 0, digit = 0;
		type = TD_TOKEN_INT;
		if(!isdigit(ch)){
			tok[0] = ch;
			i++;m_pCur++;
		}
		
		do{
			ch	= *m_pCur;
			if(isdigit(ch)) digit++;
			else
			if(ch==_T('.')){
				if(!comma){
					type = TD_TOKEN_FLOAT;
				}else type = TD_TOKEN_ERROR;
				comma++;
			}else{
				tok[i]=NULL;
				break;
			}
			tok[i] = ch;i++;
			m_pCur++;
		}while(1);
	}else
	// 첫 값이 문자이거나 '_'로 시작할 경우
	if(isalpha(ch) || (ch==_T('_'))){
		type = TD_TOKEN_NAME;
		do{
			ch	= *m_pCur;
			if(isalpha(ch) || isdigit(ch) || ch==_T('_')){
				tok[i] = ch;i++;
				m_pCur++;
			}else{
				tok[i]=NULL;
				break;
			}
		}while(1);
	}else
	// 첫 값이 string 일 경우
	if(ch==_T('"')){
		m_pCur++;	// 첫 '"' 세미콜론 넘어가기
		type = TD_TOKEN_STRING;
		do{
			ch	= *m_pCur;
			if(ch==NULL){
				type = TD_TOKEN_ERROR;
				break;
			}else
			if(ch==_T('\\')){
				m_pCur++;
				ch	= *m_pCur;
				switch(ch){
				case _T('\\'): ch = _T('\\');	break;
				case _T('t'): ch = _T('\t');	break;
				case _T('r'): ch = _T('\r');	break;
				case _T('n'): ch = _T('\n');	break;
				case _T('"'): ch = _T('"');		break;
				default: type = TD_TOKEN_ERROR; break;
				}
			}else
			if(ch==_T('"')){
				m_pCur++;
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
		type = TD_TOKEN_DELIMITER;
	}
	return type;
}


BOOL CPaser::TokenOut(const TCHAR* tok)
{
	if(!tok) return FALSE;
	if(!IsTokenable()) return FALSE;
	while(*tok){
		if(*tok != *m_pCur) return FALSE;
		m_pCur++;
		tok++;
	}
	return TRUE;
}

const TCHAR *g_PaserDelimiter[TD_DELIMITER_SIZE]={
	_T(":"),
	_T(";"),
	_T("."),
	_T(","),
	_T("("),
	_T(")"),
	_T("["),
	_T("]"),
	_T("{"),
	_T("}"),
};

BOOL CPaser::TokenOut(TD_DELIMITER_TYPE type){
	return TokenOut(g_PaserDelimiter[type]);
}
/*
BOOL CPaser::TokenOutable(const TCHAR* tok)
{
	while(*m_pCur==_T(' ')) m_pCur++;	// space
	if(_tcsstr(m_pCur, tok) == m_pCur) return TRUE;
	return FALSE;
}*/

BOOL CPaser::GetTokenName(TCHAR* token_name)
{
	if(GetToken(token_name) != TD_TOKEN_NAME) return FALSE;
	return TRUE;
}

BOOL CPaser::GetTokenString(TCHAR* token_str)
{
	if(GetToken(token_str) != TD_TOKEN_STRING) return FALSE;
	return TRUE;
}

DWORD StrGetBinary(TCHAR* token)
{
	TCHAR	ch;
	DWORD	data = 0;
	if(token)
	while(1){
		ch = *token;
		if(!ch) break;
		if(ch == _T('0')){
			data <<= 1;
		}else
		if(ch == _T('1')){
			data <<= 1;
			data |= 1;
		}else
		if(ch != _T('_')) break;
	}
	return data;
}

BOOL CPaser::GetTokenInt(int* token_int)
{
	TCHAR temp[256];
	TD_TOKEN_TYPE	type = GetToken(temp);
	switch(type){
	case TD_TOKEN_INT:
		*token_int = StrToInt(temp);
		break;
	case TD_TOKEN_HEX:
		_stscanf(temp, _T("%x"),token_int);
		break;
	case TD_TOKEN_BINARY:
		*token_int = StrGetBinary(temp);
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CPaser::GetTokenInt64(int64_t* token_int)
{
	TCHAR temp[256];
	TD_TOKEN_TYPE	type = GetToken(temp);
	switch (type) {
	case TD_TOKEN_INT:
		if(!StrToInt64Ex(temp, STIF_DEFAULT, (LONGLONG*)token_int)) return false;
		break;
	case TD_TOKEN_HEX:
		_stscanf(temp, _T("%llx"), token_int);
		break;
	case TD_TOKEN_BINARY:
		*token_int = StrGetBinary(temp);
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CPaser::GetTokenFloat(float* token_float)
{
	TCHAR temp[256];
	TD_TOKEN_TYPE	type = GetToken(temp);
	switch(type){
	case TD_TOKEN_INT:
		{
			int idata = StrToInt(temp);
			*token_float = (float)idata;
		}
		break;
	case TD_TOKEN_HEX:
		{
			int idata;
			_stscanf(temp, _T("%x"),&idata);
			*token_float = (float)idata;
		}
		break;
	case TD_TOKEN_BINARY:
		*token_float = (float)StrGetBinary(temp);
		break;
	case TD_TOKEN_FLOAT:
		_stscanf(temp, _T("%f"), token_float);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CPaser::GetTokenDouble(double* token_double)
{
	TCHAR temp[256];
	TD_TOKEN_TYPE	type = GetToken(temp);
	switch(type){
	case TD_TOKEN_INT:
		{
			int idata = StrToInt(temp);
			*token_double = (double)idata;
		}
		break;
	case TD_TOKEN_HEX:
		{
			LONGLONG ldata;
			_stscanf(temp, _T("%llx"),&ldata);
			*token_double = (double)ldata;
		}
		break;
	case TD_TOKEN_BINARY:
		*token_double = (double)StrGetBinary(temp);
		break;
	case TD_TOKEN_FLOAT:
		_stscanf(temp, _T("%le"), token_double);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

DWORD Float32to24(DWORD x_f32)
{
	DWORD		Man_Part	= 0;
	DWORD		Exp_Part	= 0;

	DWORD		x_ui32		= 0;
	DWORD	x_f24		= 0;


	x_ui32 = x_f32;

	Exp_Part = (x_ui32 >> 23) & 0xFF;
	Man_Part = x_ui32 & 0x7FFFFF;
	Man_Part += (x_ui32 & 0x80);
	if(Exp_Part==0) return 0;

	Exp_Part = (Exp_Part - 127) + 63;
	Man_Part = Man_Part >> (23 - 16);


	x_f24 = (x_ui32 >> 8) & 0x800000;			// Sign
	x_f24 |= ((Exp_Part&0x7F) << 16);
	x_f24 += Man_Part;		// 

	return x_f24;
}

BOOL CPaser::GetTokenText(PVOID pMEM, int& address)
{
	TCHAR line[128];
	int		idx=0;

	TokenOut(_T(","));

	if(!GetTokenName(line))return FALSE;
	if(!TokenOut(g_PaserDelimiter[TD_DELIMITER_SOPEN])) return FALSE;

	switch(line[0])
	{
	case _T('F'):	// float : F64/F32/F24
		{
			_stscanf(&line[1], _T("%d"), &idx);

			switch(idx)
			{
			case 64:
				{
					if(!GetTokenDouble((double*)pMEM)) return FALSE;
					address += 8;
				}break;
			case 32:
			case 24:
				{
					if(!GetTokenFloat((float*)pMEM)) return FALSE;
					if(idx==24) *(DWORD*)pMEM = Float32to24(*(DWORD*)pMEM);
					address += 4;
				}break;
			default:
				address = -1;
				return FALSE;
			}
		}
		break;
	case _T('I'):	// integer : I8/I16/I32
		{
			_stscanf(&line[1], _T("%d"), &idx);
			int idata;
			if(!GetTokenInt(&idata)) return FALSE;
			switch(idx)
			{
			case 8:
				*(char*)pMEM = (char)idata;
				address += 1;
				break;
			case 16:
				*(short*)pMEM = (short)idata;
				address += 2;
				break;
			case 32:
				*(int*)pMEM = (int)idata;
				address += 4;
				break;
			default:
				address = -1;
				return FALSE;
			}
		}
		break;
	case _T('S'):	// stride : S8/S16/S32
		{
			_stscanf(&line[1], _T("%d"), &idx);
			int iCount;
			if(!GetTokenInt(&iCount)) return FALSE;
			if((idx%8) || idx<8 || iCount<0) return FALSE;
			address = iCount * (idx>>3);
		}
		break;
	default:
		address = -1;
		return FALSE;
	}

	if(!TokenOut(g_PaserDelimiter[TD_DELIMITER_SCLOSE])) return FALSE;

	return TRUE;
}

BOOL CPaser::TrimSpace(LPTSTR pLine, BOOL bTrimStringChar)
{
	int len, is_string=0;
	if(!pLine) return FALSE;
	// 앞 SPACE 제거
	while(pLine[0]==_T(' ') || pLine[0]==_T('\t')) _tcscpy_s(pLine, _tcslen(pLine), (pLine+1));
	if(bTrimStringChar) if(pLine[0]==_T('\"')) {_tcscpy_s(pLine, _tcslen(pLine), (pLine+1));is_string++;}
	// 뒤 SPACE 제거
	while(1){
		len = (int)_tcslen(pLine);
		if (len)
			if(pLine[len-1]==_T(' ') || pLine[len-1]==_T('\t'))
			{pLine[len-1]=NULL; continue;}
			break;
	}
	if(bTrimStringChar){
		len = (int)_tcslen(pLine);
		if(len){
			if(pLine[len-1]==_T('\"')) {pLine[len-1]=NULL;is_string++;}
		}
	}
	if(is_string==2) return TRUE;
	return FALSE;
}

void CPaser::TrimComment(LPTSTR pLine)
{
	if(!pLine)return;

	{
		LPTSTR	chars;
		// Enter코드 제거
		chars=_tcschr(pLine,_T('\n'));	if(chars!=NULL)*chars=NULL;
		chars=_tcschr(pLine,_T('\r'));	if(chars!=NULL)*chars=NULL;
	}

	{
		LPTSTR	pTrim		= m_bIsComment ? pLine : NULL;
		LPTSTR	pCur		= pLine;
		BOOL	bOnString	= FALSE;

		do{
			if(*pCur == _T('\"')){
				if((pCur==pLine) || !bOnString) bOnString = !bOnString;
				else if(pCur[-1] != _T('\\')) bOnString = !bOnString;
			}
			else
			if(m_bIsComment){
				if(!bOnString && *pCur==_T('*'))
				if(pCur[1]=='/'){
					_tcscpy(pTrim, (pCur+2));pCur = pTrim-1;
					pTrim			= NULL;
					m_bIsComment	= FALSE;
				}
			}else if(!bOnString){
				switch(*pCur){
				case _T('\t'):
					*pCur = _T(' ');
				case _T(' '):
					if(pCur[1]==_T(' ') || pCur[1]==_T('\t')){
						_tcscpy(pCur+1, (pCur+2));
						pCur--;
					}
					break;
				case _T('/'):
					if(pCur[1]==_T('*')){
						m_bIsComment = TRUE;
						pTrim = pCur;pCur++;
					}else
					if(pCur[1]==_T('/')){
						*pCur = NULL;
						goto TRIM_COMMENT_OUT;
					}
					break;
				}
			}
			pCur++;
		}while(*pCur);
TRIM_COMMENT_OUT:
		if(pTrim) *pTrim = NULL;
	}

	// 앞뒤 불필요한 스페이스 제거
	TrimSpace(pLine);
}

BOOL CPaser::Seek(size_t offset, int origin)
{
	if(m_File.m_pStream) return !m_File.Seek(offset, origin);
	return 0;
}

DWORD CPaser::Read(void* pBuff, DWORD max_size)
{
	if(!m_File.m_pStream || !pBuff || !max_size) return 0;
	return (DWORD)m_File.Read(pBuff, max_size);
}

void CPaser::Write(const void* pBuff, DWORD max_size)
{
	if(!m_File.m_pStream || !pBuff || !max_size) return;
	m_File.Write(pBuff, max_size);
}

uint64_t CPaser::GetFileSize(void)
{
	if(!m_File.m_pStream) return 0;
	return m_File.GetLength();
}

DWORD CPaser::AppendText(LPCTSTR fmt, ...)
{
	if(!m_File.m_pStream) return 0;
	
	CString str;
	va_list args;
	va_start(args, fmt);
	str.FormatV(fmt,args);
	va_end(args);

	m_File.WriteString(str);

	//WideCharToMultiByte(CP_ACP, 0, wszBuffer, -1, szDstBuffer, 128, NULL, NULL);


	return str.GetLength();
}