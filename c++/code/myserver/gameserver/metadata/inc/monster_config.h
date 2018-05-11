#ifndef __MONSTER_CONFIG_H__
#define __MONSTER_CONFIG_H__

#include "config_data.h"

#include <map>
#include <string>
#include <vector>

class MonsterItem
{
    
public:
    
    MonsterItem();
    
    ~MonsterItem();
    
public:
    
    int _id;
    int _level;
    int _armyid;
    int _num;
    int _unlock;
    std::map<std::string, std::vector<int>* > _firstKill;
    std::map<std::string, std::vector<int>* > _reward;
    
    float _restore;
    float _hurt;
    int _type;
};

class MonsterConfig : public ConfigData
{
    
public:
    
    MonsterConfig();
    
    ~MonsterConfig();
    
    int parse();
    
    void clear();
    
    MonsterItem* item(int key);
    std::vector<MonsterItem*> itemType(int type);
    
public:
    
    std::map<int, MonsterItem* > _datas;
    std::map<int, std::vector<MonsterItem*>> _datasType;
};


#endif // __MONSTER_CONFIG_H__

