#include "StdAfx.h"
#include "Buffer.h"
#include "Main.h"
#include "Output.h"
#include "TestDriveImp.h"
#include <atlimage.h>
#include <math.h>

const TCHAR* g_sColorTypes[COLORTYPE_SIZE] = {
	_T("ALPHA"),
	_T("LUMINANCE"),
	_T("LUMINANCE_ALPHA"),
	_T("RGB_565"),
	_T("RGBA_4444"),
	_T("RGBA_5551"),
	_T("RGB_888"),
	_T("ABGR_8888"),
	_T("RGBA_8888"),
	_T("ARGB_8888")
};

typedef enum{
	VIDEO_QVGA,
	VIDEO_VGA,
	VIDEO_SVGA,
	VIDEO_XGA,
	VIDEO_WXGA,
	VIDEO_SXGA,
	VIDEO_WSXGA,
	VIDEO_SXGA_PLUS,
	VIDEO_WSXGA_PLUS,
	VIDEO_UXGA,
	VIDEO_WUXGA,
	VIDEO_QXGA,
	VIDEO_WQXGA,
	VIDEO_QSXGA,
	VIDEO_WQSXGA,
	VIDEO_QUXGA,
	VIDEO_WQUXGA,
	VIDEO_WUQSXGA,
	VIDEO_SQCIF,
	VIDEO_QCIF,
	VIDEO_SD,
	VIDEO_HD,
	VIDEO_FullHD,
	VIDEO_SIZE
}VIDEO_TYPE;

const int g_iVideoDimension[VIDEO_SIZE][2] = {
	{320, 240},
	{640, 480},
	{800, 600},
	{1024, 768},
	{1280, 800},
	{1280, 1024},
	{1440, 900},
	{1400, 1050},
	{1680, 1050},
	{1600, 1200},
	{1920, 1200},
	{2048, 1536},
	{2560, 1600},
	{2560, 2048},
	{3200, 2048},
	{3200, 2400},
	{3840, 2400},
	{4200, 2690},
	{128, 96},
	{176, 144},
	{720, 480},
	{1280, 720},
	{1920, 1080},
};

const TCHAR* g_sVideoDimension[VIDEO_SIZE] = {
	_T("QVGA"),
	_T("VGA"),
	_T("SVGA"),
	_T("XGA"),
	_T("WXGA"),
	_T("SXGA"),
	_T("WSXGA"),
	_T("SXGA+"),
	_T("WSXGA+"),
	_T("UXGA"),
	_T("WUXGA"),
	_T("QXGA"),
	_T("WQXGA"),
	_T("QSXGA"),
	_T("WQSXGA"),
	_T("QUXGA"),
	_T("WQUXGA"),
	_T("WUQSXGA"),
	_T("SQCIF"),
	_T("QCIF"),
	_T("SD"),
	_T("HD"),
	_T("FullHD"),
};

CBuffer::CBuffer(void){
	m_lpDIBits			= NULL;
	m_memDC				= NULL;
	m_hbmScreen			= NULL;
	m_pGraphics			= NULL;
	m_pPen				= NULL;
	m_pBrush			= NULL;
	m_bAntialiasing		= FALSE;
	m_bOutline			= TRUE;
	m_crBackColor		= 0xCFCFCF;
	m_fAspectRatio		= 0;
	ReleaseAll();
}

CBuffer::~CBuffer(void) {
	ReleaseAll();
}

