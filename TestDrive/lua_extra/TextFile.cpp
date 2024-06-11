//================================================================================
// Copyright (c) 2013 ~ 2021. HyungKi Jeong(clonextop@gmail.com)
// Freely available under the terms of the 3-Clause BSD License
// (https://opensource.org/licenses/BSD-3-Clause)
// 
// Redistribution and use in source and binary forms,
// with or without modification, are permitted provided
// that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
// OF SUCH DAMAGE.
// 
// Title : utility framework
// Rev.  : 7/30/2021 Fri (clonextop@gmail.com)
//================================================================================
#include "StdAfx.h"
#include "TextFile.h"
#include <stdarg.h>
#include <vector>

TextFile::TextFile(void)
{
	m_fp				= NULL;
	m_iLineNumber		= 0;
	m_bWrite			= false;
	m_bComment			= false;
}

TextFile::~TextFile(void)
{
	Close();
}

bool TextFile::Open(const char* sFileName)
{
	Close();
	m_sFileName		= sFileName;
	m_fp			= fopen(sFileName, "rt");
	m_iLineNumber	= 0;
	m_bWrite		= false;
	return (m_fp != NULL);
}

bool TextFile::Create(const char* sFileName)
{
	Close();
	m_sFileName		= sFileName;
	m_fp			= fopen(sFileName, "wt");
	m_iLineNumber	= 0;
	m_bWrite		= true;
	return (m_fp != NULL);
}

void TextFile::Close(void)
{
	if(m_fp) {
		fclose(m_fp);
		m_fp			= NULL;
		m_iLineNumber	= 0;
		m_bWrite		= false;
	}
}

__int64 TextFile::Offset(void)
{
	return m_fp ? _ftelli64(m_fp) : 0;
}

void TextFile::SetOffset(__int64 offset, int base)
{
	if(m_fp)
		_fseeki64(m_fp, offset, base);
}

void TextFile::Puts(const char* sStr)
{
	if(!m_fp || !m_bWrite || !sStr) return;

	fputs(sStr, m_fp);
}

const char* TextFile::Gets(bool bUseComment)
{
	static cstring sLine;
	bool bRet	= GetLine(sLine, bUseComment);

	if(bRet && bUseComment) {
		sLine.Trim("\r\n");
	}

	return bRet ? sLine.c_str() : NULL;
}

void TextFile::Write(const char* sFormat, ...)
{
	if(!m_fp || !m_bWrite || !sFormat) return;

	{
		int iLen		= 0;
		va_list vaArgs;
		va_start(vaArgs, sFormat);
		{
			// get size only
			va_list vaCopy;
			va_copy(vaCopy, vaArgs);
			iLen = std::vsnprintf(NULL, 0, sFormat, vaCopy);
			va_end(vaCopy);
		}
		{
			char* pBuff = new char[iLen + 1];
			std::vector<char> zc(iLen + 1);
			std::vsnprintf(pBuff, iLen + 1, sFormat, vaArgs);
			va_end(vaArgs);
			{
				// write to string
				fputs(pBuff, m_fp);
			}
			delete [] pBuff;
		}
	}
}

bool TextFile::GetLine(cstring& sLine, bool bUseComment)
{
	static char		sTemp[1024];
	char*	sTok		= NULL;
	sLine.clear();

	if(!m_fp || m_bWrite) return false;

RE_GET:

	if(!fgets(sTemp, 1024, m_fp)) return (sLine.size() != 0);

	// get full line
	sLine	+= sTemp;
	{
		int iLen = strlen(sTemp);

		if(iLen > 0 && sTemp[iLen - 1] != '\n' && !feof(m_fp)) goto RE_GET;
	}
	m_iLineNumber++;
RE_CHECK_COMMENT:

	if(bUseComment) {
		if(m_bComment) {
			int iPos = sLine.find("*/");

			if(iPos >= 0) {
				m_bComment	= false;
				sLine.erase(0, iPos + 2);
				goto RE_CHECK_COMMENT;
			} else sLine.clear();
		} else {
			sLine.CutBack("//", true);
			int iPos = sLine.find("/*");

			if(iPos >= 0) {
				int iEnd = sLine.find("*/", iPos + 2);

				if(iEnd >= 0) {
					sLine.erase(iPos, iEnd - iPos + 2);
					goto RE_CHECK_COMMENT;
				} else {
					sLine.erase(iPos, -1);
					m_bComment	= true;
				}
			}
		}
	}

	if(!sLine.size()) goto RE_GET;

	return true;
}

void TextFile::GetAll(cstring& sContents, bool bUseComment)
{
	cstring sLine;
	sContents.clear();

	while(GetLine(sLine))
		sContents	+= sLine;
}
