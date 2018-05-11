#include "army_type_config.h"
#include "../common_err.h"

ArmyTypeItem::ArmyTypeItem()
{
    _damage.clear();
}
ArmyTypeItem::~ArmyTypeItem()
{
    _damage.clear();
}

ArmyTypeConfig::ArmyTypeConfig()
{
    _datas.clear();
}
ArmyTypeConfig::~ArmyTypeConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int ArmyTypeConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new ArmyTypeItem();
        
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto damage = item_obj["damage"].ToObject();
        auto wallrest = item_obj["Wallrestriction"].ToFloat();

        
        item->_id = oid;
        item->_wallRestriction = wallrest;

        
        for (auto it=damage.begin(); it!=damage.end(); ++it) {
            item->_damage[atoi(it->first.c_str())] = it->second.ToFloat();
        }

        _datas[oid] = item;
    }
    return 0;
}

void ArmyTypeConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

ArmyTypeItem* ArmyTypeConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
}