BOOL CBuffer::Create(DWORD dwWidth, DWORD dwHeight, COLORFORMAT Type){
	ReleaseAll();

	if(!dwWidth || !dwHeight) return FALSE;

	m_ColorFormat		= Type;
	m_dwWidth			= dwWidth;
	m_dwHeight			= dwHeight;
	m_fAspectRatio		= (float)dwHeight / dwWidth;
	m_bmpRect.right		= dwWidth;
	m_bmpRect.bottom	= dwHeight;

	memset(&m_bmpHeader, 0, sizeof(BITMAPV5HEADER));
	m_bmpHeader.bV5Size					= sizeof(BITMAPV5HEADER);
	m_bmpHeader.bV5Width				= dwWidth;
	m_bmpHeader.bV5Height				= dwHeight;
	m_bmpHeader.bV5Planes				= 1;
	m_bmpHeader.bV5Compression			= BI_BITFIELDS;

	switch(Type){
	case COLORTYPE_ALPHA:			// A8
		m_bmpHeader.bV5BitCount			= 8;
		m_bmpHeader.bV5AlphaMask		= 0xFF;
		break;
	case COLORTYPE_LUMINANCE:		// L8
		m_bmpHeader.bV5BitCount			= 8;
		m_bmpHeader.bV5RedMask			= 0xFF;
		m_bmpHeader.bV5GreenMask		= 0xFF;
		m_bmpHeader.bV5BlueMask			= 0xFF;
		break;
	case COLORTYPE_LUMINANCE_ALPHA:	// L8A8
		m_bmpHeader.bV5BitCount			= 16;
		m_bmpHeader.bV5RedMask			= 0xFF00;
		m_bmpHeader.bV5GreenMask		= 0xFF00;
		m_bmpHeader.bV5BlueMask			= 0xFF00;
		m_bmpHeader.bV5AlphaMask		= 0x00FF;
		break;
	case COLORTYPE_RGB_565:			// R5G6B5
		m_bmpHeader.bV5BitCount			= 16;
		m_bmpHeader.bV5RedMask			= 0xF800;
		m_bmpHeader.bV5GreenMask		= 0x07E0;
		m_bmpHeader.bV5BlueMask			= 0x001F;
		break;
	case COLORTYPE_RGBA_4444:		// R4G4B4A4
		m_bmpHeader.bV5BitCount			= 16;
		m_bmpHeader.bV5RedMask			= 0xF000;
		m_bmpHeader.bV5GreenMask		= 0x0F00;
		m_bmpHeader.bV5BlueMask			= 0x00F0;
		m_bmpHeader.bV5AlphaMask		= 0x000F;
		break;
	case COLORTYPE_RGBA_5551:		// R5G5B5A1
		m_bmpHeader.bV5BitCount			= 16;
		m_bmpHeader.bV5RedMask			= 0xF800;
		m_bmpHeader.bV5GreenMask		= 0x07C0;
		m_bmpHeader.bV5BlueMask			= 0x003E;
		m_bmpHeader.bV5AlphaMask		= 0x0001;
		break;
	case COLORTYPE_RGB_888:
		m_bmpHeader.bV5BitCount			= 24;
		m_bmpHeader.bV5Compression		= BI_RGB;
		m_bmpHeader.bV5Size				= sizeof(BITMAPINFOHEADER);
		break;
	case COLORTYPE_ABGR_8888:		// A8B8G8R8
		m_bmpHeader.bV5BitCount			= 32;
		m_bmpHeader.bV5RedMask			= 0x000000FF;
		m_bmpHeader.bV5GreenMask		= 0x0000FF00;
		m_bmpHeader.bV5BlueMask			= 0x00FF0000;
		m_bmpHeader.bV5AlphaMask		= 0xFF000000;
		break;
	case COLORTYPE_RGBA_8888:		// R8G8B8A8
		m_bmpHeader.bV5BitCount			= 32;
		m_bmpHeader.bV5RedMask			= 0xFF000000;
		m_bmpHeader.bV5GreenMask		= 0x00FF0000;
		m_bmpHeader.bV5BlueMask			= 0x0000FF00;
		m_bmpHeader.bV5AlphaMask		= 0x000000FF;
		break;
	case COLORTYPE_ARGB_8888:		// A8R8G8B8
		// default 32 bit color
		m_bmpHeader.bV5BitCount			= 32;
		//m_bmpHeader.bV5Compression		= BI_RGB;
		//m_bmpHeader.bV5Size				= sizeof(BITMAPINFOHEADER);
		m_bmpHeader.bV5RedMask			= 0x00FF0000;
		m_bmpHeader.bV5GreenMask		= 0x0000FF00;
		m_bmpHeader.bV5BlueMask			= 0x000000FF;
		m_bmpHeader.bV5AlphaMask		= 0xFF000000;
		break;
	default:
		return FALSE;
	}

	m_bmpHeader.bV5SizeImage	= ((dwWidth*m_bmpHeader.bV5BitCount+31)/32 * 4)*dwHeight;

	{
		HDC	hdc	= ::GetDC(NULL);
		if(!hdc) return FALSE;
		m_memDC	= CreateCompatibleDC(hdc);
		SetStretchBltMode(m_memDC, HALFTONE);
		ReleaseDC(NULL, hdc);
	}

	m_hbmScreen = CreateDIBSection(m_memDC, (BITMAPINFO*)&m_bmpHeader, DIB_RGB_COLORS, &m_lpDIBits, NULL, 0);
	m_hbmScreen	= (HBITMAP)SelectObject(m_memDC, (HGDIOBJ)m_hbmScreen);

	SetPenColor(255, 255, 255);
	SetBrushSolidColor(255, 255, 255);

	m_pGraphics	= new Graphics(m_memDC);
	//m_pGraphics->SetSmoothingMode(SmoothingModeHighQuality);

	if(!m_lpDIBits) return FALSE;
	
	return TRUE;
}

