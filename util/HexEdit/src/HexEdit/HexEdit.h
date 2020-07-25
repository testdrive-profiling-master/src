//
//  HexEdit.h
//
//  www.catch22.net
//
//  Copyright (C) 2012 James Brown
//  Please refer to the file LICENCE.TXT for copying permission
//

#ifndef HEXEDIT_INCLUDED
#define HEXEDIT_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "..\HexView\HexView.h"


#define APPNAME TEXT("HexEdit  for TestDrive")
extern HINSTANCE	g_hInstance;
extern HWND			g_hwndMain;
extern HWND			g_hwndHexView;
extern HWND			g_hwndStatusBar;
extern HWND			g_hwndGoto;
extern HWND			g_hwndSearch;

extern BOOL			g_fStatusHexCursor	;
extern BOOL			g_fStatusHexSize	;
extern int			g_nStatusValueType  ;
extern BOOL			g_fFitToWindow ;
extern BOOL			g_bViewPhysicalAddress;

extern TCHAR		g_szFileTitle[MAX_PATH];
extern TCHAR		g_szAppName[];
extern TCHAR		g_szFileName[MAX_PATH];

#define WEBSITE_STR _T("www.catch22.net")
#define WEBSITE_URL _T("http://") WEBSITE_STR
#define SYSLINK_STR _T("Updates available at <A HREF=\"") WEBSITE_URL _T("\">") WEBSITE_STR _T("</A>")

//
//	Useful SetWindowPos constants (saves space!)
//
#define SWP_SIZEONLY  (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE)
#define SWP_MOVEONLY  (SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE)
#define SWP_ZONLY     (SWP_NOSIZE | SWP_NOMOVE   | SWP_NOACTIVATE)
#define SWP_SHOWONLY  (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW)
#define SWP_HIDEONLY  (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_HIDEWINDOW)

void SetStatusBarText(HWND hwndSB, int part, unsigned style, TCHAR *fmt, ...);
void UpdateStatusBarText(HWND hwndSB, HWND hwndHV);
void UpdateToolbarText(HWND hwndTB, HWND hwndHV);
void UpdateStatusbar(HWND hwndSB);

int HexPasteSpecialDlg(HWND hwnd);
BOOL CopyAsDlg(HWND hwnd);

HWND GetActiveHexView(HWND hwndMain);


//
//	Tool-window IDs
//	Each one must be UNIQUE!
//
#define DWID_TOOLBAR	1
#define DWID_SEARCHBAR	2
#define DWID_TYPEVIEW	3
#define DWID_HIGHLIGHT	4
#define DWID_ALLTYPES	5
#define DWID_STRINGS	6
#define DWID_CHECKSUMS	7
#define DWID_DIFF		8


//
//	Define some import/export formats
//	
typedef enum 
{
	FORMAT_RAWDATA	= 0,
	FORMAT_HEXDUMP	= 1,
	FORMAT_RAWHEX	= 2,
	FORMAT_HTML		= 3,
	FORMAT_CPP		= 4,
	FORMAT_ASM		= 5,
	FORMAT_INTELHEX	= 6,
	FORMAT_SRECORD	= 7,
	FORMAT_BASE64	= 8,
	FORMAT_UUENCODE = 9

} IMPEXP_FORMAT;

typedef enum
{
	SEARCHTYPE_HEX,
	SEARCHTYPE_ASCII,
	SEARCHTYPE_UTF8,
	SEARCHTYPE_UTF16,
	SEARCHTYPE_UTF32,
	SEARCHTYPE_BYTE,
	SEARCHTYPE_WORD,
	SEARCHTYPE_DWORD,
	SEARCHTYPE_QWORD,
	SEARCHTYPE_FLOAT,
	SEARCHTYPE_DOUBLE,

} SEARCHTYPE;

typedef struct
{
	IMPEXP_FORMAT	format;
	SEARCHTYPE		basetype;

	BOOL			fBigEndian;
	BOOL			fAppend;
	BOOL			fUseAddress;

	size_t			linelen;

} IMPEXP_OPTIONS;

