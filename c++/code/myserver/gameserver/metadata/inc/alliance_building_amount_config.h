#ifndef __ALLIANCE_BUILDING_AMOUNT_CONFIG_H__
#define __ALLIANCE_BUILDING_AMOUNT_CONFIG_H__

#include "config_data.h"

//  联盟建筑
class AllianceBuildingAmountItem
{
public:
    AllianceBuildingAmountItem();
    ~AllianceBuildingAmountItem();
    
public:
    int iId;                            //  编号
    std::map<int, int> mbuildNum;       //  建筑数量
};

class AllianceBuildingAmountConfig : public ConfigData
{
public:
    AllianceBuildingAmountConfig();
    ~AllianceBuildingAmountConfig();
    
    int parse();
    
    void clear();
    
    AllianceBuildingAmountItem* item(int key);
    
public:
    
    std::map<int, AllianceBuildingAmountItem* > _datas;
};

#endif