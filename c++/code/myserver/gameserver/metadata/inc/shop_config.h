#ifndef __SHOP_CONFIG_H__
#define __SHOP_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>

class ShopItem
{
    
public:
    
    ShopItem();
    
    ~ShopItem();
    
public:
    
    int _id;
    int _shop;;
    int _rank;
    int _item;
    int _num;
    int _price;
    int _expend;
    float _discount;
};

class ShopConfig : public ConfigData
{
    
public:
    
    ShopConfig();
    
    ~ShopConfig();
    
    int parse();
    
    void clear();
    
    ShopItem* item(int key);
    
public:
    
    std::map<int, ShopItem*> _datas;
};

#endif // __GOODS_CONFIG_H__

