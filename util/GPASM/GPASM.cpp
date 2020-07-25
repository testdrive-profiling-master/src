#include "GPCompile.h"

const char* g_sHelpMessage =	"Usage : GPASM [options] source_file [output_file]\n\n"\
								"Options :\n"\
								"   -? | -help    : Prints this message\n"
								"   -debug        : Prints out additional log of syntax\n"\
								"                   progressive tracing log.\n"\
								"   -log log_file : Set the log file name, if you don't named,\n"\
								"                   the log file has same name with output file.\n"\
								"   -silent       : no log output.\n" \
								"   -i inc_path   : Add include path.\n";

BOOL		g_bDebugMode	= FALSE;

void ChangeFileExtension(char *filename, const char* extension)
{
	char* pTok = filename;
	if(!filename || !extension) return;

	if(strlen(filename)>1)
	while(1){
		if(strchr(pTok+1, '.')) pTok = strchr(pTok+1, '.');
		else break;
	}
	if(pTok!=filename) *pTok=NULL;
	strcat(filename, ".");
	strcat(filename, extension);
}

int MainFunction(int argc, char* argv[])
{
	// Usage
	if(argc<2){
SHOW_HELP_MESSAGE:
		// Title
		printf("General Purpose Assembly(GPASM) v0.6 : (" __DATE__ " : clonextop@gmail.com)\n\n");
		printf(g_sHelpMessage);
		return 0;
	}

	//// filename parsing
	char src_file[MAX_PATH], dest_file[MAX_PATH], log_file[MAX_PATH];
	*src_file = *dest_file = *log_file = NULL;

	// source
	{
		int i;
		char* pName;
		for(i=1;i<argc;i++){
			if(argv[i][0]=='-'){
				pName = &(argv[i][1]);
				//strlwr(pName);
				if(!strcmp(pName, "help") || !strcmp(pName, "?")) {
					goto SHOW_HELP_MESSAGE;
				}else
					if(!strcmp(pName, "debug")){
						g_bDebugMode = TRUE;
					}else
					if (!strcmp(pName, "silent")) {
						CLog::m_bSilentLog = TRUE;
					}else
					if(!strcmp(pName, "log")){
						i++;
						if(i==argc) goto UNEXPECTED_OPTION;
						strcpy(log_file, argv[i]);
					}else
					if(!strcmp(pName, "i")){
						i++;
						if(i==argc) goto UNEXPECTED_OPTION;
						// Add includes path
						new IncludePath(argv[i]);
					}else{
UNEXPECTED_OPTION:
						printf("Unexpected option \"%s\"\n", pName);
						return 1;
					}
			}else{	// Set the source / dest filename
				pName = argv[i];
				if(!*src_file) strcpy(src_file, pName);
				else if(!*dest_file) strcpy(dest_file, pName);
				else{
					printf("Unexpected parameter \"%s\"\n", pName);
					return 1;
				}
			}
		}
	}

	// source file
	if(!*src_file){
		printf("You must describe a source file at least.\n");
		return 1;
	}

	// output file
	if(!*dest_file){
		strcpy(dest_file, src_file);
		ChangeFileExtension(dest_file, "obj");
	}

	// log file
	if(!*log_file){
		strcpy(log_file, dest_file);
		ChangeFileExtension(log_file, "log");
	}

	//// compile process
	CCompileResource	cRsc;
	// resource initialize
	cRsc.m_Log.Create(log_file);
	cRsc.m_pSourceFile		= new CNodeStr(src_file);

	cRsc.m_Log.LogOut(_L("Compile the source file(\"%s\") to output file(\"%s\").", src_file, dest_file));

	// open source file
	if(!SourceOpen(src_file)){
		cRsc.m_Log.LogOut(_L("Can't open the source file : %S!", src_file));
		return 1;
	}

	// compilation
	CGPCompile cc(&cRsc);
	if(!cc.Compile()){
		cRsc.m_Log.LogOut("Compilation is aborted by error!");
		return 1;
	}

	// close the source & include paths

	if(!cc.SaveObject(dest_file)){
		cRsc.m_Log.LogOut("Compilation is aborted by error!");
		return 1;
	}else{
		cRsc.m_Log.LogOut(_L("Output object file : %s", dest_file));
	}

	return 0;
}

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	printf("Debug mode is activated.\n\n");
#endif

	int iRet	= MainFunction(argc, argv);

	SourceRelease();
	IncludePathReleaseAll();
	return iRet;
}
