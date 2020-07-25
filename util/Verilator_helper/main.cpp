#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <Windows.h>
using namespace std;
// Unicode build only

int _tmain(int argc, _TCHAR* argv[])
{
	wstring	sArg;	// argument path
	wstring	sExe;	// verilator path
	wstring	sCmd;	// modified command line

					// make arguments
	if (argc != 1) {
		// get given arguments
		for (int i = 1; i<argc; i++) {
			if (i != 1) sArg += _T(" ");
			sArg += argv[i];
		}
		// get extra arguments from file description
		{
			TCHAR	sEnv[4096];
			if (GetEnvironmentVariable(_T("SIM_TOP_FILE"), sEnv, 4096)) {
				wstring	sPath(sEnv);
				wstring::size_type	pos;
				// delete '"'
				for (; (pos = sPath.find(_T('\"'))) != wstring::npos;) {
					sPath.erase(pos, 1);
				}
				// change linux style path to windows'
				for (; (pos = sPath.find(_T('/'))) != wstring::npos;) {
					sPath[pos] = _T('\\');
				}
				if ((pos = sPath.find_last_of(_T('\\'))) != wstring::npos) {
					sPath.erase(pos, sPath.length() - pos);
					sPath += _T("\\.verilator");
					FILE* fp = _tfopen(sPath.c_str(), _T("rt"));
					if (fp) {
						TCHAR sLine[4096];
						if (_fgetts(sLine, 4096, fp)) {
							sArg += _T(" ");
							sArg += sLine;
							for (; (pos = sArg.find(_T('\n'))) != wstring::npos;) sArg.erase(pos, 1);
							for (; (pos = sArg.find(_T('\r'))) != wstring::npos;) sArg.erase(pos, 1);
						}
						fclose(fp);
					}
				}
			}
		}
	}
	{	// get verilator path
		TCHAR	sPath[4096];
		GetModuleFileName(NULL, sPath, 4096);
		sExe = sPath;
		wstring::size_type	pos = sExe.find_last_of(_T('\\'));
		sExe.erase(pos, sExe.length() - pos);
		sExe += _T("\\msys64\\mingw64\\bin");

		{	// set 'VERILATOR_ROOT' environment variable
			wstring	sEnv(sExe);
			wstring::size_type	pos;
			for (; (pos = sEnv.find(_T('\\'))) != wstring::npos;) sEnv[pos] = _T('/');
			SetEnvironmentVariable(_T("VERILATOR_ROOT"), sEnv.c_str());
		}

		sExe += _T("\\verilator");
		
	}

	{	// make command
		sCmd += _T("perl \"");
		sCmd += sExe;
		sCmd += _T('\"');

		if (sArg.size()) {
			sCmd += ' ';
			sCmd += sArg;
		}
	}

	return _tsystem(sCmd.c_str());
}
