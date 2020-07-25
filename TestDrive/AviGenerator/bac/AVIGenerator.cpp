#include "stdafx.h"
#include "AVIGenerator.h"
#include "TestDrive.h"
#include "Main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CAVIGenerator::CAVIGenerator()
: m_bInitialized(FALSE),
m_pAVIFile(NULL), m_pStream(NULL), m_pStreamCompressed(NULL)
{
	MakeExtAvi();
}

CAVIGenerator::~CAVIGenerator(){
	SafeRelease();
}

void CAVIGenerator::SafeRelease(void){
	if(m_pStream){
		AVIStreamRelease(m_pStream);
		m_pStream=NULL;
	}

	if(m_pStreamCompressed){
		AVIStreamRelease(m_pStreamCompressed);
		m_pStreamCompressed=NULL;
	}

	if(m_pAVIFile){
		AVIFileRelease(m_pAVIFile);
		m_pAVIFile=NULL;
	}

	if(m_bInitialized){
		AVIFileExit();	// Release AVIFile library
		m_bInitialized = FALSE;
	}

	m_Buffer.ReleaseAll();
}

void CAVIGenerator::SetFileName(LPCTSTR lpszFileName){
	m_sFile	= lpszFileName;
	MakeExtAvi();
}

BOOL CAVIGenerator::Initialize(LPCTSTR lpszFileName, DWORD dwWidth, DWORD dwHeight, DWORD dwFrameRate){
	AVISTREAMINFO strHdr; // information for a single stream
	AVICOMPRESSOPTIONS opts;
	AVICOMPRESSOPTIONS FAR * aopts[1] = {&opts};

	SafeRelease();

	if(!lpszFileName) return FALSE;
	SetFileName(lpszFileName);

	m_Buffer.Create(dwWidth, dwHeight, ITDBuffer::COLORTYPE_RGB_888);

	// make sure buffer width and height size must be multiple of 4
	if(!m_Buffer.IsInitialize() || (m_Buffer.Width()%4) || (m_Buffer.Height()%4)) return FALSE;
	m_lFrameCount	= 0;	// initialize step counter

	{
		// Step 0 : Let's make sure we are running on 1.1 
		DWORD wVer = HIWORD(VideoForWindowsVersion());
		if (wVer < 0x010a){
			g_pTestDrive->LogOut(_T("Your Video for Windows too old."), ITDSystem::SYSMSG_ERROR);
			return S_FALSE;
		}
	}

	// Step 1 : initialize AVI engine
	AVIFileInit();	// Initialize AVIFile library
	m_bInitialized	= TRUE;

	{	// Step 2 : Open the movie file for writing....
		HRESULT hr = AVIFileOpen(&m_pAVIFile,			// Address to contain the new file interface pointer
								 m_sFile,				// Null-terminated string containing the name of the file to open
								 OF_WRITE | OF_CREATE,	// Access mode to use when opening the file. 
								 NULL);					// use handler determined from file extension.
														// Name your file .avi -> very important
		if (hr != AVIERR_OK){
			CString error_str;
			error_str.Format(_T("AVI Engine failed to initialize. Check filename \"%s\" : "), m_sFile);
			
			// Check it succeded.
			switch(hr){
			case AVIERR_BADFORMAT:		error_str+=_T("The file couldn't be read, indicating a corrupt file or an unrecognized format.");break;
			case AVIERR_MEMORY:			error_str+=_T("The file could not be opened because of insufficient memory.");break;
			case AVIERR_FILEREAD:		error_str+=_T("A disk error occurred while reading the file.");break;
			case AVIERR_FILEOPEN:		error_str+=_T("A disk error occurred while opening the file.");break;
			case REGDB_E_CLASSNOTREG:	error_str+=_T("According to the registry, the type of file specified in AVIFileOpen does not have a handler to process it");break;
			}
			g_pTestDrive->LogOut(error_str, ITDSystem::SYSMSG_ERROR);
			return FALSE;
		}
	}

	{	// Step 3 : Create the stream
		// Fill in the header for the video stream....
		memset(&strHdr, 0, sizeof(strHdr));
		strHdr.fccType                = streamtypeVIDEO;	// video stream type
		strHdr.fccHandler             = 0;
		strHdr.dwScale                = 1;					// should be one for video
		strHdr.dwRate                 = dwFrameRate;		// fps
		strHdr.dwSuggestedBufferSize  = m_Buffer.GetHeader()->bV5SizeImage;	// Recommended buffer size, in bytes, for the stream.
		m_Buffer.GetRect(&strHdr.rcFrame);				// rectangle for stream

		HRESULT hr = AVIFileCreateStream(	m_pAVIFile,		// file pointer
											&m_pStream,		// returned stream pointer
											&strHdr);		// stream header

		// Check it succeded.
		if (hr != AVIERR_OK){
			CString error_str(_T("AVI Stream creation failed. Check Bitmap info."));
			if (hr==AVIERR_READONLY)
				error_str+=_T(" Read only file.");
			g_pTestDrive->LogOut(error_str, ITDSystem::SYSMSG_ERROR);
			SafeRelease();
			return FALSE;
		}
	}

	{	// Step 4: Get codec and infos about codec
		memset(&opts, 0, sizeof(opts));
		theApp.GetMainWnd()->EnableWindow(FALSE);
		// Poping codec dialog
		if (!AVISaveOptions(NULL, 0, 1, &m_pStream, (LPAVICOMPRESSOPTIONS FAR *) &aopts)){
			AVISaveOptionsFree(1,(LPAVICOMPRESSOPTIONS FAR *) &aopts);
			SafeRelease();
			theApp.GetMainWnd()->EnableWindow(TRUE);
			return S_FALSE;
		}else theApp.GetMainWnd()->EnableWindow(TRUE);


		// Step 5:  Create a compressed stream using codec options.
		HRESULT hr = AVIMakeCompressedStream(	&m_pStreamCompressed, 
												m_pStream, 
												&opts, 
												NULL);

		if(hr != AVIERR_OK){
			CString error_str(_T("AVI Compressed Stream creation failed."));

			switch(hr){
			case AVIERR_NOCOMPRESSOR:	error_str+=_T(" A suitable compressor cannot be found.");break;
			case AVIERR_MEMORY:			error_str+=_T(" There is not enough memory to complete the operation.");break; 
			case AVIERR_UNSUPPORTED:	error_str+=_T("Compression is not supported for this type of data. This error might be returned if you try to compress data that is not audio or video.");break;
			}
			g_pTestDrive->LogOut(error_str, ITDSystem::SYSMSG_ERROR);
			SafeRelease();
			return FALSE;
		}

		// releasing memory allocated by AVISaveOptionFree
		hr = AVISaveOptionsFree(1,(LPAVICOMPRESSOPTIONS FAR *) &aopts);
		if(hr!=AVIERR_OK){
			g_pTestDrive->LogOut(_T("Error releasing memory"), ITDSystem::SYSMSG_ERROR);
			SafeRelease();
			return FALSE;
		}

		{
			BITMAPV5HEADER* pHeader	= m_Buffer.GetHeader();
			// Step 6 : sets the format of a stream at the specified position
			hr = AVIStreamSetFormat(m_pStreamCompressed,
									0,					// position
									pHeader,			// stream format
									pHeader->bV5Size +	// format size
									pHeader->bV5ClrUsed * sizeof(RGBQUAD));
			if (hr != AVIERR_OK){
				g_pTestDrive->LogOut(_T("AVI Compressed Stream format setting failed."), ITDSystem::SYSMSG_ERROR);
				SafeRelease();
				return FALSE;
			}
		}
	}
	return TRUE;
}

void CAVIGenerator::Release(void)
{
	SafeRelease();
	delete this;
}

BOOL CAVIGenerator::DrawFrame(ITDBuffer* pBuffer, int x, int y, int sx, int sy, int width, int height){
	return m_Buffer.CopyFromBuffer(pBuffer, x, y, sx, sy, width, height);
}

BOOL CAVIGenerator::AddFrame(void)
{
	if(!m_Buffer.IsInitialize() || !m_pStreamCompressed) return FALSE;
	// compress bitmap
	if(!AVIStreamWrite(	m_pStreamCompressed,		// stream pointer
						m_lFrameCount,				// time of this frame
						1,							// number to write
						m_Buffer.GetPointer(),	// image buffer
						m_Buffer.GetHeader()->bV5SizeImage,	// size of this frame
						AVIIF_KEYFRAME,				// flags....
						NULL,
						NULL)){
		m_lFrameCount++;	// updating frame counter
		return TRUE;
	}

	return FALSE;
}

void CAVIGenerator::MakeExtAvi()
{
	
	// finding avi
	if( _tcsstr(m_sFile,_T("avi"))==NULL )
	{
		m_sFile+=_T(".avi");
	}
}
