#include "mission_config.h"
#include "../common_err.h"

//  酒吧悬赏任务配置表
MissionItem::MissionItem()
{
    _condition.clear();
    _reward.clear();
}
MissionItem::~MissionItem()
{
    for (auto it=_condition.begin(); it!=_condition.end(); ++it)
    {
        it->second->clear();
    }
    _condition.clear();
    
    for (auto it=_reward.begin(); it!=_reward.end(); ++it)
    {
        it->second->clear();
    }
    _reward.clear();
}
//------------------------------------------
//------------------------------------------

MissionConfig::MissionConfig()
{
    _datas.clear();
}

MissionConfig::~MissionConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int MissionConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto item = new MissionItem();
        
        auto item_obj   = it->second.ToObject();
        
        auto id         = item_obj["id"].ToInt();           //  ID
        auto unlock     = item_obj["unlock"].ToInt();       //  需要城堡等级
        auto quality    = item_obj["quality"].ToInt();      //  品质
        auto condition  = item_obj["condition"].ToObject(); //  完成条件
        auto reward     = item_obj["reward"].ToObject();    //  任务奖励
        auto next       = item_obj["next"].ToInt();         //  更高品质
        auto weight     = item_obj["weight"].ToInt();       //  刷新权重
        
        item->_id       = id;
        item->_unlock   = unlock;
        item->_quality  = quality;
        item->_next     = next;
        item->_weight   = weight;
        
        for (auto it=condition.begin(); it!=condition.end(); ++it)
        {
            auto v = new std::vector<int>;
            item->_condition[it->first] = v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit)
            {
                v->push_back(xit->ToInt());
            }
        }
        
        for (auto it=reward.begin(); it!=reward.end(); ++it)
        {
            auto v = new std::vector<int>;
            item->_reward[it->first] = v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit)
            {
                v->push_back(xit->ToInt());
            }
        }
        
        _datas[id] = item;
    }
    
    return 0;
}

void MissionConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

MissionItem* MissionConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("MissionConfig::item:{}", key);
}