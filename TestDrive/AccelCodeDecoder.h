#pragma once

typedef enum{
	ACCEL_CODE_COPY,
	ACCEL_CODE_PASTE,
	ACCEL_CODE_CUT,
	ACCEL_CODE_UNDO,
	ACCEL_CODE_SELECTALL,
	ACCEL_CODE_PRINT,
	ACCEL_CODE_SAVE,
	ACCEL_CODE_TOGGLE_FULLSCREEN,
}ACCEL_CODE;

class AccelCodeDecoder
{
	static AccelCodeDecoder*	m_pCurrentCopynPasteAction;
public:
	AccelCodeDecoder(void);
	~AccelCodeDecoder(void);

	static void DoAccel(ACCEL_CODE code);

protected:
	void SetCurrentCopynPasteAction(BOOL bSet = TRUE);

	virtual void OnAccel(ACCEL_CODE code) = 0;
};
