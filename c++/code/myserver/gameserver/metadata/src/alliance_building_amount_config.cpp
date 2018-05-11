#include "alliance_building_amount_config.h"

AllianceBuildingAmountItem::AllianceBuildingAmountItem()
{
    mbuildNum.clear();
}
AllianceBuildingAmountItem::~AllianceBuildingAmountItem()
{
    mbuildNum.clear();
}
//------------------------------------------
//------------------------------------------

AllianceBuildingAmountConfig::AllianceBuildingAmountConfig()
{
    _datas.clear();
}

AllianceBuildingAmountConfig::~AllianceBuildingAmountConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int AllianceBuildingAmountConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto item = new AllianceBuildingAmountItem();
        
        auto item_obj   = it->second.ToObject();
        
        auto id                 = item_obj["id"].ToInt();
        auto buildNum           = item_obj["buildNum"].ToObject();
        
        item->iId               = id;
        for (auto l=buildNum.begin(); l!=buildNum.end(); ++l)
        {
            item->mbuildNum[atoi(l->first.c_str())] = l->second.ToInt();
        }
        
        _datas[id] = item;
    }
    
    return 0;
}

void AllianceBuildingAmountConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

AllianceBuildingAmountItem* AllianceBuildingAmountConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    
    return NULL;
}