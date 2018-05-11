#ifndef __ACHIEVEMENT_CONFIG_H__
#define __ACHIEVEMENT_CONFIG_H__

#include "config_data.h"

#include <map>

class AchievementItem
{
    
public:
    
    AchievementItem();
    
    ~AchievementItem();
    
public:
    
    int _id;
    int _starlv;
    int _type;
    int _cond;
    int _reward;
};

class AchievementConfig : public ConfigData
{
    
public:
    
    AchievementConfig();
    
    ~AchievementConfig();
    
    int parse();
    
    void clear();
    
    AchievementItem* item(int key);
    
public:
    
    std::map<int, AchievementItem* > _datas;
};

#endif // __ACHIEVEMENT_CONFIG_H__