BOOL CBuffer::CreateFromFile(LPCTSTR sFilename, COLORFORMAT Type){
	CImage Image;
	if(FAILED(Image.Load(sFilename))) return FALSE;
	if(!Create(Image.GetWidth(), Image.GetHeight(), Type)) return FALSE;
	Image.BitBlt(m_memDC,0,0,SRCCOPY);
	InvalidateLayout();
	return TRUE;
}

static BOOL __LoadImageFromResource (CImage& img, LPCTSTR pName, LPCTSTR pType, HMODULE hInst = NULL) 
{          
	IStream* pStream = NULL;
	HRSRC hResource = ::FindResource (hInst, pName, pType); if (!hResource) return FALSE;
	DWORD nImageSize = ::SizeofResource (hInst, hResource); if (!nImageSize) return FALSE;

	HGLOBAL m_hBuffer = ::GlobalAlloc (GMEM_MOVEABLE, nImageSize); 

	BYTE* pBytes = (BYTE*)::LoadResource (hInst, hResource); 

	if (m_hBuffer) 
	{
		void* pSource = ::LockResource (::LoadResource (hInst, hResource)); 
		if (!pSource) return FALSE; 

		void* pDest = ::GlobalLock (m_hBuffer); 

		if (pDest) 
		{ 
			CopyMemory (pDest, pSource, nImageSize); 
			if (::CreateStreamOnHGlobal (m_hBuffer, FALSE, &pStream) == S_OK) 
			{ 
				img.Load (pStream); // 여기만 수정 하면 
				pStream->Release (); 
			} 
			::GlobalUnlock (m_hBuffer); 
		} 
		::GlobalFree (m_hBuffer); 
		m_hBuffer = NULL; 
	} 

	if (img.IsNull ()) return FALSE; 
	return TRUE; 
} 


BOOL CBuffer::CreateFromResource(UINT nIDResource, COLORFORMAT ColorType, LPCTSTR sImgType, HINSTANCE hInstance){
	CImage Image;
	__LoadImageFromResource(Image, (LPCTSTR)MAKEINTRESOURCE(nIDResource), sImgType, hInstance);
	if(Image.IsNull()) return FALSE;
	if(!Create(Image.GetWidth(), Image.GetHeight(), ColorType)) return FALSE;
	Image.BitBlt(m_memDC,0,0,SRCCOPY);
	InvalidateLayout();
	return TRUE;
}

BOOL CBuffer::IsInitialize(void){
	return (m_lpDIBits!=0);
}

BOOL CBuffer::IsExistAlphaChanel(void){
	if(!m_lpDIBits) return FALSE;
	return (m_bmpHeader.bV5AlphaMask != 0);
}

void CBuffer::ReleaseAll(void){
	SAFE_DELETE(m_pGraphics);
	SAFE_DELETE(m_pPen);
	SAFE_DELETE(m_pBrush);
	if(m_memDC){
		if(m_hbmScreen){
			m_hbmScreen	= (HBITMAP)SelectObject(m_memDC, (HGDIOBJ)m_hbmScreen);
			DeleteObject((HGDIOBJ)m_hbmScreen);
		}
		DeleteDC(m_memDC);
		m_memDC		= NULL;
	}

	m_dwWidth		= 0;
	m_dwHeight		= 0;

	m_lpDIBits		= NULL;
	m_bmpRect.SetRect(0,0,0,0);
}

BOOL CBuffer::Compare(ITDBuffer* pBuffer){
	CBuffer* pBuff = (CBuffer*)pBuffer;
	if(!pBuff) return FALSE;
	if(!pBuff->IsInitialize() || !IsInitialize()){
		g_pTestDrive->LogError(_T("초기화되지 않은 버퍼는 비교하지 못합니다."));
		return FALSE;
	}
	if(pBuff->Width() != m_dwWidth || pBuff->Height() != m_dwHeight || pBuff->ColorFormat() != m_ColorFormat){
		g_pTestDrive->LogError(_T("비교할 버퍼 형식이 서로 다릅니다."));
		return FALSE;
	}
	{
		DWORD x, y;
		PBYTE pA = (PBYTE)pBuff->GetPointer();
		PBYTE pB = (PBYTE)m_lpDIBits;

		switch(m_bmpHeader.bV5BitCount){
		case 8:
			for(y=0;y<m_dwHeight;y++){
				for(x=0;x<m_dwWidth;x++) if(pA[x] != pB[x]) return FALSE;
				pA += m_dwWidth;
				pB += m_dwWidth;
			}break;
		case 16:
			for(y=0;y<m_dwHeight;y++){
				for(x=0;x<m_dwWidth;x++) if(((WORD*)pA)[x] != ((WORD*)pB)[x]) return FALSE;
				pA += m_dwWidth*2;
				pB += m_dwWidth*2;
			}break;
		case 32:
			for(y=0;y<m_dwHeight;y++){
				for(x=0;x<m_dwWidth;x++) if(((DWORD*)pA)[x] != ((DWORD*)pB)[x]) return FALSE;
				pA += m_dwWidth*4;
				pB += m_dwWidth*4;
			}break;
		}
	}
	return TRUE;
}

BOOL CBuffer::Present(CDC* pDC, CRect* pRcDraw, CRect* pRcClip){
	if(!IsInitialize() || !pDC || !pRcDraw) return FALSE;
	int prevBltMode = pDC->SetStretchBltMode(m_bAntialiasing|(pRcDraw->Width() < (int)m_dwWidth) ? HALFTONE : COLORONCOLOR);

	HDC dc = pDC->GetSafeHdc();

	{
		int x		= 0,
			y		= 0,
			width	= (int)m_dwWidth,
			height	= (int)m_dwHeight;
		int dx		= pRcDraw->left;
		int dy		= pRcDraw->top;
		int dwidth	= pRcDraw->Width();
		int dheight	= pRcDraw->Height();
	
		if(pRcClip){
			{	// X 보정
				float	scale		= (float)dwidth / m_dwWidth;

				if(dx < pRcClip->left){
					int ex			= dx + dwidth;
					int gap			= pRcClip->left - dx;
				
					x				= (int)(gap / scale);
					dx				+= (int)(x * scale);
					dwidth			= ex - dx;
					width			-= x;
				}
				if(dx + dwidth > pRcClip->right){
					int	gap		= (int)((dx + dwidth - pRcClip->right) / scale);
					width		-= gap;
					dwidth		-= (int)(gap*scale);
				}
			}
			{	// Y 보정
				float	scale		= (float)dheight / m_dwHeight;

				if(dy < pRcClip->top){
					int ey			= dy + dheight;
					int gap			= (int)((pRcClip->top - dy) / scale);
					
					dy				+= (int)(gap * scale);
					dheight			= ey - dy;
					height			-= gap;

				}
				if(dy + dheight > pRcClip->bottom){
 					y			= (int)((dy + dheight - pRcClip->bottom) / scale);
					height		-= y;
					dheight		-= (int)(y*scale);
				}
			}
			
		}

		if(pRcClip){
			{	// 이미지 이외 영역 지우기
				CBrush	back_color(m_crBackColor);
				// 왼쪽
				if(dx > 0){
					CRect	rect(0,0,dx,pRcClip->bottom);
					pDC->FillRect(rect, &back_color);
					if(m_bOutline){
						pDC->MoveTo(dx-1,0);
						pDC->LineTo(dx-1,pRcClip->bottom);
					}
				}
				// 오른쪽
				if(dx+dwidth < pRcClip->right){
					CRect	rect(dx+dwidth,0,pRcClip->right,pRcClip->bottom);
					pDC->FillRect(rect, &back_color);
					if(m_bOutline){
						pDC->MoveTo(dx+dwidth,0);
						pDC->LineTo(dx+dwidth,pRcClip->bottom);
					}
				}
				// 위
				if(dy > 0){
					CRect	rect(dx,0,dx+dwidth,dy);
					pDC->FillRect(rect, &back_color);
					if(m_bOutline){
						pDC->MoveTo(0,dy-1);
						pDC->LineTo(pRcClip->right,dy-1);
					}
				}
				// 아래
				if(dy+dheight < pRcClip->bottom){
					CRect	rect(dx,dy+dheight,dx+dwidth,pRcClip->bottom);
					pDC->FillRect(rect, &back_color);
					if(m_bOutline){
						pDC->MoveTo(0, dy+dheight);
						pDC->LineTo(pRcClip->right,dy+dheight);
					}
				}
			}
		}

		::StretchDIBits(dc,
						dx,dy,dwidth,dheight,		// destination buffer
						x,y,width, height,			// source image
						m_lpDIBits,(BITMAPINFO*)&m_bmpHeader, DIB_RGB_COLORS, SRCCOPY);
		//m_pGraphics->DrawImage()	를 사용해야 함.
		/*{
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			::AlphaBlend(dc,
				dx,dy,dwidth,dheight,
				m_memDC,x,y,width,dwidth,bf);
		}*/
	}
	
	pDC->SetStretchBltMode(prevBltMode);
	return TRUE;
}

