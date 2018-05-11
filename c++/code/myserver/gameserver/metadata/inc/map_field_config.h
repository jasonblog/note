#ifndef __MAP_FIELD_CONFIG_H__
#define __MAP_FIELD_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>

class MapFieldItem
{
    
public:
    
    MapFieldItem();
    
    ~MapFieldItem();
    
public:
    
    int _id;
    int _type;
    int _level;
    int _capacity;
    int _collectSpeed;
    int _load;
    int _castleLevel;
};

class MapFieldConfig : public ConfigData
{
    
public:
    
    MapFieldConfig();
    
    ~MapFieldConfig();
    
    int parse();
    
    void clear();
    
    MapFieldItem* item(int key);
    
    MapFieldItem* find(int type, int level);
    
public:
    
    std::map<int, MapFieldItem* > _datas;
};

#endif // __MAP_FIELD_CONFIG_H__
