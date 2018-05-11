#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/time.h>

#include "base.h"

// 通过unix时间戳获取是当年的第几天
int GetDayOfYear(time_t tTime)
{
	struct tm tm;
	tm = *localtime(&tTime);
	return tm.tm_yday;
}

// 判断两个时间戳是否是同一天
bool IsSameDay(time_t tFirstTime, time_t tSecondTime)
{
	struct tm tmFirst;
	struct tm tmSecond;
	tmFirst = *localtime(&tFirstTime);
	tmSecond = *localtime(&tSecondTime);

	if (tmFirst.tm_year == tmSecond.tm_year
			&& tmFirst.tm_mon == tmSecond.tm_mon
			&& tmFirst.tm_mday == tmSecond.tm_mday)
		return true;

	return false;
}

// 获取当前秒数
int GetSecondTime()
{
	struct timeval tmval = {0};
	int nRetCode = gettimeofday(&tmval, NULL);
	if (nRetCode != 0)
	{
		return 0;
	}
	return (int)(tmval.tv_sec);
}

// 获取当前毫秒数
long GetMSTime()
{
	struct timeval tmval = {0};
	int nRetCode = gettimeofday(&tmval, NULL);
	if (nRetCode != 0)
	{
		return 0;
	}
	return (long)((tmval.tv_sec * 1000) + (tmval.tv_usec / 1000));
}


// 获取当前微秒
time_t GetUSTime()
{
	struct timeval tmval = {0};
	int nRetCode = gettimeofday(&tmval, NULL);
	if (nRetCode != 0)
	{
		return 0;
	}
	return (long)((tmval.tv_sec * 1000 * 1000) + tmval.tv_usec);
}


// 分割字符串，获取单词
// 拷贝字符串里面的单词（以空格回车换行等隔开）到pb里面
void TrimStr( char *strInput )
{
	char *pb;
	char *pe;
	int iTempLength;
	if( strInput == NULL )
	{
		return;
	}

	iTempLength = strlen(strInput);

	if( iTempLength == 0 )
	{
		return;
	}

	pb = strInput;

	while (((*pb == ' ') || (*pb == '\t') || (*pb == '\n') || (*pb == '\r')) && (*pb != 0))
	{
		pb ++;  // 当遇到控制字符的时候字符串指针++
	}

	pe = &strInput[iTempLength-1];  // pe指针指向strInput的最后一个字符

	while ((pe >= pb) && ((*pe == ' ') || (*pe == '\t') || (*pe == '\n') || (*pe == '\r')))
	{
		pe --;
	}
	*(pe+1) = '\0';

	strcpy( strInput, pb );

	return;

}

int SockAddrToString(sockaddr_in *pstSockAddr, char *szResult)
{
	char *pcTempIP = NULL;
	unsigned short nTempPort = 0;

	if( !pstSockAddr || !szResult )
	{
		return -1;
	}

	pcTempIP = inet_ntoa(pstSockAddr->sin_addr);

	if( !pcTempIP )
	{
		return -1;
	}

	nTempPort = ntohs(pstSockAddr->sin_port);
	//unsigned short nTempPort_hton = htons(pstSockAddr->sin_port);

	sprintf(szResult, "%s:%d", pcTempIP, nTempPort);
	//sprintf(szResult, "%s", pstSockAddr->sa_data);

	return 0;
}

int SockAddrToString(unsigned int ip, unsigned short port, char* szResult)
{
	if (!szResult)
	{
		return -1;
	}

	struct in_addr addr;
	addr.s_addr = (in_addr_t)ip;
	char* pcTempIP = inet_ntoa(addr);

	if (!pcTempIP)
	{
		return -1;
	}

	sprintf(szResult, "%s:%d", pcTempIP, port);

	return 0;
}


// 获取两段时间的间隔
int TimeValMinus(timeval& tvA, timeval& tvB, timeval& tvResult)
{
	timeval tvTmp;

	if (tvA.tv_usec < tvB.tv_usec)
	{
		tvTmp.tv_usec = (1000000 + tvA.tv_usec) - tvB.tv_usec;
		tvTmp.tv_sec = tvA.tv_sec - tvB.tv_sec - 1;
	}
	else
	{
		tvTmp.tv_usec = tvA.tv_usec - tvB.tv_usec;
		tvTmp.tv_sec = tvA.tv_sec - tvB.tv_sec;
	}

	tvResult.tv_sec = tvTmp.tv_sec;
	tvResult.tv_usec = tvTmp.tv_usec;

	return 0;
}


Tokens StrSplit(const std::string &src, const std::string &sep)
{
	Tokens r;
	std::string s;
	for (std::string::const_iterator i = src.begin(); i != src.end(); i++)
	{
		if (sep.find(*i) != std::string::npos)
		{
			if (s.length()) r.push_back(s);
			s = "";
		}
		else
		{
			s += *i;
		}
	}
	if (s.length()) r.push_back(s);
	return r;
}

