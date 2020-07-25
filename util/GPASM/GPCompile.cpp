#include "GPCompile.h"
// concrete message out
#ifdef _DEBUG
#define DEBUG_MESSAGE_OUT		TRUE
#else
#define DEBUG_MESSAGE_OUT		FALSE
#endif

//
#define CMSG(str, id)	m_rsc->m_Log.LogOut(str, id, DEBUG_MESSAGE_OUT)
// debug message out
#define DMSG(str)	m_rsc->m_Log.LogOut(str, g_bDebugMode ? LOG_INFORMATION : LOG_NONE)
#define LMSG(str)	m_rsc->m_Log.LogOut(str, g_bDebugMode ? LOG_INFORMATION : LOG_NONE, FALSE)

CGPCompile::CGPCompile(CCompileResource* rsc) {
	m_rsc				= rsc;
	m_pStr				= &rsc->m_Str;
	m_pCurVar			= NULL;
	m_dwDelimCount		= 0;
	m_dwFuncCount		= 0;
	m_dwFuncDepth		= 0;
}

CGPCompile::~CGPCompile(void) {}

BOOL CGPCompile::Compile(void)
{
	CStr	str;
	char	tok[MAX_LINE];
	DWORD	type;

	while(GetLine()){
		if(*Source->GetLatestLine()!='#') DMSG(_L("Compile[GetLine] : %s", Source->GetLatestLine()));
		m_pStr->Set(Source->GetLatestLine());
		type	= m_pStr->GetToken(tok);
RE_COMPILE:
		switch(type)
		{
		case STRTYPE_SPECIAL:		// 전처리 선행기
			if(*tok=='#') {
				if(!Preprocess()) return FALSE;
			}else goto STRTYPE_NAME_;
			break;
		case STRTYPE_ADDRESSTAG:	// 주소 처리기
			{
				if(m_dwFuncDepth){
					char temp[MAX_LINE];
					strcpy(temp, tok);
					sprintf(tok, "@%d:%s", m_dwFuncCount, temp);
				}
				CNodeData* pNode = m_rsc->NewVariable(tok);
				if(!pNode){
					CMSG(_L("Can't make new address tag. : %s", tok), LOG_ERROR);
					return FALSE;
				}else{
					DWORD type = pNode->GetType();
					if(!type) pNode->SetType(STRTYPE_ADDRESSTAG);
					else
					if(type != STRTYPE_ADDRESSTAG){
						CMSG(_L("Invalid type. : %s,(type #%d)", tok, type), LOG_ERROR);
						return FALSE;
					}
					if(pNode->IsSet()){
						CMSG(_L("Already address tag is initialized.: %s", tok), LOG_ERROR);
						return FALSE;
					}
				}
				pNode->SetData(m_rsc->m_pCurPC->GetData());
			}
			break;
		case STRTYPE_NAME:
STRTYPE_NAME_:
			m_bFuncCallFirst	= TRUE;
			if(!Function(tok)) return FALSE;
			break;
		default:
			return FALSE;
			break;
		}
		type	= m_pStr->GetToken(tok);
		if(type) goto RE_COMPILE;
	}

	if(m_dwFuncDepth){
		CMSG(_L("Function Delimiters are not a pair"), LOG_ERROR);
		return FALSE;
	}

	CMSG("Compilation is completed.\n\nLinking...", LOG_INFORMATION);
	if(!Link()) return FALSE;
	CMSG("Linking is completed.", LOG_INFORMATION);


	const char *strname[] = {
		"NONE",
		"INT",
		"FLOAT",
		"HEX",
		"BINARY",
		"NAME",
		"ADDRESSTAG",
		"STRING",
		"SPECIAL",
		"FUNCTION"
	};
	
	LMSG("\n-------------------------------------------");
	{
		CNodeStr* pNode;
		DWORD i,type;
		for(i=0;i<2;i++){
			switch(i){
			case 0: LMSG("------------- define list -------------");
					pNode	= m_rsc->m_pNodeDefine;
					break;
			case 1: LMSG("------------- m_pNodeFunction -------------");
					pNode	= m_rsc->m_pNodeFunction;
					break;
			}

			while(pNode){
				type = pNode->GetType();
				LMSG(_L("%s [%s] : %s", pNode->GetName(), 
					type==STRTYPE_NONE ? "NONE" :
					type==STRTYPE_INT ? "INT" :
					type==STRTYPE_FLOAT ? "FLOAT" :
					type==STRTYPE_HEX ? "HEX" :
					type==STRTYPE_BINARY ? "BINARY" :
					type==STRTYPE_NAME ? "NAME" :
					type==STRTYPE_ADDRESSTAG ? "ADDRESS" :
					type==STRTYPE_STRING ? "STRING" :
					type==STRTYPE_SPECIAL ? "SPECIAL" :
					type==STRTYPE_FUNCTION ? "FUNCTION" : "Invalid"
					, pNode->GetData()));
				pNode = pNode->GetNext();
			}
		}
	}

	{
		char	line[1024];
		CNodeData* pNode;
		DWORD i,type;
		for(i=0;i<1;i++){
			switch(i){
			case 0: LMSG("------------- m_pNodeVariable -------------");
				pNode	= m_rsc->m_pNodeVariable;
				break;
			}

			while(pNode){
				type = pNode->GetType();
				sprintf(line, _L("%s [%s (%s)] :", pNode->m_pName,
					type==STRTYPE_NONE ? "NONE" :
					type==STRTYPE_INT ? "INT" :
					type==STRTYPE_FLOAT ? "FLOAT" :
					type==STRTYPE_NAME ? "NAME" :
					type==STRTYPE_STRING ? "STRING" :
					type==STRTYPE_ADDRESSTAG ? "ADDRESS" : "Invalid"
					, pNode->IsSet() ? "Set" : "Unset"));
				switch(type){
				case STRTYPE_FLOAT:
					LMSG(_L("%s %f", line, (*(float*)&pNode->m_iData)));
					break;
				case STRTYPE_NAME:
					LMSG(_L("%s %s", line, pNode->m_sData));
					break;
				case STRTYPE_STRING:
					LMSG(_L("%s \"%s\"", line, pNode->m_sData));
					break;
				default:
					LMSG(_L("%s %d", line, pNode->m_iData));
				}
				
				pNode = pNode->GetNext();
			}
		}
	}
	LMSG("-------------------------------------------\n");

	return TRUE;
}

