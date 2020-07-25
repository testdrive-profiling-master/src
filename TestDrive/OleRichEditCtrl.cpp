// OleRichEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "OleRichEditCtrl.h"
#include "Output.h"
//#include <Excel/>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR	g_sRichEdit50ClassName = _T("RichEdit50W");

/////////////////////////////////////////////////////////////////////////////
// COleRichEditCtrl

HINSTANCE	COleRichEditCtrl::m_hInstRichEdit50W	= NULL;      // handle to MSFTEDIT.DLL

COleRichEditCtrl::COleRichEditCtrl() : m_szFileName(_T("")), m_bOpen(FALSE)
{
	m_bCallbackSet	= FALSE;
	m_bEditable		= FALSE;

	if(!m_hInstRichEdit50W)
		m_hInstRichEdit50W = LoadLibrary(_T("msftedit.dll"));
}

COleRichEditCtrl::~COleRichEditCtrl()
{
	// IExRichEditOleCallback class is a reference-counted class  
	// which deletes itself and for which delete should not be called

	// delete m_pIRichEditOleCallback;
}


BEGIN_MESSAGE_MAP(COleRichEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(COleRichEditCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEWHEEL()
	ON_WM_NCPAINT()
	ON_MESSAGE(CWM_FILE_CHANGED_NOTIFICATION, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int COleRichEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
 		return -1;
 	
	// m_pIRichEditOleCallback should have been created in PreSubclassWindow

 	ASSERT( m_pIRichEditOleCallback != NULL );

	SendMessage(EM_SETLANGOPTIONS, 0, (SendMessage(EM_GETLANGOPTIONS, 0, 0)& ~(IMF_AUTOFONT| IMF_AUTOFONTSIZEADJUST)));	// 자동 폰트 변경 방지
	// set the IExRichEditOleCallback pointer if it wasn't set 
	// successfully in PreSubclassWindow

	if ( !m_bCallbackSet ){
		SetOLECallback( m_pIRichEditOleCallback );
	}

	return 0;
}

void COleRichEditCtrl::OnAccel(ACCEL_CODE code){
	switch(code){
	case ACCEL_CODE_COPY:		Copy();			break;
	case ACCEL_CODE_PASTE:		Paste();		break;
	case ACCEL_CODE_CUT:		Cut();			break;
	case ACCEL_CODE_UNDO:		Undo();			break;
	case ACCEL_CODE_SELECTALL:	SetSel(0, -1);	break;
	}
}

void COleRichEditCtrl::OnSetFocus(CWnd* pOldWnd){
	if(m_bEditable){
		SetCurrentCopynPasteAction(TRUE);
		CRichEditCtrl::OnSetFocus(pOldWnd);
	}
	else{
		GetParent()->GetParent()->SetFocus();
	}
	//SendMessage(EM_SETZOOM, 20, 100);

	//SendMessage(EM_GETZOOM,(WPARAM)&d1, (LPARAM)&d2);
}

void COleRichEditCtrl::OnKillFocus(CWnd* pNewWnd){
	SetCurrentCopynPasteAction(FALSE);
	CRichEditCtrl::OnKillFocus(pNewWnd);
}

BOOL COleRichEditCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt){
	// Prevent self-zoom control
	return (nFlags&MK_CONTROL)!=0;
}

void COleRichEditCtrl::OnLButtonDown(UINT nFlags, CPoint point){
	if(m_bEditable) CRichEditCtrl::OnLButtonDown(nFlags,point);;
}

void COleRichEditCtrl::OnLButtonUp(UINT nFlags, CPoint point){
	if(m_bEditable) CRichEditCtrl::OnLButtonUp(nFlags,point);
	//GetParent()->SendMessage(WM_LBUTTONUP,0,0);
}

void COleRichEditCtrl::OnRButtonDown(UINT nFlags, CPoint point){
	if(m_bEditable) CRichEditCtrl::OnRButtonDown(nFlags,point);;
}

void COleRichEditCtrl::OnRButtonUp(UINT nFlags, CPoint point){
	if(m_bEditable) CRichEditCtrl::OnRButtonUp(nFlags,point);
}

void COleRichEditCtrl::Test(void){
	LPRICHEDITOLE pRichEditOle = GetIRichEditOle();

	if(!pRichEditOle) return;

	int			count = pRichEditOle->GetObjectCount();
	REOBJECT	reObj;
	
	ZeroMemory(&reObj, sizeof(REOBJECT));
	reObj.cbStruct  = sizeof(REOBJECT);
	
	for(int i=0;i<count;i++){
		if(pRichEditOle->GetObject(0, &reObj, REO_GETOBJ_ALL_INTERFACES) == S_OK){
			IDataObject*	lpDataObject;
			COleDataObject	dataObject;

			WCHAR* sProgID;
			ProgIDFromCLSID(reObj.clsid, &sProgID);
			if(wcscmp(sProgID, L"MSGraph.Chart") > 0){

			}
			//reObj.poleobj->Release();

			if(reObj.poleobj->QueryInterface(IID_IDispatch, (void**)&lpDataObject) == S_OK){
				/*STGMEDIUM	stgm;	// out
				FORMATETC	fm;		// in
				fm.cfFormat	= CF_BITMAP;		// Clipboard format = CF_BITMAP
				fm.ptd		= NULL;				// Target device	= Screen
				fm.dwAspect	= DVASPECT_CONTENT;	// Level of detail	= FULL content
				fm.lindex	= -1;				// Index			= Not applicable
				fm.tymed	= TYMED_GDI;		// Storage medium	= HBITMAP handle	// TYMED_GDI

				dataObject.Attach(lpDataObject);*/
			}
		}
	}

	pRichEditOle->Release();
}

LRESULT COleRichEditCtrl::OnUpdate(WPARAM wParam, LPARAM lParam){
// Get the IRichEditOle interface.
	if(wParam==0){
		//AfxMessageBox(m_WatchDogFolderList.front()->GetFileName());
		Open(m_WatchDogFolderList.front()->GetPath(), TRUE);
		return 0;
	}
	LPRICHEDITOLE pRichEditOle = NULL;
	SendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle);
	if (pRichEditOle == NULL) return 0;


	CString str;
	int count = pRichEditOle->GetObjectCount();
	
	pRichEditOle->GetLinkCount();

	for(int i=0;i<count;i++){
		REOBJECT	reObj;
		ZeroMemory(&reObj, sizeof(REOBJECT));
		reObj.cbStruct  = sizeof(REOBJECT);
		//CLSID id;

		pRichEditOle->GetObject(i, &reObj, REO_GETOBJ_POLEOBJ);
		LPOLEOBJECT OleObject = reObj.poleobj;


		//ZeroMemory(&id, sizeof(CLSID));
		//if(OleObject->GetUserClassID(&id) != S_OK) continue;
		//OleObject->
		//LPOLESTR	pName;
		//ProgIDFromCLSID(id, &pName);
		//엑셀 데이터를 업데이트 한다.
		//if(StrStr(pName, _T("Excel"))){
		if(reObj.dwFlags & (REO_LINK|REO_LINKAVAILABLE)) reObj.poleobj->Update();
		//if(reObj.poleobj->IsUpToDate()==S_FALSE) reObj.poleobj->Update();
		//}
		reObj.poleobj->Release();
		//reObj.poleobj->Close(OLECLOSE_SAVEIFDIRTY);
	}
	pRichEditOle->Release();
	return 0;
}

