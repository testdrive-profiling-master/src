#include "stdafx.h"

WMIProcess*		g_pWMI		= NULL;
CString			g_sTarget;
CString			g_sParam;
BOOL			g_bFailed	= FALSE;
OPTIONS			g_Options	= {
	FALSE,		// retry
	FALSE,		// debug
	2000,		// 2 second
};
