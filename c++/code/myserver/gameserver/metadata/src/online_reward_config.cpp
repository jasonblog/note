#include "online_reward_config.h"
#include "../common_err.h"

OnlineRewardItem::OnlineRewardItem()
{
    _id = 0;
    _coldDown = 0;
    _rewards.clear();
}

OnlineRewardItem::~OnlineRewardItem()
{
    _id = 0;
    _coldDown = 0;
    for (auto it = _rewards.begin(); it!=_rewards.end(); ++it) {
        it->second->clear();
        delete it->second;
    }
    _rewards.clear();
}

OnlineRewardConfig::OnlineRewardConfig()
{
    _datas.clear();
}

OnlineRewardConfig::~OnlineRewardConfig()
{
    clear();
}

int OnlineRewardConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto one = new OnlineRewardItem();
        auto item = it->second.ToObject();
        auto oid = item["id"].ToInt();
        auto cold = item["coldDown"].ToInt();
        one->_id = oid;
        one->_coldDown = cold;
        auto reward = item["reward"].ToObject();
        
        for (auto x = reward.begin(); x!=reward.end(); ++x) {
            auto rewardlist = new std::vector<int>;
            auto l = x->second.ToArray();
            for (auto m = l.begin(); m!=l.end(); ++m) {
                rewardlist->push_back(m->ToInt());
            }
            one->_rewards[x->first] = rewardlist;
        }
        _datas[oid] = one;
    }
    return 0;
}

OnlineRewardItem* OnlineRewardConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("OnlineRewardConfig::item:{}", key);
}

void OnlineRewardConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}
