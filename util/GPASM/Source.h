#ifndef __CLASS_CSOURCE__
#define __CLASS_CSOURCE__

#include "Log.h"
#include "IncludePath.h"

#define Source			(CSource::GetCurrent())
#define SourceOpen(s)	CSource::Open(s)
#define SourceRelease()	CSource::Release()

class CSource
{	// 소스 파일 관리와 주석문/각종 코드(enter/tab/double space) 처리
private:
	CSource*		m_pPrevSource;					// 이전 소스
	static CSource*	m_pCurSource;					// 최상위 소스
	IncludePath*	m_pIncludePath;					// 현재 include 경로
//	static CNodeListData<int>* m_pSourceHistory;		// 소스가 열린 횟수

protected:
	FILE*			m_pFile;						// 파일 포인터
	DWORD			m_dwLineCount;					// 현재 라인 카운트
	BOOL			m_bIsComment;					// 주석 처리중 상태
	char			m_sSourceFileName[4096];		// 소스 파일 이름
	static char		m_sLineLatest[MAX_LINE];		// 최근의 라인
	BOOL			m_bSetNewLine;

	void TrimComment(char *line);
	void SetCurrentIncludePath(const char* sPath);

public:
	CSource(const char* filename);
	~CSource(void);

	static CSource* GetCurrent(void) {return m_pCurSource;}
	static BOOL Open(const char* filename);
	DWORD GetLineCount(void) {return m_dwLineCount;}
	char* GetFileName(void) {return m_sSourceFileName;}
	BOOL GetLine(char *line = NULL);

	void SetLatestLine(const char* line) {if(line) {strcpy(m_sLineLatest, line);m_bSetNewLine=TRUE;}}
	char* GetLatestLine(void) {return m_sLineLatest;}
	void SetIncludePathEnable(BOOL bEnable = TRUE);
	static void Release(void);
};

#endif