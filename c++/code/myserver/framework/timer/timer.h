#ifndef _TIMER_H_
#define _TIMER_H_

#include <map>
#include <unordered_map>
#include <mythread.h>
#include "object.h"
#include "server_tool.h"
#include "object_manager.h"

// 类型取值 0 ~ 255 为了防止越界，去掉头尾边界，所以有效取值范围为 1 ~ 254
// 由于游戏内部的objmanager从1开始，为了不与游戏内的类型重复，所以这里取值从254（0xFE）开始递减
enum ETimerType
{
	ETT_INVALID = 0x00,    // 无效的类型

	// 新类型往上添加，取值递减(0xFE ~ 0x01)
		ETT_SESSION = 0xFD,    // SESSION类型
	ETT_TIMER = 0xFE,    // TIMER类型

	ETT_PERIPHERY = 0xFF    // 类型边界
};

// ----------------------------------------------------------------------------------
// 定时器基类
// ----------------------------------------------------------------------------------
class CTimerBase: public CObj, public CDoubleLinker
{
public:
	typedef void(*CallFunc)(CTimerBase *);

	// 定义构造，析构，初始化，resume函数
public:
	CTimerBase()
	{
		Initialize();
	}
	~CTimerBase()
	{}
	int Initialize();
	int Resume();

public:
	// 定时器类型
	ETimerType GetTimerType()
	{ return m_eTimerType; }
	// 超时时间(单位 : 毫秒)
	time_t GetTimeout()
	{ return m_tTimeout; }
	// 扩展参数个数(无法设置,只能调用SetOtherInfos接口重新设置参数)
	int GetOtherInfoNum()
	{ return m_iOtherInfoNum; }
	// 获取扩展参数
	virtual int GetOtherInfo(int iIdx) = 0;
	// 超时处理
	virtual void Timeout(time_t tNow) = 0;
	// 注册回调函数
	void RegistCallFunc(CallFunc func);
	// 设置定时器待销毁状态
	void NeedDestroy()
	{ mbDestroy = true; }

protected:
	// 定时器类型
	ETimerType m_eTimerType;
	// 超时时间(单位 : 毫秒)
	time_t m_tTimeout;
	// 扩展参数个数(无法设置,只能调用SetOtherInfos接口重新设置参数)
	int m_iOtherInfoNum;
	// 回调函数
	CallFunc mCallFunc;
	// 定时器待销毁状态
	bool mbDestroy;
};

// ----------------------------------------------------------------------------------
// 定时器
// ----------------------------------------------------------------------------------
class CTimer: public CTimerBase
{
public:
	enum
	{
		TIMER_FOREVER = -1,        // 次数小于0,则表示永久执行
		MAX_TIMER_PARAM_NUM = 4,        // 单个timer的最多扩展参数数量
	};

	enum ERunTimeMode
	{
		ERTM_GAME = 0,    // 游戏时间
		ERTM_PHYSICAL = 1,    // 物理时间

		ERTM_NUM
	};


	// 定义构造，析构，初始化，resume函数
public:
	CTimer()
	{
		Initialize();
	}
	~CTimer()
	{}
	int Initialize();
	int Resume();

public:
	// 定时器类型
	int GetTimerMark()
	{ return m_iTimerMark; }
	// 定时器时间运行模式
	ERunTimeMode GetRunTimeMode()
	{ return m_eRunTimeMode; }
	// timer拥有者的实体ID
	OBJ_ID GetOwnerEntityID()
	{ return m_ulOwnerEntityID; }
	// 单次间隔时间(单位 : 毫秒,用于计算下次Timeout的值)
	time_t GetMillTime()
	{ return m_tMillTime; }
	// timer执行次数(大于0则按此执行,小于0则为永久执行)
	int GetCalledNum()
	{ return m_iCalledNum; }
	// 获取扩展参数
	int GetOtherInfo(int iIdx);
	// 设置timer信息（返回Timer的ID）Owner可以为空,主要为了确认玩家关联时使用
	void SetTimer(int iTimerMark,
				  ERunTimeMode enRunTimeMode,
				  time_t tMillTime,
				  CallFunc func,
				  int iCalledNum,
				  int iOwnerEntityID,
				  OBJ_ID lOwnerID);
	// 设置timer扩展参数（返回值 0：成功 < 0：失败）
	int SetOtherInfos(int iNum, int *piParams);
	// 获取timer剩余时间（ERTM_GAME模式则返回剩余时间，ERTM_PHYSICAL模式则返回触发的时间。单位：毫秒）
	time_t GetRemainTime();
	// 设置剩余时间（ERTM_GAME模式则tTime+当前时间，ERTM_PHYSICAL模式则直接为tTime）
	void SetRemainTime(time_t tTime);
	// 超时处理
	void Timeout(time_t tNow);
	// timer拥有者的ID
	OBJ_ID GetOwnerID()
	{ return m_ulOwnerID; }

protected:
	// 扩展参数列表
	int mOtherInfos[MAX_TIMER_PARAM_NUM];
	// timer拥有者的ID
	OBJ_ID m_ulOwnerID;
	// 定时器类型
	int m_iTimerMark;
	// 定时器时间运行模式
	ERunTimeMode m_eRunTimeMode;
	// timer拥有者的实体ID
	OBJ_ID m_ulOwnerEntityID;
	// 单次间隔时间(单位 : 毫秒,用于计算下次Timeout的值)
	time_t m_tMillTime;
	// timer执行次数(大于0则按此执行,小于0则为永久执行)
	int m_iCalledNum;
};

