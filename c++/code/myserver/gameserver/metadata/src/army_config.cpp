#include "army_config.h"
#include "../common_err.h"

ArmyItem::ArmyItem()
{
    _upgradeCost.clear();
    _trainCost.clear();
}
ArmyItem::~ArmyItem()
{
    for (auto it=_upgradeCost.begin(); it!=_upgradeCost.end(); ++it) {
        it->second->clear();
    }
    _upgradeCost.clear();
    
    for (auto it=_trainCost.begin(); it!=_trainCost.end(); ++it) {
        it->second->clear();
    }
    _trainCost.clear();
}

ArmyConfig::ArmyConfig()
{
    _datas.clear();
}
ArmyConfig::~ArmyConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int ArmyConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new ArmyItem();
        
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto type = item_obj["type"].ToInt();
        auto unLockLimit = item_obj["unLockLimit"].ToInt();
        auto upgradeLimit = item_obj["upgradeLimit"].ToInt();
        auto upgradeCost = item_obj["upgradeCost"].ToObject();
        auto upgradeTime = item_obj["upgradeTime"].ToInt();
        auto upgradeTo = item_obj["upgradeTo"].ToInt();
        auto exp = item_obj["exp"].ToInt();
        auto level = item_obj["level"].ToInt();
        auto hp = item_obj["hp"].ToInt();
        auto atk = item_obj["atk"].ToInt();
        auto speed = item_obj["speed"].ToInt();
        auto weight = item_obj["weight"].ToInt();
        auto mobility = item_obj["mobility"].ToInt();
        auto trainCost = item_obj["trainCost"].ToObject();
        auto trainTime = item_obj["trainTime"].ToInt();
        auto armyType = item_obj["armyType"].ToInt();
        auto fscore = item_obj["score"].ToFloat();
        
        item->_id = oid;
        item->_type = type;
        item->_unLockLimit = unLockLimit;
        item->_upgradeLimit = upgradeLimit;
        item->_upgradeTime = upgradeTime;
        item->_upgradeTo = upgradeTo;
        item->_exp = exp;
        item->_level = level;
        item->_hp = hp;
        item->_atk = atk;
        item->_speed = speed;
        item->_weight = weight;
        item->_mobility = mobility;
        item->_trainTime = trainTime;
        item->_armyType = armyType;
        item->_score = fscore;
        
        for (auto it=upgradeCost.begin(); it!=upgradeCost.end(); ++it) {
            auto v = new std::vector<int>;
            item->_upgradeCost[it->first] = v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit) {
                v->push_back(xit->ToInt());
            }
        }
        
        for (auto it=trainCost.begin(); it!=trainCost.end(); ++it) {
            auto v = new std::vector<int>;
            item->_trainCost[it->first] = v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit) {
                v->push_back(xit->ToInt());
            }
        }
        _datas[oid] = item;
    }
    return 0;
}

void ArmyConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}
ArmyItem* ArmyConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
}

