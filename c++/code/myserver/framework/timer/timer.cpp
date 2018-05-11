#include "timer.h"
#include "my_macro.h"

char *GetTimerTypeName(ETimerType eType)
{
	char *p;

	switch (eType) {
	case ETT_TIMER: {
		p = (char *) "ETT_TIMER";
		break;
	}
	case ETT_SESSION: {
		p = (char *) "ETT_SESSION";
		break;
	}
	default: {
		p = (char *) "ETT_INVALID";
		break;
	}
	}

	return p;
}

// ----------------------------------------------------------------------------------
// 定时器基类
// ----------------------------------------------------------------------------------
int CTimerBase::Initialize()
{
	CDoubleLinker::Init();

	m_tTimeout = 0;
	m_eTimerType = ETT_INVALID;
	m_iOtherInfoNum = 0;
	mbDestroy = false;

	return 0;
}

int CTimerBase::Resume()
{
	CDoubleLinker::Resume();

	return 0;
}

// 注册回调函数
void CTimerBase::RegistCallFunc(CallFunc func)
{
	mCallFunc = func;
}

// ----------------------------------------------------------------------------------
// 定时器
// ----------------------------------------------------------------------------------
int CTimer::Initialize()
{
	m_eTimerType = ETT_TIMER;
	m_ulOwnerEntityID = 0;
	m_tTimeout = 0;
	m_tMillTime = 0;
	m_iCalledNum = 0;
	m_iTimerMark = 0;
	m_eRunTimeMode = ERTM_GAME;
	m_iOtherInfoNum = 0;

	for (int i = 0; i < MAX_TIMER_PARAM_NUM; i++) {
		mOtherInfos[i] = 0;
	}

	return 0;
}

int CTimer::Resume()
{
	return 0;
}

// 获取扩展参数
int CTimer::GetOtherInfo(int iIdx)
{
	if ((iIdx < 0) || (iIdx >= MAX_TIMER_PARAM_NUM) || (iIdx >= m_iOtherInfoNum)) {
		return 0;
	}

	return mOtherInfos[iIdx];
}

// 设置timer信息
void CTimer::SetTimer(int iTimerMark,
					  ERunTimeMode enRunTimeMode,
					  time_t tMillTime,
					  CallFunc func,
					  int iCalledNum,
					  int iOwnerEntityID,
					  unsigned long ulOwnerID)
{
	m_ulOwnerEntityID = iOwnerEntityID;
	m_ulOwnerID = ulOwnerID;
	m_iTimerMark = iTimerMark;
	m_eRunTimeMode = enRunTimeMode;
	m_tMillTime = tMillTime;
	m_tTimeout = (GetMSTime() + tMillTime);
	m_iCalledNum = iCalledNum;

	mCallFunc = func;

	Clear();
}

// 设置timer扩展参数
int CTimer::SetOtherInfos(int iNum, int *piParams)
{
//	if ((iNum <= 0) || (iNum > ARRAY_CNT(mOtherInfos)) || (piParams == NULL))
//	{
//		return -1;
//	}

	for (int i = 0; i < iNum; i++) {
		mOtherInfos[i] = piParams[i];
	}

	m_iOtherInfoNum = iNum;

	return 0;
}

// 获取timer剩余时间（ERTM_GAME模式则返回剩余时间，ERTM_PHYSICAL模式则返回触发的时间。单位：毫秒）
time_t CTimer::GetRemainTime()
{
	if (m_eRunTimeMode == ERTM_PHYSICAL) {
		return m_tTimeout;
	}

	return (m_tTimeout - GetMSTime());
}

// 设置剩余时间（ERTM_GAME模式则tTime+当前时间，ERTM_PHYSICAL模式则直接为tTime）
void CTimer::SetRemainTime(time_t tTime)
{
	if (m_eRunTimeMode == ERTM_PHYSICAL) {
		m_tTimeout = tTime;
	}
	else {
		m_tTimeout = (tTime + GetMSTime());
	}
}

// 超时处理
void CTimer::Timeout(time_t tNow)
{
	if (mbDestroy) return;

	// 回调
	mCallFunc(this);

	// 计算次数
	if (m_iCalledNum > 0) {
		m_iCalledNum--;
	}

	if (m_iCalledNum == 0) {
		return;
	}

	// 计算下次超时时间,并补偿
	time_t tCompensation = tNow - m_tTimeout;
	if (tCompensation < 0) {
		// 其实这里不可能小于0
		tCompensation = 0;
	}

	m_tTimeout = (tNow + m_tMillTime - tCompensation);
}