// ----------------------------------------------------------------------------------
// CSession会话类 (CSession不会自动销毁,每次超时后继续按间隔触发,直到手动销毁)
// ----------------------------------------------------------------------------------
class CSession: public CTimerBase
{
public:
	enum
	{
		MAX_SESSION_PARAM_NUM = 3,    // 单个session的最多扩展参数数量
	};

	// 定义构造，析构，初始化，resume函数
public:
	CSession()
	{
		Initialize();
	}
	~CSession()
	{

	}
	int Initialize();
	int Resume();

public:
	// 获取扩展参数
	int GetOtherInfo(int iIdx);
	// 设置session信息（返回session的ID）Owner可以为空,主要为了确认玩家关联时使用
	void SetSession(time_t tMillTime,
					CallFunc func,
					OBJ_ID iOwnerEntityID,
					OBJ_ID ulOwnerID = 0,
					bool bContinues = false);
	// 设置session扩展参数（返回值 0：成功 < 0：失败）
	int SetOtherInfos(int iNum, int *piParams);
	// 超时处理
	void Timeout(time_t tNow);
	// 是否可持续
	bool IsContinues()
	{ return m_bContinues; }

	// 创建session的玩家的实体ID
	OBJ_ID GetOwnerEntityID()
	{ return m_ulOwnerEntityID; }
	// 创建session的玩家的id
	unsigned long GetOwnerID()
	{ return m_ulOwnerID; }

protected:
	int mOtherInfos[MAX_SESSION_PARAM_NUM];    // 扩展参数列表
	OBJ_ID m_ulOwnerEntityID;        // 创建session的玩家的实体ID
	OBJ_ID m_ulOwnerID;        // 创建session的玩家的id(玩家创建时才需要,其它不需要该参数)
	time_t m_tMillTime;        // session间隔时间
	bool m_bContinues;    // session是否持续
};

// ----------------------------------------------------------------------------------
// 定时器管理器（在同一个精度周期里面，不保证定时器顺序）
// ----------------------------------------------------------------------------------
class CTimerManager
{
public:
	CTimerManager();
	~CTimerManager();
	int Initialize();
	// 统计输出
	void Dump(char *pcBuffer, unsigned int &uiLen);
protected:
	enum
	{
		TIMER_PERCISION = 100,            // 定时器精度 100 ms
		TIMER_QUEUE_LENGTH = 120000,    // 定时器最大个数
		SESSION_QUEUE_LENGTH = 120000,    // 会话最大个数
	};

#define FINDER_QUEUE_LENGTH (TIMER_QUEUE_LENGTH + SESSION_QUEUE_LENGTH)

	typedef unordered_map<OBJ_ID, CTimer> TimerObjMap;
	typedef unordered_map<OBJ_ID, CSession> SessionObjMap;
	typedef std::map<time_t, CDoubleLinkerInfo> TIMER_FINDER;
	typedef std::vector<OBJ_ID> DELETE_LIST;

	// CTimer的objmanager
	CObjectManager *m_pTimerManager;
	TimerObjMap m_mTimerMap;
	// CSession的objmanager
	CObjectManager *m_pSessionManager;
	SessionObjMap m_mSessionMap;
	// 以超时tick为key，链表信息为value的hash_map
	TIMER_FINDER m_mTimerFinder;
	// 上次检测timer队列的tick即 毫秒 / 100
	time_t m_tLastCheckTick;
	// 定时器销毁队列(定时器不能即时销毁,因为定时器里面有可能销毁定时器,所以放入待销毁列表中)
	DELETE_LIST m_aDeleteList;

	// 插入mTimerFinder
	void InsertIntoFinder(CTimerBase *pTimer);
	// 从mTimerFinder移除
	void EraseFromFinder(CTimerBase *pTimer);
	// 超时处理（返回当前timer的下一个timer，一个小技巧，方便遍历删除）
	CTimerBase *Timeout(CTimerBase *pTimer, time_t tNow, bool bClear = false);
	// 创建实体
	CObj *CreateObject(ETimerType eType);
	// 删除实体
	int DeleteObject(OBJ_ID objId);
	// 真实处理定时器实体销毁的接口
	void RealDestroyTimer(OBJ_ID objId);

public:
	// ----------------------------------------------------------------------------- //
	// 共用接口
	// ----------------------------------------------------------------------------- //
	// 检测定时器队列（当前时间（单位：毫秒）除以TIMER_PERCISION）
	int CheckTimerQueue(time_t tNow);
	// 提供外部使用的销毁实体的接口，同时从链表中删除对应的结点
	int DestroyObject(OBJ_ID iObjID);
	// 提供给外部使用的获取实体的接口
	CObj *GetObject(OBJ_ID iObjID);

	// ----------------------------------------------------------------------------- //
	// CTimer相关接口
	// ----------------------------------------------------------------------------- //
	// 创建timer
	CTimer *CreateTimer(int iTimerMark,
						CTimer::ERunTimeMode enRunTimeMode,
						time_t tMillTime,
						CTimer::CallFunc func,
						int iCalledNum = CTimer::TIMER_FOREVER,
						time_t tRemainTime = 0,
						int iOwnerEntityID = 0,
						unsigned long ulOwnerID = 0);
	// timer强制超时（bClear表示本次超时以后是否直接清除timer）
	void ForceTimeout(OBJ_ID iTimerID, bool bClear = false);

	// ----------------------------------------------------------------------------- //
	// CSission相关接口
	// ----------------------------------------------------------------------------- //
	// 创建session
	CSession *CreateSession(time_t tMillTime, CSession::CallFunc func, int iOwnerEntityID = 0, OBJ_ID ulOwnerID = 0);

};


#endif // _TIMER_H_
