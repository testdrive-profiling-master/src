#include "stdafx.h"
#include "TestDriveImp.h"
#include "BufferCtrl.h"
#include "DocumentWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CBufferCtrl

#define ENABLE_BITS(n)	(1<<(n))

IMPLEMENT_DYNAMIC(CBufferCtrl, CViewObject)

CBufferCtrl::CBufferCtrl() : m_ControlMask(ENABLE_BITS(BUFFER_INTERFACE_ZOOM)|
										 ENABLE_BITS(BUFFER_INTERFACE_ANTIALIASING)|
										 ENABLE_BITS(BUFFER_INTERFACE_LINK)),
										 m_PrevViewLinkPoint(0,0),
										 m_pManager(NULL), m_dwID(0)

{
	m_bCaptureMouse		= FALSE;
	m_bUseAlpha			= FALSE;
	m_bSyncronizeAction	= TRUE;
	m_bFitToScreen		= FALSE;
	m_OpID				= OP_NONE;
	m_bInitialize		= FALSE;
	m_dwBaseAddress		= 0;
	m_dwByteStride		= 0;
	m_LinkMode			= LINK_NONE;
	m_bZoomRectDraw		= FALSE;
	m_bScaleToOrignal	= FALSE;
}

CBufferCtrl::~CBufferCtrl()
{
}

BEGIN_MESSAGE_MAP(CBufferCtrl, CViewObject)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
//	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_LOADFILE, OnLoad)
	ON_COMMAND(ID_SAVE_AS, OnSaveAs)
	ON_COMMAND(ID_SCALE_PITTING, OnScaleOneByOne)
	ON_COMMAND(ID_SCREEN_PITTING, OnScreenPitting)
	ON_COMMAND(ID_ANTIALIASING, OnToggleAntialiasing)
	ON_COMMAND(ID_SYNCHRONIZE_ACTION, OnToggleSyncronizeAction)
END_MESSAGE_MAP()


// CBufferCtrl 메시지 처리기입니다.


void CBufferCtrl::OnPaint(void){
	CPaintDC dc(this);

	//dc.IntersectClipRect(&m_ClientRect);
	//if(!IsInitialize())
	if(m_OpID == OP_ZOOM){
		if(m_bZoomRectDraw){
			int oldRop = dc.SetROP2(R2_NOT);
			dc.MoveTo(m_ZoomRectPrev.TopLeft());
			dc.LineTo(m_ZoomRectPrev.right,m_ZoomRectPrev.top);
			dc.LineTo(m_ZoomRectPrev.BottomRight());
			dc.LineTo(m_ZoomRectPrev.left,m_ZoomRectPrev.bottom);
			dc.LineTo(m_ZoomRectPrev.TopLeft());
			dc.MoveTo(m_ZoomRect.TopLeft());
			dc.LineTo(m_ZoomRect.right,m_ZoomRect.top);
			dc.LineTo(m_ZoomRect.BottomRight());
			dc.LineTo(m_ZoomRect.left,m_ZoomRect.bottom);
			dc.LineTo(m_ZoomRect.TopLeft());
			dc.SetROP2(oldRop);
			m_bZoomRectDraw = FALSE;
		}
	}else{
		if(!CBuffer::Present(&dc, &m_DrawRect, &m_ClientRect)){
			// 이미지가 없을 때에는 'X' 표시를 한다.
			CRect	rc;
			CBrush	brush(RGB(255,255,255));
			GetClientRect(&rc);
			dc.FillRect(&rc, &brush);
			{
				Graphics G(dc.m_hDC);
				Pen P(Color(63,63,63));
				G.SetSmoothingMode(SmoothingModeHighQuality);
				G.DrawLine(&P,rc.left, rc.top, rc.right-1, rc.bottom-1);
				G.DrawLine(&P,rc.right-1, rc.top, rc.left, rc.bottom-1);
			}
		}else UpdateLinkedScreen();
	}
	
	//dc.SelectClipRgn(NULL);
}

