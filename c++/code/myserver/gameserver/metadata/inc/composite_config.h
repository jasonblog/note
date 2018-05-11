#ifndef __COMPOSITE_CONFIG_H__
#define __COMPOSITE_CONFIG_H__

#include "config_data.h"

#include <map>
#include <string>

class CompositeItem
{
    
public:
    
    CompositeItem();
    
    ~CompositeItem();
    
public:
    
    int _id;
    std::map<std::string, std::vector<int>* > _material;
    std::map<std::string, std::vector<int>* > _composite;
};

class CompositeConfig : public ConfigData
{
    
public:
    
    CompositeConfig();
    
    ~CompositeConfig();
    
    
    int parse();
    
    void clear();
    
    CompositeItem* item(int key);
    
public:
    
    std::map<int, CompositeItem*> _datas;
};

#endif // __COMPOSITE_CONFIG_H__
