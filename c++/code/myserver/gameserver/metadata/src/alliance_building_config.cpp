#include "alliance_building_config.h"
#include "../common_err.h"

AllianceBuildingItem::AllianceBuildingItem()
{
    mConditon.clear();
    vecParameter.clear();
}
AllianceBuildingItem::~AllianceBuildingItem()
{
    mConditon.clear();
    vecParameter.clear();
}
//------------------------------------------
//------------------------------------------

AllianceBuildingConfig::AllianceBuildingConfig()
{
    _datas.clear();
}

AllianceBuildingConfig::~AllianceBuildingConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int AllianceBuildingConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto item = new AllianceBuildingItem();
        
        auto item_obj   = it->second.ToObject();
        
        auto id                 = item_obj["id"].ToInt();               //  编号
        auto level              = item_obj["level"].ToInt();            //  等级
        auto isChose            = item_obj["isChose"].ToInt();       //  建造是否需要选择
        auto buildingType       = item_obj["buildingType"].ToInt();     //  建筑类型
        auto condition          = item_obj["condition"].ToObject();     //  前置建筑
        auto upgradeTo          = item_obj["upgradeTo"].ToInt();        //  升级到
        auto buildingExpend     = item_obj["buildingExpend"].ToInt();   //  需要联盟积分
        auto hp                 = item_obj["hp"].ToInt();               //  生命值
        auto parameter          = item_obj["parameter"].ToArray();      //  参数
        auto castleLevel        = item_obj["castleLevel"].ToInt();      //  采集需要城堡等级
        auto output             = item_obj["output"].ToInt();           //  产出类型
        auto land               = item_obj["land"].ToInt();             //  建筑占格
        auto isInflunce         = item_obj["isInflunce"].ToInt();       //  是否需要联盟势力
        
        item->iId               = id;
        item->iLevel            = level;
        item->bIsChose          = isChose;
        item->iBuildingType     = buildingType;
        item->iUpgradeTo        = upgradeTo;
        item->iBuildingExpend   = buildingExpend;
        item->iHp               = hp;
        item->iCastleLevel      = castleLevel;
        item->iOutput           = output;
        item->iLand             = land;
        item->bIsInflunce       = isInflunce;
        
        for (auto l=condition.begin(); l!=condition.end(); ++l)
        {
            item->mConditon[atoi(l->first.c_str())] = l->second.ToInt();
        }
        
        for (auto it=parameter.begin(); it!=parameter.end(); ++it)
        {
            item->vecParameter.push_back(it->ToFloat());
        }
        
        _datas[id] = item;
    }
    
    return 0;
}

void AllianceBuildingConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

AllianceBuildingItem* AllianceBuildingConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    
    return NULL;
   // throw ERROR::MSG("AllianceBuildingConfig::item:{}", key);
}