#ifndef __TASK_CONFIG_H__
#define __TASK_CONFIG_H__

#include "config_data.h"
#include <vector>
#include <map>

class TaskConfItem
{
    
public:
    
    TaskConfItem();
    
    ~TaskConfItem();
    
public:
    
    int _id;
    
    int _cond;
    
    int _buildingType;
    
    int _num;
    
    int _next;
    
    std::map<std::string, std::vector<int>* > _reward;
};

class TaskConfig : public ConfigData
{
    
public:
    
    TaskConfig();
    
    ~TaskConfig();
    
    int parse();
    
    void clear();
    
    bool check();
    
    TaskConfItem* item(int key);
    
public:
    
    std::map<int, TaskConfItem* > _datas;
};


#endif // __TASK_CONFIG_H__
