#include "stdafx.h"
#include "RichEditControl50W.h"

HINSTANCE	CRichEditControl50W::m_hInstRichEdit50W	= NULL;
DWORD		CRichEditControl50W::m_dwRefcount		= 0;

// CRichEditControl50W
IMPLEMENT_DYNAMIC(CRichEditControl50W, CWnd)
CRichEditControl50W::CRichEditControl50W()
{
	if (!m_hInstRichEdit50W) {
		if (!(m_hInstRichEdit50W = LoadLibrary(_T("msftedit.dll")))) {
			AfxMessageBox(_T("MSFTEDIT.DLL Didn't Load"));
			return;
		}
	}
	m_dwRefcount++;
}

CRichEditControl50W::~CRichEditControl50W()
{
	m_dwRefcount--;
	//Free the MSFTEDIT.DLL library
	if (!m_dwRefcount) {
		if (m_hInstRichEdit50W) {
			FreeLibrary(m_hInstRichEdit50W);
			m_hInstRichEdit50W = NULL;
		}
	}
}

BEGIN_MESSAGE_MAP(CRichEditControl50W, CWnd)
END_MESSAGE_MAP()

BOOL CRichEditControl50W::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CWnd* pWnd = this;
	BOOL bRet = pWnd->Create(_T("RichEdit50W"), NULL, dwStyle, rect, pParentWnd, nID);
	SendMessage(EM_SETLANGOPTIONS, 0, (SendMessage(EM_GETLANGOPTIONS, 0, 0)& ~(IMF_AUTOFONT | IMF_AUTOFONTSIZEADJUST)));	// 자동 폰트 변경 방지
	return bRet;
}

void CRichEditControl50W::SetTextTo50WControl(CString csText, int nSTFlags, int nSTCodepage)
{	//Set the options. SETTEXTEX m_st50W declared in RichEditControl50W.h
	m_st50W.codepage = nSTCodepage;	
	m_st50W.flags = nSTFlags;
	SendMessage(EM_SETTEXTEX, (WPARAM)&m_st50W, (LPARAM)(LPCTSTR)csText);
}
void CRichEditControl50W::LimitText50W(int nChars)
{
	SendMessage(EM_LIMITTEXT, nChars, 0);
}

void CRichEditControl50W::SetOptions50W(WORD wOp, DWORD dwFlags)
{
	SendMessage(EM_SETOPTIONS, (WPARAM)wOp, (LPARAM)dwFlags);
}

DWORD CRichEditControl50W::SetEventMask50W(DWORD dwEventMask)
{
	return (DWORD)SendMessage(EM_SETEVENTMASK, 0, dwEventMask);
}

void CRichEditControl50W::GetTextRange50W(int ncharrMin, int ncharrMax)
{
	//Set the CHARRANGE for the trRE50W = the characters sent by ENLINK 
	m_trRE50W.chrg.cpMin = ncharrMin;
	m_trRE50W.chrg.cpMax = ncharrMax;

	//Set the size of the character buffers, + 1 for null character
	int nLength = int((m_trRE50W.chrg.cpMax - m_trRE50W.chrg.cpMin +1));

	//create an ANSI buffer and a Unicode (Wide Character) buffer
	m_lpszChar = new CHAR[nLength];
	LPWSTR lpszWChar = new WCHAR[nLength];

	//Set the trRE50W LPWSTR character buffer = Unicode buffer
	m_trRE50W.lpstrText = lpszWChar;

	//Get the Unicode text
	SendMessage(EM_GETTEXTRANGE, 0,  (LPARAM) &m_trRE50W);  

	// Convert the Unicode RTF text to ANSI.
	WideCharToMultiByte(CP_ACP, 0, lpszWChar, -1, m_lpszChar, nLength, NULL, NULL);

	//Release buffer memory
	delete lpszWChar;

	return;
}

