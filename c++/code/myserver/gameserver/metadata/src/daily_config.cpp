#include "daily_config.h"
#include "../common_err.h"

DailyItem::DailyItem()
{
    _id = 0;
    _unlock = 0;
    _type = 0;
    _condition = 0;
    _score = 0;
    _times = 0;
}

DailyItem::~DailyItem()
{
    _id = 0;
    _unlock = 0;
    _type = 0;
    _condition = 0;
    _score = 0;
    _times = 0;
}

DailyConfig::DailyConfig()
{
    _datas.clear();
}

DailyConfig::~DailyConfig()
{
    for (auto it = _datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int DailyConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new DailyItem;
        auto value = it->second.ToObject();
        auto oid = value["id"].ToInt();
        auto unlock = value["unlock"].ToInt();
        auto type = value["type"].ToInt();
        auto condition = value["condition"].ToInt();
        auto score = value["score"].ToInt();
        auto times = value["times"].ToInt();
        
        item->_id = oid;
        item->_unlock = unlock;
        item->_type = type;
        item->_condition = condition;
        item->_score = score;
        item->_times = times;
        _datas[oid] = item;
    }
    return 0;
}

void DailyConfig::clear()
{
    for (auto it = _datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

DailyItem* DailyConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("DailyConfig::item:{}", key);
}