void COleRichEditCtrl::PreSubclassWindow() 
{
	// base class first
	CRichEditCtrl::PreSubclassWindow();	

	m_pIRichEditOleCallback = NULL;
	m_pIRichEditOleCallback = new IExRichEditOleCallback;
	ASSERT( m_pIRichEditOleCallback != NULL );

	m_bCallbackSet = SetOLECallback( m_pIRichEditOleCallback );
}

BOOL COleRichEditCtrl::PreCreateWindow(CREATESTRUCT& cs){
	CRichEditCtrl::PreCreateWindow(cs);
	cs.lpszClass = g_sRichEdit50ClassName;
	return TRUE;
}

long COleRichEditCtrl::StreamInFromResource(int iRes, LPCTSTR sType)
{
	HINSTANCE hInst = AfxGetInstanceHandle();
	HRSRC hRsrc = ::FindResource(hInst,
		MAKEINTRESOURCE(iRes), sType);
	
	DWORD len = SizeofResource(hInst, hRsrc); 
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc); 
	ASSERT(lpRsrc); 
 
	CMemFile mfile;
	mfile.Attach(lpRsrc, len); 

	EDITSTREAM es;
	es.pfnCallback = readFunction;
	es.dwError = 0;
	es.dwCookie = (DWORD_PTR) &mfile;

	return StreamIn( SF_RTF, es );
}

