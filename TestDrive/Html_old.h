#pragma once
#include "ViewObject.h"
#include "TestDrive.h"
#include "AccelCodeDecoder.h"

class CHtmlCtrl;

class CHtml_old :
	public CHtmlView,
	public AccelCodeDecoder
{
	DECLARE_DYNAMIC(CHtml_old)
public:
	CHtml_old(void);
	virtual ~CHtml_old(void);

	virtual void PostNcDestroy(){}
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
		LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
		LPCTSTR lpszHeaders, BOOL* pbCancel);
// 	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid,
// 		VARIANT* pvar);

	void SetManager(ITDHtmlManager* pManager, DWORD dwID);
	BOOL CallJScript(LPCTSTR lpszScript);
	void SetBlockNewWindow(BOOL bBlock = TRUE);
	BOOL PutText(CString strFormName, CString strObjectID, CString strPutText);
	BOOL GetText(CString strFormName, CString strObjectID, CString& lpszText);
	BOOL ClickButton(LPCTSTR sObjectID);
	BOOL RunScript(LPCTSTR sScript);

	HRESULT ExecFormCommand(const GUID *pGuid, long cmdID, long cmdExecOpt, VARIANT* pInVar, VARIANT* pOutVar) const;
	HRESULT ExecFormCommand(long cmdID, long cmdExecOpt, VARIANT* pInVar = NULL, VARIANT* pOutVar = NULL) const ;

protected:

	BOOL CheckVisible(void);

	virtual void OnAccel(ACCEL_CODE code);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DWORD				m_dwID;
	BOOL				m_bBlockNewWindow;
	ITDHtmlManager*		m_pManager;

	DECLARE_MESSAGE_MAP()
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
};
