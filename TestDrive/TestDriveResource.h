#pragma once

class CTestDrive;

class TestDriveResource {
public:
	friend class CTestDrive;

	TestDriveResource(void);
	virtual ~TestDriveResource(void);

	static LPCTSTR InstalledPath(void) {
		return m_sInstalledPath;
	}

	static LPCTSTR ProjectPath(void) {
		return m_sProjectDir;
	}

private:
	static TCHAR		m_sInstalledPath[4096];
	static CString		m_sProjectDir;
	static CString		m_sProjectFile;
};

void DispatchEnvionmentString(LPCTSTR sIn, CString& sOut);
