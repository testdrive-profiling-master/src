#pragma once
#include "WMIQuery.h"

class WMIProcess : protected WMIQuery{
public:
	WMIProcess(WMIConnection& connection);
	virtual ~WMIProcess(void);

	inline void Refresh(void)				{Open();}
	inline BOOL Next(void)					{return MoveNext();}

	inline 	CString Name(void)				{return GetPropertyStr(L"Caption");}
	inline	DWORD PID(void)					{return GetPropertyInt(L"Handle");}
	inline	DWORD SessionID(void)			{return GetPropertyInt(L"SessionId");}
	inline 	CString CommandLine(void)		{return GetPropertyStr(L"CommandLine");}
	inline	DWORD ParentPID(void)			{return GetPropertyInt(L"ParentProcessId");}
	inline	DWORD ThreadCount(void)			{return GetPropertyInt(L"ThreadCount");}
	inline	DWORD HandleCount(void)			{return GetPropertyInt(L"HandleCount");}
	inline 	CString CreationDate(void)		{return GetPropertyStr(L"CreationDate");}
	inline	DWORD PageFileUsage(void)		{return GetPropertyInt(L"PageFileUsage");}
	inline	DWORD PeakPageFileUsage(void)	{return GetPropertyInt(L"PeakPageFileUsage");}
	inline	DWORD WorkingSetSize(void)		{return GetPropertyInt(L"WorkingSetSize");}
	inline	DWORD PeakWorkingSetSize(void)	{return GetPropertyInt(L"PeakWorkingSetSize");}
	inline	ULONG64 UserModeTime(void)		{return GetPropertyInt64(L"UserModeTime");}
	inline	ULONG64 KernelModeTime(void)	{return GetPropertyInt64(L"KernelModeTime");}
};