void CBuffer::Present(BOOL bImmediate){
	// NULL implementation
}

BOOL CBuffer::LoadImage(LPCTSTR sFilename){
	if(!IsInitialize()) return FALSE;

	CImage Image;
	if(FAILED(Image.Load(sFilename))){
		OnBeforeLoadImage(sFilename, 0, 0);
		return FALSE;
	}
	//Image.GetWidth()
	if(!OnBeforeLoadImage(sFilename, Image.GetWidth(), Image.GetHeight())) return FALSE;
	Image.BitBlt(m_memDC,0,0,SRCCOPY);

	/*
	// 이미지 DC를 생성한다.
	CDC* pDC = CDC::FromHandler(Image.GetDC());
	// 이 이미지 DC에 문자열을 출력한다.
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut(200, 30, "test");
	Image.ReleaseDC();

	Image.BitBlt(dc.m_hDC, 0, 0);
	*/
	
	return TRUE;
}

BOOL CBuffer::OnBeforeLoadImage(LPCTSTR sFilename, DWORD dwWidth, DWORD dwHeight){
	return TRUE;
}

void CBuffer::InvalidateLayout(void){
}

BOOL CBuffer::LoadFromResource(UINT nIDResource){
	if(!IsInitialize()) return FALSE;

	CImage Image;
	Image.LoadFromResource(AfxGetInstanceHandle(), nIDResource);
	if(Image.IsNull()) return FALSE;
	if(!OnBeforeLoadImage(NULL, Image.GetWidth(), Image.GetHeight())) return FALSE;
	

	Image.BitBlt(m_memDC,0,0,SRCCOPY);

	return TRUE;
}

BOOL CBuffer::SaveToFile(LPCTSTR sFilename, IMAGETYPE Type, BOOL bStoreAlpha){
	if(!IsInitialize()) return FALSE;

	CImage Image;
	{	// Create Image
		int iBPP = 0;
		DWORD dwFlags = 0;
		if(m_bmpHeader.bV5BitCount<16) iBPP = 32;
		else iBPP = m_bmpHeader.bV5BitCount;
		if(m_bmpHeader.bV5AlphaMask && bStoreAlpha) dwFlags = CImage::createAlphaChannel;
		if(!Image.Create(m_dwWidth, m_dwHeight, iBPP, dwFlags)) return FALSE;
	}
	{	// copy image
		HDC dc = Image.GetDC();
		BitBlt(dc, 0,0, m_dwWidth, m_dwHeight, m_memDC, 0, 0, SRCCOPY);
		Image.ReleaseDC();
	}
	
	{	// save
		const GUID* pGUID = NULL;
RETRY_TYPE_CHECK:
		switch(Type){
		default:
		case IMAGETYPE_PNG:
			pGUID = &(Gdiplus::ImageFormatPNG);		break;
		case IMAGETYPE_JPG:
			pGUID = &(Gdiplus::ImageFormatJPEG);	break;
		case IMAGETYPE_TIFF:
			pGUID = &(Gdiplus::ImageFormatTIFF);	break;
		case IMAGETYPE_BMP:
			pGUID = &(Gdiplus::ImageFormatBMP);		break;
		case IMAGETYPE_GIF:
			pGUID = &(Gdiplus::ImageFormatGIF);		break;
		case IMAGETYPE_AUTO:
			{
				CString ext;
				{	// get file part
					TCHAR	Path[1024];
					LPTSTR	pFilePart	= NULL;
					GetFullPathName(sFilename, 1024, Path, &pFilePart);
					if(!pFilePart) return FALSE;
					ext = pFilePart;
				}
				{	// get file extension
					while(1){
						//str = str.GetBuffer()[pos+1];
						int pos = ext.Find(_T('.'));
						if(pos<0) break;
						{
							CString temp = &((ext.GetBuffer())[pos+1]);
							ext = temp;
						}
					}
					ext.MakeLower();
				}
				Type	= IMAGETYPE_PNG;

				if(!ext.Compare(_T("png"))) Type	= IMAGETYPE_PNG;
				else
				if(!ext.Compare(_T("jpg"))) Type	= IMAGETYPE_JPG;
				else
				if(!ext.Compare(_T("tif"))) Type	= IMAGETYPE_TIFF;
				else
				if(!ext.Compare(_T("bmp"))) Type	= IMAGETYPE_BMP;
				else
				if(!ext.Compare(_T("gif"))) Type	= IMAGETYPE_GIF;
			}
			goto RETRY_TYPE_CHECK;
		}
		if(FAILED(Image.Save(sFilename, *pGUID))) return FALSE;
	}
	return TRUE;
}

