#pragma once
#include "Paser.h"
class CChartColor
{
public:
	CChartColor(COLORREF c = RGB(0,0,0));
	~CChartColor(void);

	BOOL Paser(CPaser* pPaser);
	void SetColor(int r, int g, int b);
	void operator=(COLORREF c);
	operator COLORREF() const;

	COLORREF	color;
};
