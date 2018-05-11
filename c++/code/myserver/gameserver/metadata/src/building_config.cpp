#include "building_config.h"
#include "../common_err.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "../../framework/framework.h"

BuildingData::BuildingData()
{
    _conditon.clear();
    _buildingExpend.clear();
    _parameter.clear();
}

BuildingData::~BuildingData()
{
    _conditon.clear();
    for (auto it=_buildingExpend.begin(); it!=_buildingExpend.end(); ++it) {
        it->second->clear();
    }
    _buildingExpend.clear();
    _parameter.clear();
}

BuildingConfig::BuildingConfig()
{
    _datas.clear();
}

BuildingConfig::~BuildingConfig()
{
    for (auto it = _datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int BuildingConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new BuildingData();
        
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto level = item_obj["level"].ToInt();
        auto buildingType = item_obj["buildingType"].ToInt();
        auto output = item_obj["output"].ToInt();
        auto land = item_obj["land"].ToInt();
        auto playerLevel = item_obj["playerLevel"].ToInt();
        auto condition = item_obj["condition"].ToObject();
        auto upgradeTo = item_obj["upgradeTo"].ToInt();
        auto buildingExpend = item_obj["buildingExpend"].ToObject();
        auto time = item_obj["time"].ToInt();
        auto fightValue = item_obj["fightValue"].ToInt();
        auto exp = item_obj["exp"].ToInt();
        auto buildDown = item_obj["buildDown"].ToInt();
        auto isRemove = item_obj["isRemove"].ToInt();
        auto parameter = item_obj["parameter"].ToArray();
        auto openParameter = item_obj["openParameter"].ToInt();
        
        item->_id = oid;
        item->_level = level;
        item->_buildType = buildingType;
        item->_output = output;
        item->_land = land;
        item->_playerLevel = playerLevel;
        for (auto l=condition.begin(); l!=condition.end(); ++l) {
            item->_conditon[atoi(l->first.c_str())] = l->second.ToInt();
        }
        item->_upgradeTo = upgradeTo;
        item->_time = time;
        item->_fightValue = fightValue;
        item->_exp = exp;
        item->_buildDown = buildDown;
        item->_isRemove = isRemove;
        for (auto pl=parameter.begin(); pl!=parameter.end(); ++pl) {
            auto y = pl->ToFloat();
            item->_parameter.push_back(y);
        }
        item->_openParameter = openParameter;
        
        for (auto mit = buildingExpend.begin(); mit!=buildingExpend.end(); ++mit) {
            auto v = new std::vector<int>;
            item->_buildingExpend[mit->first] = v;
            auto ar = mit->second.ToArray();
            for (auto l = ar.begin(); l!=ar.end(); ++l) {
                v->push_back(l->ToInt());
            }
        }
        _datas[oid] = item;
        
    }
    return 0;
}

void BuildingConfig::clear()
{
    for (auto it = _datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();

}

BuildingData* BuildingConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("BuildingConfig::item:{}", key);
}

BuildingData* BuildingConfig::getItemByTypeAndLvl(int type, int level)
{
    for (auto it = _datas.begin(); it != _datas.end(); ++it)
    {
        if (it->second->_buildType == type && it->second->_level == level)
        {
            return it->second;
        }
    }
    
    return NULL;
}
