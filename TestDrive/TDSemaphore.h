#pragma once
#include "TestDrive.h"

class TDSemaphore : public ITDSemaphore
{
	HANDLE				m_Sema;
public:
	TDSemaphore(int iInitValue);
	~TDSemaphore(void);

	STDMETHOD_(int, Down)(void);
	STDMETHOD_(int, Up)(void);
	STDMETHOD_(void, Release)(void);
};
