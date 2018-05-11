#include "area_level_config.h"
#include "../common_err.h"

AreaLevelItem::AreaLevelItem()
{
    _id = 0;
    _field.clear();
}

AreaLevelItem::~AreaLevelItem()
{
    _id = 0;
    _field.clear();
}

AreaLevelConfig::AreaLevelConfig()
{
    _datas.clear();
}

AreaLevelConfig::~AreaLevelConfig()
{
    clear();
}

int AreaLevelConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto one = new AreaLevelItem;
        auto one_item = it->second.ToObject();
        one->_id = one_item["id"].ToInt();
        auto field = one_item["field"].ToObject();
        for (auto xit=field.begin(); xit!=field.end(); ++xit) {
            one->_field[atoi(xit->first.c_str())] = xit->second.ToFloat();
        }
        _datas[one->_id] = one;
    }
    return 0;
}

void AreaLevelConfig::clear()
{
    for (auto it =_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

AreaLevelItem* AreaLevelConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("AreaLevelConfig::item:{}", key);
}
