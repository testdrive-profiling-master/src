#pragma once
#include "Buffer.h"
#include "ViewObject.h"
// CBufferCtrl

class CBufferCtrl :
	public CViewObject,
	public CBuffer
{
	DECLARE_DYNAMIC(CBufferCtrl)
	typedef enum{
		OP_NONE,
		OP_ZOOM,
		OP_MOVE,
		OP_PIXEL,
	}OP_MODE;

	friend class CBufferCtrl;
public:
	CBufferCtrl();
	virtual ~CBufferCtrl();

	// virtual interface
	virtual BOOL Create(CWnd* pParentWnd);
	virtual BOOL Paser(CPaser* pPaser, int x, int y);
	STDMETHOD_(void, UpdateLayout)(void);

	// extended interface
	STDMETHOD_(ITDObject*, GetObject)(void);
	STDMETHOD_(void, SetManager)(ITDBufferManager* pManager, DWORD dwID = 0);
	STDMETHOD_(void, Present)(BOOL bImmediate = TRUE);
	STDMETHOD_(BOOL, OpenDialog)(void);
	STDMETHOD_(BOOL, SaveDialog)(void);
	STDMETHOD_(void, SetEnable)(BUFFER_INTERFACE id, BOOL bEnable = TRUE);
	STDMETHOD_(void, GetDrawRect)(RECT* pRect);
	STDMETHOD_(void, SetDrawRect)(const RECT* pRect);

protected:
	DECLARE_MESSAGE_MAP()
//public:
	afx_msg void OnPaint(void);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnScaleOneByOne(void);
	afx_msg void OnScreenPitting(void);
	afx_msg void OnToggleAntialiasing(void);
	afx_msg void OnToggleSyncronizeAction(void);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg void OnLoad(void);
	afx_msg void OnSaveAs(void);

//	CBuffer			m_Buffer;
	BOOL			m_bCaptureMouse;
	OP_MODE			m_OpID;
	BOOL			m_bZoomRectDraw;
	CRect			m_ZoomRect, m_ZoomRectPrev;
	CRect			m_DrawRect, m_ClientRect;
	CPoint			m_PrevMousePos;
	DWORD			m_dwBaseAddress;
	DWORD			m_dwByteStride;
	DWORD			m_ControlMask;
	BOOL			m_bInitialize;
	BOOL			m_bUseAlpha;
	BOOL			m_bSyncronizeAction;
	BOOL			m_bFitToScreen;
	BOOL			m_bScaleToOrignal;

	//CToolTipCtrl	m_ToolTip;
	//BOOL			m_bUseToolTip;
	CPoint			m_PrevViewLinkPoint;

	ITDBufferManager* m_pManager;
	DWORD			m_dwID;
	
	typedef enum{
		LINK_NONE,
		LINK_PREV,
		LINK_NEXT,
	}LINK_MODE;

	LINK_MODE			m_LinkMode;
	list<CBufferCtrl*>	m_pLinkPrev, m_pLinkNext;

	void AddLink(CBufferCtrl* pLink, LINK_MODE mode);
	void UpdateLinkedScreen(CBufferCtrl* pNext = NULL);
	void ClipDrawRect(void);
	BOOL RetrieveScreenPos(int& x, int& y);

	BOOL RefreshCtrl(void);
	//void DrawAll(void);

	virtual BOOL OnBeforeLoadImage(LPCTSTR sFilename, DWORD dwWidth, DWORD dwHeight);
	virtual void InvalidateLayout(void);

};
