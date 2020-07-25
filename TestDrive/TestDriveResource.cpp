#include "stdafx.h"
#include "TestDriveResource.h"

TCHAR		TestDriveResource::m_sInstalledPath[4096] = { '\0' };
CString		TestDriveResource::m_sProjectDir;
CString		TestDriveResource::m_sProjectFile;

TestDriveResource::TestDriveResource(void) {
	if (!*m_sInstalledPath) {
		// Search installed path
		TCHAR filename[4096];
		TCHAR* pPart;
		GetModuleFileName(NULL, filename, 4096);
		GetFullPathName(filename, 4096, m_sInstalledPath, &pPart);
		*pPart = NULL;
	}
}

TestDriveResource::~TestDriveResource(void) {

}

void DispatchEnvionmentString(LPCTSTR sIn, CString& sOut)
{
	int	iPos;
	CString sStr(sIn);

	while ((iPos = sStr.Find(_T("$("))) >= 0) {
		int iEndPos = sStr.Find(_T(")"), iPos + 2);
		int iLength = iEndPos - iPos - 2;

		if (iEndPos > 0) {
			TCHAR sEnvVariable[1024];
			{
				CString	sEnv = ((LPCTSTR)sStr) + iPos + 2;
				sEnv.Delete(iLength, -1);
				GetEnvironmentVariable(sEnv, sEnvVariable, 1023);
			}
			sStr.Delete(iPos, iLength + 3);
			sStr.Insert(iPos, sEnvVariable);
		}
		else break;
	}

	sOut = sStr;
}
