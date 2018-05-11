#ifndef __PLAYER_SKILL_CONFIG_H__
#define __PLAYER_SKILL_CONFIG_H__

#include "config_data.h"

//  玩家技能配置表
class PlayerSkillItem
{
public:
    PlayerSkillItem();
    
    ~PlayerSkillItem();
    
public:
    int     _id;        //  技能ID
    int     _unlock;    //  解锁等级
    int     _type;      //  技能类型
    int     _level;     //  技能等级
    float   _parameter1;//  加成效果
    int     _parameter2;//  持续时间
    int     _cooldown;  //  冷却时间
    int     _gold;      //  升级所需金币
    int     _time;      //  升级所需时间
    int     _exp;       //  领主经验
    int     _next;      //  下一等级
};

class PlayerSkillConfig : public ConfigData
{
public:
    PlayerSkillConfig();
    
    ~PlayerSkillConfig();
    
    int parse();
    
    void clear();
    
    PlayerSkillItem* item(int key);
    
public:
    
    std::map<int, PlayerSkillItem* > _datas;
};

#endif