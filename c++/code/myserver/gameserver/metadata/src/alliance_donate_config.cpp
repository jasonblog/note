#include "alliance_donate_config.h"
AllianceDonateConfig::AllianceDonateConfig()
{
    _datas.clear();
}

AllianceDonateConfig::~AllianceDonateConfig()
{
    for (auto itr = _datas.begin(); itr != _datas.end(); ++itr) {
        delete itr->second;
    }
    _datas.clear();
}

void AllianceDonateConfig::clear()
{
    for (auto itr = _datas.begin(); itr != _datas.end(); ++itr) {
        delete itr->second;
    }
    _datas.clear();
}

int AllianceDonateConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto itr = obj.begin(); itr != obj.end(); ++itr) {
        auto item = new AllianceDonateItem;
        auto item_obj = itr->second.ToObject();
        item->_id = item_obj["id"].ToInt();
        item->_alliancePoint = item_obj["alliancePoint"].ToInt();
        item->_contribution = item_obj["contribution"].ToInt();
        item->_isClose = item_obj["isClose"].ToInt();
        //=======
        item->_goldWeight = item_obj["goldWeight"].ToInt();
        item->_goldCost = item_obj["goldCost"].ToInt();
        item->_goldUpgrade= item_obj["goldUpgrade"].ToFloat()*100;
        item->_goldCoolDown= item_obj["goldCoolDown"].ToInt();
        //==========
        item->_gemWeight = item_obj["gemWeight"].ToInt();
        item->_gemCost = item_obj["gemCost"].ToInt();
        item->_gemUpgrade= item_obj["gemUpgrade"].ToFloat()*100;
        item->_gemCoolDown= item_obj["gemCoolDown"].ToInt();
        //==========
        _datas[item->_id] = item;
    }
    return 0;
}

AllianceDonateItem* AllianceDonateConfig::item(int key) {
    auto itr = _datas.find(key);
    if (itr != _datas.end()) {
        return itr->second;
    }
    return NULL;
}

