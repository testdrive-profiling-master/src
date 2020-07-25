#ifndef __CLASS_CLOG__
#define __CLASS_CLOG__
#include "define_base.h"

typedef enum{
	LOG_NONE,
	LOG_WARNING,
	LOG_ERROR,
	LOG_INFORMATION
} LOG_ID;

const char* _L(const char *format, ...);	// line maker

class CLog
{
	FILE	*m_fpLogFile;
protected:
	int		m_iErrorCount;
	int		m_iWarningCount;
	
	char	m_sLastLog[MAX_LINE];

public:
	CLog(void);
	~CLog(void);

	static BOOL		m_bSilentLog;

	BOOL Create(const char *filename, BOOL echo = true);
	void Release(void);
	int GetWarningCount(void){return m_iWarningCount;}
	int GetErrorCount(void){return m_iErrorCount;}
	int LogOut(const char *msg, LOG_ID id = LOG_INFORMATION, BOOL bEcho = TRUE);

};

#endif