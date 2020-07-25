#include "StdAfx.h"
#include "TDSemaphore.h"

TDSemaphore::TDSemaphore(int iInitValue){
	m_Sema	= CreateSemaphore(NULL, iInitValue, 1024, NULL);
}

TDSemaphore::~TDSemaphore(void){
	CloseHandle(m_Sema);
}

int TDSemaphore::Down(void){
	return WaitForSingleObject(m_Sema, INFINITE);
}

int TDSemaphore::Up(void){
	return ReleaseSemaphore(m_Sema, 1, NULL);
}

void TDSemaphore::Release(void){
	delete this;
}