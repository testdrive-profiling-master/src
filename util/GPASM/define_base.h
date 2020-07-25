#ifndef __DEFINE_BASE__
#define __DEFINE_BASE__

#ifdef WIN32						// for deprecated function @ visual studio 8.0
#define WIN32_LEAN_AND_MEAN			// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <Windows.h>

#define MAX_PATH 260
#define MAX_LINE 2048

#define SAFE_DELETE(p)			{ if(p) { delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p); (p)=NULL; } }
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p) = NULL; } } 

/*
typedef unsigned int		DWORD;
typedef unsigned short		WORD;
typedef void				VOID;
typedef VOID*				LPVOID;
typedef bool				BOOL;

#define	TRUE				true
#define FALSE				false
*/
extern BOOL		g_bDebugMode;

#endif