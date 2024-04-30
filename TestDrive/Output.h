#pragma once
#include "AccelCodeDecoder.h"
#include "OutputWnd.h"
#include "OleRichEditCtrl.h"

class COutputFrame;

class COutput :
	public COleRichEditCtrl
{
	CHARFORMAT	m_cf;
public:
	typedef enum{
		TD_OUTPUT_SYSTEM,
		TD_OUTPUT_APPLICATION,
		TD_OUTPUT_NOTIFY,
		TD_OUTPUT_SIZE
	}TD_OUTPUT;

	COutput();
	virtual ~COutput();

	operator CWnd*(){return (CWnd*)this;}

	BOOL Create(COutputWnd* pPaneWnd, CWnd* pParentWnd, CFont* pFont, UINT nID);
	void LogOut(LPCTSTR szMsg, COLORREF color = RGB(0,0,0), DWORD dwEffects = 0);
	void LogLink(LPCTSTR szLink, LPCTSTR szName, COLORREF color = RGB(0,0,255));
	void Clear(void);

protected:
	//virtual void OnAccel(ACCEL_CODE code);

	COutputWnd*	m_pParent;

protected:
	//afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg BOOL OnLink(NMHDR *pNotifyHeader, LRESULT *pResult);
	//afx_msg void OnSetFocus(CWnd* pOldWnd);
	//afx_msg void OnKillFocus(CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()
};

extern COutput g_Output[COutput::TD_OUTPUT_SIZE];