BOOL COleRichEditCtrl::Open(LPCTSTR lpszFileName, BOOL bAutoUpdate){

	//ModifyStyle(WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_AUTOHSCROLL,0);
	
	Clear();
	{	// Retrieve a full filename
		TCHAR name[1024];
		TCHAR* pFilePart;
		GetFullPathName(lpszFileName, 1024, name, &pFilePart);
		if(!pFilePart) return FALSE;
		m_szFileName = name;
	}
	//ModifyStyle(0, ES_MULTILINE, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	{	// open file
		CFile	file;
		if(!file.Open(m_szFileName, CFile::modeRead|CFile::typeBinary)) return FALSE;
		EDITSTREAM es;
		es.pfnCallback	= readFunction;
		es.dwError		= 0;
		es.dwCookie		= (DWORD_PTR) &file;

		StreamIn(SF_RTF, es);
	}

	// set watching thread
	ClearWatchingFileAll();
	if(bAutoUpdate) AddWatchingFile(lpszFileName, m_hWnd, 0, FALSE);
	PostMessage(CWM_FILE_CHANGED_NOTIFICATION, 1);

	m_bOpen	= TRUE;

	return TRUE;
}

BOOL COleRichEditCtrl::Save(LPCTSTR lpszFileName){
	CFile	file;
	if(!lpszFileName){
		lpszFileName = m_szFileName;
	}

	if(!file.Open(lpszFileName, CFile::modeWrite|CFile::typeBinary|CFile::modeCreate)){
		return FALSE;
	}
	EDITSTREAM es;
	es.pfnCallback	= writeFunction;
	es.dwError		= 0;
	es.dwCookie		= (DWORD_PTR) &file;

	StreamOut(SF_RTF, es);
	return TRUE;
}

void COleRichEditCtrl::Clear(BOOL bClose){
	CRichEditCtrl::SetSel(0,-1);
	CRichEditCtrl::ReplaceSel(_T(""));
	if(bClose){
		ClearWatchingFileAll();
		m_bOpen = FALSE;
	}
}

void COleRichEditCtrl::AppendText(LPCTSTR lpszText){
	CRichEditCtrl::SetSel(-1,-1);
	CRichEditCtrl::ReplaceSel(lpszText);
}

void COleRichEditCtrl::SetScale(int iScale){
	if(iScale<=0) return;
	SendMessage(EM_SETZOOM, iScale, 100);
}

/* static */
DWORD CALLBACK COleRichEditCtrl::readFunction(DWORD_PTR dwCookie,
											  LPBYTE lpBuf,			// the buffer to fill
											  LONG nCount,			// number of bytes to read
											  LONG* nRead)			// number of bytes actually read
{
	CFile* fp = (CFile *)dwCookie;
	*nRead = fp->Read(lpBuf,nCount);
	return 0;
}

DWORD CALLBACK COleRichEditCtrl::writeFunction(DWORD_PTR dwCookie,
											  LPBYTE lpBuf,			// the buffer to fill
											  LONG nCount,			// number of bytes to read
											  LONG* nRead)			// number of bytes actually read
{
	CFile* fp = (CFile *)dwCookie;
	fp->Write(lpBuf,nCount);
	*nRead = nCount;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////

COleRichEditCtrl::IExRichEditOleCallback::IExRichEditOleCallback()
{
	pStorage = NULL;
	m_iNumStorages = 0;
	m_dwRef = 0;

	// set up OLE storage

	HRESULT hResult = ::StgCreateDocfile(NULL,
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE /*| STGM_DELETEONRELEASE */|STGM_CREATE ,
		0, &pStorage );

	if ( pStorage == NULL ||
		hResult != S_OK )
	{
		AfxThrowOleException( hResult );
	}
}

COleRichEditCtrl::IExRichEditOleCallback::~IExRichEditOleCallback()
{
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetNewStorage(LPSTORAGE* lplpstg)
{
	m_iNumStorages++;
	WCHAR tName[50];
	swprintf(tName, L"REOLEStorage%d", m_iNumStorages);

	HRESULT hResult = pStorage->CreateStorage(tName, 
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
		0, 0, lplpstg );

	if (hResult != S_OK )
	{
		::AfxThrowOleException( hResult );
	}

	return hResult;
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;
	
	if ( iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback )
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}



ULONG STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::AddRef()
{
	return ++m_dwRef;
}



ULONG STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::Release()
{
	if ( --m_dwRef == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
	LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	return E_NOTIMPL;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return E_NOTIMPL;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
	DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}



HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	COleDataSource *pSource = new COleDataSource();
	CSharedFile sf(GMEM_MOVEABLE|GMEM_DDESHARE|GMEM_ZEROINIT);

	STATSTG stat;
	pStorage->Stat(&stat, STATFLAG_DEFAULT );

	IStream *pStream;
	CString strName(stat.pwcsName);
	HRESULT res = pStorage->OpenStream(NULL, NULL, STGM_SHARE_EXCLUSIVE | STGM_READ, 0, &pStream);
	if( res != S_OK )
		return S_FALSE;

	ULONGLONG dw = sf.GetLength();
	int nSize = INT_MAX;

	if( dw < INT_MAX )
		nSize = (int)dw;

	CString strRTF;
	LPTSTR pStr = strRTF.GetBufferSetLength(nSize);
	ULONG nRead(0);
	pStream->Read(pStr, nSize, &nRead);

	sf.Write(pStr, nRead);

	strRTF.ReleaseBufferSetLength(nRead);

	HGLOBAL hMem = sf.Detach();
	if( !hMem )
		return S_FALSE;

	UINT format = ::RegisterClipboardFormat(CF_RTF);
	pSource->CacheGlobalData(format, hMem);
	pSource->SetClipboard();
	lplpdataobj = (LPDATAOBJECT *)&pSource;

	return S_OK;

#if _MFC_VER <= 0x0421
	::GlobalUnlock(hMem);
#endif


	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE 
COleRichEditCtrl::IExRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
	HMENU FAR *lphmenu)
{
	return E_NOTIMPL;
}

/*
BOOL InsertObject(HWND hRichEdit, LPCTSTR pszFileName)
{
	HRESULT hr;

	// Get the IRichEditOle interface.
	LPRICHEDITOLE pRichEditOle;
	SendMessage(hRichEdit, EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle);
	if (pRichEditOle == NULL)
	{
		return FALSE;
	}
	pRichEditOle->GetObjectCount();

	pRichEditOle->GetObject()
	// Create structured storage.
	LPLOCKBYTES pLockBytes = NULL;

	hr = CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes);
	if (FAILED(hr))
	{
		return FALSE;
	}

	LPSTORAGE pStorage;
	hr = StgCreateDocfileOnILockBytes(pLockBytes,
		STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 
		0, &pStorage);
	if (FAILED(hr))
	{
		return FALSE;
	}

	// Set up data format.
	FORMATETC formatEtc;
	formatEtc.cfFormat = 0;
	formatEtc.ptd = NULL;
	formatEtc.dwAspect = DVASPECT_CONTENT;
	formatEtc.lindex = -1;
	formatEtc.tymed = TYMED_NULL;

	// Get an interface to the display site.
	LPOLECLIENTSITE	pClientSite;
	hr = pRichEditOle->GetClientSite(&pClientSite);
	if (FAILED(hr))
	{
		return FALSE;
	}

	// Create the object and retrieve its IUnknown.
	LPUNKNOWN pUnk;
	CLSID clsid = CLSID_NULL;
	hr = OleCreateFromFile(clsid, pszFileName,
		IID_IUnknown, OLERENDER_DRAW, &formatEtc, 
		pClientSite, pStorage, (void**)&pUnk);
	pClientSite->Release();
	if (FAILED(hr))
	{
		return FALSE;
	}

	// Get the IOleObject interface to the object.
	LPOLEOBJECT pObject;
	hr = pUnk->QueryInterface(IID_IOleObject, (void**)&pObject);
	pUnk->Release();
	if (FAILED(hr))
	{
		return FALSE;
	}

	// Notify the object that it is contained, so reference counting
	// is done correctly.
	OleSetContainedObject(pObject, TRUE);

	// Set up object info.
	REOBJECT reobject = { sizeof(REOBJECT)};
	hr = pObject->GetUserClassID(&clsid);
	if (FAILED(hr))
	{
		pObject->Release();
		return FALSE;
	}
	reobject.clsid = clsid;
	reobject.cp = REO_CP_SELECTION;
	reobject.dvaspect = DVASPECT_CONTENT;
	reobject.dwFlags = REO_RESIZABLE | REO_BELOWBASELINE;
	reobject.dwUser = 0;
	reobject.poleobj = pObject;
	reobject.polesite = pClientSite;
	reobject.pstg = pStorage;
	SIZEL sizel = { 0 };
	reobject.sizel = sizel;

	// Move the caret to the end of the text and add a CR.
	SendMessage(hRichEdit, EM_SETSEL, 0, -1);
	DWORD dwStart, dwEnd;
	SendMessage(hRichEdit, EM_GETSEL, (WPARAM)&dwStart, (LPARAM)&dwEnd);
	SendMessage(hRichEdit, EM_SETSEL, dwEnd+1, dwEnd+1);
	SendMessage(hRichEdit, EM_REPLACESEL, TRUE, (WPARAM)L"\n"); 

	// Insert the object.
	hr = pRichEditOle->InsertObject(&reobject);

	// Clean up.
	pObject->Release();
	pRichEditOle->Release();

	if (FAILED(hr))
	{
		return FALSE;
	}
	return TRUE;
}
*/
/*
bool InsertFile()
{
	//Inserting an excel sheet
	//Pointer to the IRichEditOle interface of the rich edit control.
	LPRICHEDITOLE m_pRichEditOle;
	//Pointer to the embedded object.
	LPOLEOBJECT m_lpObject;
	//Pointer to the storage.
	LPSTORAGE m_lpStorage;
	//Pointer to the clientsite.
	LPOLECLIENTSITE m_lpClientSite;
	LPLOCKBYTES lpLockBytes = NULL;
	CLSID clsid = CLSID_NULL;
	OLERENDER render = OLERENDER_DRAW;
	CLIPFORMAT cfFormat = 0;
	LPFORMATETC lpFormatEtc = NULL;
	//Getting the pointer for IRichEditOle from the richedit control
	//::SendMessage(RichEditSS->Handle, EM_SETOLECALLBACK, 0,
	(LPARAM)m_pRichEditOle);
	if (!::SendMessage(RichEditSS->Handle, EM_GETOLEINTERFACE, 0, (LPARAM)
		&m_pRichEditOle))
		m_pRichEditOle = 0;
	//Creating storage for the object
	HRESULT hr;
	hr = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
	if (FAILED(hr))
		return false;
	hr = ::StgCreateDocfileOnILockBytes(lpLockBytes,
		STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_READWRITE, 0, &m_lpStorage);
	lpLockBytes->Release();
	//--------------------------------------------------------------------------
	// fill in FORMATETC struct
	FORMATETC formatEtc;
	lpFormatEtc = &formatEtc;
	lpFormatEtc->cfFormat = cfFormat;
	lpFormatEtc->ptd = NULL;
	lpFormatEtc->dwAspect = DVASPECT_CONTENT;
	lpFormatEtc->lindex = -1;
	lpFormatEtc->tymed = TYMED_NULL;
	//Converting
	//LPCOLESTR lpszFileName =OLESTR("C:\\Program
	Files\\Borland\\CBuilder6\\Projects\\Temporary\\ExcelSheetProgrammatically\\
		NewTestExcel.xls");
		//LPCOLESTR lpszFileName =OLESTR("c:\\NewTestExcel.xls");
		//File to be inserted.
		AnsiString asXLSFileName("c:\\NewTestExcel.xls");
	size_t size = asXLSFileName.Length();
	LPOLESTR lpszFileName = new OLECHAR[size];
	//wmemset(lpszFileName, 0 , size);
	mbstowcs(lpszFileName, asXLSFileName.c_str(), size);
	// attempt to create the object(to get the client site)
	m_pRichEditOle->GetClientSite(&m_lpClientSite);
	hr = ::OleCreateFromFile(clsid, lpszFileName,
		IID_IUnknown, OLERENDER_DRAW, lpFormatEtc, m_lpClientSite, m_lpStorage,
		(void**)&m_lpObject);
	if (FAILED(hr))
		return false;
	// m_lpObject is currently an IUnknown, convert to IOleObject
	if (m_lpObject != NULL)
	{
		LPUNKNOWN lpUnk = m_lpObject;
		lpUnk->QueryInterface(IID_IOleObject, (void**)&m_lpObject);
		lpUnk->Release();
		if (m_lpObject == NULL)
			return false;
	}
	// all items are "contained" -- this makes our reference to this object
	// weak -- which is needed for links to embedding silent update.
	OleSetContainedObject(m_lpObject, TRUE);
	//Forming the REOBJECT Structure. This structure contains the
	information about the object.
		REOBJECT reobject;
	//ZeroMemory(&reobject, sizeof(REOBJECT));
	::memset(&reobject, 0, sizeof(reobject));
	reobject.cbStruct = sizeof(REOBJECT);
	//CLSID clsid;
	hr = m_lpObject->GetUserClassID(&clsid);
	if (FAILED(hr))
		return false;
	reobject.clsid = clsid;
	reobject.cp = REO_CP_SELECTION;
	reobject.dvaspect = DVASPECT_CONTENT;
	reobject.dwFlags = REO_DYNAMICSIZE;
	reobject.dwUser = 0;
	reobject.poleobj = m_lpObject;
	//ASSERT(m_lpClientSite != NULL);
	reobject.polesite = m_lpClientSite;
	//ASSERT(m_lpStorage != NULL);
	reobject.pstg = m_lpStorage;
	//Size of the rectangle window.
	SIZEL sizel;
	sizel.cx = sizel.cy = 0;
	reobject.sizel = sizel;
	//finally inserting the object
	m_pRichEditOle->InsertObject(&reobject);
	DeleteFile("c:\\NewTestExcel.xls");
	delete [] lpszFileName;
}
*/