#ifndef __GRID_LIMIT_CONFIG__
#define __GRID_LIMIT_CONFIG__

#include "config_data.h"

#include <map>
#include <vector>

class GridLimitItem
{
    
public:
    
    GridLimitItem();
    
    ~GridLimitItem();
    
public:
    
    int _id;
    int _unLockLimit;
    std::map<std::string, std::vector<int>* > _unLockCost;
    std::map<std::string, std::vector<int>* > _unlockReward;
    std::vector<int> _gridID;
};

class GridLimitConfig : public ConfigData
{
    
public:
    
    GridLimitConfig();
    
    ~GridLimitConfig();
    
    int parse();
    
    void clear();
    
    GridLimitItem* item(int key);
    
public:
    
    std::map<int, GridLimitItem* > _datas;
};

#endif // __GRID_LIMIT_CONFIG__

