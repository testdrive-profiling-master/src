#include "stdafx.h"
#include "Output.h"
#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "Localization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CToolBaseWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 탭 창을 만듭니다.
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_3D_VS2005, rectDummy, this, 1))
	{
		TRACE0("출력 탭 창을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 이미지 리스트를 생성
	m_TabImgList.Create(16, 16, ILC_COLOR32, 0, 0);
	ASSERT(m_TabImgList.m_hImageList);
	{
		CBitmap bmp;
		bmp.LoadBitmap(IDB_MSG_TAB);
		m_TabImgList.Add(&bmp,RGB(0,0,0));
	}
	m_wndTabs.SetImageList(m_TabImgList.m_hImageList);

	// 출력 창을 만듭니다.
	for(int i=0;i<COutput::TD_OUTPUT_SIZE;i++){
		if(!g_Output[i].Create(this, &m_wndTabs, &afxGlobalData.fontRegular, i+2)){
			TRACE0("출력 창을 만들지 못했습니다.\n");
			return -1;      // 만들지 못했습니다.
		}
		{
			LPCTSTR	sName	= NULL;
			switch (i) {
			case 0:sName = _S(TAB_SYSTEM); break;
			case 1:sName = _S(TAB_APPLICATION); break;
			case 2:sName = _S(TAB_NOTIFY); break;
			}
			m_wndTabs.AddTab(g_Output[i], sName, (UINT)i);
		}
	}

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CToolBaseWnd::OnSize(nType, cx, cy);

	// Tab 컨트롤은 전체 클라이언트 영역을 처리해야 합니다.
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::UpdateFonts()
{
	for(int i=0;i<COutput::TD_OUTPUT_SIZE;i++){
		g_Output[i].SetFont(&afxGlobalData.fontRegular);
	}
}
