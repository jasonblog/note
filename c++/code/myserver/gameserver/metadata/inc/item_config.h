#ifndef __ITEM_CONFIG_H__
#define __ITEM_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>

class ItemItem
{
    
public:
    
    ItemItem();
    
    ~ItemItem();
    
public:
    
    int _id;
    int _quality;
    int _rank;
    int _usage;
    int _type;
    int _parameter1;
    float _parameter2;
    int _parameter3;
    int _shopid;
    int _useLimit;
    int _batch;
    int _stack;
    int _ownLimit;
    std::vector<int> _liveTime;
    int _dropid;

};

class ItemConfig : public ConfigData
{
    
public:
    
    ItemConfig();
    
    ~ItemConfig();
    
    int parse();
    
    void clear();
    
    ItemItem* item(int key);
    
public:
    
    std::map<int, ItemItem*> _datas;
};
#endif // __ITEM_CONFIG_H__

