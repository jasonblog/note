//
//  vip_config.cpp
//  cserver
//
//  Created by zhaolong on 16/8/13.
//
//

#include "vip_config.h"

VipConfig::VipConfig()
{
    _datas.clear();
}

VipConfig::~VipConfig()
{
    clear();
}

int VipConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        VipCfgInfo* item = new VipCfgInfo();
        
        auto item_obj = it->second.ToObject();
        int oid = item_obj["id"].ToInt();
        item->nID = oid;
        item->nExp = item_obj["point"].ToInt();
        item->fOutputAdd = item_obj["outputAdd"].ToFloat();
        item->fHpAdd = item_obj["hpAdd"].ToFloat();
        item->fAtkAdd = item_obj["atkAdd"].ToFloat();
        item->nTeamAdd = item_obj["teamAdd"].ToInt();
        item->fSpeedAdd = item_obj["speedAdd"].ToFloat();
        item->fPveSpeedAdd = item_obj["pveSpeedAdd"].ToFloat();
        item->nBuildingAdd = item_obj["buildingAdd"].ToInt();
        item->fCollectAdd = item_obj["collectAdd"].ToFloat();
        
        _datas[oid] = item;
    }
    
    return 0;
}

void VipConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

VipCfgInfo* VipConfig::getCfg(int nIndex)
{
    auto it=_datas.find(nIndex);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
}