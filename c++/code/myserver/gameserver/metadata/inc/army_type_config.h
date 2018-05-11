#ifndef __ARMY_TYPE_CONFIG_H__
#define __ARMY_TYPE_CONFIG_H__


#include "config_data.h"

#include <map>
#include <vector>

class ArmyTypeItem
{
    
public:
    
    ArmyTypeItem();
    
    ~ArmyTypeItem();
    
public:
    
    int _id;
    std::map<int, float> _damage;
    float _wallRestriction;

};

class ArmyTypeConfig : public ConfigData
{
    
public:
    
    ArmyTypeConfig();
    
    ~ArmyTypeConfig();
    
    int parse();
    
    void clear();
    
    ArmyTypeItem* item(int key);
    
public:
    
    std::map<int, ArmyTypeItem* > _datas;
};

#endif // __ARMY_TYPE_CONFIG_H__
