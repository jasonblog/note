#include "grid_config.h"
#include "../common_err.h"

GridItem::GridItem()
{
    
}

GridItem::~GridItem()
{
    
}

GridConfig::GridConfig()
{
    _datas.clear();
}

GridConfig::~GridConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int GridConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new GridItem();
        
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto gridType = item_obj["gridType"];
        item->_id = oid;
        item->_gridType = gridType;
        _datas[oid] = item;
        
    }
    return 0;
}

void GridConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

GridItem* GridConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("GridConfig::item:{}", key);
}
