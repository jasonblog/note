#ifndef __GRID_CONFIG_H__
#define __GRID_CONFIG_H__


#include "config_data.h"

#include <map>
#include <vector>

class GridItem
{
    
public:
    
    GridItem();
    
    ~GridItem();
    
public:
    
    int _id;
    int _gridType;
};

class GridConfig : public ConfigData
{
    
public:
    
    GridConfig();
    
    ~GridConfig();
    
    int parse();
    
    void clear();
    
    GridItem* item(int key);
    
public:
    
    std::map<int, GridItem* > _datas;
};


#endif // __GRID_CONFIG_H__