#include "item_config.h"

ItemItem::ItemItem()
{

}

ItemItem::~ItemItem()
{

}

ItemConfig::ItemConfig()
{
    _datas.clear();
}

ItemConfig::~ItemConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int ItemConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new ItemItem();
        
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto quality = item_obj["quality"].ToInt();
        auto rank = item_obj["rank"].ToInt();
        auto usage = item_obj["usage"].ToInt();
        auto type = item_obj["type"].ToInt();
        auto parameter1 = item_obj["parameter1"].ToInt();
        auto parameter2 = item_obj["parameter2"].ToFloat();
        auto parameter3 = item_obj["parameter3"].ToInt();
        auto shopid = item_obj["shopId"].ToInt();
        auto useLimit = item_obj["useLimit"].ToInt();
        auto batch = item_obj["batch"].ToInt();
        auto stack = item_obj["stack"].ToInt();
        auto ownLimit = item_obj["ownLimit"].ToInt();
        auto liveTime = item_obj["liveTime"].ToArray();
        auto dropid = item_obj["drop"].ToInt();
        
        item->_id = oid;
        item->_quality = quality;
        item->_rank = rank;
        item->_usage = usage;
        item->_type = type;
        item->_parameter1 = parameter1;
        item->_parameter2 = parameter2;
        item->_parameter3 = parameter3;
        item->_shopid = shopid;
        item->_useLimit = useLimit;
        item->_batch = batch;
        item->_stack = stack;
        item->_ownLimit = ownLimit;
        for (auto it1 = liveTime.begin(); it1 != liveTime.end(); ++it1)
        {
            item->_liveTime.push_back(*it1);
        }
        item->_dropid = dropid;

        _datas[oid] = item;
        
    }
    return 0;
}

void ItemConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

ItemItem* ItemConfig::item(int key)
{
    auto it=_datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
}
