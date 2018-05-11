#include "alliance_reward_config.h"
AllianceRewardItem::AllianceRewardItem()
{
    _rewards.clear();
}

AllianceRewardItem::~AllianceRewardItem()
{
    for (auto itr = _rewards.begin(); itr != _rewards.end(); ++itr) {
        delete itr->second;
    }
    _rewards.clear();
}

AllianceRewardConfig::AllianceRewardConfig()
{
    _datas.clear();
}

AllianceRewardConfig::~AllianceRewardConfig()
{
    for (auto itr = _datas.begin(); itr != _datas.end(); ++itr) {
        delete itr->second;
    }
    _datas.clear();
}

void AllianceRewardConfig::clear()
{
    for (auto itr = _datas.begin(); itr != _datas.end(); ++itr) {
        delete itr->second;
    }
    _datas.clear();
}

AllianceRewardItem* AllianceRewardConfig::item(int key) {
    auto itr = _datas.find(key);
    if (itr != _datas.end()) {
        return itr->second;
    }
    return NULL;
}

int AllianceRewardConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto itr = obj.begin(); itr != obj.end(); ++itr) {
        auto item = new AllianceRewardItem;
        auto item_obj = itr->second.ToObject();
        item->_id = item_obj["id"].ToInt();
        item->_contribution = item_obj["contribution"].ToInt();
        auto rewards = item_obj["reward"].ToObject();
        for (auto mit = rewards.begin(); mit != rewards.end(); ++mit) {
            auto v = new std::vector<int>;
            item->_rewards[mit->first] = v;
            auto ar = mit->second.ToArray();
            for (auto l = ar.begin(); l != ar.end(); ++l) {
                v->push_back(l->ToInt());
            }
        }
        _datas[item->_id] = item;
    }
    return 0;
}

