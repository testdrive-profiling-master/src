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
// Rev.  : 11/15/2021 Mon (clonextop@gmail.com)
//================================================================================
#ifndef __TEXT_FILE_H__
#define __TEXT_FILE_H__
#include "cstring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

class TextFile {
public:
	TextFile(void);
	virtual ~TextFile(void);

	bool Open(const char* sFileName);
	bool Create(const char* sFileName);
	void Close(void);

	__int64 Offset(void);
	void SetOffset(__int64 offset, int base = SEEK_SET);
	void Puts(const char* sStr);
	const char* Gets(bool bUseComment = false);
	void Write(const char* sFormat, ...);
	bool GetLine(cstring& sLine, bool bUseComment = false);
	void GetAll(cstring& sContents, bool bUseComment = false);
	inline string& FileName(void)	{
		return m_sFileName;
	}
	inline int LineNumber(void) 	{
		return m_iLineNumber;
	}
	inline bool IsOpen(void)		{
		return m_fp != NULL;
	}
	inline bool IsEOF(void)			{
		return !IsOpen() || (feof(m_fp) != 0);
	}

protected:
	FILE*					m_fp;
	string					m_sFileName;
	bool					m_bWrite;
	bool					m_bComment;
	int						m_iLineNumber;
};

#endif// __TEXT_FILE_H__
