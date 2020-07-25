#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	SetModifiedFlag(FALSE);
	UpdateTite();

	return TRUE;
}

BOOL CTestDriveDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!g_pTestDrive->SetProjectFile(lpszPathName)) return FALSE;

	// .profile �� �����Ͽ� ���α׷��� ���۵� ��� �ٷ� ������Ʈ�� ���� �ȵǱ� ������, �޽����� �÷� ���߿� �������� �Ѵ�.
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
	// �ڵ� Ÿ��Ʋ �ٲ� �㰡���� ����
}

#ifdef SHARED_HANDLERS

// ����� �׸��� �����մϴ�.
void CTestDriveDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// ������ �����͸� �׸����� �� �ڵ带 �����Ͻʽÿ�.
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

// �˻� ó���⸦ �����մϴ�.
void CTestDriveDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ������ �����Ϳ��� �˻� �������� �����մϴ�.
	// ������ �κ��� ";"�� ���еǾ�� �մϴ�.

	// ��: strSearchContent = _T("point;rectangle;circle;ole object;");
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

// CTestDriveDoc ����

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


// CTestDriveDoc ���
