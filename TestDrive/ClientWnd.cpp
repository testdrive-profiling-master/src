#include "stdafx.h"

#include "ClientWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(ClientWnd, CWnd)
ClientWnd::ClientWnd()
{
}

ClientWnd::~ClientWnd()
{
}

BEGIN_MESSAGE_MAP(ClientWnd, CWnd)
	//ON_WM_CREATE()
	//ON_WM_SIZE()
END_MESSAGE_MAP()

/*
int ClientWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void ClientWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

}
*/