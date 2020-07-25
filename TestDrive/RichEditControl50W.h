#pragma once

// CRichEditControl50W
class CRichEditControl50W : public CWnd
{
	DECLARE_DYNAMIC(CRichEditControl50W)

protected:
	DECLARE_MESSAGE_MAP()
	CHARRANGE m_crRE50W;
	CHARFORMAT2 m_cfRE50W;
	SETTEXTEX m_st50W;

// Constructors
public:
	CRichEditControl50W();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	//virtual BOOL CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect,
	//	CWnd* pParentWnd, UINT nID);

// Attributes
	TEXTRANGEW m_trRE50W;	//TextRangeW structure, for Unicode
	LPSTR m_lpszChar;

	inline BOOL SetAutoURLDetect(BOOL bEnable = TRUE)		{	return (BOOL) SendMessage(EM_AUTOURLDETECT, (WPARAM) bEnable, 0);}


	inline void SetSel(long nStartChar, long nEndChar)		{	CHARRANGE cr={nStartChar, nEndChar};SetSel(cr);}
	inline void SetSel(CHARRANGE &cr)						{	SendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);}
	inline BOOL SetDefaultCharFormat(CHARFORMAT &cf)		{	return (BOOL) SendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf);}
	inline BOOL SetDefaultCharFormat(CHARFORMAT2 &cf)		{	return (BOOL) SendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf);}

	void SetTextTo50WControl(CString csText, int nSTFlags, int nSTCodepage);
	void LimitText50W(int nChars);
	void SetOptions50W(WORD wOp, DWORD dwFlags);
	DWORD SetEventMask50W(DWORD dwEventMask);
	void GetTextRange50W(int ncharrMin, int ncharrMax);

	virtual ~CRichEditControl50W();
protected:
private:
	static HINSTANCE	m_hInstRichEdit50W;      // handle to MSFTEDIT.DLL
	static DWORD		m_dwRefcount;
};
