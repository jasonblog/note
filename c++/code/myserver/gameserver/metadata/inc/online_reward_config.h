#ifndef __ONLINE_REWARD_CONFIG_H__
#define __ONLINE_REWARD_CONFIG_H__

#include "config_data.h"
#include <map>
#include <vector>

class OnlineRewardItem
{
public:
    
    OnlineRewardItem();
    
    ~OnlineRewardItem();
    
public:
    
    int _id;
    int _coldDown;
    std::map<std::string, std::vector<int>* > _rewards;
};

class OnlineRewardConfig : public ConfigData
{
    
public:
    
    OnlineRewardConfig();
    
    ~OnlineRewardConfig();
    
    int parse();
    
    void clear();
    
    OnlineRewardItem* item(int key);
    
    std::map<int, OnlineRewardItem* > _datas;
};

#endif // __ONLINE_REWARD_CONFIG_H__
