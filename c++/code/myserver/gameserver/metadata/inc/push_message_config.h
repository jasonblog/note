#ifndef __PUSH_MESSAGE_CONFIG_H__
#define __PUSH_MESSAGE_CONFIG_H__

#include "config_data.h"
#include <vector>
#include <map>
#include <string>

class PushMessageItem
{
    
public:
    
    PushMessageItem();
    
    ~PushMessageItem();
    
public:
    
    std::string _id;
    std::string _cn;
    std::string _en;
    std::string _de;
    
    const std::string& getByLanguage(const std::string& language);
};

class PushMessageConfig : public ConfigData
{
    
public:
    
    PushMessageConfig();
    
    ~PushMessageConfig();
    
    int parse();
    
    void clear();
    
    PushMessageItem* item(const std::string& key);
    
public:
    
    std::map<std::string, PushMessageItem*> _datas;
};


#endif //__PUSH_MESSAGE_CONFIG_H__
