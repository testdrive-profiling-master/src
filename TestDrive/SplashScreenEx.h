#pragma once
#include "Buffer.h"
#include "AVIGenerator.h"

class CSplashScreenEx : public CWnd
{
public:
	CSplashScreenEx();
	virtual ~CSplashScreenEx();

	BOOL Create(HWND hWnd, LPCTSTR sFileName);
	void Start(DWORD dwTimeout = 2000, DWORD dwShowUpVelocity = 15);
	void ReleaseAll(void);

protected:
	BOOL Update(BYTE bAlpha);

	typedef enum{
		TIMER_ID_FADE_IN,
		TIMER_ID_SHOW_TIME,
		TIMER_ID_FADE_OUT,
		TIMER_ID_MOVIE,
		TIMER_ID_EXIT
	}TIMER_ID;

	int		m_iFadeAlpha;
	DWORD	m_dwTimeout;
	DWORD	m_dwShowUpVelocity;

	int m_nxPos;
	int m_nyPos;

	BLENDFUNCTION		m_Blend;
	CBuffer*			m_pBuffer;
	struct{
		CAVIGenerator*		pObject;
		LONG				lFrameCurrent;
		LONG				lNumOfFrames;
		DWORD				dwFrameRate;
	}m_Movie;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual void PostNcDestroy();
};
