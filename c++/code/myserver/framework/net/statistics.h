#ifndef _STATISTICS_H_
#define _STATISTICS_H_

class CProcessMsgDelay
{
public:
	enum ETimeGrade
	{
		ETimeGrade_Lv1	= 0,	// 小于1秒钟
		ETimeGrade_Lv2,			// 1秒 ~ 2秒
		ETimeGrade_Lv3,			// 2秒 ~ 3秒
		ETimeGrade_Lv4,			// 3秒 ~ 5秒
		ETimeGrade_Lv5,			// 5秒 ~ 10秒
		ETimeGrade_Lv6,			// 10秒 ~ 30秒
		ETimeGrade_Lv7,			// 30秒 ~ 60秒
		ETimeGrade_Lv8,			// 大于60秒

		ETimeGrade_Num			// 总档数
	};

public:
	CProcessMsgDelay();
	~CProcessMsgDelay();

private:
	unsigned long m_nTotalMsgNum;
	unsigned long m_aryStatistics[ETimeGrade_Num];

public:
	void CountMsg(long tmDelay);
	void PrintStatistics(char* pBuffer, unsigned int& unLen);
};

#endif // _STATISTICS_H_


