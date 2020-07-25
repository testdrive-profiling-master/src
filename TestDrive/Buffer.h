#pragma once
#include "TestDrive.h"

class CBuffer :
	public ITDBuffer
{
protected:
	LPVOID				m_lpDIBits;
	BITMAPV5HEADER		m_bmpHeader;
	HDC					m_memDC;
	HBITMAP				m_hbmScreen;

	// frame buffer
	CRect				m_bmpRect;
	DWORD				m_dwWidth, m_dwHeight;
	float				m_fAspectRatio;
	COLORFORMAT			m_ColorFormat;
	BOOL				m_bAntialiasing;
	Graphics*			m_pGraphics;
	Pen*				m_pPen;
	Brush*				m_pBrush;
	BOOL				m_bOutline;
	COLORREF			m_crBackColor;

	virtual BOOL OnBeforeLoadImage(LPCTSTR sFilename, DWORD dwWidth, DWORD dwHeight);
	virtual void InvalidateLayout(void);

public:
	CBuffer(void);
	virtual ~CBuffer(void);

	STDMETHOD_(ITDObject*, GetObject)(void);
	STDMETHOD_(void, SetManager)(ITDBufferManager* pManager, DWORD dwID = 0);
	STDMETHOD_(BOOL, Create)(DWORD dwWidth, DWORD dwHeight, COLORFORMAT Type = COLORTYPE_ABGR_8888);
	STDMETHOD_(BOOL, CreateFromFile)(LPCTSTR sFilename, COLORFORMAT Type = COLORTYPE_ABGR_8888);
	BOOL CreateFromResource(UINT nIDResource, COLORFORMAT ColorType = COLORTYPE_ABGR_8888, LPCTSTR sImgType = _T("PNG"), HINSTANCE hInstance = NULL);
	STDMETHOD_(BOOL, LoadImage)(LPCTSTR sFilename);
	STDMETHOD_(BOOL, CopyToMemory)(BYTE* pMEM, DWORD dwByteStride = 0);
	STDMETHOD_(BOOL, CopyFromMemory)(const BYTE* pMEM, DWORD dwByteStride = 0, BOOL bReverse = FALSE);
	STDMETHOD_(BOOL, CopyFromBuffer)(ITDBuffer* pBuffer, int x = 0, int y = 0, int sx = 0, int sy = 0, int width = 0, int height = 0, int dwidth = 0, int dheight = 0);
	BOOL LoadFromResource(UINT nIDResource);
	STDMETHOD_(BOOL, SaveToFile)(LPCTSTR sFilename, IMAGETYPE Type = IMAGETYPE_AUTO, BOOL bStoreAlpha = FALSE);
	STDMETHOD_(void, ReleaseAll)(void);
	STDMETHOD_(BOOL, Compare)(ITDBuffer* pBuffer);
	STDMETHOD_(BOOL, IsInitialize)(void);
	STDMETHOD_(BOOL, IsExistAlphaChanel)(void);
	BOOL Present(CDC* pDC, CRect* pRcDraw, CRect* pRcClip = NULL);
	STDMETHOD_(void, Present)(BOOL bImmediate = TRUE);
	STDMETHOD_(LPVOID, GetPointer)(void);
	STDMETHOD_(COLORFORMAT, ColorFormat)(void);
	STDMETHOD_(DWORD, GetBytesStride)(void);
	STDMETHOD_(DWORD, ColorBitCount)(void);
	STDMETHOD_(DWORD, Width)(void);
	STDMETHOD_(DWORD, Height)(void);
	STDMETHOD_(void, GetRect)(RECT* pRect);
	STDMETHOD_(void, EnableAntialiasing)(BOOL bEnable = TRUE);
	STDMETHOD_(BOOL, IsAntialiasing)(void);
	STDMETHOD_(BOOL, OpenDialog)(void);
	STDMETHOD_(BOOL, SaveDialog)(void);
	// CheckPSNR supports only ABGR_8888/RGBA_8888/ARGB_8888 color format
	STDMETHOD_(BOOL, CheckPSNR)(ITDBuffer* pCompare, double &dPSNR, BOOL &bSame, BOOL bCompareAlpha = FALSE);
	STDMETHOD_(BOOL, SetViewLink)(ITDBuffer* pBuffer);
	STDMETHOD_(void, SetEnable)(BUFFER_INTERFACE id, BOOL bEnable = TRUE);
	STDMETHOD_(void, GetDrawRect)(RECT* pRect);
	STDMETHOD_(void, SetDrawRect)(const RECT* pRect);
	STDMETHOD_(HDC, GetDC)(void);

	STDMETHOD_(void, SetPenColor)(BYTE r, BYTE g, BYTE b, BYTE a = 255);
	STDMETHOD_(void, SetPenDashStyle)(ITDDashStyle style);
	STDMETHOD_(void, SetPenDashOffset)(float offset);
	STDMETHOD_(void, SetPenDashCap)(ITDDashCap cap);
	STDMETHOD_(void, SetPenWidth)(float width);
	STDMETHOD_(void, SetBrushSolidColor)(BYTE r, BYTE g, BYTE b, BYTE a = 255);
	STDMETHOD_(void, SetSmoothModeHighQuality)(BOOL bEnable = TRUE);
	STDMETHOD_(void, UseAlphaChannel)(BOOL bUse = TRUE);
	STDMETHOD_(void, DrawLine)(float x, float y, float ex, float ey);
	STDMETHOD_(void, DrawLine)(ITDPoint* pStart, ITDPoint* pEnd);
	STDMETHOD_(void, DrawLinef)(ITDPointf* pStart, ITDPointf* pEnd);
	STDMETHOD_(void, DrawLines)(ITDPoint* pPoints, DWORD dwCount);
	STDMETHOD_(void, DrawLinesf)(ITDPointf* pPoints, DWORD dwCount);
	STDMETHOD_(void, DrawRectangle)(float x, float y, float width, float height);
	STDMETHOD_(void, FillRectangle)(float x, float y, float width, float height);
	STDMETHOD_(void, DrawEllipse)(float x, float y, float width, float height);
	STDMETHOD_(void, FillEllipse)(float x, float y, float width, float height);
	STDMETHOD_(void, DrawBezier)(ITDPoint* pPt1, ITDPoint* pPt2, ITDPoint* pPt3, ITDPoint* pPt4);
	STDMETHOD_(void, DrawBezierf)(ITDPointf* pPt1, ITDPointf* pPt2, ITDPointf* pPt3, ITDPointf* pPt4);
	STDMETHOD_(void, DrawBeziers)(ITDPoint* pPoints, DWORD dwCount);
	STDMETHOD_(void, DrawBeziersf)(ITDPointf* pPoints, DWORD dwCount);
	STDMETHOD_(void, DrawPolygon)(ITDPoint* pPoints, DWORD dwCount);
	STDMETHOD_(void, DrawPolygonf)(ITDPointf* pPoints, DWORD dwCount);
	STDMETHOD_(void, FillPolygon)(ITDPoint* pPoints, DWORD dwCount);
	STDMETHOD_(void, FillPolygonf)(ITDPointf* pPoints, DWORD dwCount);

	//HDC GetDC(void);
	BITMAPV5HEADER* GetHeader(void);

	CString GetDescription(void);
	void EnableOutLine(BOOL bEnable = TRUE) {m_bOutline = bEnable;}
	void SetBackColor(COLORREF crColor) {m_crBackColor = crColor;}

protected:
	virtual void OnCreateBuffer(void){};
};

extern const TCHAR* g_sColorTypes[COLORTYPE_SIZE];