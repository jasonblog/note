#include <stdio.h>

#include "performance.h"

CPerfInfo::CPerfInfo()
	: mPerfLog(1), mTotalCalled(0), mCostTime(0), mMostCostTime(0), mLestCostTime(1000)
{
	INIT_ROATING_LOG("Perf", "../log/perf.log", level_enum::info, 10 * 1024 * 1024, 20);
}

CPerfInfo::~CPerfInfo()
{
}

/// 输入这次调用经过的时间，返回被调用的次数
int CPerfInfo::AddCalled(int iElapsed)
{

	++mTotalCalled;
	mCostTime += iElapsed;
	if (iElapsed > mMostCostTime) {
		mMostCostTime = iElapsed;
	}
	if (iElapsed < mLestCostTime) {
		mLestCostTime = iElapsed;
	}

	return mTotalCalled;
}

/// 重置统计数据
void CPerfInfo::Reset()
{
	mTotalCalled = 0;
	mCostTime = 0;
	mMostCostTime = 0;
	mLestCostTime = 1000;
}

CPerfStat::PerfMap CPerfStat::msPerfMap;

void CPerfStat::LogPerfInfo()
{
//	INIT_ROLLINGFILE_LOG("Perf", "../log/perf.log", LEVEL_INFO, 10 * 1024 * 1024, 20 );


	static char szTitle[256];
	sprintf(szTitle, "%38s\t%6s\t%6s\t%6s\t%6s\t%6s",
			"name", "called", "cost", "most", "lest", "avg");

	LOG_INFO("Perf", "{}", szTitle);

	PerfMapIterator endi = msPerfMap.end();

	for (PerfMapIterator p = msPerfMap.begin(); p != endi; ++p) {
		char szRecord[256];
		const CPerfIndex &perfIndex = (*p).first;
		CPerfInfo &perfInfo = (*p).second;
		sprintf(szRecord, "%38s\t%6d\t%6d\t%6d\t%6d\t%6d",
				perfIndex.mName.c_str(), perfInfo.mTotalCalled,
				perfInfo.mCostTime, perfInfo.mMostCostTime, perfInfo.mLestCostTime,
				(perfInfo.mTotalCalled != 0) ? perfInfo.mCostTime / perfInfo.mTotalCalled : 0);

		LOG_INFO("Perf", "{}", szRecord);

		perfInfo.Reset();
	}

	msPerfMap.clear();
	LOG_INFO("Perf", "----------------------------------------------------------------------------------");
}
