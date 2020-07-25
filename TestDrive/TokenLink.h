#pragma once

class TokenLink
{
public:
	TokenLink(LPCTSTR lpszToken = NULL, DWORD_PTR iID = (DWORD_PTR)-1);
	virtual ~TokenLink(void);

	void SetToken(LPCTSTR lpszToken, DWORD_PTR iID = (DWORD_PTR)-1);
	void AddLink(TokenLink* pLink);
	void RemoveLink(TokenLink* pLink);
	TokenLink* GetNext(void);
	CString& GetToken(void);
	CString& GetParamToken(void);
	DWORD_PTR GetID(void);
	BOOL IsParamterize(void);

protected:
	BOOL			m_bParamterize;
	CString			m_sParamToken;		// "%" 문자 이전까지의 토큰
	CString			m_sToken;			// 전체 토큰
	DWORD_PTR		m_dwID;
	TokenLink*		m_pNext;
};