void CBufferCtrl::OnLButtonDown(UINT nFlags, CPoint point){
	if(!m_lpDIBits) return;
	if(m_ControlMask&(ENABLE_BITS(BUFFER_INTERFACE_ZOOM)))
	if(!m_OpID){
		m_ZoomRectPrev.SetRectEmpty();
		m_ZoomRect.SetRect(point, point);
		m_OpID	= OP_ZOOM;
		SetCapture();
		RedrawWindow();

		if(m_pManager){
			int x, y;
			x = point.x;
			y = point.y;
			RetrieveScreenPos(x, y);
			m_pManager->OnBufferMouseEvent(m_dwID, x, y, 0x80000001);
		}
	}
}

void CBufferCtrl::OnLButtonUp(UINT nFlags, CPoint point){
	if(!m_lpDIBits) return;
	if(m_OpID == OP_ZOOM){
		m_OpID	= OP_NONE;
		if(abs(m_ZoomRect.Width()) < 3 && abs(m_ZoomRect.Height()) < 3){
			// normal click
			if(m_pManager){
				int x, y;
				x = point.x;
				y = point.y;
				RetrieveScreenPos(x, y);
				m_pManager->OnBufferClick(m_dwID);
				m_pManager->OnBufferMouseEvent(m_dwID, x, y, 0x1);
			}
		}else
		if(m_ZoomRect.Width() <= 0 || m_ZoomRect.Height() <= 0){
			// zoom out
			if(m_bScaleToOrignal) 
				OnScaleOneByOne();
			else
				OnScreenPitting();
			if(m_pManager){
				int x, y;
				x = point.x;
				y = point.y;
				RetrieveScreenPos(x, y);
				m_pManager->OnBufferMouseEvent(m_dwID, x, y, 0x4);
			}
		}else{
			// zoom int
			float	nScale;
			int center_x	= m_ZoomRect.CenterPoint().x;
			int center_y	= m_ZoomRect.CenterPoint().y;
			RetrieveScreenPos(center_x, center_y);			// zoom rect 중앙 점 얻기

			{	// zoom 확대 배율(nScale) 얻기
				float	board_width, board_height;
				float	vScale, hScale;
				{	// 확대 선택 영역의 이미지 너비/높이 구함
					int sx	= m_ZoomRect.left,
						ex	= m_ZoomRect.right,
						sy	= m_ZoomRect.top,
						ey	= m_ZoomRect.bottom;
					RetrieveScreenPos(sx,sy);
					RetrieveScreenPos(ex,ey);
					board_width		= (float)(ex - sx);
					board_height	= (float)(ey - sy);
				}
				hScale = board_width  ? m_ClientRect.Width()  / board_width  : 64.f;	// 수평 줌 배율
				vScale = board_height ? m_ClientRect.Height() / board_height : 64.f;	// 수직 줌 배율
				nScale	= hScale > vScale ? vScale : hScale;							// 가장 작은 줌을 기준으로 함.
			}

			// 64배 이상 확대 제한함.
			if(nScale>64.f) nScale = 64.f;

			{	// 확대 부분을 중앙 위치로 옮김 & 확대
				m_DrawRect.TopLeft()		= m_ClientRect.CenterPoint();
				m_DrawRect.BottomRight()	= m_ClientRect.CenterPoint();
				m_DrawRect.left				-= (LONG)(center_x * nScale);
				m_DrawRect.right			+= (LONG)((m_dwWidth - center_x) * nScale);
				m_DrawRect.top				-= (LONG)(center_y * nScale);
				m_DrawRect.bottom			+= (LONG)((m_dwHeight - center_y) * nScale);
			}

// 			// 64배 이상 확대 제한함.
// 			if(m_DrawRect.Width() > (int)m_dwWidth * 64){
// 				CPoint mpoint = m_ClientRect.CenterPoint();
// 				nScale	= 64.f * m_dwWidth / m_DrawRect.Width();
// 
// 				m_DrawRect.left		= mpoint.x + (int)((m_DrawRect.left - mpoint.x) * nScale);
// 				m_DrawRect.right	= mpoint.x + (int)((m_DrawRect.right - mpoint.x) * nScale);
// 				m_DrawRect.top		= mpoint.y + (int)((m_DrawRect.top - mpoint.y) * nScale);
// 				m_DrawRect.bottom	= mpoint.y + (int)((m_DrawRect.bottom - mpoint.y) * nScale);
// 			}

			ClipDrawRect();
			if(m_pManager){
				int x, y;
				x = point.x;
				y = point.y;
				RetrieveScreenPos(x, y);
				m_pManager->OnBufferMouseEvent(m_dwID, x, y, 0x4);
			}
		}
		RedrawWindow();
		ReleaseCapture();
	}
	GetParent()->SetFocus();
}

