#pragma once
#include "ViewObject.h"
// CButtonCtrl

//UINT OnClickAction(LPVOID pParam);
class CButtonCtrl :
	public CViewObject,
	public ITDButton
{
	DECLARE_DYNAMIC(CButtonCtrl)

	friend UINT OnClickAction(LPVOID pParam);

public:
	CButtonCtrl();
	virtual ~CButtonCtrl();

	virtual BOOL Create(CWnd* pParentWnd);
	virtual BOOL Paser(CPaser* pPaser, int x, int y);

	STDMETHOD_(void, UpdateLayout)(void);
	STDMETHOD_(ITDObject*, GetObject)(void);
	STDMETHOD_(void, SetManager)(ITDButtonManager* pManager, DWORD dwID = 0);
	STDMETHOD_(void, SetEnable)(BOOL bEnable = TRUE);
	STDMETHOD_(void, SetText)(LPCTSTR lpszText);
	STDMETHOD_(void, SetStyle)(BUTTON_STYLE style);
	STDMETHOD_(void, SetCheck)(BOOL bCheck = TRUE);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClicked();
	afx_msg void OnNcPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	CButton				m_Button;
	ITDButtonManager*	m_pManager;
	DWORD				m_dwID;
};
