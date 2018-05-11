#include "map_config.h"
#include "../common_err.h"

MapConfig::MapConfig()
{
    _datas.clear();
}

MapConfig::~MapConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
    

}

int MapConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it = obj.begin(); it!=obj.end(); ++it) {
        auto item = new MapItem;
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto area = item_obj["area"].ToArray();
        auto areaLevel = item_obj["areaLevel"].ToInt();
        item->_id = oid;
        for (auto xit = area.begin(); xit!=area.end(); ++xit) {
            item->_area.push_back(xit->ToInt());
        }
        item->_level = areaLevel;
        _datas[oid] = item;
    }
    return 0;
}

void MapConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

MapItem* MapConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("MapConfig::item:{}", key);
}