BOOL CBuffer::CopyToMemory(BYTE* pMEM, DWORD dwByteStride){
	if(!IsInitialize()) return FALSE;
	{
		DWORD dwLineSize	= GetBytesStride();
		BYTE* pSrc			= ((BYTE*)m_lpDIBits);
		if(!dwByteStride) dwByteStride	= dwLineSize;
		else if(dwByteStride < dwLineSize) dwByteStride	= dwLineSize;
		pMEM				+= dwByteStride*m_dwHeight;
		for(DWORD i=0;i<m_dwHeight;i++){
			pMEM			-= dwByteStride;
			memcpy(pMEM, pSrc, dwLineSize);
			pSrc			+= dwLineSize;
		}
	}

	return TRUE;
}

BOOL CBuffer::CopyFromMemory(const BYTE* pMEM, DWORD dwByteStride, BOOL bReverse){
	if(!IsInitialize()) return FALSE;
	{
		DWORD dwLineSize	= GetBytesStride();
		BYTE* pDst			= ((BYTE*)m_lpDIBits);
		if(!dwByteStride) dwByteStride	= dwLineSize;
		else if(dwByteStride < dwLineSize) dwByteStride	= dwLineSize;

		if(bReverse){
			for(DWORD i=0;i<m_dwHeight;i++){
				memcpy(pDst, pMEM, dwLineSize);
				pMEM			+= dwByteStride;
				pDst			+= dwLineSize;
			}
		}else{
			pMEM				+= dwByteStride*m_dwHeight;
			for(DWORD i=0;i<m_dwHeight;i++){
				pMEM			-= dwByteStride;
				memcpy(pDst, pMEM, dwLineSize);
				pDst			+= dwLineSize;
			}
		}
	}

	return TRUE;
}

BOOL CBuffer::CopyFromBuffer(ITDBuffer* pBuffer, int x, int y, int sx, int sy, int width, int height, int dwidth, int dheight){
	if(sx < 0 || sy < 0 || !IsInitialize() || !pBuffer) return FALSE;
	if(!pBuffer->IsInitialize()) return FALSE;
	if(width<=0)	width = pBuffer->Width() - sx;
	if(height<=0)	height = pBuffer->Height() - sy;

	if(dwidth && dheight)
		StretchBlt(m_memDC, x, y, dwidth, dheight, pBuffer->GetDC(), sx, sy, width, height, SRCCOPY);
	else
		BitBlt(m_memDC, x, y, width, height, pBuffer->GetDC(), sx, sy, SRCCOPY);

	return TRUE;
}

LPVOID CBuffer::GetPointer(void){
	return m_lpDIBits;
}

COLORFORMAT CBuffer::ColorFormat(void){
	return m_ColorFormat;
}

DWORD CBuffer::GetBytesStride(void){
	return ((m_bmpHeader.bV5BitCount/8) * m_dwWidth);
}

DWORD CBuffer::Width(void){
	return m_dwWidth;
}

DWORD CBuffer::Height(void){
	return m_dwHeight;
}

DWORD CBuffer::ColorBitCount(void){
	return m_bmpHeader.bV5BitCount;
}

void CBuffer::GetRect(RECT* pRect){
	if(!pRect) return;
	(*((CRect*)pRect)) = m_bmpRect;
}

void CBuffer::EnableAntialiasing(BOOL bEnable){
	m_bAntialiasing = bEnable;
}

BOOL CBuffer::IsAntialiasing(void){
	return m_bAntialiasing;
}

BOOL CBuffer::OpenDialog(void){
	// null implementation
	return FALSE;
}

