#include "StdAfx.h"
#include "ChartColor.h"

CChartColor::CChartColor(COLORREF c)
{
	color = c;
}

CChartColor::~CChartColor(void)
{
}

BOOL CChartColor::Paser(CPaser* pPaser){
	if(!pPaser) return FALSE;
	int r, g, b;
	if(!pPaser->GetTokenInt(&r)) return FALSE;
	if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) return FALSE;
	if(!pPaser->GetTokenInt(&g)) return FALSE;
	if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) return FALSE;
	if(!pPaser->GetTokenInt(&b)) return FALSE;
	color = RGB(r,g,b);

	return TRUE;
}

void CChartColor::SetColor(int r, int g, int b){
	color = RGB(r,g,b);
}

void CChartColor::operator=(COLORREF c){
	color = c;
}


CChartColor::operator COLORREF() const{
	return color;
}