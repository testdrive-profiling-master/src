#ifndef __INCLUDE_PATH_H__
#define __INCLUDE_PATH_H__
#include "define_base.h"
#include <string>

using namespace std;

#define IncludePathReleaseAll()	IncludePath::ReleaseAll()

class IncludePath
{
public:
	IncludePath(const char* sIncludePath);
	virtual ~IncludePath(void);

	IncludePath*	Next(void)	{return m_pNext;}
	void Set(const char* sIncludePath);
	static IncludePath*	Head(void)	{return m_pHead;}
	static void ReleaseAll(void);
	const char* Path(void)			{return m_sPath.c_str();}
	inline void SetEnable(BOOL bEnable = TRUE) {m_bEnable = bEnable;}
	inline BOOL IsEnabled(void)		{return m_bEnable;}

private:
	static IncludePath*		m_pHead;
	IncludePath*			m_pNext;

	string					m_sPath;
	BOOL					m_bEnable;
};

#endif//__INCLUDE_PATH_H__
