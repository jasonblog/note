#include "statistics.h"
#include <stdio.h>

CProcessMsgDelay::CProcessMsgDelay()
{
	m_nTotalMsgNum = 0;
	for (int i = 0; i < ETimeGrade_Num; i++)
	{
		m_aryStatistics[i] = 0;
	}
}

CProcessMsgDelay::~CProcessMsgDelay()
{

}

void CProcessMsgDelay::CountMsg(long tmDelay)
{
	m_nTotalMsgNum++;
	if (tmDelay <= 1)
	{
		// 小于1秒
		m_aryStatistics[ETimeGrade_Lv1]++;
	} 
	else if (tmDelay <= 2)
	{
		// 1秒 ~ 2秒
		m_aryStatistics[ETimeGrade_Lv2]++;
	}
	else if (tmDelay <= 3)
	{
		// 2秒 ~ 3秒
		m_aryStatistics[ETimeGrade_Lv3]++;
	}
	else if (tmDelay <= 5)
	{
		// 3秒 ~ 5秒
		m_aryStatistics[ETimeGrade_Lv4]++;
	}
	else if (tmDelay <= 10)
	{
		// 5秒 ~ 10秒
		m_aryStatistics[ETimeGrade_Lv5]++;
	}
	else if (tmDelay <= 30)
	{
		// 10秒 ~ 30秒
		m_aryStatistics[ETimeGrade_Lv6]++;
	}
	else if (tmDelay <= 60)
	{
		// 30秒 ~ 60秒
		m_aryStatistics[ETimeGrade_Lv7]++;
	}
	else
	{
		// 大于60秒
		m_aryStatistics[ETimeGrade_Lv8]++;
	}
}

void CProcessMsgDelay::PrintStatistics(char* pBuffer, unsigned int& unLen)
{
	unsigned int unMaxLen = unLen;
	unLen = 0;

	unLen += snprintf( pBuffer+unLen, unMaxLen-unLen, "--------------------------------------------------------\n");

	unLen += snprintf( pBuffer+unLen, unMaxLen-unLen, "%15s\t%15s\t%15s\t%15s\n", "grade", "count", "total", "percentage");

	unLen += snprintf( pBuffer+unLen, unMaxLen-unLen, "%15s\t%15lu\t%15lu\t%13.2f\n",
		"<= 1 seconds", m_aryStatistics[ETimeGrade_Lv1], m_nTotalMsgNum,
		m_aryStatistics[ETimeGrade_Lv1] / (double)m_nTotalMsgNum * 100);

	unLen += snprintf( pBuffer+unLen, unMaxLen-unLen, "%15s\t%15lu\t%15lu\t%13.2f\n",
		"<= 2 seconds", m_aryStatistics[ETimeGrade_Lv2], m_nTotalMsgNum,
		m_aryStatistics[ETimeGrade_Lv2] / (double)m_nTotalMsgNum * 100);

	unLen += snprintf( pBuffer+unLen, unMaxLen-unLen, "%15s\t%15lu\t%15lu\t%13.2f\n",
		"<= 3 seconds", m_aryStatistics[ETimeGrade_Lv3], m_nTotalMsgNum,
		m_aryStatistics[ETimeGrade_Lv3] / (double)m_nTotalMsgNum * 100);

	unLen += snprintf( pBuffer+unLen, unMaxLen-unLen, "%15s\t%15lu\t%15lu\t%13.2f\n",
		"<= 5 seconds", m_aryStatistics[ETimeGrade_Lv4], m_nTotalMsgNum,
		m_aryStatistics[ETimeGrade_Lv4] / (double)m_nTotalMsgNum * 100);

	unLen += snprintf( pBuffer+unLen, unMaxLen-unLen, "%15s\t%15lu\t%15lu\t%13.2f\n",
		"<= 10 seconds", m_aryStatistics[ETimeGrade_Lv5], m_nTotalMsgNum,
		m_aryStatistics[ETimeGrade_Lv5] / (double)m_nTotalMsgNum * 100);

	unLen += snprintf( pBuffer+unLen, unMaxLen-unLen, "%15s\t%15lu\t%15lu\t%13.2f\n",
		"<= 30 seconds", m_aryStatistics[ETimeGrade_Lv6], m_nTotalMsgNum,
		m_aryStatistics[ETimeGrade_Lv6] / (double)m_nTotalMsgNum * 100);

	unLen += snprintf( pBuffer+unLen, unMaxLen-unLen, "%15s\t%15lu\t%15lu\t%13.2f\n",
		"<= 60 seconds", m_aryStatistics[ETimeGrade_Lv7], m_nTotalMsgNum,
		m_aryStatistics[ETimeGrade_Lv7] / (double)m_nTotalMsgNum * 100);

	unLen += snprintf( pBuffer+unLen, unMaxLen-unLen, "%15s\t%15lu\t%15lu\t%13.2f\n",
		"> 60 seconds", m_aryStatistics[ETimeGrade_Lv8], m_nTotalMsgNum,
		m_aryStatistics[ETimeGrade_Lv8] / (double)m_nTotalMsgNum * 100);

}

