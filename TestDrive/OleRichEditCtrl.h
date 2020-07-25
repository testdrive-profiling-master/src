#if !defined(AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_)
#define AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OleRichEditCtrl.h : header file
//
#include <richedit.h>
#include <richole.h>
#include "WatchDogFile.h"
#include "TestDrive.h"
#include "AccelCodeDecoder.h"

// reference : http://www.codeproject.com/KB/edit/COleRichEditCtrl.aspx
/////////////////////////////////////////////////////////////////////////////
// COleRichEditCtrl window

class COleRichEditCtrl :
	public CRichEditCtrl,
	//public CRichEditView,
	public CWatchDogFileClient,
	public AccelCodeDecoder

{
	static HINSTANCE	m_hInstRichEdit50W;      // handle to MSFTEDIT.DLL
// Construction
public:
	COleRichEditCtrl();
	virtual ~COleRichEditCtrl();

	long StreamInFromResource(int iRes, LPCTSTR sType);
	void Test(void);
	void SetScale(int iScale);

	// interface
	BOOL Open(LPCTSTR lpszFileName, BOOL bAutoUpdate = FALSE);
	BOOL Save(LPCTSTR lpszFileName);
	void Clear(BOOL bClose = FALSE);
	void AppendText(LPCTSTR lpszText);



	int SetTextEx(LPCTSTR lpstrText, DWORD dwFlags = ST_SELECTION, UINT uCodePage = 1200)
	{
		CStringA sLine(lpstrText);
		SETTEXTEX ste = { 0 };
		ste.flags		= dwFlags;
		ste.codepage	= uCodePage;
		return (int)::SendMessage(m_hWnd, EM_SETTEXTEX, (WPARAM)&ste, (LPARAM)((const char*)sLine));
	}

	BOOL	m_bEditable;

protected:
	static DWORD CALLBACK readFunction(DWORD_PTR dwCookie,
		LPBYTE lpBuf,			// the buffer to fill
		LONG nCount,			// number of bytes to read
		LONG* nRead);			// number of bytes actually read
	static DWORD CALLBACK writeFunction(DWORD_PTR dwCookie,
		LPBYTE lpBuf,			// the buffer to fill
		LONG nCount,			// number of bytes to read
		LONG* nRead);			// number of bytes actually read

	virtual void OnAccel(ACCEL_CODE code);

	interface IExRichEditOleCallback;	// forward declaration (see below in this header file)

	IExRichEditOleCallback* m_pIRichEditOleCallback;
	BOOL		m_bCallbackSet;
	CString		m_szFileName;
	BOOL		m_bOpen;

	
	interface IExRichEditOleCallback : public IRichEditOleCallback
	{
	public:
		IExRichEditOleCallback();
		virtual ~IExRichEditOleCallback();
		int			m_iNumStorages;
		IStorage*	pStorage;
		DWORD		m_dwRef;

		virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE* lplpstg);
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
		virtual HRESULT STDMETHODCALLTYPE GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
			LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo);
 		virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
 		virtual HRESULT STDMETHODCALLTYPE QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
 		virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
 		virtual HRESULT STDMETHODCALLTYPE QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
			DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
 		virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
 		virtual HRESULT STDMETHODCALLTYPE GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj);
 		virtual HRESULT STDMETHODCALLTYPE GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
 		virtual HRESULT STDMETHODCALLTYPE GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
			HMENU FAR *lphmenu);
	};
 

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COleRichEditCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:


	// Generated message map functions
protected:
	//{{AFX_MSG(COleRichEditCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg LRESULT OnUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_)
