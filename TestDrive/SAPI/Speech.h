#ifndef __SPEECH_H_
#define __SPEECH_H_
#pragma once
//#include <sapi.h>		// already include "stdafx.h"

#define WM_SPEACH		(WM_USER+1014)

class CSpeech
{
public:
	CSpeech(void);
	virtual ~CSpeech(void);

	static BOOL Initialize(void);
	static void Relase(void);

	void Speak(LPCTSTR str, DWORD flags = SPF_DEFAULT);
	void SetNotifyEvent(HWND hWnd);
	void OnEvent(void);

	BOOL CheckSpeechEnable(void);
#define CheckMe		if(!CheckSpeechEnable()) return;

private:
	static ISpVoice*	m_pVoice;
};

#endif // __SPEECH_H_
