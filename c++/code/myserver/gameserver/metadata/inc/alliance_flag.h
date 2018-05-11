#ifndef __ALLIANCE_FLAG_CONFIG_H__
#define __ALLIANCE_FLAG_CONFIG_H__


#include "config_data.h"

#include <map>
#include <vector>

class AllianceFlagConfigItem
{
    
public:
    AllianceFlagConfigItem();
    
    ~AllianceFlagConfigItem();
    
public:
    int _id;
    int _price;
    int _icon;
};

class AllianceFlagConfig : public ConfigData
{
    
public:
    AllianceFlagConfig();
    
    ~AllianceFlagConfig();
 
    int parse();
    
    void clear();
    
    AllianceFlagConfigItem* item(int key);
    
public:
    std::map<int, AllianceFlagConfigItem* > _datas;
};

#endif // __ALLIANCE_FLAG_CONFIG_H__

