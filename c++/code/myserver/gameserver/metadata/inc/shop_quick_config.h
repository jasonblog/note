#ifndef __SHOP_QUICK_CONFIG_H__
#define __SHOP_QUICK_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>

class ShopQuickItem
{
    
public:
    
    ShopQuickItem();
    
    ~ShopQuickItem();
    
public:
    
    int _iId;
    int _iShop;;
    int _iRrank;
    int _iItem;
    int _iNum;
    int _iExpend;
    int _iUse;    
};

class ShopQuickConfig : public ConfigData
{
    
public:
    
    ShopQuickConfig();
    
    ~ShopQuickConfig();
    
    int parse();
    
    void clear();
    
    ShopQuickItem* item(int key);
    
public:
    
    std::map<int, ShopQuickItem*> _datas;
};

#endif // __GOODS_CONFIG_H__