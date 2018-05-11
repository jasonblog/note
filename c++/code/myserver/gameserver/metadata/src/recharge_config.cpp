#include "recharge_config.h"
#include "../common_err.h"

RechargeItem::RechargeItem()
{}

RechargeItem::~RechargeItem()
{}

RechargeConfig::RechargeConfig()
{
    _datas.clear();
}

RechargeConfig::~RechargeConfig()
{
    
}

int RechargeConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new RechargeItem();
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        item->_id = oid;
        item->_platform = item_obj["platform"].ToString();
        item->_level = item_obj["level"].ToInt();
        item->_rewardGem = item_obj["rewardGem"].ToInt();
        item->_firstGive = item_obj["fristGive"].ToInt();
        item->_everydayGive = item_obj["everydayGive"].ToInt();
        item->_USD = item_obj["USD"].ToFloat();
        _datas[oid] = item;
    }
    return 0;
}

void RechargeConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

RechargeItem* RechargeConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("RechargeConfig::item:{}", key);
}