BOOL CBuffer::SaveDialog(void){
	// null implementation
	return FALSE;
}

BOOL CBuffer::SetViewLink(ITDBuffer* pBuffer){
	// null implementation
	return FALSE;
}

void CBuffer::SetEnable(BUFFER_INTERFACE id, BOOL bEnable){
	// null implementation
}

void CBuffer::GetDrawRect(RECT* pRect){
	// null implementation
}

void CBuffer::SetDrawRect(const RECT* pRect){
	// null implementation
}

ITDObject* CBuffer::GetObject(void){
	return NULL;
}

BOOL CBuffer::CheckPSNR(ITDBuffer* pCompare, double &dPSNR, BOOL &bSame, BOOL bCompareAlpha){
	LONGLONG	mse_sum		= 0;

	if(!pCompare || !IsInitialize()) return FALSE;
	// 해상도와 컬러포멧이 같지 않으면 실패
	if(pCompare->Width() != m_dwWidth || pCompare->Height() != m_dwHeight || pCompare->ColorFormat() != m_ColorFormat) return FALSE;
	// ABGR_8888/RGBA_8888/ARGB_8888 의 8비트 해상도만 지원한다.
	if(m_ColorFormat < COLORTYPE_ABGR_8888) return FALSE;

	BYTE* pOrg			= (BYTE*)pCompare->GetPointer();	// source image
	BYTE* pCmp			= (BYTE*)GetPointer();				// this image
	DWORD color_byte	= m_bmpHeader.bV5BitCount/8;		// color bytes
	DWORD cmp_size		= color_byte;						// compare color count
	DWORD pixel_count	= m_dwWidth * m_dwHeight;			// total pixel count

	if(!bCompareAlpha){	// 알파 비교를 제외한다.
		cmp_size--;
		if(m_ColorFormat == COLORTYPE_RGBA_8888){
			pOrg++;
			pCmp++;
		}
	}

	{	// mse 계산
		DWORD	i,t;
		int		differ;
		for(i=0;i<pixel_count;i++){
			for(t=0;t<cmp_size;t++){
				differ	= (int)pOrg[t] - (int)pCmp[t];
				mse_sum	+= (differ * differ);
			}
			pOrg	+= color_byte;
			pCmp	+= color_byte;
		}
	}
	if(mse_sum == 0){
		dPSNR	= 0;
		bSame	= TRUE;
	}else{
		double	mse	= (double)mse_sum / (pixel_count * cmp_size);
		dPSNR	= 10 * log10((double)(255*255) / mse );
		bSame	= FALSE;
	}
	return TRUE;
}

CString CBuffer::GetDescription(void){
	CString str = _T("None");
	if(IsInitialize()){
		int index;
		for(index=0;index<VIDEO_SIZE;index++){
			if(g_iVideoDimension[index][0]==m_dwWidth && g_iVideoDimension[index][1]==m_dwHeight)
				break;
		}

		str.Format(_T("%s(%dx%dx%dbits:%s)"), (index<VIDEO_SIZE) ? g_sVideoDimension[index] : _T(""),
			m_dwWidth, m_dwHeight, m_bmpHeader.bV5BitCount, g_sColorTypes[m_ColorFormat]);
	}
	return str;
}

HDC CBuffer::GetDC(void){
	return m_memDC;
}

void CBuffer::SetPenColor(BYTE r, BYTE g, BYTE b, BYTE a){
	if(!m_pPen) m_pPen = new Pen(Color(a,r,g,b));
	else m_pPen->SetColor(Color(a, r, g, b));
}

void CBuffer::SetBrushSolidColor(BYTE r, BYTE g, BYTE b, BYTE a){
	SAFE_DELETE(m_pBrush);
	m_pBrush = new SolidBrush(Color(a, r, g, b));
}

void CBuffer::SetPenDashStyle(ITDDashStyle style){
	if(m_pPen) m_pPen->SetDashStyle((DashStyle)style);
}

void CBuffer::SetPenDashOffset(float offset){
	if(m_pPen) m_pPen->SetDashOffset(offset);
}

void CBuffer::SetPenDashCap(ITDDashCap cap){
	if(m_pPen) m_pPen->SetDashCap((DashCap)cap);
}
void CBuffer::SetPenWidth(float width){
	if(m_pPen) m_pPen->SetWidth(width);
}

void CBuffer::SetSmoothModeHighQuality(BOOL bEnable){
	if(m_pGraphics) m_pGraphics->SetSmoothingMode(bEnable ? SmoothingModeHighQuality : SmoothingModeNone);
}

