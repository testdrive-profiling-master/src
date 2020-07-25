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
	CString			m_sParamToken;		// "%" ���� ���������� ��ū
	CString			m_sToken;			// ��ü ��ū
	DWORD_PTR		m_dwID;
	TokenLink*		m_pNext;
};
