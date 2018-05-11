#ifndef __ARMY_CONFIG_H__
#define __ARMY_CONFIG_H__


#include "config_data.h"

#include <map>
#include <vector>

class ArmyItem
{
    
public:
    
    ArmyItem();
    
    ~ArmyItem();
    
public:
    
    int _id;
    int _type;
    int _armyType;
    int _unLockLimit;
    int _upgradeLimit;
    std::map<std::string, std::vector<int>* > _upgradeCost;
    int _upgradeTime;
    int _upgradeTo;
    int _exp;
    int _level;
    int _hp;
    int _atk;
    int _speed;
    int _weight;
    int _mobility;
    float _score;
    std::map<std::string, std::vector<int>* > _trainCost;
    int _trainTime;
};

class ArmyConfig : public ConfigData
{
    
public:
    
    ArmyConfig();
    
    ~ArmyConfig();
    
    int parse();
    
    void clear();
    
    ArmyItem* item(int key);
    
public:
    
    std::map<int, ArmyItem* > _datas;
};

#endif // __ARMY_CONFIG_H__

