#ifndef __DAILY_CONFIG_H__
#define __DAILY_CONFIG_H__

#include "config_data.h"

#include <map>

class DailyItem
{
    
public:
    
    DailyItem();
    
    ~DailyItem();
    
public:
    
    int _id;
    
    int _unlock;
    
    int _type;
    
    int _condition;
    
    int _score;
    
    int _times;
    
};

class DailyConfig : public ConfigData
{
    
public:
    
    DailyConfig();
    
    ~DailyConfig();
    
    int parse();
    
    void clear();
    
    DailyItem* item(int key);
    
public:
    
    std::map<int, DailyItem*> _datas;
};

#endif // __DAILY_CONFIG_H__
