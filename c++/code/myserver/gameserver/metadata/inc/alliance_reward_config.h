#ifndef __ALLIANCE_REWARD_CONFIG_H__
#define __ALLIANCE_REWARD_CONFIG_H__
#include "config_data.h"
#include <map>
#include <vector>

class AllianceRewardItem
{
public:
    AllianceRewardItem();
    ~AllianceRewardItem();
public:
    int _id;
    int _contribution;
    std::map<std::string, std::vector<int>* > _rewards;
};

class AllianceRewardConfig : public ConfigData
{
public:
    AllianceRewardConfig();
    ~AllianceRewardConfig();
    int parse();
    void clear();
    AllianceRewardItem* item(int key);
public:
    std::map<int, AllianceRewardItem*> _datas;
};

#endif  //__ALLIANCE_REWARD_CONFIG_H__
