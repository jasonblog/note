#ifndef __ALLIANCE_BUILDING_CONFIG_H__
#define __ALLIANCE_BUILDING_CONFIG_H__

#include "config_data.h"
#include <vector>

//  联盟建筑
class AllianceBuildingItem
{
public:
    AllianceBuildingItem();
    ~AllianceBuildingItem();
    
public:
    int iId;                            //  编号
    int iLevel;                         //  等级
    int bIsChose;                       //  建造是否需要选择
    int iBuildingType;                  //  建筑类型
    std::map<int, int> mConditon;       //  前置建筑
    int iUpgradeTo;                     //  升级到
    int iBuildingExpend;                //  需要联盟积分
    int iHp;                            //  生命值
    std::vector<float> vecParameter;    //  参数
    int iCastleLevel;                   //  采集需要城堡等级
    int iOutput;                        //  产出类型
    int iLand;                          //  建筑占格
    bool bIsInflunce;                   //  是否需要联盟势力
};

class AllianceBuildingConfig : public ConfigData
{
public:
    AllianceBuildingConfig();
    ~AllianceBuildingConfig();
    
    int parse();
    
    void clear();
    
    AllianceBuildingItem* item(int key);
    
public:
    
    std::map<int, AllianceBuildingItem* > _datas;
};

#endif