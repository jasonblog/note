//
//  alliance_gift_config.cpp
//  cserver
//
//  Created by zhaolong on 16/9/2.
//
//

#include "alliance_gift_config.h"
#include "../package/common_resp.h"
#include "../global.h"

AllianceGiftCfgInfo::AllianceGiftCfgInfo()
{
    
}

AllianceGiftCfgInfo::~AllianceGiftCfgInfo()
{
    
}

AllianceGiftConfig::AllianceGiftConfig()
{
    _datas.clear();
}

AllianceGiftConfig::~AllianceGiftConfig()
{
    clear();
}

int AllianceGiftConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        AllianceGiftCfgInfo* item = new AllianceGiftCfgInfo();
        
        auto item_obj = it->second.ToObject();
        int oid = item_obj["id"].ToInt();
        item->_id = oid;
        item->_rechargeIOS = item_obj["rechargeIOS"].ToInt();
        item->_rechargeAndroid = item_obj["rechargeAndroid"].ToInt();
        item->_alliancePoint = item_obj["alliancePoint"].ToInt();
        item->_contribution = item_obj["contribution"].ToInt();
        item->_rewardGem = item_obj["rewardGem"].ToInt();
        std::string rewardItem = item_obj["rewardItem"].ToString();
        fillStrToMap(rewardItem.c_str(), item->_rewardItem);
        std::string playerReward = item_obj["playerReward"].ToString();
        fillStrToMap(playerReward.c_str(), item->_playerReward);
        item->_mail = item_obj["mail"].ToInt();
        
        _datas[oid] = item;
    }
    
    return 0;
}

bool AllianceGiftConfig::check()
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
        
        //检测配置的奖品正确性
        if(!checkVectorIsItem(it->second->_rewardItem))
        {
            ERROR_LOG("ERR: check AllianceGiftConfig _rewardItem item id is err!!! id = {}", it->first);
            return false;
        }
        
        if(!checkVectorIsItem(it->second->_playerReward))
        {
            ERROR_LOG("ERR: check AllianceGiftConfig _playerReward item id is err!!! id = {}", it->first);
            return false;
        }
    }
    return true;
}

void AllianceGiftConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

AllianceGiftCfgInfo* AllianceGiftConfig::getCfg(int nIndex)
{
    auto it=_datas.find(nIndex);
    if (it!=_datas.end())
    {
        return it->second;
    }
    return NULL;
}

