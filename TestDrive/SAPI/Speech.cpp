#include "stdafx.h"
#include "Speech.h"
#include <sphelper.h>

ISpVoice*	CSpeech::m_pVoice	= NULL;

CSpeech::CSpeech(void){
	//if (FAILED(CoInitialize(NULL)))
	//	return;
}

CSpeech::~CSpeech(void){
	//CoUninitialize();
}

BOOL CSpeech::Initialize(void){
	if (!SUCCEEDED(CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&m_pVoice))){
		m_pVoice = NULL;
		return FALSE;
	}
	return TRUE;
}
void CSpeech::Relase(void){
	if (m_pVoice){
		m_pVoice->Release();
		m_pVoice = NULL;
	}
}

BOOL CSpeech::CheckSpeechEnable(void){
	if(m_pVoice) return TRUE;
	return FALSE;
}

void CSpeech::Speak(LPCTSTR str, DWORD flags){
	CheckMe
	m_pVoice->Speak(str, flags, NULL);
}

void CSpeech::SetNotifyEvent(HWND hWnd){
	CheckMe
	ULONGLONG ullEventInterest = SPFEI(SPEI_RECOGNITION) | SPFEI(SPEI_END_SR_STREAM);
	m_pVoice->SetInterest(ullEventInterest, ullEventInterest);
	m_pVoice->SetNotifyWindowMessage(hWnd, WM_SPEACH, 0, 0);
}

void CSpeech::OnEvent(void){
	/*
	SPEVENT eventItem;
	memset( &eventItem, 0,sizeof(SPEVENT));
	while( m_pVoice->GetEvents(1, &eventItem, NULL ) == S_OK )
	{
		switch(eventItem.eEventId)
		{
		case SPEI_WORD_BOUNDARY:
			SPVOICESTATUS eventStatus;
			m_pVoice->GetStatus( &eventStatus, NULL );

			ULONG start, end;
			start = eventStatus.ulInputWordPos;
			end = eventStatus.ulInputWordLen;
			StringCchCopyN( tempString, theString + start , end );
			tempString[ end ] = '\0';

			g_pTestDrive->LogInfo("");
			break;

		default:
			break;
		}

		SpClearEvent( &eventItem );
	}*/
}