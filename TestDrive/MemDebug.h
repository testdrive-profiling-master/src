#ifndef __MEM_DEBUG_H__
#define __MEM_DEBUG_H__

#if defined(WIN32) && (defined(DEBUG) || defined(_DEBUG))
	#include <windows.h>
	#define _CRTDBG_MAP_ALLOC
	#define _CRTDBG_MAP_ALLOC_NEW
	#include <crtdbg.h>

	#define CHECK_MEMORY_LEAK				_ASSERTE( _CrtCheckMemory( ) );
	#define SET_MEMORY_ASSERT_BREAK(idx)	_CrtSetBreakAlloc(idx)

	#if __cplusplus
		extern "C" {
	#endif
		void MemoryDebug_Initialize(void);
		void MemoryDebug_Uninitialize(void);
	#if __cplusplus
		};
	#endif
#else
	#define CHECK_MEMORY_LEAK
	#define SET_MEMORY_ASSERT_BREAK(idx)

// #if defined(USE_EGL_DEBUG) && defined(WIN32)	// for memory debugging
// #include "mmgr/mmgr.h"
// #endif
	#define MemoryDebug_Initialize()
	#define MemoryDebug_Uninitialize()
#endif


#endif //__MEM_DEBUG_H__
