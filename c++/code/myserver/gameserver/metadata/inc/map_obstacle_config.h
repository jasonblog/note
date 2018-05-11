#ifndef __MAP_OBSTACLE_CONFIG_H__
#define __MAP_OBSTACLE_CONFIG_H__

#include "config_data.h"

#include <map>
#include <string>
#include <vector>

class MapObstacleItem
{
    
public:
    
    MapObstacleItem();
    
    ~MapObstacleItem();
    
public:
    
    int _id;
    std::vector<int> _listOdd;
    std::vector<int> _listEven;
};

class MapObstacleConfig : public ConfigData
{
public:
    
    MapObstacleConfig();
    
    ~MapObstacleConfig();
    
    int parse();
    
    void clear();
    
    MapObstacleItem* item(int key);
    
public:
    
    std::map<int, MapObstacleItem* > _datas;
    std::map<int, std::vector<int>> _dataArea;
};

#endif