#include "StdAfx.h"
#include "AccelCodeDecoder.h"

AccelCodeDecoder*	AccelCodeDecoder::m_pCurrentCopynPasteAction = NULL;

AccelCodeDecoder::AccelCodeDecoder(void)
{
}

AccelCodeDecoder::~AccelCodeDecoder(void)
{
	SetCurrentCopynPasteAction(FALSE);
}

void AccelCodeDecoder::DoAccel(ACCEL_CODE code){
	if(m_pCurrentCopynPasteAction) m_pCurrentCopynPasteAction->OnAccel(code);
}

void AccelCodeDecoder::SetCurrentCopynPasteAction(BOOL bSet){
	if(bSet) m_pCurrentCopynPasteAction = this;
	else
	if(m_pCurrentCopynPasteAction == this) m_pCurrentCopynPasteAction = NULL;
}
