#pragma once
#include "Output.h"
#include "TokenLink.h"
#include "TestDrive.h"

class CRedirectExecute
{
public:
	//	constructor / destructor
	CRedirectExecute(LPCTSTR lpszCommand, COutput *pMsg, LPCTSTR lpszArgument = NULL, LPCTSTR lpszWorkDir = NULL);
	virtual ~CRedirectExecute(void);

	//	public member functions
	virtual BOOL	Run(void);
	virtual void	Stop(void);
	int				GetErrorCode(void);
	void			AddErrorToken(LPCTSTR error_token, DWORD_PTR dwID = 0);
	void			ClearErrorToken(void);

	void			SetLogFunction(TD_REDIRECT_LOG func);

	BOOL			SendKey(char ch);

	static CRedirectExecute*	GetCurrent(void)	{return m_pCurrent;}

protected:
	//	member functions
	void			AppendText(LPCTSTR Text);
	BOOL			PeekAndPump();
	void			SetSleepInterval(DWORD dwMilliseconds);
	void			ShowLastError(LPCTSTR szText);

	//	member data
	int				m_iErrorCode;
	BOOL			m_bKillDebug;
	TokenLink*		m_pErrorTokenLink;

	BOOL			m_bStopped;
	DWORD			m_dwSleepMilliseconds;
	COutput*		m_pMsg;
	CString			m_sCommand;
	CString			m_sWorkDirectory;

	HANDLE			m_hSTDIN;

	TD_REDIRECT_LOG	m_fLogFunc;

private:
	static CRedirectExecute*	m_pCurrent;
	CRedirectExecute*			m_pNext;
};
