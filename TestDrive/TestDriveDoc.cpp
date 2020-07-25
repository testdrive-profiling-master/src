#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Main.h"
#endif

#include "TestDriveDoc.h"
#include "DocumentCtrl.h"
#include <propkey.h>
#include "TestDriveImp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTestDriveDoc

IMPLEMENT_DYNCREATE(CTestDriveDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestDriveDoc, CDocument)
	//ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
END_MESSAGE_MAP()


CTestDriveDoc::CTestDriveDoc()
{
}

CTestDriveDoc::~CTestDriveDoc()
{
}

BOOL CTestDriveDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	SetModifiedFlag(FALSE);
	UpdateTite();

	return TRUE;
}

BOOL CTestDriveDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!g_pTestDrive->SetProjectFile(lpszPathName)) return FALSE;

	// .profile 을 실행하여 프로그램이 시작된 경우 바로 프로젝트를 열만 안되기 때문에, 메시지에 올려 나중에 열리도록 한다.
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_TESTDRIVE_POST_PROJECT_OPEN);

	return CDocument::OnOpenDocument(lpszPathName);
}

void CTestDriveDoc::OnCloseDocument(){
	if(!g_pTestDrive->CloseProject()) return;

	m_strPathName = _T("");
	SetModifiedFlag(FALSE);
	CDocument::OnCloseDocument();
}

CString& CTestDriveDoc::UpdateTite(void){
	CString* pTitle = &(g_pTestDrive->UpdateTite());
	CDocument::SetTitle(*pTitle);
	return *pTitle;
}

void CTestDriveDoc::ReleaseAll(){
	//CDocument::OnFileClose();
}

void CTestDriveDoc::SetTitle(LPCTSTR lpszTitle){
	// 자동 타이틀 바뀜 허가하지 않음
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CTestDriveDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CTestDriveDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CTestDriveDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CTestDriveDoc 진단

#ifdef _DEBUG
void CTestDriveDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTestDriveDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTestDriveDoc 명령
