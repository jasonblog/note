#include "building_amount_config.h"
#include "../common_err.h"

BuildingAmountItem::BuildingAmountItem()
{
    _buildNum.clear();
}

BuildingAmountItem::~BuildingAmountItem()
{
    _buildNum.clear();
}



BuildingAmountConfig::BuildingAmountConfig()
{
    _datas.clear();
}

BuildingAmountConfig::~BuildingAmountConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int BuildingAmountConfig::init()
{
    return loadFromFile("building_config.json");
}

int BuildingAmountConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new BuildingAmountItem;
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto buildNum = item_obj["buildNum"].ToObject();
        for (auto x=buildNum.begin(); x!=buildNum.end(); ++x) {
            auto btype = x->first;
            auto level = x->second.ToInt();
            item->_buildNum[atoi(btype.c_str())] = level;
        }
        item->_id = oid;
        _datas[oid] = item;
    }
    return 0;
}

void BuildingAmountConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

BuildingAmountItem* BuildingAmountConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("BuildingAmountConfig::item:{}", key);
}
