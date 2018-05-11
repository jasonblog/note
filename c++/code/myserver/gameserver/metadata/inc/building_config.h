#ifndef __BUILDING_CONFIG_H__
#define __BUILDING_CONFIG_H__

#include "config_data.h"
#include <vector>
#include <map>

class BuildingData
{
public:
    
    BuildingData();
    
    ~BuildingData();
    
public:
    
    int _id;
    int _level;
    int _buildType;
    int _output;
    int _land;
    int _playerLevel;
    std::map<int, int> _conditon;
    int _upgradeTo;
    std::map<std::string, std::vector<int>* > _buildingExpend;
    int _time;
    int _fightValue;
    int _exp;
    int _buildDown;
    int _isRemove;
    std::vector<float> _parameter;
    int _openParameter;
};


class BuildingConfig : public ConfigData
{
    
public:
    
    BuildingConfig();
    
    ~BuildingConfig();
    
    int parse();
    
    void clear();
    
    BuildingData* item(int key);
    
    BuildingData* getItemByTypeAndLvl(int type, int level);
    
public:
    
    std::map<int, BuildingData*> _datas;
};


#endif // __BUILDING_CONFIG_H__

