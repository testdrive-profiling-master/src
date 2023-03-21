#pragma once
#include "ViewObject.h"
#include "WatchDogFile.h"
#include "Paser.h"
#include "Memory.h"

#define		WM_USER_COMMAND		(WM_USER+2)
typedef struct{
	DWORD		command;
	WPARAM		wParam;
	LPARAM		lParam;
}USER_COMMAND;

class CDocumentView :
	public CWnd,
	public CWatchDogFileClient,
	public ITDDocument
{
	DECLARE_DYNAMIC(CDocumentView)
	list<CViewObject*>	m_ViewObjectList;

public:
	CDocumentView();
	virtual ~CDocumentView();

	BOOL Create(CWnd* pParent, LPCTSTR lpszName, CPaser* pPaser);
	void ReleaseAll(void);
	void AddObject(CViewObject*	pVO);
	void DeleteObject(CViewObject* pVO);
	BOOL Load(CPaser* pPaser);
	const CString& GetName(void);
	void UpdateOrigin(void);
	void UpdateLayout(void);
	void GetDrawRect(LPRECT pRect);
	BOOL SetProgram(LPCTSTR lpszFileName);
	BOOL CheckUpdateFile(LPCTSTR lpszDllPath, CString* pUpdateFile = NULL);	// dll 경로로 부터 update 파일 경로 얻고 존재하는지 채크한다.

	CViewObject* GetObject(OBJECT_TYPE Type, LPCTSTR lpszName);
	CMFCPropertyGridProperty* GetProperty(void);

	DWORD GetReferenceCount(void);
	BOOL TryUnlock(BOOL bForce = FALSE);

	// interface
	STDMETHOD_(ITDBuffer*, GetBuffer)(LPCTSTR lpszName);
	STDMETHOD_(ITDReport*, GetReport)(LPCTSTR lpszName);
	STDMETHOD_(ITDChart*, GetChart)(LPCTSTR lpszName);
	STDMETHOD_(ITDButton*, GetButton)(LPCTSTR lpszName);
	STDMETHOD_(ITDHtml*, GetHtml)(LPCTSTR lpszName);
	STDMETHOD_(ITDImplDocument*, GetImplementation)(void);
	STDMETHOD_(LPCTSTR, DocumentName)(void);
	STDMETHOD_(LPCTSTR, DocumentTitle)(LPCTSTR lpszTitle = NULL);
	STDMETHOD_(ITDSystem*, GetSystem)(void);
	STDMETHOD_(ITDBuffer*, CreateBuffer)(LPCTSTR lpszName, int x, int y, int width, int height);
	STDMETHOD_(ITDReport*, CreateReport)(LPCTSTR lpszName, int x, int y, int width, int height);
	STDMETHOD_(ITDButton*, CreateButton)(LPCTSTR lpszName, int x, int y, int width, int height);
	STDMETHOD_(ITDHtml*, CreateHtml)(LPCTSTR lpszName, int x, int y, int width, int height, BOOL bWebView2);
	STDMETHOD_(void, Lock)(void);
	STDMETHOD_(void, UnLock)(void);
	STDMETHOD_(BOOL, IsLocked)(void);
	STDMETHOD_(void, SetAutoClose)(BOOL bClose = TRUE);
	STDMETHOD_(void, SetDescription)(LPCTSTR lpszDesc);

	STDMETHOD_(int, GetConfigInt)(LPCTSTR lpszKeyName, int iDefaultValue = 0);
	STDMETHOD_(void, SetConfigInt)(LPCTSTR lpszKeyName, int iSetValue);
	STDMETHOD_(void, GetConfigString)(LPCTSTR lpszKeyName, LPTSTR lpszStr, DWORD dwSize, LPCTSTR lpszDefault = _T(""));
	STDMETHOD_(void, SetConfigString)(LPCTSTR lpszKeyName, LPCTSTR lpszStr);

	STDMETHOD_(void, ClearAllProperty)(void);
	STDMETHOD_(ITDPropertyData*, AddPropertyData)(PROPERTY_TYPE Type, DWORD iID, LPCTSTR lpszName, DWORD_PTR pData, LPCTSTR lpszDesc = NULL);

	STDMETHOD_(void, SetTimer)(UINT nIDEvent, UINT nElapse);
	STDMETHOD_(void, KillTimer)(UINT nIDEvent);

	STDMETHOD_(HWND, GetWindowHandle)(void);
	STDMETHOD_(void, SetForegroundDocument)(void);
	STDMETHOD_(void, AddWatchDogPath)(LPCTSTR lpszPath, DWORD dwID = 0, BOOL bSearchSubDir = FALSE);
	STDMETHOD_(void, ClearWatchDogPath)(DWORD dwID = -1);

	STDMETHOD_(void, InvalidateLayout)(void);
	STDMETHOD_(void, Show)(BOOL bShow = TRUE);
	STDMETHOD_(BOOL, IsVisible)(void){return m_bShow;};

protected:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg LRESULT OnWatchDog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()

	CString		m_sName, m_sTitle;	// 도큐먼트 이름, 도큐먼트 타이틀
	CSize		m_DrawSize;
	CSize		m_ViewSize;
	CPoint		m_BasePoint;
	HMODULE		m_hModule;
	DWORD		m_dwRefCount;
	BOOL		m_bAutoClose;
	BOOL		m_bShow;

	ITDImplDocument*			m_pDocumentImp;
	CMFCPropertyGridProperty*	m_pProperty;
};