BOOL CGPCompile::SaveObject(const char* file_name)
{
	FILE* fp;
	DWORD dwCodeSize = 0;
	
	if(!(fp = fopen(file_name, "wb"))) {
		CMSG(_L("Can't make new object file : %s", file_name), LOG_ERROR);
		return FALSE;
	}

	CNodeCode* pCode = m_rsc->m_pNodeCode;
	// magic code
	fwrite(&pCode->m_dwCode, sizeof(DWORD), 1, fp);
	pCode = pCode->Next();

	fwrite(&dwCodeSize, sizeof(DWORD), 1, fp);	// store code size

	while(pCode){
		fwrite(&pCode->m_dwCode, pCode->m_dwBytes, 1, fp);
		dwCodeSize	+= pCode->m_dwBytes;
		pCode = pCode->Next();
	}
	fseek(fp, 4, SEEK_SET);
	fwrite(&dwCodeSize, sizeof(DWORD), 1, fp);	// store code size

	fclose(fp);
	return TRUE;
}

BOOL CGPCompile::Preprocess(void)
{
	char	token[MAX_LINE];
	// preprocess tag 얻기
	if(m_pStr->GetToken(token)!=STRTYPE_NAME){
ERROR_PREPROCESSOR_TAG:
		CMSG(_L("unacceptable preprocess tag : %s", token), LOG_ERROR);
		return FALSE;
	}

	if(!strcmp(token, "define")){	// define
		if(m_pStr->GetToken(token)!=STRTYPE_NAME) goto ERROR_PREPROCESSOR_NAME;	// define 이름 얻기

		CNodeStr* pNode = m_rsc->m_pNodeDefine->Add(token);
		char *pStr = m_pStr->Get(TRUE);
		while(*pStr == ' ') pStr++;
		if(!(int)strlen(pStr)) goto ERROR_PREPROCESSOR_NAME;

		DMSG(_L("Define : %s = \"%s\"", token, pStr));
		
		pNode->SetData(pStr);
		*m_pStr->m_pCur = NULL;
	}else
	if(!strcmp(token, "function")){	// define
		if(!m_pStr->GetToken(token)){	// function 이름 얻기
			CMSG(_L("No function defined"), LOG_ERROR);
			return FALSE;
		}
		//DMSG(_L("Preprocess_function : %s", token));

		// function 설정
		while(*m_pStr->m_pCur == ' ') m_pStr->m_pCur++;
		if (!strlen(m_pStr->m_pCur)){
			// 내용 없음.
			m_rsc->m_pNodeFunction->Add(token)->SetData("", STRTYPE_FUNCTION);
			return TRUE;
		}
		while(m_pStr->m_pCur[strlen(m_pStr->m_pCur)-1]=='\\'){
			char line[MAX_LINE];
			m_pStr->m_pCur[strlen(m_pStr->m_pCur)-1] = NULL;
			strcpy(line, m_pStr->m_pCur);
			if(GetLine()){
				strcat(line, Source->GetLatestLine());
				m_pStr->Set(line);
			}else{
				CMSG(_L("No line."), LOG_ERROR);
				return FALSE;
			}
		}
		m_rsc->m_pNodeFunction->Add(token)->SetData(m_pStr->m_pCur, STRTYPE_FUNCTION);
		m_pStr->m_pCur = &m_pStr->m_pStr[strlen(m_pStr->m_pStr)];

	}else
	if(!strcmp(token, "include")){	// include
			if(m_pStr->GetToken(token)!=STRTYPE_STRING) goto ERROR_PREPROCESSOR_NAME;	// 파일명 얻기
			DMSG(_L("Include : \"%s\"", token));
			m_rsc->m_pSourceFile->Add(token);
			if(!SourceOpen(token)){
				CMSG(_L("Can not open the source file : %s", token), LOG_ERROR);
				return FALSE;
			}
	}else{	// error
		goto ERROR_PREPROCESSOR_TAG;
ERROR_PREPROCESSOR_NAME:
		CMSG(_L("unacceptable preprocess name : %s", m_pStr->Get()), LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}

BOOL CustomSMASK(CNodeData* pWMASK, CNodeData* pSMASK){
	if(!pWMASK || !pSMASK) return FALSE;
	if(!pWMASK->m_sData || !pSMASK->m_sData) return FALSE;

	char *pW = pWMASK->m_sData;
	char *pS = pSMASK->m_sData;
	int wlen, slen, mlen, i, com;
	char smask[5];
	memset(smask, 'x', 5);
	wlen = (int)strlen(pW);
	slen = (int)strlen(pS);
	mlen = 0;
	for(i=0;i<wlen;i++){
		com =	(*pW =='x') ? 0 :
				(*pW =='y') ? 1 :
				(*pW =='z') ? 2 : 3;
		if(com >= mlen) mlen = com + 1;
		pW++;
		smask[com] = *pS;
		if((i+1)<slen) pS++;
	}
	smask[mlen] = NULL;
	pSMASK->SetString(smask);

	return TRUE;
}

BOOL CGPCompile::Function(const char* func_name)
{
	if(!func_name) return FALSE;
	CNodeStr* pNodeFunc	= m_rsc->m_pNodeFunction->Find(func_name);
	if(!pNodeFunc){
		pNodeFunc	= m_rsc->m_pNodeFunction->Find("_");
		if(!pNodeFunc){
			CMSG(_L("Can't find function name : %s", func_name), LOG_ERROR);
			return FALSE;
		}
		//CMSG(_L("TOO : %s", func_name), LOG_INFORMATION);
		m_pStr->Set(Source->GetLatestLine());
		//CMSG(_L("line : %s", m_pStr->m_pCur), LOG_INFORMATION);
	}

	CStr*	pTokStr;
	char	token[MAX_LINE];
	CStr	func(pNodeFunc->GetData());
	DWORD	type,op;
	BOOL	bInCourt	= FALSE;
	if(!func.Get()){
		CMSG(_L("Not defined function : %s", func_name), LOG_ERROR);
		return FALSE;
	}
	DMSG(_L("%-9s[%s] : %s", m_bFuncCallFirst ? "Interpret" : "", pNodeFunc->GetName(), m_pStr->m_pCur));
	m_bFuncCallFirst	= FALSE;

	while((type=func.GetToken(token)))
	switch(type){
	case STRTYPE_SPECIAL:
		switch(*token){
		case '#':
			bInCourt	= FALSE;
			pTokStr		= m_pStr;
			goto FUNCTION_INTERPRET;
		case '@':
			bInCourt	= TRUE;
			pTokStr		= &func;
FUNCTION_INTERPRET:
			if(!func.GetToken(token)){
FUNCTION_DESC_ERROR:
				CMSG(_L("Function description Syntex error [%s] : %s(token :  \"%s\")", func_name, func.Get(), token), LOG_ERROR);
				return FALSE;
			}
			if(token[1]) goto FUNCTION_DESC_ERROR;
			switch(*token){
			case 'M':	// message out
				if(pTokStr->GetToken(token) != STRTYPE_STRING) goto FUNCTION_DESC_ERROR;
				CMSG(token, LOG_INFORMATION);
				break;
			case 'E':	// error occur
				CMSG("Error occurred by description", LOG_ERROR);
				return FALSE;
				break;
			case 'N':	// next instruction
				{
					if(pTokStr->GetToken(token) != STRTYPE_INT) goto FUNCTION_DESC_ERROR;
					DWORD	dwBytes = pTokStr->RetrieveVariable();
					if(dwBytes<=0 || dwBytes>4){
						CMSG("Out of range for instruction word size", LOG_ERROR);
						return FALSE;
					}
					m_rsc->NextCode(dwBytes);
				}
				break;
			case 'P':	// previous instruction
				m_rsc->PrevCode();
				break;
			case 'F':	// recursive function description
				if(pTokStr->GetToken(token) != STRTYPE_NAME) goto FUNCTION_DESC_ERROR;
				if(!Function(token)) return FALSE;
				break;
			case 'I':	// insert line
				{
					char	line[MAX_LINE];
					BOOL	bRedirect	= FALSE;
					type = func.GetToken(token);

					if(type == STRTYPE_NAME){
						bRedirect	= TRUE;
						CNodeData* pV = m_rsc->m_pNodeVariable->Find(token);
						if(!pV) goto UNDEFINDABLE_VARIABLE;
						if(!pV->IsSet()) goto UNINTIALIZED_VARIABLE;
						switch(pV->GetType()){
						case STRTYPE_NAME:
						case STRTYPE_STRING:
							sprintf(line, "%s%s", pV->m_sData, m_pStr->m_pCur);
							break;
						case STRTYPE_INT:
							sprintf(line, "%d%s", pV->GetData(), m_pStr->m_pCur);
							break;
						case STRTYPE_HEX:
						case STRTYPE_BINARY:
							sprintf(line, "0x%X%s", pV->GetData(), m_pStr->m_pCur);
							break;
						case STRTYPE_FLOAT:
							{
								DWORD data = (DWORD)pV->GetData();
								sprintf(line, "%f%s", *(float*)&data, m_pStr->m_pCur);
							}
							break;
						default:
							goto FUNCTION_DESC_ERROR;
						}
					}else if(type&(STRTYPE_STRING|STRTYPE_INT|STRTYPE_HEX|STRTYPE_BINARY|STRTYPE_FLOAT)){
						sprintf(line, "%s%s", token, m_pStr->m_pCur);
					}else goto FUNCTION_DESC_ERROR;
					m_pStr->Set(line);
				}
				break;
			case 'V':	// set variable
				if(pTokStr->GetToken(token) != STRTYPE_NAME) goto FUNCTION_DESC_ERROR;
				if(!strcmp(token, "PC")) {	// select PC
					m_pCurVar	= m_rsc->m_pCurPC;
					break;
				}
				m_pCurVar = m_rsc->m_pNodeVariable->Find(token);
				if(!m_pCurVar) {
UNDEFINDABLE_VARIABLE:
					CMSG(_L("Can't find variable on function[%s] : %s", func_name, token), LOG_ERROR);
					return FALSE;
				}
				break;
			case '$':	// add new variable & set current
				if(func.TokenOutable("i")){
					func.TokenOut("i");
					type = STRTYPE_INT;
				}else
				if(func.TokenOutable("f")){
					func.TokenOut("f");
					type = STRTYPE_FLOAT;
				}else
				if(func.TokenOutable("s")){
					func.TokenOut("s");
					type = STRTYPE_STRING;
				}else
				if(func.TokenOutable("n")){
					func.TokenOut("n");
					type = STRTYPE_NAME;
				}else type = 0;

				if((pTokStr->GetToken(token))!= STRTYPE_NAME) goto FUNCTION_DESC_ERROR;
				if(m_rsc->m_pNodeVariable->Find(token)) {
					CMSG(_L("The variable is already exist. : %s", token), LOG_ERROR);
					return FALSE;
				}
				m_pCurVar = m_rsc->m_pNodeVariable->Add(token);
				m_pCurVar->m_Type = type;
				//m_pCurVar->m_bSet = FALSE;
				break;
			case '=':	// set variable
			case '+':	// increase variable
			case '-':	// decrease variable
			case '*':	// multiply
			case '/':	// divide
			case '>':	// shift right
			case '<':	// shift left
			case '&':	// and
			case '|':	// or
			case '^':	// xor
			case '~':	// not
				//----------------------------------------------------------------------
				op = *token;
				if(op == '='){
					if(func.TokenOutable("$")){
						func.TokenOut("$");
						op = '$';
					}
				}
				if(!m_pCurVar) {
UNDEFINED_VARIABLE:
					CMSG(_L("Variable is not defined on function [%s] : %s", func_name, m_pCurVar->m_pName), LOG_ERROR);
					return FALSE;
				}
				// 변수가 초기화되지 않으면 연산할 수 없다.
				if(!m_pCurVar->GetType() && op!='='){
UNINTIALIZED_VARIABLE:
					CMSG(_L("Variable is not intitialized on function [%s] : %s", func_name, m_pCurVar->m_pName), LOG_ERROR);
					return FALSE;
				}
				type = pTokStr->GetToken(token);
				if((m_pCurVar->m_Type == STRTYPE_NAME) && op == '='){
					if(type!=STRTYPE_NAME) goto NOT_ACCEPTABLE_VARIABLE_ERROR;
					m_pCurVar->SetString(token);
				}else
				if((m_pCurVar->m_Type == STRTYPE_STRING) && op == '='){
					if(type!=STRTYPE_STRING) goto NOT_ACCEPTABLE_VARIABLE_ERROR;
					m_pCurVar->SetString(token);
				}else
				if(type & (STRTYPE_NAME|STRTYPE_INT|STRTYPE_HEX|STRTYPE_FLOAT|STRTYPE_BINARY)){
					DWORD idata;
					if(type==STRTYPE_NAME){
						// increase with variable
						CNodeData* pV = m_rsc->m_pNodeVariable->Find(token);
						if(!pV) goto UNDEFINDABLE_VARIABLE;
						if(!pV->GetType()) goto UNINTIALIZED_VARIABLE;
						idata	= pV->m_iData;
						type	= pV->m_Type;
						if(!(type & (STRTYPE_INT|STRTYPE_HEX|STRTYPE_FLOAT|STRTYPE_BINARY))){
NOT_ACCEPTABLE_VARIABLE_ERROR:
							CMSG(_L("Following variable can't not acceptable. : %s", token), LOG_ERROR);
							goto SYNTAX_ERROR;
						}
					}else{	// increase with constant value
						idata = pTokStr->RetrieveVariable();
						//if(type==STRTYPE_FLOAT) CMSG(_L("TOK = %f", *(float*)&idata), LOG_INFORMATION);
					}
					if((type != STRTYPE_INT) && (op=='>' || op=='<')){
INTEGER_VARIABLE_ERROR:
						CMSG(_L("This operation is needed a integer variable!"), LOG_ERROR);
						goto SYNTAX_ERROR;
					}
					{
						DWORD cur_type = m_pCurVar->m_Type;
						switch(op){
						case '$':	m_pCurVar->m_iData	= idata;
									m_pCurVar->m_Type	= type;
									//if(type == STRTYPE_FLOAT) CMSG(_L("VV : %f", *(float*)&idata), LOG_INFORMATION);
									break;
						case '=':	if(cur_type!=STRTYPE_FLOAT && type==STRTYPE_FLOAT)
										m_pCurVar->m_iData	= (int)*(float*)&idata;
									else
									if(cur_type==STRTYPE_FLOAT && type!=STRTYPE_FLOAT)
										*(float*)&m_pCurVar->m_iData	= (float)idata;
									else m_pCurVar->m_iData	= idata;

									if (cur_type == STRTYPE_FLOAT)
										DMSG(_L("%s = %d\n", m_pCurVar->GetName(), m_pCurVar->m_iData));
									else
										DMSG(_L("%s = %d\n", m_pCurVar->GetName(), m_pCurVar->m_iData));
									break;
						case '+':	if(cur_type==STRTYPE_FLOAT){
										if(type==STRTYPE_FLOAT) *(float*)&m_pCurVar->m_iData += *(float*)&idata;
										else *(float*)&m_pCurVar->m_iData += *(int*)&idata;
									}else{
										if(type==STRTYPE_FLOAT) *(int*)&m_pCurVar->m_iData += (int)*(float*)&idata;
										else *(int*)&m_pCurVar->m_iData += *(int*)&idata;
									}
									break;
						case '-':	if(cur_type==STRTYPE_FLOAT){
										if(type==STRTYPE_FLOAT) *(float*)&m_pCurVar->m_iData -= *(float*)&idata;
										else *(float*)&m_pCurVar->m_iData -= *(int*)&idata;
									}else{
										if(type==STRTYPE_FLOAT) *(int*)&m_pCurVar->m_iData -= (int)*(float*)&idata;
										else *(int*)&m_pCurVar->m_iData -= *(int*)&idata;
									}
									break;
						case '*':	if(cur_type==STRTYPE_FLOAT){
										if(type==STRTYPE_FLOAT) *(float*)&m_pCurVar->m_iData *= *(float*)&idata;
										else *(float*)&m_pCurVar->m_iData *= *(int*)&idata;
									}else{
										if(type==STRTYPE_FLOAT) *(int*)&m_pCurVar->m_iData *= (int)*(float*)&idata;
										else *(int*)&m_pCurVar->m_iData *= *(int*)&idata;
									}
									break;
						case '/':	if(!idata){
										CMSG(_L("Division by zero is occurred!"), LOG_ERROR);
										goto SYNTAX_ERROR;
									}
									if(cur_type==STRTYPE_FLOAT){
										if(type==STRTYPE_FLOAT) *(float*)&m_pCurVar->m_iData /= *(float*)&idata;
										else *(float*)&m_pCurVar->m_iData /= *(int*)&idata;
									}else{
										if(type==STRTYPE_FLOAT) *(int*)&m_pCurVar->m_iData /= (int)*(float*)&idata;
										else *(int*)&m_pCurVar->m_iData /= *(int*)&idata;
									}
									break;
						case '>': m_pCurVar->m_iData >>= idata; m_pCurVar->m_Type	= STRTYPE_INT; break;
						case '<': m_pCurVar->m_iData <<= idata;	m_pCurVar->m_Type	= STRTYPE_INT; break;
						case '&': m_pCurVar->m_iData &= idata;	m_pCurVar->m_Type	= STRTYPE_INT; break;
						case '|': m_pCurVar->m_iData |= idata;	m_pCurVar->m_Type	= STRTYPE_INT; break;
						case '^': m_pCurVar->m_iData ^= idata;	m_pCurVar->m_Type	= STRTYPE_INT; break;
						case '~': m_pCurVar->m_iData = ~idata;	m_pCurVar->m_Type	= STRTYPE_INT; break;
						}
					}
				}else{
					CMSG(_L("Unexceptable variable on function [%s] : %s", func_name, m_pCurVar->m_pName), LOG_ERROR);
					return FALSE;
				}

				m_pCurVar->Set();
				break;
			case '?':	// selective function with variable
				if(!m_pCurVar) goto UNDEFINED_VARIABLE;
				if(!m_pCurVar->GetType()) goto UNINTIALIZED_VARIABLE;
				type = pTokStr->GetToken(token);	// get variable

				if((type == STRTYPE_SPECIAL) && bInCourt){
					switch(token[0]){
					case '#':
						if(m_pCurVar->GetType()&(STRTYPE_INT|STRTYPE_HEX|STRTYPE_BINARY)) goto FUNCTION_RUN_SELECTION_TRUE;
						goto FUNCTION_RUN_SELECTION_FALSE;
						break;
					case '%':
						if(m_pCurVar->GetType()&STRTYPE_FLOAT) goto FUNCTION_RUN_SELECTION_TRUE;
						goto FUNCTION_RUN_SELECTION_FALSE;
						break;
					default: goto FUNCTION_DESC_ERROR;
					}
				}
				{
					int iData;
					if(type & (STRTYPE_NAME|STRTYPE_STRING)){
						// 현재 비교 변수가 문자열이거나 이름일 경우
						if(m_pCurVar->GetType() == type){
							// 동일한 타입일 경우의 비교
							if(!strcmp(m_pCurVar->m_sData, token)) goto FUNCTION_RUN_SELECTION_TRUE;
							else goto FUNCTION_RUN_SELECTION_FALSE;
						}else{
							CNodeData* pV = m_rsc->m_pNodeVariable->Find(token);
							if(!pV) goto FUNCTION_DESC_ERROR;
							type	= pV->GetType();
							iData	= pV->GetData();
						}
					}else {
						// 상수 타입일 경우
						iData = pTokStr->RetrieveVariable();
					}
					if(type & (STRTYPE_FLOAT|STRTYPE_INT|STRTYPE_HEX|STRTYPE_BINARY)){
						if(!(m_pCurVar->GetType() & (STRTYPE_FLOAT|STRTYPE_INT|STRTYPE_HEX|STRTYPE_BINARY))) goto FUNCTION_DESC_ERROR;
						if(m_pCurVar->GetData()==iData) goto FUNCTION_RUN_SELECTION_TRUE;
						goto FUNCTION_RUN_SELECTION_FALSE;
					}else goto FUNCTION_DESC_ERROR;
				}
				break;
			case 'C':	// check string in string
			case 'W':	// check string width
				if(!m_pCurVar) goto UNDEFINED_VARIABLE;
				if(!(m_pCurVar->GetType()&(STRTYPE_NAME|STRTYPE_STRING))) goto NOT_ACCEPTABLE_VARIABLE_ERROR;
				if(!m_pCurVar->IsSet()) goto UNINTIALIZED_VARIABLE;
				switch(*token){
				case 'C':
					if(func.GetToken(token)!=STRTYPE_STRING) goto FUNCTION_DESC_ERROR;
					if(strstr(m_pCurVar->m_sData, token)) goto FUNCTION_RUN_SELECTION_TRUE;
					else goto FUNCTION_RUN_SELECTION_FALSE;
				case 'W':
					if(func.GetToken(token)!=STRTYPE_INT) goto FUNCTION_DESC_ERROR;
					if(strlen(m_pCurVar->m_sData)==func.RetrieveVariable()) goto FUNCTION_RUN_SELECTION_TRUE;
					else goto FUNCTION_RUN_SELECTION_FALSE;
				}
				break;
			case '!':	// Selective function with syntex
				if(func.TokenOutable("!")){
					func.TokenOut("!");
					type = 0;
				}else type = 1;

				if(func.GetToken(token)!=STRTYPE_STRING) goto FUNCTION_DESC_ERROR;
				{
					if(m_pStr->TokenOutable(token)){
						if(type) m_pStr->TokenOut(token);
						goto FUNCTION_RUN_SELECTION_TRUE;
					} else goto FUNCTION_RUN_SELECTION_FALSE;
				}

				{
					BOOL bTrue;
FUNCTION_RUN_SELECTION_FALSE:
					bTrue = FALSE;
					goto FUNCTION_RUN_SELECTION_RUN;
FUNCTION_RUN_SELECTION_TRUE:
					bTrue = TRUE;
FUNCTION_RUN_SELECTION_RUN:
					type = func.GetToken(token);

					if(bTrue){
						if(type == STRTYPE_NAME){
							if(!Function(token)) return FALSE;
						}else
						if(*token == '*') return TRUE;
						else
						if(*token == '@'){
							if(func.GetToken(token) != STRTYPE_NAME) goto FUNCTION_DESC_ERROR;
							if(!Function(token)) return FALSE;
							return TRUE;
						}else
						if(*token != '#') goto FUNCTION_DESC_ERROR;
					}else{
						if(*token == '@') func.GetToken(token);
					}
					if(func.TokenOutable(":")){
						func.TokenOut(":");
						
						type = func.GetToken(token);
						
						if(!bTrue){
							if(type == STRTYPE_NAME){
								if(!Function(token)) return FALSE;
							}else
							if(*token == '*') return TRUE;
							else
							if(*token == '@'){
								if(func.GetToken(token) != STRTYPE_NAME) goto FUNCTION_DESC_ERROR;
								if(!Function(token)) return FALSE;
								return TRUE;
							}else
							if(*token != '#') goto FUNCTION_DESC_ERROR;
						}else{
							if(*token == '@') func.GetToken(token);
						}
					}
				}
				break;
			case 'A':	// Set address tag
				{
					CNodeData* pNode = m_rsc->NewVariable(g_CUR_ADDRESS_TAG_NAME);	// g_CUR_ADDRESS_TAG_NAME = "*pc"
					if(pNode){
						pNode->SetType(STRTYPE_ADDRESSTAG);
						pNode->SetData(m_rsc->m_pCurPC->GetData());
						m_rsc->m_pCurPC = pNode;
					}
				}
				break;
			case 'L':	// line feed
				if(!m_pStr->IsTokenable()){
					GetLine();
					m_pStr->Set(Source->GetLatestLine());
				}
				break;
			case 'T':	// Set address tag name
				op = func.IsTokenable(TRUE);
				type = pTokStr->GetToken(token);

				if((type == STRTYPE_SPECIAL) && bInCourt && op){
					CNodeData* pLink = NULL;
					CNodeData* pNode = NULL;
					switch(*token){
					case '{':
						if(func.TokenOutable("{")){
							func.TokenOut("{");
							type = 1;
						}else type = 0;
						sprintf(token, "@{%d", m_dwDelimCount);
						pNode = m_rsc->NewVariable(token);
						pNode->SetType(STRTYPE_ADDRESSTAG);
						pNode->SetData(m_rsc->m_pCurPC->GetData());

						sprintf(token, "{%s%d", type ? "{" : "", m_dwFuncDepth);
						pLink = m_rsc->NewVariable(token);
						pLink->SetType(STRTYPE_LINK);
						pLink->m_pLink = pNode;

						//CMSG(_L("PROC : %s : %d", token, m_dwFuncDepth+1), LOG_INFORMATION);
						sprintf(token, "@}%d", m_dwDelimCount);
						pNode = m_rsc->NewVariable(token);
						pNode->SetType(STRTYPE_ADDRESSTAG);

						sprintf(token, "}%s%d", type ? "}" : "", m_dwFuncDepth);
						pLink = m_rsc->NewVariable(token);
						pLink->SetType(STRTYPE_LINK);
						pLink->m_pLink = pNode;

						m_dwFuncDepth++;
						m_dwDelimCount++;
						break;
					case '}':
						if(m_dwFuncDepth) m_dwFuncDepth--;
						else {
							CMSG(_L("Function Delimiters are not a pair!"), LOG_ERROR);
							return FALSE;
						}
						// find '}'
						sprintf(token, "}%d", m_dwFuncDepth);
						pLink = m_rsc->FindVariable(token);
						if(!pLink){
							sprintf(token, "}}%d", m_dwFuncDepth);
							pLink = m_rsc->FindVariable(token);
						}
						pNode = pLink->m_pLink;
						pLink->ReleaseData();
						if(pNode) pNode->SetData(m_rsc->m_pCurPC->GetData());
	
						// find '{'
						sprintf(token, "{%d", m_dwFuncDepth);
						pLink = m_rsc->FindVariable(token);
						if(!pLink){
							sprintf(token, "{{%d", m_dwFuncDepth);
							pLink = m_rsc->FindVariable(token);
						}
						pLink->ReleaseData();

						if(!m_dwFuncDepth) m_dwFuncCount++;
						break;
					case '#':	// function depth
					case '$':	// function count
					case '%':	// current procedure is loop ?
						if(!m_pCurVar) goto UNDEFINDABLE_VARIABLE;
						if(m_pCurVar->GetType() != STRTYPE_INT) goto INTEGER_VARIABLE_ERROR;
						if(*token=='%'){
							if(!m_dwFuncDepth) {
								m_pCurVar->SetData(0);
								break;
							}
							sprintf(token, "}}%d", m_dwFuncDepth-1);
							pLink = m_rsc->FindVariable(token);
							m_pCurVar->SetData(pLink!=NULL);
						}else m_pCurVar->SetData(*token=='#' ? m_dwFuncDepth : m_dwFuncCount);
						break;
					default: goto FUNCTION_DESC_ERROR;
					}
				}else
				if(type == STRTYPE_NAME){
					CNodeData* pNode;
					pNode	= m_rsc->FindVariable(token);
					if(pNode->GetType() == STRTYPE_NAME){
						if(!pNode->m_sData) {
							CMSG(_L("Can't find function name [%s] : %s", func_name, pNode->GetName()), LOG_ERROR);
							return FALSE;
						}
						strcpy(token, pNode->m_sData);
					}
					// address tag
					if(m_dwFuncDepth){
						char temp[MAX_LINE];
						strcpy(temp, token);
						sprintf(token, "@%d:%s", m_dwFuncCount, temp);
					}
					pNode	= m_rsc->NewVariable(token);
					type	= pNode->GetType();
					if(!type) pNode->SetType(STRTYPE_ADDRESSTAG);
					else
					if(type!=STRTYPE_ADDRESSTAG){
						CMSG(_L("Conflict other variable [%s] : %s", func_name, token), LOG_ERROR);
						return FALSE;
					}
					if(pNode->IsSet()){
						CMSG(_L("Already the address tag is intialized [%s] : %s", func_name, token), LOG_ERROR);
						return FALSE;
					}
					pNode->SetData(m_rsc->m_pCurPC->GetData());
				}else goto FUNCTION_DESC_ERROR;
				break;
			case 'D':	// Set direct addressing field
				op = 0;
				goto ADRESSING_TAG_ADD;
			case 'R':	// Set indirect addressing field
				op = 1;
ADRESSING_TAG_ADD:
				{
					BOOL bGlobal	= FALSE;
					// 어드레스 연결 노드 추가
					CNodeRelation*	pNode = m_rsc->m_pNodeRelation->Add();
					// 대상 코드를 현재 코드로 추가
					pNode->m_pCode = m_rsc->m_pCurCode;

					// indirect addressing mode
					if(op){
						pNode->m_pSrcPos		= m_rsc->m_pCurPC;
						m_rsc->m_pCurPC->SetName("*base");
					}
					
					type = func.GetToken(token);
					if((type == STRTYPE_SPECIAL)){
						if(*token=='@') bGlobal	= TRUE;
						else
						if(bInCourt){
							DWORD dwFuncDepth;
							char temp[MAX_LINE];

							if(!m_dwFuncDepth){
//FUNCTION_DEPTH_LEVEL_FAULT:
								CMSG(_L("Current function depth level is zero!"), LOG_ERROR);
								return FALSE;
							}
							dwFuncDepth = m_dwFuncDepth - 1;
							switch(*token){
							case '(':	// find loop start
							case ')':	// find loop end
								if(*token=='(') *token = '{';
								else *token = '}';
								while(dwFuncDepth){
									sprintf(temp, "%s%s%d", token, token, dwFuncDepth);
									pNode->m_pDestPos	= m_rsc->FindVariable(temp);
									if(pNode->m_pDestPos) break;
									dwFuncDepth--;
								}
								if(!pNode->m_pDestPos){
									CMSG(_L("No loop procedure is found!"), LOG_ERROR);
									return FALSE;
								}
								pNode->m_pDestPos = pNode->m_pDestPos->m_pLink;
								break;
							case '[':	// function start
							case ']':	// function end
								if(*token=='[') *token = '{';
								else *token = '}';
								dwFuncDepth = 0;
							case '{':	// procedure start
							case '}':	// procedure end
								sprintf(temp, "%s%d", token, dwFuncDepth);
								pNode->m_pDestPos	= m_rsc->FindVariable(temp);
								if(!pNode->m_pDestPos){
									sprintf(temp, "%s%s%d", token, token, dwFuncDepth);
									pNode->m_pDestPos	= m_rsc->FindVariable(temp);
								}

								if(!pNode->m_pDestPos){
									goto INVALID_ADDRESS_TAG;
								}
								pNode->m_pDestPos = pNode->m_pDestPos->m_pLink;
								break;
							case '@':	// use global address
								bGlobal	= TRUE;
								break;
							default:goto FUNCTION_DESC_ERROR;
							}
						}else goto FUNCTION_DESC_ERROR;

						type = func.GetToken(token);
					}

					// bit width
					if(type != STRTYPE_INT) goto FUNCTION_DESC_ERROR;						// bit width
					pNode->m_iBitwidth	= func.RetrieveVariable();
					// bit position
					if(func.GetToken(token) != STRTYPE_INT) goto FUNCTION_DESC_ERROR;		// binary position
					pNode->m_iPosition	= func.RetrieveVariable();

					if(pNode->m_pDestPos) break;

					{
						if(pTokStr->GetToken(token) != STRTYPE_NAME) goto FUNCTION_DESC_ERROR;	// destination address tag name
						if(!bGlobal && m_dwFuncDepth){
							char temp[MAX_LINE];
							strcpy(temp, token);
							sprintf(token, "@%d:%s", m_dwFuncCount, temp);
						}
						pNode->m_pDestPos	= m_rsc->NewVariable(token);
						if(!pNode->m_pDestPos->GetType()) pNode->m_pDestPos->SetType(STRTYPE_ADDRESSTAG);

						if(pNode->m_pDestPos->m_Type!=STRTYPE_ADDRESSTAG){
INVALID_ADDRESS_TAG:
							CMSG(_L("Invalid address tag : %s", token), LOG_ERROR);
							return FALSE;
						}
					}
				}
				break;
			case 'S':	// Set instruction field
				{
					int bitwidth, position;
					DWORD data;
					if(func.GetToken(token) != STRTYPE_INT) goto FUNCTION_DESC_ERROR;	// bit width
					bitwidth = func.RetrieveVariable();
					if(func.GetToken(token) != STRTYPE_INT) goto FUNCTION_DESC_ERROR;	// binary position
					position = func.RetrieveVariable();
					type = pTokStr->GetToken(token);	// function name
					if(type == STRTYPE_NAME){
						CNodeData* pV = m_rsc->m_pNodeVariable->Find(token);
						if(!pV) goto UNDEFINED_VARIABLE;
						if(!pV->GetType()) goto UNINTIALIZED_VARIABLE;
						data = pV->GetData();
						//if(pV->GetType()==STRTYPE_FLOAT) CMSG(_L("@S : %X", data), LOG_INFORMATION);
					}else
					if(type & (STRTYPE_INT|STRTYPE_HEX|STRTYPE_BINARY)){
						data = pTokStr->RetrieveVariable();
					}else{
						CMSG(_L("It's not acceptable Varialbe on function [%s] : %s", func_name, token), LOG_ERROR);
						return FALSE;
					}
					m_rsc->m_pCurCode->Set(data, bitwidth, position);
				}
				break;
			case '@':
				{
					DWORD func_num = 0;
					if(func.GetToken(token) != STRTYPE_INT) goto FUNCTION_DESC_ERROR;	// function #
					func_num	= func.RetrieveVariable();
					switch(func_num){
					case 0:	// 소스 마스크를 쓰기 마스크에 맞게 조정한다.
						if(pTokStr->GetToken(token)!=STRTYPE_NAME) goto FUNCTION_DESC_ERROR;
						if(!CustomSMASK(m_pCurVar, m_rsc->m_pNodeVariable->Find(token))) goto FUNCTION_DESC_ERROR;
						break;
					default:
						CMSG(_L("Unsupported function number : %d", func_num), LOG_ERROR);
						goto FUNCTION_DESC_ERROR;
					}
				}
				break;
			default: goto FUNCTION_DESC_ERROR;
			}
			break;
		default:
			goto TOKEN_CHECK;
		}
		break;
	default:
TOKEN_CHECK:
		if(!m_pStr->TokenOut(token)){
SYNTAX_ERROR:
			CMSG(_L("Syntex error at function[%s] : %s(token \"%s\")", func_name, Source->GetLatestLine(), token), LOG_ERROR);
			return FALSE;
		}
		break;
	}
	
	return TRUE;
}

BOOL CGPCompile::GetLine(void){
	if(!Source->GetLine()) return FALSE;
	DWORD		dwCount					= 0;
	char		*pName, *pTok, *pLine	= Source->GetLatestLine();
	CNodeStr	*pNode					= m_rsc->m_pNodeDefine;
	char		cData, line[MAX_LINE];
	DWORD		slen;

	while(pNode){
		pName	= pNode->GetName();
		if(!pName) goto NEXT_DEFINE;
		slen	= (DWORD)strlen(pName);
		pTok	= strstr(pLine, pName);
		if(pTok){
			if(pTok > pLine){
				cData = *(pTok-1);
				if((cData >= 'a' && cData <= 'z') ||
				   (cData >= 'A' && cData <= 'Z') ||
				   (cData >= '0' && cData <= '9') ||
				   cData == '_')
				   goto NEXT_DEFINE;
			}
			{
				cData = *(pTok + slen);
				if((cData >= 'a' && cData <= 'z') ||
				   (cData >= 'A' && cData <= 'Z') ||
				   (cData >= '0' && cData <= '9') ||
				   cData == '_')
					goto NEXT_DEFINE;
			}
			*pTok = NULL;
			pTok = pTok + slen;
			sprintf(line, "%s%s%s", pLine, pNode->GetData(), pTok);
			strcpy(pLine, line);
			pNode = m_rsc->m_pNodeDefine;
			dwCount++;
			if(dwCount>100){
				CMSG(_L("Too many defined strings are found, please take more long define name or rename others : %s", pName), LOG_ERROR);
				return FALSE;
			}
			continue;
		}
NEXT_DEFINE:
		pNode = pNode->GetNext();
	}

	return TRUE;
}

BOOL CGPCompile::Link(void)
{
	CNodeRelation* pNode = m_rsc->m_pNodeRelation;
	CNodeCode*		pCode;
	DWORD	pc;

	while(pNode){
		// branch target 정하기
		pCode = pNode->m_pCode;
		if(!pCode || !pNode->m_pDestPos){pNode = pNode->Next(); continue;}
		if(!pNode->m_pDestPos->IsSet()){
			CMSG(_L("Destination address tag is not exist : %s", pNode->m_pDestPos->m_pName), LOG_ERROR);
			return FALSE;
		}
		pc = pNode->m_pDestPos->m_iData;
		if(pNode->m_pSrcPos){
			if(!pNode->m_pSrcPos->IsSet()){
				CMSG(_L("Source address tag is not exist : %s", pNode->m_pDestPos->m_pName), LOG_ERROR);
				return FALSE;
			}
			pc -= pNode->m_pSrcPos->m_iData;
		}

		pNode->m_pCode->Set(pc, pNode->m_iBitwidth, pNode->m_iPosition);
		DMSG(_L("pc (%d) m_iBitwidth(%d), m_iPosition(%d)", pc, pNode->m_iBitwidth, pNode->m_iPosition));
		pNode = pNode->Next();
	}

	return TRUE;
}