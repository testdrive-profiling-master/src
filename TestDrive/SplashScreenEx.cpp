#include "stdafx.h"
#include "SplashScreenEx.h"

// CSplashScreenEx
CSplashScreenEx::CSplashScreenEx()
{
	m_nxPos				= 0;
	m_nyPos				= 0;
	m_dwTimeout			= 2000;
	m_iFadeAlpha		= 0;
	m_dwShowUpVelocity	= 10;
	m_pBuffer			= NULL;
	m_Movie.pObject		= NULL;

	m_Blend.BlendOp				= AC_SRC_OVER;
	m_Blend.BlendFlags			= 0;
	m_Blend.AlphaFormat			= AC_SRC_ALPHA;
	m_Blend.SourceConstantAlpha	= m_iFadeAlpha;
}

CSplashScreenEx::~CSplashScreenEx()
{
	ReleaseAll();
}

void CSplashScreenEx::ReleaseAll(void){
	if(m_Movie.pObject) m_pBuffer	= NULL;
	SAFE_DELETE(m_Movie.pObject);
	SAFE_DELETE(m_pBuffer);
}

BOOL CSplashScreenEx::Create(HWND hWnd, LPCTSTR sFileName)
{
	ReleaseAll();
	{
		m_pBuffer	= new CBuffer;
		m_pBuffer->UseAlphaChannel();
		if(!m_pBuffer->CreateFromFile(sFileName, COLORTYPE_ARGB_8888)){
			// 일반 이미지가 아니면 동영상을 로드한다.
			SAFE_DELETE(m_pBuffer);
			m_Movie.pObject	= new CAVIGenerator;
			m_Movie.pObject->EnableAlpha();
			if(!m_Movie.pObject->Open(sFileName, m_Movie.lFrameCurrent, m_Movie.lNumOfFrames, &m_Movie.dwFrameRate)){
				SAFE_DELETE(m_Movie.pObject);
				return FALSE;	// 동영상도 아님...
			}
			m_pBuffer	= (CBuffer*)m_Movie.pObject->Buffer();
			m_Movie.pObject->GetFrame(m_Movie.lFrameCurrent);
		}

		{	// 반투명도 결정
			DWORD* pPixel	= (DWORD*)m_pBuffer->GetPointer();
			// pre-multiply rgb channels with alpha channel
			for(DWORD y=0;y<m_pBuffer->Height();y++){
				BYTE* pColor	= (BYTE*)pPixel;
				for(DWORD x=0;x<m_pBuffer->Width();x++){
					pColor[0]	= (DWORD)pColor[0]*pColor[3]/255;
					pColor[1]	= (DWORD)pColor[1]*pColor[3]/255;
					pColor[2]	= (DWORD)pColor[2]*pColor[3]/255;
					pColor		+= 4;
				}
				pPixel	+= m_pBuffer->Width();
			}
		}
		
		
		if(hWnd){
			RECT rc;
			::GetWindowRect(hWnd, &rc);
			{
				HMONITOR hMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
				MONITORINFO mi = {0};
				mi.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo( hMonitor, &mi );
				rc = mi.rcMonitor;
			}		
 			m_nxPos	= rc.left + (rc.right - rc.left  - m_pBuffer->Width())/2;
 			m_nyPos	= rc.top + (rc.bottom - rc.top - m_pBuffer->Height())/2;
		}else{
			m_nxPos	= (GetSystemMetrics(SM_CXFULLSCREEN)-m_pBuffer->Width())/2;
			m_nyPos	= (GetSystemMetrics(SM_CYFULLSCREEN)-m_pBuffer->Height())/2;
		}
	}

	{
		WNDCLASSEX wcx;

		wcx.cbSize			= sizeof(wcx);
		wcx.lpfnWndProc		= AfxWndProc;
		wcx.style			= CS_DBLCLKS|CS_SAVEBITS;
		wcx.cbClsExtra		= 0;
		wcx.cbWndExtra		= 0;
		wcx.hInstance		= AfxGetInstanceHandle();
		wcx.hIcon			= NULL;
		wcx.hCursor			= LoadCursor(NULL,IDC_ARROW);
		wcx.hbrBackground	= NULL;
		wcx.lpszMenuName	= NULL;
		wcx.lpszClassName	= _T("TDSplashScreen");
		wcx.hIconSm			= NULL;

		ATOM classAtom = RegisterClassEx(&wcx);

		if (classAtom==NULL)
			return FALSE;

		if (!CreateEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,wcx.lpszClassName,NULL,WS_POPUP,0,0,0,0,hWnd,NULL))
			return FALSE;
		
		Update(0);
		ShowWindow(SW_SHOW);
	}

	return TRUE;
}