// ----------------------------------------------------------------------------------
// CSession会话类
// ----------------------------------------------------------------------------------
int CSession::Initialize()
{
	m_eTimerType = ETT_SESSION;
	m_tTimeout = 0;
	m_iOtherInfoNum = 0;
	m_tMillTime = 0;
	m_bContinues = false;
	for (int i = 0; i < MAX_SESSION_PARAM_NUM; i++) {
		mOtherInfos[i] = 0;
	}

	return 0;
}

int CSession::Resume()
{
	return 0;
}

// 获取扩展参数
int CSession::GetOtherInfo(int iIdx)
{
	if ((iIdx < 0) || (iIdx >= MAX_SESSION_PARAM_NUM) || (iIdx >= m_iOtherInfoNum)) {
		return 0;
	}

	return mOtherInfos[iIdx];
}

void CSession::SetSession(time_t tMillTime, CallFunc func, OBJ_ID ulOwnerEntityID, OBJ_ID ulOwnerID, bool bContinues)
{
	m_bContinues = bContinues;
	m_tMillTime = tMillTime;
	m_tTimeout = (GetMSTime() + tMillTime);
	mCallFunc = func;
	m_ulOwnerEntityID = ulOwnerEntityID;
	m_ulOwnerID = ulOwnerID;
	Clear();
}

// 设置session扩展参数（返回值 0：成功 < 0：失败）
int CSession::SetOtherInfos(int iNum, int *piParams)
{
	if ((iNum <= 0) || (iNum > MAX_SESSION_PARAM_NUM) || (piParams == NULL)) {
		return -1;
	}

	for (int i = 0; i < iNum; i++) {
		mOtherInfos[i] = piParams[i];
	}

	m_iOtherInfoNum = iNum;

	return 0;
}

// 超时处理
void CSession::Timeout(time_t tNow)
{
	if (mbDestroy) return;
	// 先设置超时时间,因为应用里面会用到
	// 设置下次超时时间
	if (m_bContinues == true) {
		m_tTimeout += m_tMillTime;
	}
	// 回调
	mCallFunc(this);
}

// ----------------------------------------------------------------------------------
// 定时器管理器
// ----------------------------------------------------------------------------------

CTimerManager::CTimerManager()
{
	Initialize();
}

CTimerManager::~CTimerManager()
{
	SAFE_DELETE(m_pSessionManager);
	SAFE_DELETE(m_pTimerManager);
	m_mTimerMap.clear();
	m_mTimerFinder.clear();
	m_mSessionMap.clear();
	m_aDeleteList.clear();
}

int CTimerManager::Initialize()
{
	m_pSessionManager = new CObjectManager(EnObjType::OBJ_SESSION_TIMER);
	m_pTimerManager = new CObjectManager(EnObjType::OBJ_GAMER_TIMER);
	m_mTimerMap.clear();
	m_mTimerFinder.clear();
	m_mSessionMap.clear();
	m_aDeleteList.clear();
	m_tLastCheckTick = (GetMSTime() / TIMER_PERCISION);
	return 0;
}

// 统计输出
void CTimerManager::Dump(char *pcBuffer, unsigned int &uiLen)
{
	unsigned int uiMaxLen = uiLen;
	uiLen = 0;

	uiLen += snprintf(pcBuffer + uiLen,
					  uiMaxLen - uiLen,
					  "------------------------------CTimerManager------------------------------");
	uiLen += snprintf(pcBuffer + uiLen, uiMaxLen - uiLen, "\n%30s\t%10s\t%10s", "name", "free", "total");
	uiLen += snprintf(pcBuffer + uiLen,
					  uiMaxLen - uiLen,
					  "\n%30s\t%10lu\t%10lu",
					  "CTimer",
					  m_mTimerMap.size(),
					  m_mTimerMap.max_size());
	uiLen += snprintf(pcBuffer + uiLen,
					  uiMaxLen - uiLen,
					  "\n%30s\t%10lu\t%10lu",
					  "CSession",
					  m_mSessionMap.size(),
					  m_mSessionMap.max_size());
}

// 插入mTimerFinder
void CTimerManager::InsertIntoFinder(CTimerBase *pTimer)
{
	TIMER_FINDER::iterator tIter = m_mTimerFinder.find((pTimer->GetTimeout() / TIMER_PERCISION));
	if (tIter == m_mTimerFinder.end()) {
		CDoubleLinkerInfo tInfo;
		tInfo.Initialize();
		tInfo.insert(pTimer);
		m_mTimerFinder.insert(TIMER_FINDER::value_type((pTimer->GetTimeout() / TIMER_PERCISION), tInfo));
	}
	else {
		tIter->second.insert(pTimer);
	}
}

