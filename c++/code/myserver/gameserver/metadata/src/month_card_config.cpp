//
//  month_card_config.cpp
//  cserver
//
//  Created by zhaolong on 16/8/29.
//
//

#include "month_card_config.h"
#include "../global.h"

MonthCardCfgInfo::MonthCardCfgInfo()
{
    _id = 0;                //id
    _rechargeIOS = 0;       //IOS充值档
    _rechargeAndroid = 0;   //Android充值档
    _rewardGem = 0;         //充值获得
    _dailyReward = 0;       //每日奖励
    _times = 0;             //有效期
}

MonthCardCfgInfo::~MonthCardCfgInfo()
{
    
}

MonthCardConfig::MonthCardConfig()
{
    _datas.clear();
}

MonthCardConfig::~MonthCardConfig()
{
    clear();
}

int MonthCardConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        MonthCardCfgInfo* item = new MonthCardCfgInfo();
        
        auto item_obj = it->second.ToObject();
        int oid = item_obj["id"].ToInt();
        item->_id = oid;
        item->_rechargeIOS = item_obj["rechargeIOS"].ToInt();
        item->_rechargeAndroid = item_obj["rechargeAndroid"].ToInt();
        item->_rewardGem = item_obj["rewardGem"].ToInt();
        item->_dailyReward = item_obj["dailyReward"].ToInt();
        item->_times = item_obj["times"].ToInt();
 
        _datas[oid] = item;
    }
    return 0;
}

bool MonthCardConfig::check()
{
    for (auto it = _datas.begin(); it != _datas.end(); ++it)
    {
        //去充值配置表 查看充值的正确性
        if (NULL == g_rechargeConfig.item(it->second->_rechargeIOS))
        {
            ERROR_LOG("ERR: check AllianceGiftConfig _rechargeIOS is err!!! id = {}, _rechargeIOS = {}", it->first, it->second->_rechargeIOS);
            return false;
        }
        if (NULL == g_rechargeConfig.item(it->second->_rechargeAndroid))
        {
            ERROR_LOG("ERR: check AllianceGiftConfig _rechargeAndroid is err!!! id = {}, _rechargeAndroid = {}", it->first, it->second->_rechargeAndroid);
            return false;
        }
    }
    return true;
}

void MonthCardConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

MonthCardCfgInfo* MonthCardConfig::getCfg(int nIndex)
{
    auto it=_datas.find(nIndex);
    if (it!=_datas.end())
    {
        return it->second;
    }
    return NULL;
}
