#include "StdAfx.h"
#include "Mail.h"

CMail::CMail(void)
{
}

CMail::~CMail(void)
{
}


void CMail::SetSender(LPCTSTR lpszAddress, LPCTSTR lpszName){
	m_Message.SetSender(lpszAddress);
	m_Message.SetSenderName(lpszName);
}

void CMail::AddRecipient(LPCTSTR lpszAddress, LPCTSTR lpszName){
	m_sDomainName.Empty();
	m_Message.AddRecipient(lpszAddress, lpszName);
	if(lpszAddress){
		CString sAddress = lpszAddress;
		int pos = sAddress.Find(_T('@'));
		if(pos){
			m_sDomainName = &(sAddress.GetBuffer()[pos+1]);
		}
	}
}

void CMail::SetSubject(LPCTSTR lpszSubject){
	m_Message.SetSubject(lpszSubject);
}

void CMail::AddText(LPCTSTR lpszText){
	if(!lpszText) return;
	m_Message.AddText(lpszText);
}

void CMail::AddFile(LPCTSTR lpszFileName){
	if(lpszFileName) m_Message.AttachFile(lpszFileName);
}

BOOL CMail::Send(void){
	CWaitCursor	wc;
	CSMTPConnection2	conn;

	if(m_sDomainName.GetLength())
	if(conn.Connect(m_sDomainName)){
		BOOL bRet = conn.SendMessage(m_Message);
		conn.Disconnect();
		return bRet;
	}

	return FALSE;
}

void CMail::Release(void){
	delete this;
}