// 从mTimerFinder移除
void CTimerManager::EraseFromFinder(CTimerBase *pTimer)
{
	TIMER_FINDER::iterator tIter = m_mTimerFinder.find((pTimer->GetTimeout() / TIMER_PERCISION));
	if (tIter != m_mTimerFinder.end()) {
		if (tIter->second.erase(pTimer) == 0) {
			// 说明没数据了
			m_mTimerFinder.erase((pTimer->GetTimeout() / TIMER_PERCISION));
		}
	}
	else {
		LOG_ERROR("default", "impossible, check logic.");
	}
}

// 创建timer
CTimer *CTimerManager::CreateTimer(int iTimerMark,
								   CTimer::ERunTimeMode enRunTimeMode,
								   time_t tMillTime,
								   CTimer::CallFunc func,
								   int iCalledNum,
								   time_t tRemainTime,
								   int iOwnerEntityID,
								   OBJ_ID ulOwnerID)
{
	// 创建timer
	CTimer *tpTimer = (CTimer *) CreateObject(ETT_TIMER);
	if (tpTimer == NULL) {
		LOG_ERROR("default", "createtimer failed, mTimerQueue create failed.");
		return NULL;
	}

	// 初始化timer信息
	tpTimer->SetTimer(iTimerMark, enRunTimeMode, tMillTime, func, iCalledNum, iOwnerEntityID, ulOwnerID);
	if (tRemainTime != 0) {
		// 说明timer不是当前创建，而是添加一个之前未执行完的timer，一般在玩家上线时使用
		tpTimer->SetRemainTime(tRemainTime);
	}

	// 插入链表
	InsertIntoFinder(tpTimer);
	return tpTimer;
}

// 真实处理定时器实体销毁的接口
void CTimerManager::RealDestroyTimer(OBJ_ID iObjID)
{
	// 获取实体
	CTimerBase *tpItem = (CTimerBase *) GetObject(iObjID);
	if (tpItem == NULL) {
		LOG_DEBUG("default", "CTimerManager::DestroyObject failed, invalid iObjID = %u.", iObjID);
		return;
	}

	// 从链表中删除
	EraseFromFinder(tpItem);

	// 销毁实体
	LOG_DEBUG("default", "[{} : {} : {}] DeleteObject.", __MY_FILE__, __LINE__, __FUNCTION__);
	DeleteObject(iObjID);
}

// 检测定时器队列
int CTimerManager::CheckTimerQueue(time_t tNow)
{
	int nowTick = tNow / TIMER_PERCISION;
	while (nowTick > m_tLastCheckTick) {
		// 先处理待销毁定时器
		auto itDel = m_aDeleteList.begin();
		for (; itDel != m_aDeleteList.end(); itDel++) {
			RealDestroyTimer((int) *itDel);
		}
		m_aDeleteList.clear();
		// 循环遍历定时器,处理超时
		for (auto tIter = m_mTimerFinder.begin(); tIter != m_mTimerFinder.end(); tIter++) {
			//mTimerFinder有序，如果当前的定时任务没有超时，则后面的也没有直接返回
			if (tIter->first > nowTick) {
				m_tLastCheckTick++;
				return 0;
			}
			CDoubleLinkerInfo tpInfo = tIter->second;
			CTimerBase *tpTemp = (CTimerBase *) tpInfo.GetHead();
			while (tpTemp != NULL) {
				LOG_DEBUG("default", "timer (id=%u).", tpTemp->get_id());
				tpTemp = Timeout(tpTemp, tNow);
				if (tpTemp) {
					LOG_DEBUG("default", "next timer (id=%u).", tpTemp->get_id());
				}
			}
		}
		m_tLastCheckTick++;
	}
	return 0;
}

// 超时处理
CTimerBase *CTimerManager::Timeout(CTimerBase *pTimer, time_t tNow, bool bClear)
{
	// 先把timer从链表中删除
	CTimerBase *tpReturn = (CTimerBase *) pTimer->GetNext();
	EraseFromFinder(pTimer);
	// 处理timer超时
	pTimer->Timeout(tNow);

	if (((pTimer->GetTimerType() == ETT_TIMER) && (bClear == false) && ((((CTimer *) pTimer))->GetCalledNum() != 0))
		|| (pTimer->GetTimerType() == ETT_SESSION) && (((CSession *) pTimer)->IsContinues() == true)) {
		// 将该结点插入新的链表
		InsertIntoFinder(pTimer);
	}
	else {
		// 因为该结点已经在上面删除，所以这里直接删除实体
		LOG_DEBUG("default", "[{} : {} : {}] DeleteObject.", __MY_FILE__, __LINE__, __FUNCTION__);
		DeleteObject(pTimer->get_id());
	}

	return tpReturn;
}