void CBufferCtrl::ClipDrawRect(void){
	int draw_width	= m_DrawRect.Width();
	int draw_height	= m_DrawRect.Height();

	if(draw_width < m_ClientRect.Width()){
		m_DrawRect.left		= (m_ClientRect.Width() - draw_width) / 2;
		m_DrawRect.right	= m_DrawRect.left + draw_width;
		//m_DrawRect.right	= m_DrawRect.left + m_ClientRect.Width();
	}else{
		if(m_DrawRect.left > m_ClientRect.left){
			int gap = m_ClientRect.left - m_DrawRect.left;
			m_DrawRect.left		+= gap;
			m_DrawRect.right	+= gap;
		}else
		if(m_DrawRect.right < m_ClientRect.right){
			int gap = m_ClientRect.right - m_DrawRect.right;
			m_DrawRect.left		+= gap;
			m_DrawRect.right	+= gap;
		}
	}
	if(draw_height < m_ClientRect.Height()){
		m_DrawRect.top		= (m_ClientRect.Height() - draw_height) / 2;
		m_DrawRect.bottom	= m_DrawRect.top + draw_height;
		//m_DrawRect.bottom = m_DrawRect.top + m_ClientRect.Height();
	}else{
		if(m_DrawRect.top > m_ClientRect.top){
			int gap = m_ClientRect.top - m_DrawRect.top;
			m_DrawRect.top		+= gap;
			m_DrawRect.bottom	+= gap;
		}else
		if(m_DrawRect.bottom < m_ClientRect.bottom){
			int gap = m_ClientRect.bottom - m_DrawRect.bottom;
			m_DrawRect.top		+= gap;
			m_DrawRect.bottom	+= gap;
		}
	}
}

BOOL CBufferCtrl::RetrieveScreenPos(int& x, int& y){
	if(!m_DrawRect.Width() || !m_DrawRect.Height()) return FALSE;

	if(x<m_DrawRect.left) x = 0;
	else
	if(x>=m_DrawRect.right) x = m_dwWidth-1;
	else{
		float	width_scale		= (float)m_dwWidth / m_DrawRect.Width();
		x = (int)((x-m_DrawRect.left) * width_scale);
		if(x>=(int)m_dwWidth) x = m_dwWidth-1;
	}
	

	if(y<m_DrawRect.top) y = 0;
	else
	if(y>=m_DrawRect.bottom) y = m_dwHeight-1;
	else{
		float	height_scale	= (float)m_dwHeight / m_DrawRect.Height();
		y = (int)((y-m_DrawRect.top) * height_scale);
		if(y>=(int)m_dwHeight) y = m_dwHeight-1;
	}

	return TRUE;
}

void CBufferCtrl::OnRButtonDown(UINT nFlags, CPoint point){
	if(m_lpDIBits)
	if(!m_OpID){
		m_OpID = OP_MOVE;
		m_PrevMousePos	= point;
		m_bInitialize	= FALSE;
		if(IsInitialize())
			CWnd::OnRButtonDown(nFlags, point);
		SetCapture();
		//SetFocus();

		if(m_pManager){
			int x, y;
			x = point.x;
			y = point.y;
			RetrieveScreenPos(x, y);
			m_pManager->OnBufferMouseEvent(m_dwID, x, y, 0x80000002);
		}
	}
}

