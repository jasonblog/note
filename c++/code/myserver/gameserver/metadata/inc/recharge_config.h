#ifndef __RECHARGE_CONFIG_H__
#define __RECHARGE_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>

class RechargeItem
{
    
public:
    
    RechargeItem();
    
    ~RechargeItem();
    
public:
    
    int             _id;
    std::string     _platform;      //平台类型
    int             _level;
    int             _rewardGem;
    int             _firstGive;
    int             _everydayGive;
    float           _USD;           //美元价格

};

class RechargeConfig : public ConfigData
{
    
public:
    
    RechargeConfig();
    
    ~RechargeConfig();
    
    int parse();
    
    void clear();
    
    RechargeItem* item(int key);
    
public:
    
    std::map<int, RechargeItem*> _datas;
};


#endif // __RECHARGE_CONFIG_H__

