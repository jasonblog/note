//
// Created by zhaolong on 16-8-3.
// 七日登陆签到奖励配置表
//

#ifndef CSERVER_SEVEN_DAY_CONFIG_H
#define CSERVER_SEVEN_DAY_CONFIG_H

#include "config_data.h"
#include <map>

enum
{
    SignDAys_First = 1,
    SignDAys_Seven = 7,
    SignDAys_Max,
};

class SevenDayCfgInfo
{
public:
    int _id;
    std::map<int, std::vector<int> > _reward;

public:
    SevenDayCfgInfo();
    ~SevenDayCfgInfo();

};

class SevenDayConfig : public ConfigData
{

public:

    SevenDayConfig();

    ~SevenDayConfig();

    int parse();

    void clear();
    
    bool check();

    SevenDayCfgInfo* getCfg(int iIndex);
public:
    std::map<int, SevenDayCfgInfo*> _datas;
};

#endif //CSERVER_SEVEN_DAY_CONFIG_H
