#ifndef __PLAYER_EXP_CONFIG_H__
#define __PLAYER_EXP_CONFIG_H__


#include "config_data.h"

#include <map>
#include <vector>

class PlayerExpItem
{
    
public:
    
    PlayerExpItem();
    
    ~PlayerExpItem();
    
public:
    
    int _id;
    int _exp;
    int _goldSpeed;
    int _woodSpeed;
    int _stoneSpeed;
    int _ironSpeed;
    int _diamondSpeed;
    int _goldRoundle;
    int _woodRoundle;
    int _otherRoundle;
    
    int _spyPrice;
    std::map<std::string, std::vector<int>* > _rewards;
    
    std::map<std::string, std::vector<int>* > _dailyReward20;
    std::map<std::string, std::vector<int>* > _dailyReward40;
    std::map<std::string, std::vector<int>* > _dailyReward60;
    std::map<std::string, std::vector<int>* > _dailyReward80;
    std::map<std::string, std::vector<int>* > _dailyReward100;
    std::map<int, int> _collect_speed;
};

class PlayerExpConfig : public ConfigData
{
    
public:
    
    PlayerExpConfig();
    
    ~PlayerExpConfig();
    
    int parse();
    
    void clear();
    
    bool check();
    
    PlayerExpItem* item(int key);
    
public:
    
    std::map<int, PlayerExpItem*> _datas;
};

#endif // __PLAYER_EXP_CONFIG_H__

