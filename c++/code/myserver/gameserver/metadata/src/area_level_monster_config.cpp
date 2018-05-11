#include "area_level_monster_config.h"
#include "../common_err.h"

AreaLevelMonsterItem::AreaLevelMonsterItem()
{
    _monster.clear();
    _id = 0;
    _week = 0;
    _level = 0;
}

AreaLevelMonsterItem::~AreaLevelMonsterItem()
{
    _monster.clear();
    _id = 0;
    _week = 0;
    _level = 0;
}

AreaLevelMonsterConfig::AreaLevelMonsterConfig()
{
    _datas.clear();
}

AreaLevelMonsterConfig::~AreaLevelMonsterConfig()
{
    clear();
}

int AreaLevelMonsterConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto one = new AreaLevelMonsterItem();
        auto one_obj = it->second.ToObject();
        one->_id        = one_obj["id"].ToInt();
        one->_week      = one_obj["week"].ToInt();
        one->_level     = one_obj["level"].ToInt();
        auto monster    = one_obj["monster"].ToObject();
        
        for (auto x=monster.begin(); x!=monster.end(); ++x)
        {
            auto monsterInfo = x->second.ToArray();
            std::vector<int> tmp;
            for (auto y = monsterInfo.begin(); y != monsterInfo.end(); ++y)
            {
                tmp.push_back(*y);
            }
            one->_monster[x->first] = tmp;
        }
        
        _datas[one->_id] = one;
    }
    
    return 0;
}

void AreaLevelMonsterConfig::clear()
{
    CONFIG_DATA_CLEAR(_datas)
}

AreaLevelMonsterItem* AreaLevelMonsterConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("AreaLevelMonsterConfig::item:{}", key);
}

AreaLevelMonsterItem* AreaLevelMonsterConfig::getByLevelAndWeek(int level, int week)
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        if (it->second->_level == level && it->second->_week == week) {
            return it->second;
        }
    }
    return NULL;
//    throw ERROR::MSG("AreaLevelMonsterConfig::getByLevelAndWeek:{},{}", level, week);
}

std::vector<std::string> AreaLevelMonsterConfig::getByLevelAndWeek(int level, int week,int iMonsterId)
{
    std::vector<std::string> temp;
    AreaLevelMonsterItem* ptMonsterTmp = NULL;
    auto iMod = iMonsterId / 100;
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        if ((it->second->_level) == level && (it->second->_week == week))
        {
            ptMonsterTmp = it->second;
            break;
        }
    }
    if (ptMonsterTmp == NULL)
    {
        return temp;
    }
    
    for (auto its = ptMonsterTmp->_monster.begin(); its != ptMonsterTmp->_monster.end(); ++its)
    {
        auto iUid = atoi((its->first).c_str());
        if((iUid / 100) == iMod)
        {
            temp.push_back(its->first);
        }
    }
    return temp;
}

