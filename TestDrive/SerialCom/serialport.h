#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__
#include "TestDrive.h"

void AfxThrowSerialException(DWORD dwError = 0);

class CSerialException : public CException
{
public:
	//Constructors / Destructors
	CSerialException(DWORD dwError);
	~CSerialException();

//Methods
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL GetErrorMessage(LPTSTR lpstrError, UINT nMaxError,	PUINT pnHelpContext = NULL);
	CString GetErrorMessage();

	//Data members
	DWORD m_dwError;

protected:
	DECLARE_DYNAMIC(CSerialException)
};


class CSerialPort : public ITDSerialCom,
					public CObject
{
public:
	CSerialPort();
	virtual ~CSerialPort();
 
	//General Methods
	STDMETHOD_(void, Release)(void);
	STDMETHOD_(BOOL, Open)(int nPort, DWORD dwBaud = 115200, Parity parity = NoParity, BYTE DataBits = 8, StopBits stopbits = OneStopBit, FlowControl fc = NoFlowControl, BOOL bOverlapped = FALSE);
	STDMETHOD_(void, Close)(void);
	STDMETHOD_(BOOL, IsOpen)(void);
	STDMETHOD_(DWORD, Read)(void* lpBuf, DWORD dwCount);
	STDMETHOD_(BOOL, Read)(void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped);
	STDMETHOD_(DWORD, Write)(const void* lpBuf, DWORD dwCount);
	STDMETHOD_(BOOL, Write)(const void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped);

	operator HANDLE() const { return m_hComm; };

#ifdef _DEBUG
  void CSerialPort::Dump(CDumpContext& dc) const;
#endif

	//Reading / Writing Methods
	void ReadEx(void* lpBuf, DWORD dwCount);
	void WriteEx(const void* lpBuf, DWORD dwCount);
	void TransmitChar(char cChar);
	void GetOverlappedResult(OVERLAPPED& overlapped, DWORD& dwBytesTransferred, BOOL bWait);
	void CancelIo(void);

	//Configuration Methods
	void GetConfig(COMMCONFIG& config);
	static void GetDefaultConfig(int nPort, COMMCONFIG& config);
	void SetConfig(COMMCONFIG& Config);
	static void SetDefaultConfig(int nPort, COMMCONFIG& config);

	//Misc RS232 Methods
	void ClearBreak();
	void SetBreak();
	void ClearError(DWORD& dwErrors);
	void GetStatus(COMSTAT& stat);
	void GetState(DCB& dcb);
	void SetState(DCB& dcb);
	void Escape(DWORD dwFunc);
	void ClearDTR();
	void ClearRTS();
	void SetDTR();
	void SetRTS();
	void SetXOFF();
	void SetXON();
	void GetProperties(COMMPROP& properties);
	void GetModemStatus(DWORD& dwModemStatus); 

	//Timeouts
	void SetTimeouts(COMMTIMEOUTS& timeouts);
	void GetTimeouts(COMMTIMEOUTS& timeouts);
	void Set0Timeout();
	void Set0WriteTimeout();
	void Set0ReadTimeout();

	//Event Methods
	void SetMask(DWORD dwMask);
	void GetMask(DWORD& dwMask);
	void WaitEvent(DWORD& dwMask);
	void WaitEvent(DWORD& dwMask, OVERLAPPED& overlapped);

	//Queue Methods
	void Flush();
	void Purge(DWORD dwFlags);
	void TerminateOutstandingWrites();
	void TerminateOutstandingReads();
	void ClearWriteBuffer();
	void ClearReadBuffer();
	void Setup(DWORD dwInQueue, DWORD dwOutQueue);

	//Overridables
	virtual void OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped);

protected:
	HANDLE m_hComm;       //Handle to the comms port
	BOOL   m_bOverlapped; //Is the port open in overlapped IO

	static void WINAPI _OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped); 

	DECLARE_DYNAMIC(CSerialPort)
};

#endif //__SERIALPORT_H__