#pragma once
#include "SocketHandle.h"

class CSocketCom
{
public:
	CSocketCom(void);
	~CSocketCom(void);

protected:
	CSocketHandle	m_hCom;
};
