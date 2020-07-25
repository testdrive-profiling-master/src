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

	STDMETHOD_(BOOL, Initialize)(LPCTSTR lpszFileName, DWORD dwWidth, DWORD dwHeight, DWORD dwFrameRate = 30);
	STDMETHOD_(BOOL, DrawFrame)(ITDBuffer* pBuffer, int x = 0, int y = 0, int sx = 0, int sy = 0, int width = 0, int height = 0);
	STDMETHOD_(BOOL, AddFrame)(void);						// Add frame to Image
	STDMETHOD_(void, Release)(void);						// Release ressources allocated for movie and close file.
	void SetFileName(LPCTSTR lpszFileName);		// sets the name of the ouput file (should be .avi)

protected:	
	void SafeRelease(void);				// Safe release

	CString		m_sFile;				// movie ouput file name
	CBuffer		m_Buffer;				// reference single stream buffer

private:
	void MakeExtAvi();

	long m_lFrameCount;					// frame counter
	PAVIFILE m_pAVIFile;				// file Interface pointer
	PAVISTREAM m_pStream;				// stream interface
	PAVISTREAM m_pStreamCompressed;		// compressed video stream interface

	BOOL		m_bInitialized;
};
