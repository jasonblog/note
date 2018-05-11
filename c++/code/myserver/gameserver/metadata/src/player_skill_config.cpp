#include "player_skill_config.h"

PlayerSkillItem::PlayerSkillItem()
{
    
}

PlayerSkillItem::~PlayerSkillItem()
{
    
}

PlayerSkillConfig::PlayerSkillConfig()
{
    _datas.clear();
}

PlayerSkillConfig::~PlayerSkillConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int PlayerSkillConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto item_obj   = it->second.ToObject();
        auto item = new PlayerSkillItem();
        
        int     id          = item_obj["id"].ToInt();           //  技能ID
        int     unlock      = item_obj["unlock"].ToInt();       //  解锁等级
        int     type        = item_obj["type"].ToInt();         //  类型
        int     level       = item_obj["level"].ToInt();        //  等级
        float   parameter1  = item_obj["parameter1"].ToFloat(); //  加成效果
        int     parameter2  = item_obj["parameter2"].ToInt();   //  持续时间
        int     cooldown    = item_obj["cooldown"].ToInt();     //  冷却时间
        int     gold        = item_obj["gold"].ToInt();         //  升级所需金币
        int     time        = item_obj["time"].ToInt();         //  升级所需时间
        int     exp         = item_obj["exp"].ToInt();          //  领主经验
        int     next        = item_obj["next"].ToInt();         //  下一等级
        
        item->_id           = id;
        item->_unlock       = unlock;
        item->_type         = type;
        item->_level        = level;
        item->_parameter1   = parameter1;
        item->_parameter2   = parameter2;
        item->_cooldown     = cooldown;
        item->_gold         = gold;
        item->_time         = time;
        item->_exp          = exp;
        item->_next         = next;
        
        _datas[id] = item;
    }

    
    return 0;
}

void PlayerSkillConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

PlayerSkillItem* PlayerSkillConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    
    return NULL;
}
