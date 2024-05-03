/*
	Copyright(C) Nishant Sivakumar (nish@voidnish.com)
	Please maintain this copyright notice if you distribute this file
	in original or modified form.
*/

#include "StdAfx.h"
#include "smtpconnection2.h"

BOOL CSMTPConnection2::Connect(LPCTSTR lpszHostDomain, 
							   DWORD dwTimeout /*= 10000*/)
{
	CSimpleArray<CString> arrSMTP;
	_GetSMTPList(lpszHostDomain, arrSMTP);
	for(int i=0; i<arrSMTP.GetSize(); i++)
	{
		if(CSMTPConnection::Connect(arrSMTP[i], dwTimeout) == TRUE)
			return TRUE;
	}
	return FALSE;
}

void CSMTPConnection2::_GetSMTPList(LPCTSTR lpszHostDomain, 
								   CSimpleArray<CString>& arrSMTP)
{
	PDNS_RECORD pRec = NULL;
	if(DnsQuery(lpszHostDomain, DNS_TYPE_MX, DNS_QUERY_STANDARD,
		NULL, &pRec, NULL) == ERROR_SUCCESS)
	{
		PDNS_RECORD pRecOrig = pRec;
		while(pRec)
		{
			if(pRec->wType == DNS_TYPE_MX)
				arrSMTP.Add(pRec->Data.MX.pNameExchange);
			pRec = pRec->pNext;
		}
		DnsRecordListFree(pRecOrig,DnsFreeRecordList);
	}
}
