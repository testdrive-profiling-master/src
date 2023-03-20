#pragma once
#include "ArchiveFile.h"
#include "TestDrive.h"
#include "Paser.h"

class CViewLayout :
	public ITDLayout
{
public:
	int		m_iX, m_iY;				// relative position
	int		m_iWidth, m_iHeight;	// real size
	int		m_iSx, m_iSy;			// start position

public:

	CViewLayout(){
		m_iX		= 0;
		m_iY		= 0;
		m_iWidth	= 0;
		m_iHeight	= 0;
		m_iSx		= 0;
		m_iSy		= 0;
	}

	STDMETHOD_(void, SetPosition)(int x, int y){
		m_iX	= x;
		m_iY	= y;
	}

	STDMETHOD_(void, SetSize)(int width, int height){
		m_iWidth	= width;
		m_iHeight	= height;
	}

	STDMETHOD_(void, Move)(int x, int y){
		m_iX	+= x;
		m_iY	+= y;
	}

	STDMETHOD_(void, SetRect)(const RECT* pRc){
		if(!pRc) return;
		m_iX		= pRc->left;
		m_iY		= pRc->top;
		m_iWidth	= pRc->right - m_iX;
		m_iHeight	= pRc->bottom - m_iY;
	}

	STDMETHOD_(void, GetRect)(RECT* pRc){
		if(!pRc) return;
		pRc->left	= m_iX;
		pRc->top	= m_iY;
		pRc->right	= m_iX + m_iWidth;
		pRc->bottom	= m_iY + m_iHeight;
	}
	
	STDMETHOD_(int, GetX)(void){
		return m_iX;
	}

	STDMETHOD_(int, GetY)(void){
		return m_iY;
	}

	STDMETHOD_(int, GetWidth)(void){
		return m_iWidth;
	}

	STDMETHOD_(int, GetHeight)(void){
		return m_iHeight;
	}

	STDMETHOD_(void, SetX)(int x){
		m_iX = x;
	}

	STDMETHOD_(void, SetY)(int y){
		m_iY = y;
	}

	STDMETHOD_(void, SetWidth)(int iWidth){
		m_iWidth = iWidth;
	}

	STDMETHOD_(void, SetHeight)(int iHeight){
		m_iHeight = iHeight;
	}
	
	void SetOrigin(int x, int y){
		m_iSx	= x;
		m_iSy	= y;
	}


	int ViewX(void){
		return m_iSx + m_iX;
	}

	int ViewY(void){
		return m_iSy + m_iY;
	}

	int ViewWidth(void){
		return m_iWidth;
	}

	int ViewHeight(void){
		return m_iHeight;
	}

	void GetViewRect(LPRECT pRect){
		if(!pRect) return;
		pRect->left		= ViewX();
		pRect->top		= ViewY();
		pRect->right	= pRect->left + ViewWidth();
		pRect->bottom	= pRect->top + ViewHeight();
	}
};

class CDocumentWnd;

class CViewObject :
	public CWnd,
	public ITDObject
{
	DECLARE_DYNAMIC(CViewObject)
	friend class CDocumentView;
	friend class CDocumentWnd;

public:
	CViewObject();
	virtual ~CViewObject();

	virtual BOOL Create(CWnd* pParentWnd) = 0;
	virtual BOOL Paser(CPaser* pPaser, int x, int y) = 0;

	static CViewObject* New(OBJECT_TYPE iViewType, CWnd* pParentWnd, CPaser* pPaser = NULL);

	STDMETHOD_(ITDLayout*, GetLayout)(void);
	STDMETHOD_(OBJECT_TYPE, GetType)(void);
	STDMETHOD_(void, SetName)(LPCTSTR lpszName);
	STDMETHOD_(void, SetEnable)(BOOL bEnable = TRUE);
	STDMETHOD_(void, Show)(BOOL bEnable = TRUE);
	STDMETHOD_(BOOL, IsVisible)(void);
	STDMETHOD_(void, Release)(void);

	CViewLayout	m_Layout;

protected:
	OBJECT_TYPE	m_iViewType;
	CString		m_sViewName;
	int			m_iControlID;
	BOOL		m_bOutline;
	BOOL		m_bShow;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNcPaint();
};
