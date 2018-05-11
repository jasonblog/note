#include "grid_limit_config.h"

GridLimitItem::GridLimitItem()
{
    _unLockCost.clear();
    _unlockReward.clear();
    _gridID.clear();
}

GridLimitItem::~GridLimitItem()
{
    for (auto it=_unLockCost.begin(); it!=_unLockCost.end(); ++it) {
        it->second->clear();
        delete it->second;
    }
    
    _unLockCost.clear();
    
    for (auto it=_unlockReward.begin(); it!=_unlockReward.end(); ++it) {
        it->second->clear();
        delete it->second;
    }
    
    _unlockReward.clear();
    
    _gridID.clear();
}

GridLimitConfig::GridLimitConfig()
{
    
}

GridLimitConfig::~GridLimitConfig()
{
    
}

int GridLimitConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        
        auto item = new GridLimitItem();
        
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto unLockLimit = item_obj["unLockLimit"].ToInt();
        auto unlockCost = item_obj["unlockCost"].ToObject();
        auto unlockReward = item_obj["unlockReward"].ToObject();
        auto gridID = item_obj["gridId"].ToArray();
        
        item->_id = oid;
        item->_unLockLimit = unLockLimit;
        
        for (auto mit = unlockCost.begin(); mit!=unlockCost.end(); ++mit) {
            auto cost = new std::vector<int>;
            auto l = mit->second.ToArray();
            for (auto it = l.begin(); it!=l.end(); ++it) {
                cost->push_back(it->ToInt());
            }
            item->_unLockCost[mit->first] = cost;
        }
        
        for (auto mit = unlockReward.begin(); mit!=unlockReward.end(); ++mit) {
            auto cost = new std::vector<int>;
            auto l = mit->second.ToArray();
            for (auto it = l.begin(); it!=l.end(); ++it) {
                cost->push_back(it->ToInt());
            }
            item->_unlockReward[mit->first] = cost;
        }
        
        for (auto mit = gridID.begin(); mit!=gridID.end(); ++mit) {
            item->_gridID.push_back(mit->ToInt());
            
        }
        _datas[oid] = item;
    }
    return 0;
}

void GridLimitConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

GridLimitItem* GridLimitConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return 0;
}
