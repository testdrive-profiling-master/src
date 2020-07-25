#include "stdafx.h"
#include "Main.h"

#include "DocumentChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDocumentChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CDocumentChildFrame, CMDIChildWndEx)
END_MESSAGE_MAP()


CDocumentChildFrame::CDocumentChildFrame()
{

}

CDocumentChildFrame::~CDocumentChildFrame()
{
}


BOOL CDocumentChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

void CDocumentChildFrame::ActivateFrame(int nCmdShow){
	CMDIChildWndEx::ActivateFrame(nCmdShow);
}

#ifdef _DEBUG
void CDocumentChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CDocumentChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