// 强制超时（bClear表示本次超时以后是否继续执行）
void CTimerManager::ForceTimeout(OBJ_ID iTimerID, bool bClear)
{
	CTimer *tpTimer = (CTimer *) GetObject(iTimerID);
	if (tpTimer == NULL) {
		return;
	}

	Timeout(tpTimer, GetMSTime(), bClear);

	return;
}

// 创建session
CSession *CTimerManager::CreateSession(time_t tMillTime, CSession::CallFunc func, int iOwnerEntityID, OBJ_ID ulOwnerID)
{
	CSession *tpSession = (CSession *) CreateObject(ETT_SESSION);
	if (tpSession == NULL) {
		LOG_ERROR("default", "createsession failed, mSessionQueue create failed.");
		return NULL;
	}

	// 初始化session信息
	tpSession->SetSession(tMillTime, func, iOwnerEntityID, ulOwnerID);

	// 插入链表
	InsertIntoFinder(tpSession);

	LOG_INFO("default", "Session ID(%u) MillTime(%ld) create now.",
			 tpSession->get_id(), tMillTime);

	return tpSession;
}

// 创建实体
CObj *CTimerManager::CreateObject(ETimerType eType)
{
	CObj *pTmpObj = NULL;

	switch (eType) {
	case ETT_TIMER: {
		pTmpObj = new CTimer();
		pTmpObj->set_id(m_pTimerManager->GetValidId());
		break;
	}

	case ETT_SESSION: {
		pTmpObj = new CSession();
		pTmpObj->set_id(m_pSessionManager->GetValidId());
		break;
	}
	default: {
		LOG_ERROR("default", "CTimerManager::CreateObject failed, object type({}) invalid.", eType);
		break;
	}
	}

	if (pTmpObj == NULL) {
		LOG_ERROR("default",
				  "CTimerManager::CreateObject failed, object type({} : {}).",
				  eType,
				  GetTimerTypeName(eType));
	}
	else {
		LOG_DEBUG("default",
				  "CTimerManager::CreateObject(%u) type({} : {}) ok.",
				  pTmpObj->get_id(),
				  eType,
				  GetTimerTypeName(eType));
	}

	return pTmpObj;
}

// 删除实体
int CTimerManager::DeleteObject(OBJ_ID iObjID)
{
	ETimerType tType = (ETimerType) CObj::ID2TYPE(iObjID);

	switch (tType) {
	case ETT_TIMER: {
		m_mTimerMap.erase(iObjID);
		break;
	}

	case ETT_SESSION: {
		m_mTimerMap.erase(iObjID);
		break;
	}
	default: {
		LOG_ERROR("default", "CTimerManager::DeleteObject failed, object id = %u, type = {}.", iObjID, tType);
		return -1;
	}
	}

	LOG_INFO("default", "DeleteObject Type({}) ObjID %u.", GetTimerTypeName(tType), iObjID);

	return 0;
}

// 提供外部使用的销毁实体的接口，同时从链表中删除对应的结点
int CTimerManager::DestroyObject(OBJ_ID iObjID)
{
	// 获取实体
	CTimerBase *tpItem = (CTimerBase *) GetObject(iObjID);
	if (tpItem == NULL) {
		LOG_ERROR("default", "CTimerManager::DestroyObject failed, invalid iObjID = %u.", iObjID);
		return -1;
	}

	// 设置定时器待销毁状态
	tpItem->NeedDestroy();

	// 定时器不能立即销毁,放入待销毁列表中
	m_aDeleteList.push_back(iObjID);

	// 从链表中删除
	EraseFromFinder(tpItem);

	// 销毁实体
	LOG_DEBUG("default", "[{} : {} : {}] DeleteObject.", __MY_FILE__, __LINE__, __FUNCTION__);
	DeleteObject(iObjID);

	return 0;
}

// 提供给外部使用的获取实体的接口
CObj *CTimerManager::GetObject(OBJ_ID iObjID)
{
	if (iObjID == INVALID_OBJ_ID) {
		return NULL;
	}

	CObj *pTmpObj = NULL;
	ETimerType tType = (ETimerType) CObj::ID2TYPE(iObjID);

	switch (tType) {
	case ETT_TIMER: {
		auto it = m_mTimerMap.find(iObjID);
		if (it != m_mTimerMap.end()) {
			return (CObj *) (&(it->second));
		}
		break;
	}

	case ETT_SESSION: {
		auto it = m_mSessionMap.find(iObjID);
		if (it != m_mSessionMap.end()) {
			return (CObj *) (&(it->second));
		}
		break;
	}

	default: {
		LOG_ERROR("default",
				  "CTimerManager::GetObject failed, object id(%u), type({} : {}), not registed.",
				  iObjID,
				  tType,
				  GetTimerTypeName(tType));
		return NULL;
	}
	}

	return pTmpObj;
}





