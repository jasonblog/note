#include "equipment_config.h"
#include "../common_err.h"

EquipmentItem::EquipmentItem()
{

}

EquipmentItem::~EquipmentItem()
{
    
}

EquipmentConfig::EquipmentConfig()
{
    _datas.clear();

    _colors = NULL;
}

EquipmentConfig::~EquipmentConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
    
    if (_colors != NULL)
    {
        for(auto it = _colors->begin();it != _colors->end();it++)
        {
            auto its = it->second;
            for (auto itx = its.begin();itx != its.end(); itx++)
            {
                itx->second.clear();
            }
            it->second.clear();
        }
        _colors->clear();
        delete  _colors;
        _colors = NULL;
    }
}

int EquipmentConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new EquipmentItem();
        
        auto item_obj = it->second.ToObject();
        
        auto oid = item_obj["id"].ToInt();
        auto equipid = item_obj["equipId"].ToInt();
        auto type = item_obj["type"].ToInt();
        auto color = item_obj["color"].ToInt();
        auto level = item_obj["level"].ToInt();
        auto exp = item_obj["exp"].ToInt();
        auto next = item_obj["next"].ToInt();
        auto zsAtk = item_obj["zsAtk"].ToFloat();
        auto zsHp = item_obj["zsHp"].ToFloat();
        auto gsAtk = item_obj["gsAtk"].ToFloat();
        auto gsHp = item_obj["gsHp"].ToFloat();
        auto qbAtk = item_obj["qbAtk"].ToFloat();
        auto qbHp = item_obj["qbHp"].ToFloat();
        auto flAtk = item_obj["flAtk"].ToFloat();
        auto flHp = item_obj["flHp"].ToFloat();
        auto fsAtk = item_obj["fsAtk"].ToFloat();
        auto fsHp = item_obj["fsHp"].ToFloat();
        
        auto gold = item_obj["gold"].ToFloat();
        auto wood = item_obj["wood"].ToFloat();
        auto stone = item_obj["stone"].ToFloat();
        auto iron = item_obj["iron"].ToFloat();
        auto resource = item_obj["resource"].ToFloat();
        auto atk = item_obj["atk"].ToFloat();
        auto def = item_obj["def"].ToFloat();
     
        auto troop = item_obj["troop"].ToFloat();
        auto hospital = item_obj["hospital"].ToFloat();
        auto load = item_obj["load"].ToFloat();
        auto tspeed = item_obj["tSpeed"].ToFloat();
        auto njAtk = item_obj["njAtk"].ToFloat();
        auto mSpeed = item_obj["mSpeed"].ToFloat();
//       
        item->_iId = oid;
        item->_iEquipId = equipid;
        item->_iType = type;
        item->_iColor = color;
        item->_iLevel = level;
        item->_iExp = exp;
        item->_iNext = next;
        item->_fTroop = troop;
        item->_fZsAtk = zsAtk;
        item->_fZsHp = zsHp;
        item->_fGsAtk = gsAtk;
        item->_fGsHp = gsHp;
        item->_fQbAtk = qbAtk;
        item->_fQbHp = qbHp;
        item->_fFlAtk = flAtk;
        item->_fFlHp = flHp;
        item->_fFsAtk = fsAtk;
        item->_fFsHp = fsHp;
        item->_fGold = gold;
        item->_fWood = wood;
        item->_fStone = stone ;
        item->_fIron = iron;
        item->_fResource = resource;
        item->_fAtk = atk;
        item->_fDef = def;
        item->_fHospital = hospital;
        item->_fLoad = load;
        item->_fTspeed = tspeed;
        item->_fNjatk = njAtk;
        item->_fMspeed = mSpeed;
     
        _datas[oid] = item;
        
    }
    getSmithyRewardData();
    return 0;
}


void EquipmentConfig::getSmithyRewardData()
{
    _colors = new std::map<int,std::map<int,std::vector<int> > >;
    
    for(auto it = _datas.begin();it!= _datas.end();++it)
    {
        auto EquipmentConfigJson = item(it->first);
        if (EquipmentConfigJson != NULL)
        {
            if (EquipmentConfigJson->_iLevel == 1)
            {
                auto itcolor = _colors->find(EquipmentConfigJson->_iColor);    //找同品质
                if (itcolor != _colors->end())
                {
                    auto ittype = itcolor->second.find(EquipmentConfigJson->_iType);
                    if (ittype != itcolor->second.end())
                    {
                        ittype->second.push_back(EquipmentConfigJson->_iId);
                    }
                    else{
                        std::vector<int> ids;
                        ids.push_back(EquipmentConfigJson->_iId);
                        itcolor->second[EquipmentConfigJson->_iType] = ids;
                    }
                }
                else{
                    std::map<int,std::vector<int>> types;  //找同品质同类型
                    std::vector<int> ids;
                    ids.push_back(EquipmentConfigJson->_iId);
                    types[EquipmentConfigJson->_iType] = ids;
                    (*_colors)[EquipmentConfigJson->_iColor] = types;
                }
            }
        }
    }
}

void EquipmentConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

const EquipmentItem* EquipmentConfig::item(const int key) const
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("EquipmentConfig::item:{}", key);
}
