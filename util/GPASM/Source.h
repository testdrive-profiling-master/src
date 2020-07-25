#ifndef __CLASS_CSOURCE__
#define __CLASS_CSOURCE__

#include "Log.h"
#include "IncludePath.h"

#define Source			(CSource::GetCurrent())
#define SourceOpen(s)	CSource::Open(s)
#define SourceRelease()	CSource::Release()

class CSource
{	// �ҽ� ���� ������ �ּ���/���� �ڵ�(enter/tab/double space) ó��
private:
	CSource*		m_pPrevSource;					// ���� �ҽ�
	static CSource*	m_pCurSource;					// �ֻ��� �ҽ�
	IncludePath*	m_pIncludePath;					// ���� include ���
//	static CNodeListData<int>* m_pSourceHistory;		// �ҽ��� ���� Ƚ��

protected:
	FILE*			m_pFile;						// ���� ������
	DWORD			m_dwLineCount;					// ���� ���� ī��Ʈ
	BOOL			m_bIsComment;					// �ּ� ó���� ����
	char			m_sSourceFileName[4096];		// �ҽ� ���� �̸�
	static char		m_sLineLatest[MAX_LINE];		// �ֱ��� ����
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