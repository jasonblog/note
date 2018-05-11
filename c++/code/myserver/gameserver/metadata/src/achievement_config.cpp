#include "achievement_config.h"

AchievementItem::AchievementItem()
{
    _id = 0;
    _starlv = 0;
    _type = 0;
    _cond = 0;
    _reward = 0;
}

AchievementItem::~AchievementItem()
{
    _id = 0;
    _starlv = 0;
    _type = 0;
    _cond = 0;
    _reward = 0;
}

AchievementConfig::AchievementConfig()
{
    _datas.clear();
}

AchievementConfig::~AchievementConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int AchievementConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new AchievementItem;
        auto value = it->second.ToObject();
        auto oid = value["id"].ToInt();
        auto starlv = value["starlv"].ToInt();
        auto type = value["tpye"].ToInt();
        auto condition = value["condition"].ToInt();
        auto reward = value["reward"].ToInt();
        item->_id = oid;
        item->_starlv = starlv;
        item->_type = type;
        item->_cond = condition;
        item->_reward = reward;
        _datas[oid] = item;
    }
    return 0;
}

void AchievementConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

AchievementItem* AchievementConfig::item(int key)
{
    auto it=_datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
}