void CSplashScreenEx::Start(DWORD dwTimeout, DWORD dwShowUpVelocity){
	m_dwTimeout			= dwTimeout;
	m_dwShowUpVelocity	= dwShowUpVelocity;
	if(m_Movie.pObject)
		SetTimer(TIMER_ID_MOVIE, 1000/m_Movie.dwFrameRate, NULL);
	else
		SetTimer(TIMER_ID_FADE_IN, 1, NULL);
}

BOOL CSplashScreenEx::Update(BYTE bAlpha)
{
	if(m_pBuffer){
		POINT ptSrc = {0, 0};
		m_Blend.SourceConstantAlpha		= bAlpha;

		CRect rectDlg;
		GetWindowRect(rectDlg);
		CPoint ptWindowScreenPosition(m_nxPos, m_nyPos);
		CSize szWindow(m_pBuffer->Width(), m_pBuffer->Height());

		{
			HDC		dcScreen	= ::GetDC(NULL);
			::UpdateLayeredWindow(m_hWnd, dcScreen, &ptWindowScreenPosition, &szWindow, m_pBuffer->GetDC(), &ptSrc, 0, &m_Blend, ULW_ALPHA);
			::ReleaseDC(NULL, dcScreen);
		}
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSplashScreenEx, CWnd)
	//ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CSplashScreenEx::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent){
	case TIMER_ID_FADE_IN:
		m_iFadeAlpha+= m_dwShowUpVelocity;
		if(m_iFadeAlpha>255) m_iFadeAlpha = 255;
		Update(m_iFadeAlpha);

		if(m_iFadeAlpha==255){
			KillTimer(TIMER_ID_FADE_IN);
			SetTimer(TIMER_ID_SHOW_TIME, 10, NULL);
		}
		break;
	case TIMER_ID_SHOW_TIME:
		if(m_dwTimeout<10){
			KillTimer(TIMER_ID_SHOW_TIME);
			SetTimer(TIMER_ID_FADE_OUT, 1, NULL);
		}else{
			m_dwTimeout	-= 10;
			if(GetKeyState(VK_ESCAPE)<0) m_dwTimeout = 0;
		}
		break;
	case TIMER_ID_FADE_OUT:
		m_iFadeAlpha-= m_dwShowUpVelocity;
		if(m_iFadeAlpha<0) m_iFadeAlpha = 0;

		Update(m_iFadeAlpha);
		if(m_iFadeAlpha==0){
			KillTimer(TIMER_ID_FADE_OUT);
			SetTimer(TIMER_ID_EXIT, 1, NULL);
		}
		break;
	case TIMER_ID_MOVIE:
		if(m_Movie.pObject && m_Movie.lNumOfFrames){
			m_Movie.pObject->GetFrame(m_Movie.lFrameCurrent);
			Update(255);
			m_Movie.lFrameCurrent++;
			m_Movie.lNumOfFrames--;
			if(GetKeyState(VK_ESCAPE)<0)
				goto MOVIE_EXIT;
		}else{
MOVIE_EXIT:
			KillTimer(TIMER_ID_MOVIE);
			SetTimer(TIMER_ID_EXIT, 1, NULL);
		}
		break;
	case TIMER_ID_EXIT:
		KillTimer(TIMER_ID_EXIT);
		DestroyWindow();
		break;
	}
}

void CSplashScreenEx::PostNcDestroy()
{
	CWnd::PostNcDestroy();
	delete this;
}
