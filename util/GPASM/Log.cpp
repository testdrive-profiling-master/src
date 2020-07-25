#include "Log.h"
#include "Source.h"
#include <stdarg.h>

CLog	g_CompileLog;

BOOL	CLog::m_bSilentLog	= FALSE;

const char* _L(const char *format, ...)
{
	static char line[MAX_LINE];
	va_list varg;
	if(format){
		va_start(varg, format);
		_vsnprintf(line, MAX_LINE, format, varg);
		va_end(varg);
	}
	return line;
}

CLog::CLog(void)
{
	m_fpLogFile	= NULL;
	Release();
}

CLog::~CLog(void)
{
	Release();
}

BOOL CLog::Create(const char* filename, BOOL echo)
{
	Release();
	if(filename)
	if(!(m_fpLogFile = fopen(filename, "w"))) return FALSE;
	return TRUE;
}

void CLog::Release(void)
{
	if(m_fpLogFile) fclose(m_fpLogFile);
	m_fpLogFile		= NULL;
	*m_sLastLog		= NULL;
}

int CLog::LogOut(const char *msg, LOG_ID id, BOOL bEcho)
{

	if(!msg) return 0;

	// tag
	switch(id){
	case LOG_WARNING:
		strcpy(m_sLastLog, "Warning : ");
		m_iWarningCount++;
		break;
	case LOG_ERROR:
		strcpy(m_sLastLog, "Error : ");
		m_iErrorCount++;
		break;
	case LOG_INFORMATION:
		*m_sLastLog = NULL;
		break;
	//case LOG_NONE:
	default:
		return 0;
	}

	// file/line info.
	CSource* pSource = Source;
	if(id != LOG_INFORMATION)
	if(pSource) sprintf(m_sLastLog, "%s[%d line - \"%s\"] ", m_sLastLog, pSource->GetLineCount(), pSource->GetFileName());
	// combinate
	strcat(m_sLastLog, msg);
	strcat(m_sLastLog, "\n");
	// output
	if(!m_bSilentLog && bEcho) printf(m_sLastLog);
	if(m_fpLogFile) fprintf(m_fpLogFile, m_sLastLog);

	return 0;
}