void CBufferCtrl::OnRButtonUp(UINT nFlags, CPoint point){
	if(m_lpDIBits)
	if(m_OpID == OP_MOVE){
		m_OpID	= OP_NONE;
		ReleaseCapture();
		if(!m_bInitialize){
			if(IsInitialize())
				CWnd::OnRButtonUp(nFlags, point);
			m_bInitialize	= TRUE;
		}

		if(m_pManager){
			int x, y;
			x = point.x;
			y = point.y;
			RetrieveScreenPos(x, y);
			m_pManager->OnBufferMouseEvent(m_dwID, x, y, 0x2);
		}
	}else{
		// context menu
		if(m_pManager){
			int x, y;
			x = point.x;
			y = point.y;
			RetrieveScreenPos(x, y);
			m_pManager->OnBufferMouseEvent(m_dwID, x, y, 0x8);
		}
	}
	GetParent()->SetFocus();
}

void CBufferCtrl::OnMouseMove(UINT nFlags, CPoint point){
	if(!m_lpDIBits) return;
	switch(m_OpID){
	case OP_ZOOM:
		{
			CRect rc;
			GetClientRect(&rc);
			if(point.x < rc.left) point.x = rc.left;
			else if(point.x >= rc.right) point.x = rc.right -1;
			if(point.y < rc.top) point.y = rc.top;
			else if(point.y >= rc.bottom) point.y = rc.bottom-1;

			m_ZoomRectPrev	= m_ZoomRect;
			m_ZoomRect.BottomRight() = point;
			m_bZoomRectDraw	= TRUE;
			RedrawWindow();
		}
		break;
	case OP_MOVE:
		{
			if(m_PrevMousePos != point){
				CPoint newPoint;
				newPoint = point - m_PrevMousePos + m_DrawRect.TopLeft();
				m_PrevMousePos	= point;

				m_DrawRect.MoveToXY(newPoint);

				newPoint.x *= newPoint.x;
				newPoint.y *= newPoint.y;

				if(newPoint.x > 4 || newPoint.y > 4)
					m_bInitialize	= TRUE;

				ClipDrawRect();
				RedrawWindow();
			}
		}
		break;
	default:
		CRect rc;
		TRACKMOUSEEVENT tme;
		ZeroMemory(&tme, sizeof(TRACKMOUSEEVENT));
		tme.cbSize		= sizeof(tme);
		tme.dwFlags		= TME_LEAVE;
		tme.dwHoverTime	= 0;
		tme.hwndTrack	= m_hWnd;

		_TrackMouseEvent(&tme);

		break;
	}

	if(m_OpID != OP_ZOOM)
	{
		int x, y;
		x = point.x;
		y = point.y;
		RetrieveScreenPos(x, y);
		point.SetPoint(x,y);

		if(m_PrevViewLinkPoint != point || m_OpID == OP_MOVE){
			m_PrevViewLinkPoint = point;
			if(m_pManager) m_pManager->OnBufferMouseEvent(m_dwID, x, y, 0);
		}

	}
}

