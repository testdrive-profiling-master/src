#include "StdAfx.h"
#include "HtmlCtrl.h"
#ifdef _DEBUG 
#define new DEBUG_NEW 
#endif 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "TestDriveImp.h"

IMPLEMENT_DYNAMIC(CHtmlCtrl, CViewObject)

CHtmlCtrl::CHtmlCtrl(void)
{
}

CHtmlCtrl::~CHtmlCtrl(void)
{
}

BEGIN_MESSAGE_MAP(CHtmlCtrl, CViewObject)
END_MESSAGE_MAP()

void CHtmlCtrl::UpdateLayout(void){
	SetWindowPos(NULL, m_Layout.ViewX(), m_Layout.ViewY(), m_Layout.ViewWidth(), m_Layout.ViewHeight(), SWP_NOACTIVATE | SWP_NOZORDER);
	m_Html.ResizeEverything();
}

ITDObject* CHtmlCtrl::GetObject(void){
	return this;
}

BOOL CHtmlCtrl::Create(CWnd* pParentWnd){
	CRect rc;
	m_Layout.GetViewRect(&rc);
	if(!CWnd::Create(NULL, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_CLIPSIBLINGS, rc, pParentWnd, GetLastWindowID(pParentWnd))) return FALSE;
	BringWindowToTop();

	m_Html.Initialize(this);

	return TRUE;
}

void CHtmlCtrl::Navigate(LPCTSTR lpszURL, LPCTSTR lpszTargetFrame){
	CString sURL;
	if(!lpszURL) return;
	{
		CString sParam;
		{
			CString sUserURL(lpszURL);
			int		iPos		= 0;
			sURL	= sUserURL.Tokenize(_T("?"), iPos);
			sParam	= sUserURL.Tokenize(_T(""), iPos);
		}
		if(sURL.Find(_T("//"))<0 && sURL.Find(_T(":"))<0){
			TCHAR sPath[1024];
			GetFullPathName(sURL, 1024, sPath, NULL);
			sURL	= sPath;
		}
		if(!sParam.IsEmpty()){
			sURL.AppendFormat(_T("?%s"), sParam);
		}
	}

	m_Html.Navigate(sURL, lpszTargetFrame);
}

void CHtmlCtrl::SetManager(ITDHtmlManager* pManager, DWORD dwID){
	m_Html.SetManager(pManager, dwID);
}

int CHtmlCtrl::GetWidth(void){
	return 100;// (int)m_Html.GetWidth();
}

int CHtmlCtrl::GetHeight(void){
	return 100;// (int)m_Html.GetHeight();
}

BOOL CHtmlCtrl::Paser(CPaser* pPaser, int x, int y){
	return TRUE;
}

BOOL CHtmlCtrl::CallJScript(LPCTSTR lpszScript, ...){
	CString str;
	va_list args;
	va_start(args, lpszScript);
	str.FormatV(lpszScript, args);
	va_end(args);

	return m_Html.CallJScript(str);
}

void CHtmlCtrl::SetBlockNewWindow(BOOL bBlock){
	m_Html.SetBlockNewWindow(bBlock);
}

BOOL CHtmlCtrl::PutText(LPCTSTR lpszName, LPCTSTR lpszObjectID, LPCTSTR lpszText){
	return m_Html.PutText(lpszName, lpszObjectID, lpszText);
}

BOOL CHtmlCtrl::GetText(LPCTSTR lpszName, LPCTSTR lpszObjectID, LPTSTR lpszText, DWORD dwSize){
	CString str;
	BOOL bRet = m_Html.GetText(lpszName, lpszObjectID, str);
	if(bRet) bRet	= !_tcscpy_s(lpszText, dwSize, str);
	return bRet;
}


BOOL CHtmlCtrl::ClickButton(LPCTSTR lpszText){
	return m_Html.ClickButton(lpszText);
}
