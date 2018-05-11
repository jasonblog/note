#include "alliance_flag.h"
#include "../common_err.h"


AllianceFlagConfigItem::AllianceFlagConfigItem(){}
    
AllianceFlagConfigItem::~AllianceFlagConfigItem(){}



AllianceFlagConfig::AllianceFlagConfig()
{
    _datas.clear();
}
    
AllianceFlagConfig::~AllianceFlagConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it){
        delete it->second;
    }
    _datas.clear();
}

int AllianceFlagConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it){
        auto item = new AllianceFlagConfigItem();
        auto item_obj = it->second.ToObject();
        auto id = item_obj["id"].ToInt();
        auto price = item_obj["price"].ToInt();
        auto icon = item_obj["icon"].ToInt();
        
        item->_id = id;
        item->_price = price;
        item->_icon = icon;
        
        _datas[id] = item;
    }
    
    return 0;
}


void AllianceFlagConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

AllianceFlagConfigItem* AllianceFlagConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()){
        return it->second;
    }
    throw ERROR::MSG("AllianceFlagConfig::item:{}", key);
}
