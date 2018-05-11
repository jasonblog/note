//
//  festl_activity_config.cpp
//  cserver
//
//  Created by zhaolong on 16/12/10.
//
//

#include "festl_activity_config.h"
#include "../package/common_resp.h"
#include "../global.h"

FestlActivityChildCfg::FestlActivityChildCfg()
{
    _id = 0;
    _type = 0;
    _drop = -1;
    _target = 0;
    _targetStr.clear();
    _reward.clear();
}

FestlActivityChildCfg::~FestlActivityChildCfg()
{
    
}

FestlActivityInfo::FestlActivityInfo()
{
    _id = 0;
    _aheadTime = 0;
    _openCondition = 0;
    _startDate = 0;
    _yearMode = 0;
    _duration = 0;
    _isOpen = false;
    _child.clear();
}

FestlActivityInfo::~FestlActivityInfo()
{
    _child.clear();
}

FestlActivityConfig::FestlActivityConfig()
{
    mnLoadType = 0;
    _base.clear();
    _child.clear();
}

FestlActivityConfig::~FestlActivityConfig()
{
    
}

int FestlActivityConfig::parse()
{
    if (mnLoadType == 0)
    {
        _base.clear();
        auto obj = m_obj.ToObject();
        for (auto it=obj.begin(); it!=obj.end(); ++it)
        {
            FestlActivityInfo oBase;
            auto item_obj = it->second.ToObject();
            int oid = item_obj["id"].ToInt();
            oBase._id = oid;
            oBase._aheadTime = item_obj["aheadTime"].ToInt();
            oBase._openCondition = item_obj["openCondition"].ToInt();
            oBase._startDate = item_obj["startDate"].ToInt();
            oBase._yearMode = item_obj["yearMode"].ToInt();
            oBase._duration = item_obj["duration"].ToInt();
            oBase._period = item_obj["period"].ToInt();
            oBase._isOpen = (item_obj["isOpen"].ToInt() == 1);
            std::string sChild = item_obj["childID"].ToString();
            splitToIntList(sChild, ",", oBase._child);
            _base[oid] = oBase;
        }
    }
    else if(mnLoadType == 1)
    {
        _child.clear();
        auto obj = m_obj.ToObject();
        for (auto it=obj.begin(); it!=obj.end(); ++it)
        {
            FestlActivityChildCfg oChild;
            auto item_obj = it->second.ToObject();
            int oid = item_obj["id"].ToInt();
            
            oChild._id = oid;
            oChild._type = item_obj["type"].ToInt();
            oChild._drop = item_obj["drop"].ToInt();
            oChild._target = item_obj["target"].ToInt();
            std::string sTarget = item_obj["targetSTR"].ToString();
            oChild._targetStr.clear();
            fillStrToMap(sTarget.c_str(), oChild._targetStr);
            std::string sReward = item_obj["reward"].ToString();
            oChild._reward.clear();
            fillStrToMap(sReward.c_str(), oChild._reward);
            
            _child[oid] = oChild;
        }
    }

    return 0;
}

bool FestlActivityConfig::check()
{
    for (auto it = _base.begin(); it != _base.end(); ++it)
    {
        for (auto it1 = it->second._child.begin(); it1 != it->second._child.end(); ++it1)
        {
            //检测子活动的合法性
            int nChildID = (*it1);
            if (nChildID != 0 && NULL == getChildCfg(nChildID))
            {
                ERROR_LOG("ERR: activity_config.json find err child id !!! activity id = {}, err child = {}", it->first, nChildID)
                return false;
            }
            
            //检测持续时间的合法性
            if (it->second._duration < 0)
            {
                ERROR_LOG("ERR: activity_config.json find err duration !!! activity id = {}, duration = {}", it->first, it->second._duration)
                return false;
            }
        }
    }
    
    for (auto it = _child.begin(); it != _child.end(); ++it)
    {
        //有奖励道具 就检测是否正确
        if (!it->second._reward.empty())
        {
            if (!checkVectorIsItem(it->second._reward))
            {
                ERROR_LOG("ERR: child_activity_config.json find err item id !!! child id = {}", it->first)
                return false;
            }
        }
        
        //特殊活动  检测数据
        if (it->second._type == ACTIVITY_CHILD_EXCHANGE)
        {
            if (!checkVectorIsItem(it->second._targetStr))
            {
                ERROR_LOG("ERR: child_activity_config.json find err item id for targetStr !!! child id = {}", it->first)
                return false;
            }
        }
        
        //如果有掉落 查看掉落ID 是否合法
        if (it->second._drop != 0)
        {
            if (NULL == g_dropConfig.item(it->second._drop))
            {
                ERROR_LOG("ERR: child_activity_config.json find err drop id !!! child id = {}", it->first)
                return false;
            }
        }
    }
    
    
    return true;
}

void FestlActivityConfig::clear()
{

}

int FestlActivityConfig::load()
{
    mnLoadType = 0;
    if (loadFromFile("activity_config.json") != 0)
    {
        return -1;
    }
    mnLoadType = 1;
    if (loadFromFile("child_activity_config.json") != 0)
    {
        return -1;
    }
    mnLoadType = 0;
    return 0;
}

FestlActivityInfo* FestlActivityConfig::getCfg(int nIndex)
{
    auto it=_base.find(nIndex);
    if (it!=_base.end())
        return &it->second;
    else
        return NULL;
}

FestlActivityChildCfg* FestlActivityConfig::getChildCfg(int nChildID)
{
    auto it = _child.find(nChildID);
    if (it != _child.end())
        return &it->second;
    else
        return NULL;
}


