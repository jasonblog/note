#include "map_field_config.h"
#include "../common_err.h"

MapFieldItem::MapFieldItem()
{
    _id = 0;
    _type = 0;
    _level = 0;
    _capacity = 0;
    _load = 0;
    _castleLevel = 0;
}

MapFieldItem::~MapFieldItem()
{
    _id = 0;
    _type = 0;
    _level = 0;
    _capacity = 0;
    _load = 0;
    _castleLevel = 0;
}

MapFieldConfig::MapFieldConfig()
{
    _datas.clear();
}

MapFieldConfig::~MapFieldConfig()
{
    clear();
}

int MapFieldConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto one = new MapFieldItem;
        auto one_item = it->second.ToObject();
        one->_id = one_item["id"].ToInt();
        one->_type = one_item["type"].ToInt();
        one->_level = one_item["level"].ToInt();
        one->_capacity = one_item["capacity"].ToInt();
        one->_collectSpeed = one_item["collectSpeed"].ToInt();
        one->_load = one_item["load"].ToInt();
        one->_castleLevel = one_item["castleLevel"].ToInt();
        _datas[one->_id] = one;
    }
    return 0;
}

void MapFieldConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

MapFieldItem* MapFieldConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("MapFieldConfig::item:{}", key);
}

MapFieldItem* MapFieldConfig::find(int type, int level)
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        if (it->second->_type == type && it->second->_level == level) {
            return it->second;
        }
    }
    return NULL;
}
