#include "composite_config.h"
#include "../common_err.h"

CompositeItem::CompositeItem()
{
    _material.clear();
    _composite.clear();
}

CompositeItem::~CompositeItem()
{
    for (auto it=_material.begin(); it!=_material.end(); ++it) {
        it->second->clear();
        delete it->second;
    }
    _material.clear();
    
    for (auto it=_composite.begin(); it!=_composite.end(); ++it) {
        it->second->clear();
        delete it->second;
    }
    _composite.clear();
}

CompositeConfig::CompositeConfig()
{
    _datas.clear();
}

CompositeConfig::~CompositeConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int CompositeConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new CompositeItem;
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto material = item_obj["material"].ToObject();
        auto composite = item_obj["composite"].ToObject();
        
        item->_id = oid;
        for (auto mt=material.begin(); mt!=material.end(); ++mt) {
            auto vl = new std::vector<int>;
            item->_material[mt->first] = vl;
            auto v = mt->second.ToArray();
            for (auto ml=v.begin(); ml!=v.end(); ++ml) {
                vl->push_back(ml->ToInt());
            }
        }
        for (auto com=composite.begin(); com!=composite.end(); ++com) {
            auto vl = new std::vector<int>;
            item->_composite[com->first] = vl;
            auto v = com->second.ToArray();
            for (auto ml=v.begin(); ml!=v.end(); ++ml) {
                vl->push_back(ml->ToInt());
            }
        }
        _datas[oid] = item;
    }

    return 0;
}

void CompositeConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

CompositeItem* CompositeConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("CompositeConfig::item:{}", key);
}
