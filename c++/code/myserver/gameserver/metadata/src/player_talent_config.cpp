#include "player_talent_config.h"
#include "../common_err.h"

PlayerTalent::PlayerTalent()
{
    _para.clear();
}

PlayerTalent::~PlayerTalent()
{
    _para.clear();
}

PlayerTalentConfig::PlayerTalentConfig()
{
    _datas.clear();
}

PlayerTalentConfig::~PlayerTalentConfig()
{
    clear();
}

int PlayerTalentConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto tal = new PlayerTalent;
        auto item = it->second.ToObject();
        auto oid = item["id"].ToInt();
        auto unlock = item["unlock"].ToInt();
        auto param = item["parameter"].ToArray();
        tal->_id = oid;
        tal->_unlock = unlock;
        for (auto ait=param.begin(); ait!=param.end(); ++ait) {
            tal->_para.push_back(ait->ToFloat());
        }
        _datas[oid] = tal;
    }

    return 0;
}

void PlayerTalentConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

PlayerTalent* PlayerTalentConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("PlayerTalentConfig::item:{}", key);
}
