#include "shop_config.h"
#include "../common_err.h"

ShopItem::ShopItem()
{

}

ShopItem::~ShopItem()
{
    

}

ShopConfig::ShopConfig()
{
    _datas.clear();
}

ShopConfig::~ShopConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int ShopConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new ShopItem();
        
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto shop = item_obj["shop"].ToInt();
        auto rank = item_obj["rank"].ToInt();
        auto item_ = item_obj["item"].ToInt();
        auto num_ = item_obj["num"].ToInt();
        auto price = item_obj["price"].ToInt();
        auto discount = item_obj["discount"].ToFloat();
        auto expend = item_obj["expend"].ToInt();
        item->_id = oid;
        item->_shop = shop;
        item->_rank = rank;
        item->_item = item_;
        item->_num = num_;
        item->_price = price;
        item->_discount = discount;
        item->_expend = expend;
        
        _datas[oid] = item;
        
    }
    return 0;
}

void ShopConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

ShopItem* ShopConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("ShopConfig::item:{}", key);
}
