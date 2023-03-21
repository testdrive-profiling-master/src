#pragma once
#include "ViewObject.h"
#include "Html.h"
#include "Html_old.h"
#include "AccelCodeDecoder.h"

class CHtmlCtrl :
	public CViewObject,
	public ITDHtml//,
	//public CopynPasteAction
{
	DECLARE_DYNAMIC(CHtmlCtrl)

public:
	CHtmlCtrl(void);
	virtual ~CHtmlCtrl(void);

	void SetWebView2(void);

	virtual BOOL Create(CWnd* pParentWnd);
	virtual BOOL Paser(CPaser* pPaser, int x, int y);

	STDMETHOD_(void, UpdateLayout)(void);
	STDMETHOD_(ITDObject*, GetObject)(void);
	STDMETHOD_(void, Navigate)(LPCTSTR lpszURL, LPCTSTR lpszTargetFrame = NULL);
	STDMETHOD_(BOOL, CallJScript)(LPCTSTR lpszScript, ...);
	STDMETHOD_(void, SetManager)(ITDHtmlManager* pManager, DWORD dwID = 0);

	STDMETHOD_(int, GetWidth)(void);
	STDMETHOD_(int, GetHeight)(void);

	STDMETHOD_(void, SetBlockNewWindow)(BOOL bBlock = TRUE);
	STDMETHOD_(BOOL, PutText)(LPCTSTR lpszName, LPCTSTR lpszObjectID, LPCTSTR lpszText);
	STDMETHOD_(BOOL, GetText)(LPCTSTR lpszName, LPCTSTR lpszObjectID, LPTSTR lpszText, DWORD dwSize);
	STDMETHOD_(BOOL, ClickButton)(LPCTSTR lpszText);

protected:
	DECLARE_MESSAGE_MAP()

private:
	BOOL				m_bOld;
	CHtml				m_Html;
	CHtml_old			m_Html_old;
};
