#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include <string>
#include <list>

using namespace std;

typedef list<string>		LineList;

class CLineLink{
public:
	char		m_line[1024];
	CLineLink*	m_pNext;
	CLineLink(void){m_pNext = NULL;}
	~CLineLink(void){if(m_pNext) delete  m_pNext;}
	CLineLink* New(void){
		if(m_pNext) return NULL;
		m_pNext = new CLineLink;
		return m_pNext;
	}
};

class CLineMgr{
	CLineLink* m_pLinefirst;
	CLineLink* m_pLineCur;
public:
	CLineMgr(void){
		m_pLinefirst	= NULL;
		m_pLineCur		= NULL;
	}
	~CLineMgr(void){
		if(m_pLinefirst) delete m_pLinefirst;
		m_pLineCur = NULL;
	}

	char* GetFirst(void){
		m_pLineCur = m_pLinefirst;
		if(!m_pLineCur) return NULL;
		return m_pLineCur->m_line;
	}

	char* GetNext(void){
		if(!m_pLineCur) return NULL;
		m_pLineCur = m_pLineCur->m_pNext;
		if(!m_pLineCur) return NULL;
		return m_pLineCur->m_line;
	}

	char* New(void){
		if(!m_pLinefirst){
			m_pLinefirst	= new CLineLink;
			m_pLineCur		= m_pLinefirst;
		}else{
			if(!m_pLineCur) return NULL;
			m_pLineCur = m_pLineCur->New();
		}
		return m_pLineCur->m_line;
	}
};

const char* g_sNumberTag[4]={"st", "nd", "rd", "th"};

int _tmain(int argc, _TCHAR* argv[])
{
	FILE*		fp;
	char		line_buf[4096];
	char*		pTok;
	const char*	pNumTag = NULL;
	int			version[4] = { 1,0,0,0 };
	bool		bMajorUpdate = false;
	LineList	LineMap;

	if (argc == 3) {
		if (!strcmp(argv[2], "major")) bMajorUpdate = true;
		else
			if (!strcmp(argv[2], "minor")) bMajorUpdate = false;
			else {
				printf("invalid argument : %s\n", argv[2]);
				return 0;
			}
	} else if (argc != 2) {
		printf("Version auto increaser\n\nUsage : UpVersion resource_file [major/minor]\n\nUpdate FILEVERSION/PRODUCTVERSION/VALUE \"FileVersion\"/VALUE \"ProductVersion\"/ID_PROJECT_VERSION");
		return 0;
	}

	{
		// read version
		if (!(fp = fopen(argv[1], "rt"))) {
			printf("Resource file is not existed\n");
			return 0;
		}
		while(fgets(line_buf, 4095, fp) != NULL) {
			char		token_buf[4096];
			strcpy(token_buf, line_buf);
			pTok = strtok(token_buf, " \t");

			if (pTok) {

				if (!strcmp(pTok, "FILEVERSION")) {
					for (int i = 0; i < 4; i++) {
						version[i] = atoi(strtok(NULL, " ,\t\n\r"));
					}
					if (bMajorUpdate) {
						version[2]++;
						version[3] = 1;
					}
					else {
						version[3]++;
					}
					sprintf(line_buf, " FILEVERSION %d,%d,%d,%d\n", version[0], version[1], version[2], version[3]);
					pNumTag = g_sNumberTag[version[3] > 4 ? 3 : (version[3] - 1)];
				}
				else if (pNumTag) {
					if (!strcmp(pTok, "PRODUCTVERSION")) {
						sprintf(line_buf, " PRODUCTVERSION %d,%d,%d,%d\n", version[0], version[1], version[2], version[3]);
					}
					else if (!strcmp(pTok, "VALUE")) {
						pTok = strtok(NULL, " ,\"\t");
						if (!strcmp(pTok, "FileVersion")) {
							sprintf(line_buf, "            VALUE \"FileVersion\", \"%d.%d.%d - %d%s release\"\n", version[0], version[1], version[2], version[3], pNumTag);
						}
						else if (!strcmp(pTok, "ProductVersion")) {
							sprintf(line_buf, "            VALUE \"ProductVersion\", \"%d.%d.%d - %d%s release\"\n", version[0], version[1], version[2], version[3], pNumTag);
						}
					}
					else if (!strcmp(pTok, "ID_PROJECT_VERSION")) {
						sprintf(line_buf, "    ID_PROJECT_VERSION      \"%d.%d.%d - %d%s release\"\n", version[0], version[1], version[2], version[3], pNumTag);
					}
				}
			}
			// put text to buffer
			LineMap.push_back(string(line_buf));
		}
		fclose(fp);
	}

	// print the new version
	printf("현재 프로젝트 버전 : %d.%d.%d - %d%s release\n", version[0], version[1], version[2], version[3], pNumTag);

	// write version
	{
		if (!(fp = fopen((char*)argv[1], "wt"))) {
			printf("Can't open the resource file to write.\n");
			return 0;
		}
		
		for(LineList::iterator i = LineMap.begin();i != LineMap.end(); i++){
			fprintf(fp, (*i).c_str());
		}

		fclose(fp);
	}

	// save header file
	{
		FILE* fp = fopen("TestDriveVersion.h", "wb");
		if(!fp){
			printf("Can't open 'TestDriveVersion.h' file.\n");
			return 0;
		}

		fprintf(fp,
			"#ifndef __TESTDRIVE_VERSION_H__\r\n" \
			"#define __TESTDRIVE_VERSION_H__\r\n" \
			"\r\n" \
			"#define TESTDRIVE_VERSION		0x%08X\r\n" \
			"\r\n" \
			"#endif//__TESTDRIVE_VERSION_H__\r\n",
			version[2] + (version[1]<<16) + (version[0]<<24)
			);

		fclose(fp);
	}

	return 0;
}

