//
//  net_addr.h
//  ip信息
//  Created by DGuco on 18-1-7.
//
//

#ifndef _NET_ADDR_H_
#define _NET_ADDR_H_

#include <cstring>
#include "net_inc.h"
#include "base.h"

class CNetAddr
{
public:
	CNetAddr(void)
		: m_uPort(0)
	{
		memset(m_szAddr, 0, sizeof(m_szAddr));
	}

	~CNetAddr(void)
	{
	}

	CNetAddr(const char *szAddr, unsigned int uPort)
		: m_uPort(uPort)
	{
		SetAddress(szAddr);
	}

	void SetAddress(const char *szAddr)
	{
		strncpy(m_szAddr, szAddr, 16);
	}

	void Copy(const CNetAddr &addr)
	{
		SetAddress(addr.m_szAddr);
		SetPort(addr.m_uPort);
	}

	const char *GetAddress(void) const
	{
		return m_szAddr;
	}

	void SetPort(unsigned int uPort)
	{
		m_uPort = uPort;
	}

	unsigned int GetPort(void) const
	{
		return m_uPort;
	}

private:
	char m_szAddr[16];
	unsigned int m_uPort;
};

#endif
