#include "stdafx.h"
#include "WMIProcess.h"

WMIProcess::WMIProcess(WMIConnection& connection)
	: WMIQuery(connection, L"SELECT * FROM Win32_Process")
{
	Open();
}

WMIProcess::~WMIProcess(void){
	Close();
}