void CBufferCtrl::OnContextMenu(CWnd* pWnd, CPoint pos){
	CMenu contmenu;
	CMenu pFile;
	contmenu.CreatePopupMenu();
	{
		pFile.CreatePopupMenu();
		contmenu.AppendMenu(MF_STRING | MF_POPUP, (UINT_PTR)pFile.m_hMenu, _S(MENU_SCREEN_FILE));
		pFile.AppendMenu(MF_STRING, ID_LOADFILE, _S(MENU_SCREEN_FILE_LOAD));
		pFile.AppendMenu(MF_STRING, ID_SAVE_AS, _S(MENU_SCREEN_FILE_SAVE_AS));
	}
	contmenu.AppendMenu(MF_SEPARATOR, (UINT_PTR)0, (LPCTSTR)NULL);
	contmenu.AppendMenu(MF_STRING, ID_SCALE_PITTING, _S(MENU_SCREEN_SCALE_PITTING));
	contmenu.AppendMenu(MF_STRING, ID_SCREEN_PITTING, _S(MENU_SCREEN_SCREEN_PITTING));
	contmenu.AppendMenu(MF_STRING, ID_ANTIALIASING, _S(MENU_SCREEN_ANTIALIASING));
	contmenu.AppendMenu(MF_STRING, ID_SYNCHRONIZE_ACTION, _S(MENU_SCREEN_SYNCHRONIZE_ACTION));

	if(!(m_ControlMask&ENABLE_BITS(BUFFER_INTERFACE_LOAD_FROM_FILE)))
		contmenu.EnableMenuItem(ID_LOADFILE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	{
		CRect rc;// = m_ClientRect;
		GetClientRect(&rc);
// 		if(Width() < (DWORD)rc.Width() || Height() < (DWORD)rc.Height() || !(m_ControlMask&ENABLE_BITS(BUFFER_INTERFACE_ANTIALIASING)) ||
// 		   (Width() == (DWORD)m_DrawRect.Width() && Height() == (DWORD)m_DrawRect.Height()))
// 			contmenu.EnableMenuItem(ID_SCALE_PITTING, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		if(Width() == (DWORD)m_DrawRect.Width() && Height() == (DWORD)m_DrawRect.Height()){
			contmenu.EnableMenuItem(ID_SCALE_PITTING, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		{
			if((m_DrawRect.Width() == rc.Width() || m_DrawRect.Height() == rc.Height()) || !(m_ControlMask&ENABLE_BITS(BUFFER_INTERFACE_ANTIALIASING)))
				contmenu.EnableMenuItem(ID_SCREEN_PITTING, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
	}
	if(!(m_ControlMask&ENABLE_BITS(BUFFER_INTERFACE_ANTIALIASING)))
		contmenu.EnableMenuItem(ID_ANTIALIASING, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	contmenu.CheckMenuItem(ID_ANTIALIASING, MF_BYCOMMAND | (IsAntialiasing() ? MF_CHECKED : MF_UNCHECKED));

	if(!m_pLinkPrev.size() && !m_pLinkNext.size()){
		contmenu.EnableMenuItem(ID_SYNCHRONIZE_ACTION, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}else contmenu.CheckMenuItem(ID_SYNCHRONIZE_ACTION, MF_BYCOMMAND | (m_ControlMask&ENABLE_BITS(BUFFER_INTERFACE_LINK) ? MF_CHECKED : MF_UNCHECKED));

	contmenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);
}

void CBufferCtrl::OnLoad(void){
	OpenDialog();
}

void CBufferCtrl::OnSaveAs(void){
	SaveDialog();
}

void CBufferCtrl::SetManager(ITDBufferManager* pManager, DWORD dwID){
	m_pManager	= pManager;
	m_dwID		= dwID;
}

void CBufferCtrl::OnScaleOneByOne(void){
	CPoint cp	= m_ClientRect.CenterPoint();
	int cx		= cp.x,
		cy		= cp.y;
	RetrieveScreenPos(cx, cy);

	{
		m_DrawRect.left		= cp.x - cx;
		m_DrawRect.top		= cp.y - cy;
		m_DrawRect.right	= m_DrawRect.left + Width();
		m_DrawRect.bottom	= m_DrawRect.top + Height();
		m_bScaleToOrignal	= TRUE;
		ClipDrawRect();
		RedrawWindow();
	}
}

void CBufferCtrl::OnScreenPitting(void){
	if((m_ControlMask & ENABLE_BITS(BUFFER_INTERFACE_AUTOFIT)) && IsInitialize()){
		int new_width	= (int)(m_ClientRect.Height() / m_fAspectRatio);
		int new_height	= (int)(m_ClientRect.Width() * m_fAspectRatio);

		if(new_width > m_ClientRect.Width())	new_width	= m_ClientRect.Width();
		else
			if(new_height > m_ClientRect.Height())	new_height	= m_ClientRect.Height();

		m_DrawRect.left		= (m_ClientRect.Width() - new_width) / 2;
		m_DrawRect.top		= (m_ClientRect.Height() - new_height) / 2;
		m_DrawRect.right	= m_DrawRect.left + new_width;
		m_DrawRect.bottom	= m_DrawRect.top + new_height;
		m_bScaleToOrignal	= FALSE;
	}else{
		m_DrawRect = m_ClientRect;
	}
	Invalidate();
}

void CBufferCtrl::OnToggleAntialiasing(void){
	if(!IsInitialize()) return;
	EnableAntialiasing(!IsAntialiasing());
	RedrawWindow();
}

void CBufferCtrl::OnToggleSyncronizeAction(void){
	m_ControlMask ^= ENABLE_BITS(BUFFER_INTERFACE_LINK);
}

void CBufferCtrl::AddLink(CBufferCtrl* pLink, LINK_MODE mode){
	if(pLink)
	switch(mode){
	case LINK_PREV:	m_pLinkPrev.push_back(pLink);	break;
	case LINK_NEXT:	m_pLinkNext.push_back(pLink);	break;
	}

}

void CBufferCtrl::UpdateLinkedScreen(CBufferCtrl* pPrev){
	if(!(m_ControlMask&ENABLE_BITS(BUFFER_INTERFACE_LINK))) return;
	if(!pPrev){
		// Upside link
		if(!m_LinkMode || m_LinkMode == LINK_PREV){
			for(list<CBufferCtrl*>::iterator iter = m_pLinkPrev.begin();iter != m_pLinkPrev.end();iter++){
				(*iter)->m_LinkMode = LINK_PREV;
				(*iter)->UpdateLinkedScreen(this);
				(*iter)->m_LinkMode = LINK_NONE;
			}
		}
		// Downside link
		if(!m_LinkMode || m_LinkMode == LINK_NEXT){
			for(list<CBufferCtrl*>::iterator iter = m_pLinkNext.begin();iter != m_pLinkNext.end();iter++){
				(*iter)->m_LinkMode = LINK_NEXT;
				(*iter)->UpdateLinkedScreen(this);
				(*iter)->m_LinkMode = LINK_NONE;
			}
		}
	}else{
		double			ScaleH, ScaleV;
		CRect*			pPrevDrawRect	= &pPrev->m_DrawRect;
		{
			CRect			*pPrevClient	= &pPrev->m_ClientRect;
			ScaleH			= (double)m_ClientRect.Width() / pPrevClient->Width();
			ScaleV			= (double)m_ClientRect.Height() / pPrevClient->Height();
		}

		m_DrawRect.left		= (LONG)(pPrevDrawRect->left * ScaleH);
		m_DrawRect.top		= (LONG)(pPrevDrawRect->top * ScaleV);

		m_DrawRect.right	= (LONG)(m_DrawRect.left + pPrevDrawRect->Width() * ScaleH);
		m_DrawRect.bottom	= (LONG)(m_DrawRect.top + pPrevDrawRect->Height() * ScaleV);

		ClipDrawRect();
		RedrawWindow();
	}
}

BOOL CBufferCtrl::Create(CWnd* pParentWnd){
	CRect rc;
	m_Layout.GetViewRect(&rc);
	if(!CWnd::Create(NULL, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | ES_MULTILINE | WS_CLIPSIBLINGS, rc, pParentWnd, GetLastWindowID(pParentWnd))) return FALSE;
	BringWindowToTop();
	return TRUE;
}

void CBufferCtrl::UpdateLayout(void){
	SetWindowPos(NULL, m_Layout.ViewX(), m_Layout.ViewY(), m_Layout.ViewWidth(), m_Layout.ViewHeight(), SWP_NOACTIVATE | SWP_NOZORDER);
	if(m_ClientRect.Width() != m_Layout.ViewWidth() || m_ClientRect.Height() != m_Layout.ViewHeight()){
		GetClientRect(&m_ClientRect);

		InvalidateLayout();
	}

	if(!m_bInitialize) {
		m_bInitialize	= TRUE;
	}
}

void CBufferCtrl::Present(BOOL bImmediate){
	if(IsWindowVisible()){
		if(bImmediate) RedrawWindow();
		else Invalidate();
	}
}

LPCTSTR	g_lpszImageFileSaveFilter	= _T("PNG (*.png)|*.png|JPEG (*.jpg)|*.jpg|BMP (*.bmp)|*.bmp|TIFF (*.tif)|*.tif|GIF (*.gif)|*.gif|All formats|*.png;*.jpg;*.bmp;*.tif;*.gif|All files (*.*)|*.*||");
LPCTSTR	g_lpszImageFileLoadFilter	= _T("All formats|*.png;*.jpg;*.bmp;*.tif;*.gif|PNG (*.png)|*.png|JPEG (*.jpg)|*.jpg|BMP (*.bmp)|*.bmp|TIFF (*.tif)|*.tif|GIF (*.gif)|*.gif|All files (*.*)|*.*||");

BOOL CBufferCtrl::OpenDialog(void){
	if(!IsInitialize())  return FALSE;
	CFileDialog dlg(TRUE, NULL,NULL,OFN_EXPLORER|OFN_LONGNAMES|OFN_FILEMUSTEXIST|OFN_ENABLESIZING,
		g_lpszImageFileLoadFilter, this);

	int		iRet	= (int)dlg.DoModal();

	if(iRet == IDOK){
		LoadImage(dlg.GetPathName());
	}
	g_pTestDrive->SetProjectDirectroyToCurrent();
	RedrawWindow();

	return (iRet == IDOK);
}

BOOL CBufferCtrl::SaveDialog(void){
	
	if(!IsInitialize())  return FALSE;
	CFileDialog dlg(FALSE,_T("png"), m_sViewName, OFN_EXPLORER|OFN_LONGNAMES|OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_ENABLESIZING|OFN_OVERWRITEPROMPT,
		g_lpszImageFileSaveFilter, this);
	int iRet = (int)dlg.DoModal();

	if(iRet == IDOK){
		if(!SaveToFile(dlg.GetPathName(), IMAGETYPE_AUTO, m_bUseAlpha)) iRet = IDCANCEL;
	}
	g_pTestDrive->SetProjectDirectroyToCurrent();

	return (iRet == IDOK);
}

void CBufferCtrl::SetEnable(BUFFER_INTERFACE id, BOOL bEnable){
	switch(id){
	case BUFFER_INTERFACE_OUTLINE:
		ModifyStyleEx(bEnable ? 0 : WS_EX_STATICEDGE, bEnable ? WS_EX_STATICEDGE : 0, SWP_DRAWFRAME);
		EnableOutLine(bEnable);
		break;
	default:
		{
			DWORD	bitmask = ENABLE_BITS(id);
			m_ControlMask	= (m_ControlMask & ~bitmask) | (bEnable ? bitmask : 0);
		}
		break;
	}
}

void CBufferCtrl::GetDrawRect(RECT* pRect){
	if(pRect){
		memcpy(pRect, &m_DrawRect, sizeof(RECT));
	}
}

void CBufferCtrl::SetDrawRect(const RECT* pRect){
	if(pRect){
		memcpy(&m_DrawRect, pRect, sizeof(RECT));
		ClipDrawRect();
		if(m_bInitialize) Present();
	}
}


ITDObject* CBufferCtrl::GetObject(void){
	return this;
}

typedef enum{
	CMD_BUFFER_CREATE,
	CMD_BUFFER_OUTLINE,
	CMD_BUFFER_USEALPHA,
	CMD_BUFFER_ENABLE_ANTIALIASING,
	CMD_BUFFER_SHOW_GRID,
	CMD_BUFFER_LINKED_SCREEN,
	CMD_BUFFER_SIZE,
}CMD_BUFFER;

static const TCHAR* g_sBufferCommand[CMD_BUFFER_SIZE]={
	_T("Create"),
	_T("ShowOutline"),
	_T("UseAlpha"),
	_T("EnableAntialiasing"),
	_T("ShowGrid"),
	_T("screen"),
};

BOOL CBufferCtrl::Paser(CPaser* pPaser, int x, int y){
	BOOL			bRet	= FALSE;
	int				iLoop	= 1;
	TCHAR			token[MAX_PATH];
	TD_TOKEN_TYPE	type;
	int				id, i;

	m_Layout.Move(x, y);

	if(pPaser)
	if(pPaser->TokenOut(TD_DELIMITER_LOPEN)){
		while(iLoop){
			if(!pPaser->IsTokenable()) break;
			type = pPaser->GetToken(token);
			switch(type){
			case TD_TOKEN_NAME:
				{
					id = CheckCommand(token, g_sBufferCommand, CMD_BUFFER_SIZE);
					if(id < CMD_BUFFER_LINKED_SCREEN)
					if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) goto ERROR_OUT;
					switch(id){
					case CMD_BUFFER_CREATE:
						{
							{	// create buffer
								int width, height;
								// width
								if(!pPaser->GetTokenInt(&width)) goto ERROR_OUT;
								if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OUT;
								// height
								if(!pPaser->GetTokenInt(&height)) goto ERROR_OUT;
								if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OUT;
								// type
								if(!pPaser->GetTokenName(token)) goto ERROR_OUT;
								i = CheckCommand(token, g_sColorTypes, COLORTYPE_SIZE);
								if(i>=COLORTYPE_SIZE) goto ERROR_OUT;
								CBuffer::Create(width, height, (COLORFORMAT)i);
							}

							if(pPaser->TokenOut(TD_DELIMITER_COMMA)){
								// base address
								if(!pPaser->GetTokenInt((int*)&m_dwBaseAddress)) goto ERROR_OUT;
								if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OUT;
								// byte stride
								if(!pPaser->GetTokenInt((int*)&m_dwBaseAddress)) goto ERROR_OUT;
								// default image
								if(pPaser->TokenOut(TD_DELIMITER_COMMA)){
									if(!pPaser->GetTokenString(token)) goto ERROR_OUT;
									LoadImage(token);
								}else{
									if(m_dwByteStride < GetBytesStride()) m_dwByteStride = GetBytesStride();
									{
										ITDMemory *pMemory = g_pTestDrive->GetMemory();
										if(!pMemory || !(pMemory->IsValidAddress(m_dwBaseAddress+(m_dwByteStride*m_dwHeight)-1))){
											g_pTestDrive->LogOut(_T("Memory access error occured."), CTestDrive::SYSMSG_ERROR);
											goto ERROR_OUT;
										}
										CopyFromMemory(pMemory->GetPointer(m_dwBaseAddress), m_dwByteStride);
									}
				
								}
							}
						}
						break;
					case CMD_BUFFER_OUTLINE:
						SetEnable(BUFFER_INTERFACE_OUTLINE, TRUE);
						break;
					case CMD_BUFFER_USEALPHA:
						m_bUseAlpha	= TRUE;
						break;
					case CMD_BUFFER_ENABLE_ANTIALIASING:
						EnableAntialiasing();
						break;
					case CMD_BUFFER_LINKED_SCREEN:
						{
							CDocumentView*	pView	= (CDocumentView*)GetParent();
							CBufferCtrl*	pVO		= (CBufferCtrl*)CViewObject::New(TDOBJECT_BUFFER, pView, pPaser);
							if(!pVO) goto ERROR_OUT;
							pView->AddObject((CViewObject*)pVO);
							AddLink(pVO, LINK_NEXT);
							pVO->AddLink(this, LINK_PREV);
							pVO->Paser(pPaser, x, y);
						}
						break;
					default:
						goto ERROR_OUT;
					}
					if(id < CMD_BUFFER_LINKED_SCREEN){
						if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) goto ERROR_OUT;
						if(!pPaser->TokenOut(TD_DELIMITER_SEMICOLON)) goto ERROR_OUT;
					}
				}break;
			case TD_TOKEN_DELIMITER:
				if(*token != *g_PaserDelimiter[TD_DELIMITER_LCLOSE]) goto ERROR_OUT;
				iLoop--;
				if(!iLoop) bRet = TRUE;
				break;
			}
		}
	}
ERROR_OUT:
	return bRet;
}

BOOL CBufferCtrl::OnBeforeLoadImage(LPCTSTR sFilename, DWORD dwWidth, DWORD dwHeight){
	if(m_pManager) return m_pManager->OnBufferBeforeFileOpen(m_dwID, sFilename, dwWidth, dwHeight);
	return TRUE;
}

void CBufferCtrl::InvalidateLayout(void){
	if(!m_bScaleToOrignal) OnScreenPitting();
	ClipDrawRect();
	Invalidate();
}
