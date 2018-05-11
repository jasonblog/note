#ifndef __MAP_CONFIG_H__
#define __MAP_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>

class MapItem
{
    
public:
    
    MapItem(){}
    
    ~MapItem(){}
    
    int sx(){return _area[0];}
    
    int sy(){return _area[1];}
    
    int ex(){return _area[2];}
    
    int ey(){return _area[3];}
    
public:
    
    int _id;
    
    std::vector<int> _area;
    
    int _level;
};

class MapConfig : public ConfigData
{
    
public:
    
    MapConfig();
    
    ~MapConfig();
    
    int parse();
    
    void clear();
    
    MapItem* item(int key);
    
public:
    
    std::map<int, MapItem* > _datas;
};

#endif // __MAP_CONFIG_H__
