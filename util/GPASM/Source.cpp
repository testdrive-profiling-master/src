#include "Source.h"
#include "IncludePath.h"

CSource*	CSource::m_pCurSource	= NULL;
//CNodeListData<int>* CSource::m_pSourceHistory	= NULL;
char		CSource::m_sLineLatest[MAX_LINE];


#ifdef _DEBUG
#define DEBUG_OUT(str)	printf(str)
#else
#define DEBUG_OUT(str)
#endif

CSource::CSource(const char* filename)
{
	m_bIsComment		= FALSE;
	m_dwLineCount		= 0;
	m_pPrevSource		= m_pCurSource;
	m_pCurSource		= this;
	m_pFile				= NULL;
	m_pIncludePath		= NULL;
	strcpy(m_sSourceFileName, filename);
	*m_sLineLatest		= NULL;
	m_bSetNewLine		= FALSE;

	{	// search source file
		m_pFile = fopen(filename, "rb");
		if(!m_pFile){	// searching include path....
			char	sSearchPath[4096];
			IncludePath* pIncPath	= IncludePath::Head();
			while(!m_pFile && pIncPath){
				if (pIncPath->IsEnabled()) {
					sprintf(sSearchPath, "%s/%s", pIncPath->Path(), filename);
					m_pFile = fopen(sSearchPath, "rb");
				}
				pIncPath	= pIncPath->Next();
			}
			if(m_pFile)
				strcpy(m_sSourceFileName, sSearchPath);
		}
		if(!m_pFile) delete this;
		else {
			SetCurrentIncludePath(m_sSourceFileName);
			if (m_pPrevSource) m_pPrevSource->SetIncludePathEnable(FALSE);
		}
	}

	DEBUG_OUT(_L("CSource : open source file (\"%s\" -%s)\n", filename, m_pFile ? "TRUE" : "FALSE"));
}

CSource::~CSource(void)
{
	DEBUG_OUT(_L("CSource : close source file (\"%s\")\n", m_sSourceFileName));

	if(m_pFile) fclose(m_pFile);
	SAFE_DELETE(m_pIncludePath);
	if (m_pPrevSource) m_pPrevSource->SetIncludePathEnable();
	m_pCurSource = m_pPrevSource;
}

void CSource::Release(void)
{
	while(m_pCurSource) delete m_pCurSource;
}

BOOL CSource::Open(const char* filename)
{
	CSource *pSource = new CSource(filename);
	if(pSource != m_pCurSource) return FALSE;
	return TRUE;
}

void CSource::SetCurrentIncludePath(const char* sPath) {
	char sIncludePath[4096];
	{
		char* sFilePart;
		GetFullPathName(sPath, 4096, sIncludePath, &sFilePart);
		if (sFilePart) *sFilePart = NULL;
		SAFE_DELETE(m_pIncludePath);
		m_pIncludePath = new IncludePath(sIncludePath);
	}
}

void CSource::SetIncludePathEnable(BOOL bEnable) {
	if (m_pIncludePath) m_pIncludePath->SetEnable(bEnable);
}

BOOL CSource::GetLine(char *line)
{
	char temp[MAX_LINE];
	if(line) *line = NULL;
	if(m_bSetNewLine) {m_bSetNewLine = FALSE; return TRUE;}
RETRY_GETLINE:
	if(!fgets(temp, MAX_LINE, m_pFile)){	// EOF
		delete this;
		if(m_pCurSource) return m_pCurSource->GetLine(line);
		return FALSE;
	}

	m_dwLineCount++;			// line count up

	TrimComment(temp);
	if(!strlen(temp)) goto RETRY_GETLINE;

	strcpy(line ? line : m_sLineLatest, temp);
	return TRUE;
}

void CSource::TrimComment(char *line)
{	// �ּ� ���� // or /* */
	if(!line) return;
	char *chars, *chars2;

	// Enter�ڵ� ����
	chars = strchr(line, '\n');	if(chars) *chars = NULL;
	chars = strchr(line, '\r');	if(chars) *chars = NULL;

	// */ �ڵ� ����
	if(m_bIsComment){
		chars = strstr(line, "*/");
		if(chars){
			strcpy(line, (chars+2));
			m_bIsComment = FALSE;
		}else *line = NULL;
	}

	// /* �ڵ� ����
	if(!m_bIsComment)
	do{
		if((chars = strstr(line, "/*"))){
			if((chars2 = strstr(line, "//")))
				if(chars2 < chars) break;

			if((chars2 = strstr(chars+2, "*/")))
				strcpy(chars, chars2+2);
			else{
				*chars			= NULL;
				m_bIsComment	= TRUE;
				break;
			}
		}
	}while(chars);

	// // �ڵ� ����
	chars = strstr(line, "//");
	if(chars) *chars=NULL;

	// TAB�ڵ� SPACE�� ��ȯ
	while((chars=strchr(line,'\t'))) *chars = ' ';
	// double space "  "�� �ϳ��� �ٲ�
	while((chars=strstr(line,"  "))) strcpy(chars, (chars+1));
	// �� SPACE ����
	while(1){
		if (strlen(line))
			if(line[strlen(line)-1]==' ') {line[strlen(line)-1]=NULL; continue;}
		break;
	}
	// �� SPACE ����
	while(*line == ' ') strcpy(line,(line+1));
}