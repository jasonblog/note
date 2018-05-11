//
//  market_config.hpp
//  cserver
//
//  Created by zhaolong on 16/8/18.
//
//

#ifndef market_config_h
#define market_config_h


#include "config_data.h"
#include <map>
#include <vector>

class MarketCfgInfo
{
public:
    int     nID;        //id
    int     nCastle;    //需要的城堡等级
    float   fAdd;       //增益效果
    int     nBuy;       //购买价格
    int     nWeight;    //刷新权重
    
};

class MarketConfig : public ConfigData
{
    
public:
    
    MarketConfig();
    
    ~MarketConfig();
    
    int parse();
    
    void clear();
    
    MarketCfgInfo* getCfg(int nIndex);
    
    bool randomId(std::vector<int>& list, int nNum);  //随机取出指定数量商人列表
    
public:
    std::map<int, MarketCfgInfo*> _datas;
};


#endif /* market_config_h */
