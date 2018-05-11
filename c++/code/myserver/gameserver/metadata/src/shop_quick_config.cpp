#include "shop_quick_config.h"
#include "../common_err.h"

ShopQuickItem::ShopQuickItem()
{

}

ShopQuickItem::~ShopQuickItem()
{
    

}

ShopQuickConfig::ShopQuickConfig()
{
    _datas.clear();
}

ShopQuickConfig::~ShopQuickConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int ShopQuickConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new ShopQuickItem();
        
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto shop = item_obj["shop"].ToInt();
        auto rank = item_obj["rank"].ToInt();
        auto item_ = item_obj["item"].ToInt();
        auto num_ = item_obj["num"].ToInt();
        auto expend = item_obj["expend"].ToInt();
        auto use = item_obj["use"].ToInt();

        item->_iId = oid;
        item->_iShop = shop;
        item->_iRrank = rank;
        item->_iItem = item_;
        item->_iNum = num_;
        item->_iExpend = expend;
        item->_iUse = use;
        
        _datas[oid] = item;
        
    }
    return 0;
}

void ShopQuickConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

ShopQuickItem* ShopQuickConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("ShopQuickConfig::item:{}", key);
}
