#ifndef _SMITHY_CONFIG_H__
#define _SMITHY_CONFIG_H__

#include "config_data.h"
#include <vector>
#include <map>

class SmithyData
{
public:
    
    SmithyData();
    
    ~SmithyData();
    
public:
    
    int _id;
    int _name;
    int _expend;
    std::vector<int> _forging;
    int _num;
    std::vector<int> _greenType;
    std::vector<int> _blueType;
    std::vector<int> _purpleType;
    std::vector<int> _orangeType;
    std::vector<int> _blueNum;
    std::vector<int> _purpleNum;
    std::vector<int> _orangeNum;

};


class SmithyConfig : public ConfigData
{
    
public:
    
    SmithyConfig();
    
    ~SmithyConfig();
    
    int parse();
    
    void clear();
    
    SmithyData* item(int key);
    
public:
    
    std::map<int, SmithyData*> _datas;
};

#endif
