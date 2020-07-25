#include "stdafx.h"

typedef struct{
	struct{
		CString		original;
		CString		new_one;
	}exchange[1024];

	DWORD	dwCount;
}SYNTAX;

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc != 3){
		printf("Text replace. (" __DATE__ " : clonextop@gmail.com)\n");
		printf("Usage : TextReplace.exe syntax_file target_file\n\n");
		return 0;
	}

	SYNTAX		syn;
	syn.dwCount	= 0;
	{	// 문법 읽기
		FILE*	fp	= fopen(argv[1], "rt");
		char	line[4096];
		if(!fp){
			printf("*E : Syntax file is not found : %s.\n", argv[1]);
			return 0;
		}

		while(fgets(line, 4096, fp)){
			CString* sOrg	= &syn.exchange[syn.dwCount].original;
			CString* sNew	= &syn.exchange[syn.dwCount].new_one;
			*sOrg	= line;
			sOrg->Trim("\r\n");
			if(!sOrg->GetLength()) continue;


			while(fgets(line, 4096, fp)){
				*sNew	= line;
				sNew->Trim("\r\n");
				if(!sOrg->GetLength()) continue;
				syn.dwCount++;
				break;
			}
		}

		fclose(fp);
	}

	if(syn.dwCount)
	{	// 문장 바꾸기
		CString	sText;
		{	// 원문 읽기
			FILE*	fp	= fopen(argv[2], "rb");
			if(!fp){
				printf("*E : Target file is not found : %s.\n", argv[2]);
				return 0;
			}
			fseek(fp, 0, SEEK_END);
			int string_size	= ftell(fp);
			if(string_size <= 0){
				fclose(fp);
				goto PROCESS_END;
			}

			fseek(fp, 0, SEEK_SET);

			char* pMem = new char[string_size+1];

			fread(pMem, 1, string_size, fp);
			pMem[string_size]	= 0;
			sText = pMem;
			delete [] pMem;

			fclose(fp);
		}

		{	// 교체하기
			for(DWORD i=0;i<syn.dwCount;i++){
				while(sText.Replace(syn.exchange[i].original, syn.exchange[i].new_one));
			}
		}

		{	// 저장
			FILE*	fp	= fopen(argv[2], "wb");
			if(!fp){
				printf("*E : Can't overwrite this file : %s.\n", argv[2]);
				return 0;
			}

			fwrite((char*)sText.GetBuffer(), 1, sText.GetLength(), fp);

			fclose(fp);
		}
	}

PROCESS_END:

	return 0;
}
