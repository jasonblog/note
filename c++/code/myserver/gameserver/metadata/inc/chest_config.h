#ifndef __CHEST_CONFIG_H__
#define __CHEST_CONFIG_H__

#include "config_data.h"
#include <vector>

//  宝箱配置
class ChestItem
{
public:
    ChestItem();
    ~ChestItem();
    
public:
    int     iID;        //  ID
    int     iTeam;      //  组别
    int     iType;      //  类型
    int     iItem;      //  道具ID
    int     iNum;       //  数量
    int     iOverTimes; //  加倍
    int     iPrice;     //  花费幸运币个数
    int     iChance;    //  几率
};

class ChestConfig : public ConfigData
{
public:
    ChestConfig();
    ~ChestConfig();
    
    int parse();
    
    void clear();
    
    ChestItem* item(int key);
    
public:
    
    std::map<int, ChestItem* > _datas;
    std::map<int, std::vector<ChestItem*>> _dataTeam;
};

#endif