#include "shop_guild_config.h"
#include "../common_err.h"

//  联盟商店配置表
ShopGuildItem::ShopGuildItem()
{
    
}
ShopGuildItem::~ShopGuildItem()
{
    
}
//------------------------------------------
//------------------------------------------

ShopGuildConfig::ShopGuildConfig()
{
    _datas.clear();
}
ShopGuildConfig::~ShopGuildConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int ShopGuildConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto one = new ShopGuildItem();
        
        auto item_obj   = it->second.ToObject();
        
        auto id         = item_obj["id"].ToInt();           //  ID
        auto rank       = item_obj["rank"].ToInt();         //  排序
        auto item       = item_obj["item"].ToInt();         //  道具ID
        auto num        = item_obj["num"].ToInt();          //  数量
        auto expend     = item_obj["expend"].ToInt();       //  需要贡献
        auto isView     = item_obj["isView"].ToInt();       //  是否显示购买
        
        one->_id        = id;
        one->_rank      = rank;
        one->_item      = item;
        one->_num       = num;
        one->_expend    = expend;
        one->_isView    = isView;
        
        _datas[id] = one;
    }

    
    return 0;
}

void ShopGuildConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

ShopGuildItem* ShopGuildConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("ShopGuildConfig::item:{}", key);
}