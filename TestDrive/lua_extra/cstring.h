//================================================================================
// Copyright (c) 2013 ~ 2024. HyungKi Jeong(clonextop@gmail.com)
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
// Rev.  : 5/27/2024 Mon (clonextop@gmail.com)
//================================================================================
#ifndef __CSTRING_H__
#define __CSTRING_H__
#include <string>

using namespace std;
#define UNUSE_CSTRING_ICONV

class cstring
{
public:
	cstring(void);
	cstring(const string &s);
	cstring(const char *s);
	cstring(const char *s, size_t size);
	virtual ~cstring(void);

	bool	 operator>=(const cstring &s);
	bool	 operator<=(const cstring &s);
	bool	 operator>(const cstring &s);
	bool	 operator<(const cstring &s);
	bool	 operator==(const char *s);
	bool	 operator!=(const char *s);
	cstring &operator=(const char *s);
	cstring &operator+=(const char *s);
	cstring &operator+=(char ch);
	cstring	 operator+(const char *s) const;
	char	&operator[](int iIndex);
	int		 Compare(const char *s);
	bool	 CompareFront(const char *s) const;
	bool	 CompareBack(const char *s) const;
	bool	 CutFront(const char *s, bool bRecursive = false);	 // delete ~s
	bool	 CutBack(const char *s, bool bRecursive = false);	 // delete s~
	bool	 DeleteFront(const char *s);						 // find first front 's' and delete
	bool	 DeleteBack(const char *s);							 // find last 's' and delete
	bool	 DeleteBlock(const char *sExpression, int iPos = 0); // ex) sExpression = "[*]"
	int FindVariableString(cstring &sVar, const char *sExpression = "$(*)", int iPos = 0); // ex : "$(" variable ")"
	int FindNameString(const char *sName, int iPos = 0); // ex) sName = 'box', 'abox'(X), 'a box'(O)
	operator const char *(void) const
	{
		return m_sStr.c_str();
	}
	void	MakeUpper(void);
	void	MakeLower(void);
	bool	Replace(const char *sSearch, const char *sReplace, bool bRecursive = false);
	bool	ReplaceVariable(const char *sSearch, const char *sReplace);
	void	TrimLeft(const char *sDelim);
	void	TrimRight(const char *sDelim);
	void	Trim(const char *sDelim);
	int		Length(void) const;
	cstring Tokenize(int &iPos, const char *sDelim = NULL);
	int		SearchBraket(int iPos = 0);
	int		RetrieveTag(const char **sTagList, int iTagSize = 0);
	void	Format(const char *sFormat, ...);
	void	AppendFormat(const char *sFormat, ...);
	void	Set(const char *sStr);
	void	Append(const char *sStr);
	void	Append(char ch);
	int		CheckFileExtension(const char **sExtList);
	bool	GetEnvironment(const char *sKey);
	void	SetEnvironment(const char *sKey);
#ifndef UNUSE_CSTRING_ICONV
	bool ChangeCharset(const char *szSrcCharset, const char *szDstCharset);
	bool ChangeCharsetToUTF8(void);
	bool ChangeCharsetToANSI(void);
#endif
	inline bool IsEmpty(void) const
	{
		return Length() == 0;
	}
	inline const char *c_str(void) const
	{
		return m_sStr.c_str();
	}
	inline string &c_string(void)
	{
		return m_sStr;
	}
	inline int find(char ch, int pos = 0)
	{
		return m_sStr.find(ch, pos);
	}
	inline int find(const char *s, int pos = 0)
	{
		return m_sStr.find(s, pos);
	}
	inline int rfind(const char *s)
	{
		return m_sStr.rfind(s);
	}
	inline int rfind(char ch)
	{
		return m_sStr.rfind(ch);
	}
	inline int size(void)
	{
		return m_sStr.size();
	}
	inline int length(void)
	{
		return m_sStr.length();
	}
	inline void clear(void)
	{
		m_sStr.clear();
	}
	inline void erase(int iPos, int iSize)
	{
		m_sStr.erase(iPos, iSize);
	}
	inline void insert(int iPos, const char *s)
	{
		m_sStr.insert(iPos, s);
	}
	inline void replace(int iPos, int iSize, const char *s)
	{
		m_sStr.replace(iPos, iSize, s);
	}

protected:
	string m_sStr;
};

#endif //__CSTRING_H__
