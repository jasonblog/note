#include "monster_config.h"
#include "../common_err.h"

MonsterItem::MonsterItem()
{
    _firstKill.clear();
    _reward.clear();
}

MonsterItem::~MonsterItem()
{
    for (auto it = _firstKill.begin(); it!=_firstKill.end(); ++it) {
        it->second->clear();
        delete it->second;
    }
    _firstKill.clear();
    
    for (auto it = _reward.begin(); it!=_reward.end(); ++it) {
        it->second->clear();
        delete it->second;
    }
    _reward.clear();
}

MonsterConfig::MonsterConfig()
{
    _datas.clear();
}

MonsterConfig::~MonsterConfig()
{
    clear();
}

int MonsterConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto one = new MonsterItem();
        auto obj_item = it->second.ToObject();
        one->_id = obj_item["id"].ToInt();
        one->_level = obj_item["level"].ToInt();
        one->_armyid = obj_item["armyID"].ToInt();
        one->_num = obj_item["num"].ToInt();
        one->_unlock = obj_item["unlock"].ToInt();
        auto firstkill = obj_item["firstKillReward"].ToObject();
        for (auto x=firstkill.begin(); x!=firstkill.end(); ++x) {
            auto v = new std::vector<int>;
            one->_firstKill[x->first] = v;
            auto l = x->second.ToArray();
            for (auto vl=l.begin(); vl!=l.end(); ++vl) {
                v->push_back(vl->ToInt());
            }
        }
        auto reward = obj_item["reward"].ToObject();
        for (auto x=reward.begin(); x!=reward.end(); ++x) {
            auto v = new std::vector<int>;
            one->_reward[x->first] = v;
            auto l = x->second.ToArray();
            for (auto vl=l.begin(); vl!=l.end(); ++vl) {
                v->push_back(vl->ToInt());
            }
        }
        one->_restore = obj_item["restore"].ToFloat();
        one->_hurt = obj_item["hurt"].ToFloat();
        one->_type = obj_item["type"].ToInt();
        auto itFind = _datasType.find(one->_type);
        if (itFind == _datasType.end())
        {
            std::vector<MonsterItem*> tmp;
            tmp.push_back(one);
            _datasType[one->_type] = tmp;
        }
        else
        {
            itFind->second.push_back(one);
        }
        
        _datas[one->_id] = one;
    }
    return 0;
}

void MonsterConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

MonsterItem* MonsterConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("MonsterConfig::item:{}", key);
}

std::vector<MonsterItem*> MonsterConfig::itemType(int type)
{
    std::vector<MonsterItem*> tmp;
    auto it = _datasType.find(type);
    if (it!=_datasType.end())
    {
        tmp = it->second;
    }
    
    return tmp;
}
