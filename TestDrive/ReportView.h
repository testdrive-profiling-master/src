#pragma once
#include "OleRichEditCtrl.h"
#include "ViewObject.h"

#include "TestDrive.h"

class CReportView :
	public CViewObject,
	public ITDReport
{
	DECLARE_DYNAMIC(CReportView)

public:
	CReportView();
	virtual ~CReportView();

	virtual BOOL Create(CWnd* pParentWnd);
	virtual BOOL Paser(CPaser* pPaser, int x, int y);
	virtual	BOOL SetDescription(LPCTSTR lpszFileName);

	// interface
	STDMETHOD_(ITDObject*, GetObject)(void);
	STDMETHOD_(void, SetManager)(ITDReportManager* pManager, DWORD dwID = 0);
	STDMETHOD_(void, UpdateLayout)(void);
	STDMETHOD_(BOOL, Open)(LPCTSTR lpszFileName, BOOL bAutoUpdate = FALSE);
	STDMETHOD_(BOOL, Save)(LPCTSTR lpszFileName);
	STDMETHOD_(void, Clear)(BOOL bClose = FALSE);
	STDMETHOD_(void, AppendText)(LPCTSTR lpszFormat, ...);
	STDMETHOD_(void, SetText)(LPCTSTR lpszFormat, ...);
	STDMETHOD_(void, ReplaceText)(long nStartChar, long nEndChar, LPCTSTR lpszFormat, ...);
	STDMETHOD_(int, GetLineCount)(void);
	STDMETHOD_(void, SetBackgroundColor)(COLORREF crBasckColor);
	STDMETHOD_(BOOL, GetText)(LPTSTR lpszBuffer, DWORD dwSize, long nStartChar = 0, long nEndChar = -1);
	STDMETHOD_(void, SetStyle)(DWORD dwAdd, DWORD dwRemove = 0);
	STDMETHOD_(void, SetColor)(COLORREF crTextColor);
	STDMETHOD_(void, SetFont)(LPCTSTR sFontName);
	STDMETHOD_(void, SetOffset)(LONG yOffset);
	STDMETHOD_(void, SetHeight)(LONG yHeight);
	STDMETHOD_(void, SetAlign)(TEXT_ALIGN align);
	STDMETHOD_(void, SetLink)(LPCTSTR sLink = NULL);
	STDMETHOD_(void, ShowScrollBar)(BOOL bShow);
	STDMETHOD_(void, EnableEdit)(BOOL bEditable);
	STDMETHOD_(void, ShowOutline)(BOOL bShow = TRUE);
	STDMETHOD_(void, CalculateHeight)(void);
	STDMETHOD_(int, GetFirstVisibleLine)(void);
	STDMETHOD_(void, LineScroll)(int iLines, int iChars = 0);
	STDMETHOD_(long, GetTextLength)(void);
	STDMETHOD_(void, ScrollToLastLine)(void);
	
	STDMETHOD_(void, SetSel)(long nStartChar, long nEndChar);
	STDMETHOD_(void, GetSel)(long& nStartChar, long& nEndChar);
	STDMETHOD_(void, ReplaceSel)(LPCTSTR lpszNewText);
	void UpdateFormat(void);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRequestResize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLink(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnProtected(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNcPaint();
	afx_msg void OnSetFocus(CWnd *pOldWnd);

	COleRichEditCtrl	m_Edit;
	CString				m_sHiperLink;

	CHARFORMAT2			m_CharFormat;
	PARAFORMAT			m_ParaFormat;
	long				m_CharStart;
	long				m_CharEnd;

	ITDReportManager*	m_pManager;
	DWORD				m_dwManagerID;
};


