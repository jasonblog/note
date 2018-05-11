//
//  vip_config.h
//  cserver
//
//  Created by zhaolong on 16/8/13.
//
//

#ifndef vip_config_h
#define vip_config_h

#include "config_data.h"
#include <map>

class VipCfgInfo
{
public:
    int     nID;        //id
    int     nExp;       //升级所需要的经验
    float   fHpAdd;     //部队生命增加    百分比
    float   fAtkAdd;    //部队攻击增加    百分比
    int     nTeamAdd;   //行军总数增加
    float   fSpeedAdd;  //行军速度增加    百分比
    float   fPveSpeedAdd;//打怪行军速度增加百分比
    int     nBuildingAdd;//建造加速      分钟数
    float   fOutputAdd; //资源产量增加    百分比
    float   fCollectAdd;//资源田采集速度  百分比
};

class VipConfig : public ConfigData
{
    
public:
    
    VipConfig();
    
    ~VipConfig();
    
    int parse();
    
    void clear();
    
    VipCfgInfo* getCfg(int nIndex);
public:
    std::map<int, VipCfgInfo*> _datas;
};

#endif /* vip_config_h */
