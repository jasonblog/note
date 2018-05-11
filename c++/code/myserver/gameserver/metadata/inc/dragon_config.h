#ifndef __DRAGON_CONFIG_H__
#define __DRAGON_CONFIG_H__

#include "config_data.h"
#include <vector>

//  龙崖BUFF配置
class DragonItem
{
public:
    DragonItem();
    ~DragonItem();
    
public:
    int     iID;        //  ID
    int     iType;      //  类型
    int     iColour;    //  颜色
    std::vector<int>     vGold;      //  金币产量
    std::vector<int>     vWood;      //  木头产量
    std::vector<int>     vStone;     //  石头产量
    std::vector<int>     vIron;      //  铁矿产量
    std::vector<int>     vResource;  //  所有资源产量
    std::vector<int>     vAtk;       //  所有兵种攻击
    std::vector<int>     vDef;       //  所有兵种生命
    std::vector<int>     vTroop;     //  出征上限
    std::vector<int>     vSpeed;     //  行军速度
};

class DragonConfig : public ConfigData
{
public:
    DragonConfig();
    ~DragonConfig();
    
    int parse();
    
    void clear();
    
    DragonItem* item(int key);
    
public:
    
    std::map<int, DragonItem* > _datas;
};

#endif