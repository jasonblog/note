#ifndef __ROUNDLE_CONFIG_H__
#define __ROUNDLE_CONFIG_H__

#include "config_data.h"
#include <map>

//  转盘配置
class RoundleItem
{
public:
    RoundleItem();
    ~RoundleItem();
    
public:
    int iID;                    //  编号
    int iType;                  //  类型
    int iOutput;                //  产出类型
    int iItem;                  //  道具ID
    int iChances;               //  概率
    std::vector<int> vRemove;   //  下次不会出现
};

class RoundleConfig : public ConfigData
{
public:
    RoundleConfig();
    ~RoundleConfig();
    
    int parse();
    
    void clear();
    
    RoundleItem* item(int key);
    
public:
    
    std::map<int, RoundleItem* > _datas;
};

#endif // __ROUNDLE_CONFIG_H__