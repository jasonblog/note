#ifndef __AREA_LEVEL_MONSTER_CONFIG_H__
#define __AREA_LEVEL_MONSTER_CONFIG_H__

#include "config_data.h"

#include <map>
#include <string>
#include <vector>

class AreaLevelMonsterItem
{
    
public:
    
    AreaLevelMonsterItem();
    
    ~AreaLevelMonsterItem();
    
public:
    
    int _id;
    int _week;
    int _level;
    std::map<std::string, std::vector<int>> _monster;
};

class AreaLevelMonsterConfig : public ConfigData
{
    
public:
    
    AreaLevelMonsterConfig();
    
    ~AreaLevelMonsterConfig();
    
    int parse();
    
    void clear();
    
    AreaLevelMonsterItem* item(int key);
    
    AreaLevelMonsterItem* getByLevelAndWeek(int level, int week);
    std::vector<std::string> getByLevelAndWeek(int level, int week,int iMonsterId);
    
public:
    
    std::map<int, AreaLevelMonsterItem* > _datas;
};


#endif // __AREA_LEVEL_MONSTER_CONFIG_H__

