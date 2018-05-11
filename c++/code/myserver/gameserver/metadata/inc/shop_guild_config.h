#ifndef __SHOP_GUILD_CONFIG_H__
#define __SHOP_GUILD_CONFIG_H__

#include "config_data.h"

#include <map>

//  联盟商店配置表
class ShopGuildItem
{
public:
    ShopGuildItem();
    ~ShopGuildItem();
    
public:
    int _id;        //  ID
    int _rank;      //  排序
    int _item;      //  道具ID
    int _num;       //  数量
    int _expend;    //  需要贡献
    int _isView;    //  是否显示购买
};

class ShopGuildConfig : public ConfigData
{
public:
    ShopGuildConfig();
    
    ~ShopGuildConfig();
    
    int parse();
    
    void clear();
    
    ShopGuildItem* item(int key);
    
public:
    
    std::map<int, ShopGuildItem* > _datas;
};

#endif