typedef struct
{
	HWND		hwndMain;
	HWND		hwndToolbar;
	HWND		hwndStatusBar;
	HWND		hwndTabView;
	HWND		hwndSearchBar;

	BOOL		fChanged;

} MAINWND, *PMAINWND;


const TCHAR * SearchTypeStr(int nId);
void AddSearchTypes(HWND hwndCombo, int startType, int endType, int initial);

void HexView_CursorChanged(HWND hwndMain, HWND hwndHV);

#define REGBASE TEXT("Software\\Catch22\\HexEdit 2.0 beta5")
#define REGLOC TEXT("Software\\Catch22\\HexEdit 2.0 beta5\\DockLib")


typedef unsigned __int64 QWORD;


extern int  g_nStatusValueType;
extern BOOL	g_fStatusSignedValue;
extern BOOL	g_fStatusHexValue;
extern BOOL	g_fStatusBigEndian;
extern BOOL	g_fQuickLoad;

UINT DefaultFileMode();
BOOL LoadHighlights(HWND hwndHexView);
BOOL UpdateHighlights(BOOL fAlways);
BOOL UpdateHighlight(LPCTSTR pszFilePath, TCHAR * pszBookPath, BOOL fAlways);

TCHAR * GetArg(TCHAR *ptr, TCHAR *buf, int len);

HWND CreateSearchBar(HWND hwndParent);

HWND CreateHighlightView(HWND hwndParent);
BOOL SaveHighlights(HWND hwndHexView);

BOOL ShowChecksumDlg(HWND hwnd);
void ShowOptions(HWND hwndParent);
BOOL ShowGotoDialog(HWND hwndOwner);

void RepeatGoto(HWND hwndHexView);
int HighlightDlg(HWND hwndHV, HBOOKMARK hBookMark);

BOOL ShowExportDlg(HWND hwnd, LPTSTR pszFileName, LPTSTR pszTitleName);
BOOL ShowImportDlg(HWND hwnd, LPTSTR pszFileName, LPTSTR pszTitleName);

BOOL ShowModifyDlg(HWND hwnd);
BOOL ShowInsertDlg(HWND hwnd);
BOOL ShowReverseDlg(HWND hwnd);
BOOL ShowCompareDlg(HWND hwnd);
BOOL FileProperties(HWND hwndParent);

BOOL HexSetCurFileName(HWND hwndMain, LPCTSTR szFileName);
BOOL HexSetCurFileHwnd(HWND hwndMain, HWND);

BOOL IsToolbarButtonChecked(HWND hwndTB, UINT nCtrlId);
HWND CreatePinToolbar(HWND hwndDlg, UINT nCtrlId, BOOL fRightAligned);
void UpdatePin(HWND hwndDlg, UINT uId, BOOL fChecked);

HWND CreateEmptyToolbar(HWND hwndParent, int nBitmapIdx, int nBitmapWidth, int nCtrlId, DWORD dwExtraStyle);
void AddButton(HWND hwndTB, UINT uCmdId, UINT uImageIdx, UINT uStyle, TCHAR *szText);
int  ResizeToolbar(HWND hwndTB);
HWND HexIsOpen(HWND hwndMain, LPCTSTR szFileName, int *idx);

BOOL UpdateSearchData(HWND hwndSource, int searchType, BYTE *searchData, int *searchLen);
BOOL UpdateSearchDataDlg(HWND hwndDlg, int sourceId, BOOL fBigEndian, BYTE *searchData, int *searchLen);

BOOL ShowFindDialog(HWND hwndOwner, int idx);
BOOL ShowFindDialog(HWND hwndMain, int idx);
BOOL UpdateProgress(MAINWND *mainWnd, BOOL fVisible, size_w pos, size_w len);

BOOL FindNext();

size_t motorola_to_bin(char *srec, int *type, int *count, unsigned long *addr, BYTE *data);
size_t intel_to_bin(char *hrec, int *type, int *count, unsigned long *addr, BYTE *data);
void transform( BYTE * buf, size_t len, int operation, BYTE * operand, int basetype, int endian );

#ifdef __cplusplus
}
#endif
#endif