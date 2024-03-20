#include "StdAfx.h"
#include "TokenLink.h"

TokenLink::TokenLink(LPCTSTR lpszToken, DWORD_PTR dwID){
	m_bParamterize		= FALSE;
	m_pNext				= NULL;
	SetToken(lpszToken, dwID);
}

TokenLink::~TokenLink(void){
	if(m_pNext) delete m_pNext;
}

void TokenLink::SetToken(LPCTSTR lpszToken, DWORD_PTR dwID){
	if(lpszToken){
		m_sToken			= lpszToken;
		{
			LPCTSTR	sParam	= NULL;
			{
				LPCTSTR	sTok	= lpszToken;
				for(;;){
					if(!(sTok = _tcsstr(sTok, _T("%")))) break;

					if(sTok[1] == _T('%')) sTok+=2;
					else {
						sParam = sTok;
						break;
					}
				}
			}

			if(sParam){
				int		iParamterOffset;
				iParamterOffset		= (int)(sParam - lpszToken);
				m_sParamToken		= m_sToken.Left(iParamterOffset);
				if (m_sToken.GetAt(iParamterOffset) == _T('%')) {	// %.10lf -> %lf 로 변경해야 한다. 일부 로케일에서 비정상 동작하므로.
					for (int i = iParamterOffset + 1; i < m_sToken.GetLength(); i++) {
						TCHAR	ch	= m_sToken.GetAt(i);

						if (isdigit(ch) || ch == _T('-') || ch == _T('+') || ch == _T('.')) {
							m_sToken.Delete(i); i--;
						} else break;
					}
				}
				m_bParamterize		= TRUE;
			}else{
				m_sParamToken		= m_sToken;
			}
		}
	}
	m_dwID = dwID;
}

void TokenLink::AddLink(TokenLink* pLink){
	if(m_pNext) m_pNext->AddLink(pLink);
	else m_pNext = pLink;
}

void TokenLink::RemoveLink(TokenLink* pLink){
	if(!pLink) return;
	if(m_pNext == pLink){
		m_pNext = pLink->m_pNext;
		pLink->m_pNext = NULL;
		delete m_pNext;
	}else if(m_pNext) m_pNext->RemoveLink(pLink);
}

TokenLink* TokenLink::GetNext(void){
	return m_pNext;
}

CString& TokenLink::GetToken(void){
	return m_sToken;
}

CString& TokenLink::GetParamToken(void){
	return m_sParamToken;
}

DWORD_PTR TokenLink::GetID(void){
	return m_dwID;
}

BOOL TokenLink::IsParamterize(void){
	return m_bParamterize;
}
