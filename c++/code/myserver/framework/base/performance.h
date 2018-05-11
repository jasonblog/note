#ifndef __PERFORMANCE_H__
#define __PERFORMANCE_H__

#include <string>
#include <map>
#include "log.h"

class CPerfInfo
{
public:
	CPerfInfo();
	~CPerfInfo();

	/// 输入这次调用经过的时间，返回被调用的次数
	int AddCalled(int iElapsed);

	/// 重置统计数据
	void Reset();

public:

	int	mPerfLog;		// 是否统计本信息。为了以后控制统计项制定.默认统计
	int	mTotalCalled;	// 被调用的总次数
	int	mCostTime;		// 总共花费的时间
	int mMostCostTime;	// 最长消耗时间
	int mLestCostTime;	// 最短消耗时间
};



/// 性能优化，不直接使用string作为map的key,封装CPerfIndex作为key
class CPerfIndex
{
public:
	CPerfIndex( unsigned int uiHashCode, const char* pszName ) : 
	  mHashCode( uiHashCode ), mName( pszName ) {}
	~CPerfIndex(){}

	unsigned int	mHashCode;		// 作为优先比较关键字
	std::string		mName;			// perf统计的信息
};



class CLessPrefIndex
{
public:

	bool operator()(const CPerfIndex& __x, const CPerfIndex& __y) const
	{
		return ( __x.mHashCode < __y.mHashCode )
			|| ( __x.mHashCode == __y.mHashCode && __x.mName < __y.mName );
	}
};


class CPerfStat
{
	typedef std::map<CPerfIndex, CPerfInfo, CLessPrefIndex> PerfMap;
	typedef PerfMap::iterator PerfMapIterator;

	static PerfMap msPerfMap;
	
public:
	/**
	* DJB Hash Function
	* An algorithm produced by Professor Daniel J. Bernstein and shown first to the
	* world on the usenet newsgroup comp.lang.c. It is one of the most efficient
	* hash functions ever published.
	*
	* @param str    需要计算hash值的字符串，必须以ASCII 0结尾
	*
	* @return 字符串对应的hash值
	*/
	static inline unsigned int DJBHash(const char* str)
	{
		unsigned int hash = 5381;

		for(const unsigned char* p = reinterpret_cast<const unsigned char*> (str); 0 != *p; ++p)
		{
			hash = ((hash << 5) + hash) + *p;
		}
		return (hash & 0x7FFFFFFF);
	}


	static inline CPerfInfo& GetPerfInfo(const char* name)
	{
		unsigned int hashCode = DJBHash(name);
		CPerfIndex perfIndex(hashCode, name);
		return msPerfMap[perfIndex];
	}

	static inline CPerfInfo& GetPerfInfo(unsigned int hashCode, const char* name)
	{
		CPerfIndex perfIndex(hashCode, name);
		return msPerfMap[perfIndex];
	}

	// 记录统计信息到日志文件
	static void LogPerfInfo();

};



// 统计函数调用开销
#define PERF_FUNC(funcname, callfunc)								\
	do																\
	{																\
		CPerfInfo& info = CPerfStat::GetPerfInfo(funcname);			\
		if (info.mPerfLog)											\
		{															\
			Clock a;												\
			a.start();												\
			callfunc;												\
			info.AddCalled(a.stop());								\
		}															\
		else														\
		{															\
			callfunc;												\
		}															\
	} while (0)

#define PERF_FUNC_RTN( funcname, rtn, callfunc )					\
	do																\
	{																\
		CPerfInfo& info = CPerfStat::GetPerfInfo(funcname);         \
		if (info.mPerfLog)                                          \
		{                                                           \
			Clock a;												\
			a.start();												\
			rtn = callfunc;											\
			info.AddCalled(a.stop());								\
		}															\
		else														\
		{															\
			rtn = callfunc;											\
		}															\
	} while (0)
	

// 可以返回流逝时间的统计
#define PERF_FUNC_ELAPSED(funcname, callfunc, _elapse )				\
		CPerfInfo& info = CPerfStat::GetPerfInfo(funcname);			\
		if (info.mPerfLog)											\
		{															\
			Clock a;												\
			a.start();												\
			callfunc;												\
			info.AddCalled(a.stop());								\
			_elapse = (int)a.elapsed();								\
		}															\
		else														\
		{															\
			callfunc;												\
		}															




// 统计数据写入日志
#define PERF_LOG CPerfStat::LogPerfInfo

#define PERF_BEGIN( funcname )													\
		CPerfInfo& funcname##_info = CPerfStat::GetPerfInfo( #funcname );		\
		Clock funcname##_a;														\
		if( funcname##_info.mPerfLog == true )									\
		{																		\
			funcname##_a.start( );												\
		}

#define PERF_END( funcname )													\
		if( funcname##_info.mPerfLog == true)									\
		{																		\
			funcname##_info.AddCalled( funcname##_a.stop( ) );					\
		}

#endif // __PERFORMANCE_H__

