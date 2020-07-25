#pragma once
#include "SMTPConnection2.h"
#include "TestDrive.h"

class CMail : public ITDMail
{
	CMimeMessage	m_Message;
	CString			m_sDomainName;
public:
	CMail(void);
	virtual ~CMail(void);

	// sender's mail address and name
	STDMETHOD_(void, SetSender)(LPCTSTR lpszAddress, LPCTSTR lpszName = NULL);
	// recipient's mail address and name
	STDMETHOD_(void, AddRecipient)(LPCTSTR lpszAddress, LPCTSTR lpszName = NULL);
	// mail title
	STDMETHOD_(void, SetSubject)(LPCTSTR lpszSubject);
	// add text of mail body
	STDMETHOD_(void, AddText)(LPCTSTR lpszText);
	// add file
	STDMETHOD_(void, AddFile)(LPCTSTR lpszFileName);
	// send email operation
	STDMETHOD_(BOOL, Send)(void);
	// release this object : make sure to call this in the end of process
	STDMETHOD_(void, Release)(void);
};
