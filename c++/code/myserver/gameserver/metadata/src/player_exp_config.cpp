#include "player_exp_config.h"
#include "../common_def.h"
#include "../package/common_resp.h"
#include "../global.h"

PlayerExpItem::PlayerExpItem()
{
    _rewards.clear();
    _dailyReward20.clear();
    _dailyReward40.clear();
    _dailyReward60.clear();
    _dailyReward80.clear();
    _dailyReward100.clear();
}

PlayerExpItem::~PlayerExpItem()
{
    for (auto it=_rewards.begin(); it!=_rewards.end(); ++it) {
        it->second->clear();
    }
    _rewards.clear();
    
    for (auto it=_dailyReward20.begin(); it!=_dailyReward20.end(); ++it) {
        it->second->clear();
    }
    _dailyReward20.clear();
    
    for (auto it=_dailyReward40.begin(); it!=_dailyReward40.end(); ++it) {
        it->second->clear();
    }
    _dailyReward40.clear();
    
    for (auto it=_dailyReward60.begin(); it!=_dailyReward60.end(); ++it) {
        it->second->clear();
    }
    _dailyReward60.clear();
    
    for (auto it=_dailyReward80.begin(); it!=_dailyReward80.end(); ++it) {
        it->second->clear();
    }
    _dailyReward80.clear();
    
    for (auto it=_dailyReward100.begin(); it!=_dailyReward100.end(); ++it) {
        it->second->clear();
    }
    _dailyReward100.clear();
}

PlayerExpConfig::PlayerExpConfig()
{
    _datas.clear();
}

PlayerExpConfig::~PlayerExpConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int PlayerExpConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new PlayerExpItem;
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto exp = item_obj["exp"].ToInt();
        auto goldSpeed = item_obj["goldSpeed"].ToInt();
        auto woodSpeed = item_obj["woodSpeed"].ToInt();
        auto stoneSpeed = item_obj["stoneSpeed"].ToInt();
        auto ironSpeed = item_obj["ironSpeed"].ToInt();
        auto diamondSpeed = item_obj["diamondSpeed"].ToInt();
        auto goldRoundle = item_obj["goldRoundle"].ToInt();
        auto woodRoundle = item_obj["woodRoundle"].ToInt();
        auto otherRoundle = item_obj["otherRoundle"].ToInt();
        auto spyPrice = item_obj["spyPrice"].ToInt();
        auto rewards = item_obj["rewards"].ToObject();
        
        auto dailyReward20 = item_obj["dailyReward20"].ToObject();
        auto dailyReward40 = item_obj["dailyReward40"].ToObject();
        auto dailyReward60 = item_obj["dailyReward60"].ToObject();
        auto dailyReward80 = item_obj["dailyReward80"].ToObject();
        auto dailyReward100 = item_obj["dailyReward100"].ToObject();
        
        
        item->_id = oid;
        item->_exp = exp;
        item->_goldSpeed = goldSpeed;
        item->_woodSpeed = woodSpeed;
        item->_stoneSpeed = stoneSpeed;
        item->_ironSpeed = ironSpeed;
        item->_diamondSpeed = diamondSpeed;
        item->_goldRoundle = goldRoundle;
        item->_woodRoundle = woodRoundle;
        item->_otherRoundle = otherRoundle;
        item->_collect_speed[PROPERTY::COIN] = goldSpeed;
        item->_collect_speed[PROPERTY::WOOD] = woodSpeed;
        item->_collect_speed[PROPERTY::STONE] = stoneSpeed;
        item->_collect_speed[PROPERTY::STEEL] = ironSpeed;
        item->_collect_speed[PROPERTY::MONEY] = diamondSpeed;
        item->_spyPrice = spyPrice;

        for (auto mit = rewards.begin(); mit!=rewards.end(); ++mit) {
            auto v = new std::vector<int>;
            item->_rewards[mit->first] = v;
            auto ar = mit->second.ToArray();
            for (auto l = ar.begin(); l!=ar.end(); ++l) {
                v->push_back(l->ToInt());
            }
        }
        
        for (auto mit = dailyReward20.begin(); mit!=dailyReward20.end(); ++mit) {
            auto v = new std::vector<int>;
            item->_dailyReward20[mit->first] = v;
            auto ar = mit->second.ToArray();
            for (auto l = ar.begin(); l!=ar.end(); ++l) {
                v->push_back(l->ToInt());
            }
        }
        
        for (auto mit = dailyReward40.begin(); mit!=dailyReward40.end(); ++mit) {
            auto v = new std::vector<int>;
            item->_dailyReward40[mit->first] = v;
            auto ar = mit->second.ToArray();
            for (auto l = ar.begin(); l!=ar.end(); ++l) {
                v->push_back(l->ToInt());
            }
        }
        
        for (auto mit = dailyReward60.begin(); mit!=dailyReward60.end(); ++mit) {
            auto v = new std::vector<int>;
            item->_dailyReward60[mit->first] = v;
            auto ar = mit->second.ToArray();
            for (auto l = ar.begin(); l!=ar.end(); ++l) {
                v->push_back(l->ToInt());
            }
        }
        
        for (auto mit = dailyReward80.begin(); mit!=dailyReward80.end(); ++mit) {
            auto v = new std::vector<int>;
            item->_dailyReward80[mit->first] = v;
            auto ar = mit->second.ToArray();
            for (auto l = ar.begin(); l!=ar.end(); ++l) {
                v->push_back(l->ToInt());
            }
        }
        
        for (auto mit = dailyReward100.begin(); mit!=dailyReward100.end(); ++mit) {
            auto v = new std::vector<int>;
            item->_dailyReward100[mit->first] = v;
            auto ar = mit->second.ToArray();
            for (auto l = ar.begin(); l!=ar.end(); ++l) {
                v->push_back(l->ToInt());
            }
        }
        
        _datas[oid] = item;
    }
    return 0;
}

void PlayerExpConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

bool PlayerExpConfig::check()
{
    for (auto it = _datas.begin(); it != _datas.end(); ++it)
    {
        //检测配置的奖品正确性
        if(!checkVectorIsItem(it->second->_rewards))
        {
            ERROR_LOG("ERR: check PlayerExpConfig _reward item id is err!!! id = {}", it->first);
            return false;
        }
    }
    return true;
}

PlayerExpItem* PlayerExpConfig::item(int key)
{
    auto it=_datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
}
