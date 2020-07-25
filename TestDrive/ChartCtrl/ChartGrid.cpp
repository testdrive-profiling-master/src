/*
 *
 *	ChartGrid.cpp
 *
 *	Written by C?ric Moonen (cedric_moonen@hotmail.com)
 *
 *
 *
 *	This code may be used for any non-commercial and commercial purposes in a compiled form.
 *	The code may be redistributed as long as it remains unmodified and providing that the 
 *	author name and this disclaimer remain intact. The sources can be modified WITH the author 
 *	consent only.
 *	
 *	This code is provided without any garanties. I cannot be held responsible for the damage or
 *	the loss of time it causes. Use it at your own risks
 *
 *	An e-mail to notify me that you are using this code is appreciated also.
 *
 *
 */

#include "stdafx.h"
#include "ChartGrid.h"
#include "ChartAxis.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChartGrid::CChartGrid() 
  : m_GridColor(RGB(128,128,128)), m_pParentCtrl(NULL), m_bIsVisible(true), 
    m_bIsHorizontal(true), m_lstTickPos()
{
}

CChartGrid::~CChartGrid()
{

}

void CChartGrid::AddTick(int Position)
{
	m_lstTickPos.push_back(Position);
}

void CChartGrid::ClearTicks()
{
	m_lstTickPos.clear();
}

void CChartGrid::Draw(CDC *pDC)
{
	if (!m_bIsVisible)
		return;
	if (!pDC->GetSafeHdc() )
		return;
	
	CRect plottingRect = m_pParentCtrl->GetPlottingRect();
	pDC->IntersectClipRect(plottingRect);

	CPen* pOldPen;
	//CPen SolidPen(PS_SOLID,0,m_GridColor);
	CPen DotPen(PS_DOT,0,m_GridColor);
	pOldPen = pDC->SelectObject(&DotPen);

	list<int>::iterator iter = m_lstTickPos.begin();
	int ActuPosition = 0;

	for (iter; iter!=m_lstTickPos.end(); iter++)
	{
		ActuPosition = *iter;

		if (!m_bIsHorizontal)
		{
			pDC->MoveTo(plottingRect.left, ActuPosition);
			pDC->LineTo(plottingRect.right, ActuPosition);

			/*int ActuX = plottingRect.left;

			while (ActuX<plottingRect.right)
			{
				pDC->MoveTo(ActuX,ActuPosition);
				ActuX += 3;
				pDC->LineTo(ActuX,ActuPosition);
				ActuX += 3;
			}*/
		}else{
			pDC->MoveTo(ActuPosition, plottingRect.bottom);
			pDC->LineTo(ActuPosition, plottingRect.top);

			/*int ActuY = plottingRect.bottom;

			while (ActuY>plottingRect.top)
			{
				pDC->MoveTo(ActuPosition,ActuY);
				ActuY -= 3;
				pDC->LineTo(ActuPosition,ActuY);
				ActuY -= 3;
			}*/
		}
	}

	pDC->SelectClipRgn(NULL);
	pDC->SelectObject(pOldPen);
	DotPen.DeleteObject();
	//SolidPen.DeleteObject();
}

void CChartGrid::SetVisible(bool bVisible)
{
	m_bIsVisible = bVisible;
	if (m_pParentCtrl)
		m_pParentCtrl->RefreshCtrl();
}

void CChartGrid::SetColor(COLORREF NewColor)
{
	m_GridColor = NewColor; 
	if (m_pParentCtrl)
		m_pParentCtrl->RefreshCtrl();
}

typedef enum{
	CMD_GRID_SET_VISIBLE,
	CMD_GRID_SET_COLOR,
	CMD_GRID_SIZE,
}CMD_GRID;

static const TCHAR* g_sChartGrid[CMD_GRID_SIZE]={
	_T("SetVisible"),
	_T("SetColor"),
};


BOOL CChartGrid::Paser(CPaser* pPaser){
	if(!pPaser) return FALSE;
	TCHAR		token[MAX_PATH];
	int id;

	if(!pPaser->GetTokenName(token)) return FALSE;
	id = CheckCommand(token, g_sChartGrid, CMD_GRID_SIZE);
	if(id >= CMD_GRID_SIZE) return FALSE;

	if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) return FALSE;

	switch(id){
	case CMD_GRID_SET_VISIBLE:
		if(!pPaser->GetTokenInt((int*)&m_bIsVisible)) return FALSE;
		break;
	case CMD_GRID_SET_COLOR:
		if(!m_GridColor.Paser(pPaser)) return FALSE;
		break;
	default: return FALSE;
	}

	if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) return FALSE;
	if(!pPaser->TokenOut(TD_DELIMITER_SEMICOLON)) return FALSE;

	return TRUE;
}