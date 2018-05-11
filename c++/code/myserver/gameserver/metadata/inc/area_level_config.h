#ifndef __AREA_LEVEL_CONFIG_H__
#define __AREA_LEVEL_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>

class AreaLevelItem
{
    
public:
    
    AreaLevelItem();
    
    ~AreaLevelItem();
    
public:
    
    int _id;
    
    std::map<int, float> _field;
};

class AreaLevelConfig : public ConfigData
{
    
public:
    
    AreaLevelConfig();
    
    ~AreaLevelConfig();
    
    int parse();
    
    void clear();
    
    AreaLevelItem* item(int key);
    
public:
    
    std::map<int, AreaLevelItem* > _datas;
};

#endif // __AREA_LEVEL_CONFIG_H__

