#ifndef __BUILDING_AMOUNT_CONFIG_H__
#define __BUILDING_AMOUNT_CONFIG_H__

#include "config_data.h"
#include <map>

class BuildingAmountItem
{
    
public:
    
    BuildingAmountItem();
    
    ~BuildingAmountItem();
    
public:
    
    int _id;
    std::map<int, int> _buildNum;
};

class BuildingAmountConfig : public ConfigData
{
    
public:
    
    BuildingAmountConfig();
    
    ~BuildingAmountConfig();
    
    virtual int init();
    
    int parse();
    
    void clear();
    
    BuildingAmountItem* item(int key);
    
public:
    
    std::map<int, BuildingAmountItem*> _datas;
};



#endif // __BUILDING_AMOUNT_CONFIG_H__