void CBuffer::UseAlphaChannel(BOOL bUse){
	if(m_pGraphics) m_pGraphics->SetCompositingMode(bUse ? CompositingModeSourceOver : CompositingModeSourceCopy);
}

void CBuffer::DrawLine(float x, float y, float ex, float ey){
	if(m_pGraphics) m_pGraphics->DrawLine(m_pPen, x, y, ex, ey);
}

void CBuffer::DrawLine(ITDPoint* pStart, ITDPoint* pEnd){
	if(m_pGraphics) m_pGraphics->DrawLine(m_pPen, *(const Point*)pStart, *(const Point*)pEnd);
}

void CBuffer::DrawLinef(ITDPointf* pStart, ITDPointf* pEnd){
	if(m_pGraphics) m_pGraphics->DrawLine(m_pPen, *(const PointF*)pStart, *(const PointF*)pEnd);
}

void CBuffer::DrawLines(ITDPoint* pPoints, DWORD dwCount){
	if(m_pGraphics) m_pGraphics->DrawLines(m_pPen, (const Point*)pPoints, dwCount);
}

void CBuffer::DrawLinesf(ITDPointf* pPoints, DWORD dwCount){
	if(m_pGraphics) m_pGraphics->DrawLines(m_pPen, (const PointF*)pPoints, dwCount);
}

void CBuffer::DrawRectangle(float x, float y, float width, float height){
	if(m_pGraphics) m_pGraphics->DrawRectangle(m_pPen, x, y, width, height);
}

void CBuffer::FillRectangle(float x, float y, float width, float height){
	if(m_pGraphics) m_pGraphics->FillRectangle(m_pBrush, x, y, width, height);
}

void CBuffer::DrawEllipse(float x, float y, float width, float height){
	if(m_pGraphics) m_pGraphics->DrawEllipse(m_pPen, x, y, width, height);
}

void CBuffer::FillEllipse(float x, float y, float width, float height){
	if(m_pGraphics) m_pGraphics->FillEllipse(m_pBrush, x, y, width, height);
}

void CBuffer::DrawBezier(ITDPoint* pPt1, ITDPoint* pPt2, ITDPoint* pPt3, ITDPoint* pPt4){
	if(m_pGraphics) m_pGraphics->DrawBezier(m_pPen, *(const Point*)pPt1, *(const Point*)pPt2, *(const Point*)pPt3, *(const Point*)pPt4);
}

void CBuffer::DrawBezierf(ITDPointf* pPt1, ITDPointf* pPt2, ITDPointf* pPt3, ITDPointf* pPt4){
	if(m_pGraphics) m_pGraphics->DrawBezier(m_pPen, *(const PointF*)pPt1, *(const PointF*)pPt2, *(const PointF*)pPt3, *(const PointF*)pPt4);
}

void CBuffer::DrawBeziers(ITDPoint* pPoints, DWORD dwCount){
	if(m_pGraphics) m_pGraphics->DrawBeziers(m_pPen, (const Point*)pPoints, dwCount);
}

void CBuffer::DrawBeziersf(ITDPointf* pPoints, DWORD dwCount){
	if(m_pGraphics) m_pGraphics->DrawBeziers(m_pPen, (const PointF*)pPoints, dwCount);
}

void CBuffer::DrawPolygon(ITDPoint* pPoints, DWORD dwCount){
	if(m_pGraphics) m_pGraphics->DrawPolygon(m_pPen, (const Point*)pPoints, dwCount);
}

void CBuffer::DrawPolygonf(ITDPointf* pPoints, DWORD dwCount){
	if(m_pGraphics) m_pGraphics->DrawPolygon(m_pPen, (const PointF*)pPoints, dwCount);
}

void CBuffer::FillPolygon(ITDPoint* pPoints, DWORD dwCount){
	if(m_pGraphics) m_pGraphics->FillPolygon(m_pBrush, (const Point*)pPoints, dwCount);
}

void CBuffer::FillPolygonf(ITDPointf* pPoints, DWORD dwCount){
	if(m_pGraphics) m_pGraphics->FillPolygon(m_pBrush, (const PointF*)pPoints, dwCount);
}

BITMAPV5HEADER* CBuffer::GetHeader(void){
	return &m_bmpHeader;
}

void CBuffer::SetManager(ITDBufferManager* pManager, DWORD dwID){
	// null
}