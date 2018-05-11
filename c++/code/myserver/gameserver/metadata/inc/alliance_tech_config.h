#ifndef __ALLIANCE_TECH_CONFIG_H__
#define __ALLIANCE_TECH_CONFIG_H__

#include "config_data.h"

//  联盟科技配置表
class AllianceTechItem
{
public:
    AllianceTechItem();
    
    ~AllianceTechItem();
    
public:
    int     _id;        //  科技ID
    int     _type;      //  科技效果类型
    int     _level;     //  科技等级
    float   _param;     //  加成
    int     _point;     //  积分
    int     _time;      //  升级时间
    int     _next;      //  升级之后的ID
    
};



class AllianceTechConfig : public ConfigData
{
public:
    AllianceTechConfig();
    
    ~AllianceTechConfig();
    
    int parse();
    
    void clear();
    
    AllianceTechItem* item(int key);
    
public:
    
    std::map<int, AllianceTechItem* > _datas;
};

#endif