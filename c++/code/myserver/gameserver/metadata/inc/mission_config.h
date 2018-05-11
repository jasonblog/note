#ifndef __MISSION_CONFIG_H__
#define __MISSION_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>

//  酒吧悬赏任务配置表
class MissionItem
{
    
public:
    MissionItem();
    ~MissionItem();
    
public:
    int _id;            //  ID
    int _unlock;        //  需要城堡等级
    int _quality;       //  品质
    //  完成条件
    std::map<std::string, std::vector<int>* > _condition;
    //  任务奖励
    std::map<std::string, std::vector<int>* > _reward;
    int _next;          //  更高品质
    int _weight;        //  刷新权重
};

class MissionConfig : public ConfigData
{
public:
    MissionConfig();
    
    ~MissionConfig();
    
    int parse();
    
    void clear();
    
    MissionItem* item(int key);
    
public:
    
    std::map<int, MissionItem* > _datas;
};

#endif // __MISSION_CONFIG_H__