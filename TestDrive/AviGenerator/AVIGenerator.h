#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// needed headers
#include <comdef.h>
#include <memory.h>
#include <tchar.h>
#include <string.h>
#include <vfw.h>
#include "Buffer.h"

#pragma comment (lib, "vfw32.lib")

/*! \brief A simple class to create AVI video stream.
\par Usage

  Step 1 : Declare an CAVIGenerator object
  Step 2 : Set Bitmap by calling SetBitmapHeader functions + other parameters
  Step 3 : Initialize engine by calling InitEngine
  Step 4 : Send each frames to engine with function AddFrame
  Step 5 : Close engine by calling ReleaseEngine

\par Demo Code:

\code
	CAVIGenerator AviGen;
	BYTE* bmBits;

	// set characteristics
	AviGen.SetRate(20);							// set 20fps
	AviGen.SetBitmapHeader(GetActiveView());	// give info about bitmap

	AviGen.InitEngine();

	..... // Draw code, bmBits is the buffer containing the frame
	AviGen.AddFrame(bmBits);
	.....

	AviGen.ReleaseEngine();
\endcode

*/
class CAVIGenerator : public ITDMovie
{
public:
	CAVIGenerator();
	~CAVIGenerator();

	// for save
	STDMETHOD_(BOOL, Create)(LPCTSTR lpszFileName, DWORD dwWidth, DWORD dwHeight, DWORD dwFrameRate = 30);
	STDMETHOD_(BOOL, SetFrame)(PVOID pBuff = NULL);				// Add frame to Image

	// for load
	STDMETHOD_(BOOL, Open)(LPCTSTR lpszFileName, LONG& lFirstFrame, LONG& lNumOfFrames, DWORD* pdwFrameRate = NULL);
	STDMETHOD_(BOOL, GetFrame)(LONG lFrame, PVOID* pBuff = NULL);

	// common
	STDMETHOD_(ITDBuffer*, Buffer)(void);	// get buffer
	STDMETHOD_(int, Width)(void);			// get width
	STDMETHOD_(int, Height)(void);			// get height
	STDMETHOD_(void, Release)(void);		// Release ressources allocated for movie and close file.

	// option
	STDMETHOD_(void, EnableAlpha)(BOOL bEnable = TRUE);

protected:	
	void SafeRelease(void);					// Safe release

	CString		m_sFile;					// movie ouput file name
	CBuffer		m_Buffer;					// reference single stream buffer

private:
	long		m_lFrameCount;				// frame counter
	PAVIFILE	m_pAVIFile;					// file Interface pointer
	PAVISTREAM	m_pStream;					// stream interface
	PAVISTREAM	m_pStreamCompressed;		// compressed video stream interface
	AVIFILEINFO	m_AVIInfo;
	PGETFRAME	m_pFrame;
	LONG		m_lFirstFrame, m_lNumOfFrames;

	BOOL		m_bInitialized;
	BOOL		m_bUseAlpha;
};
