//
//  month_card_config.h
//  cserver
//
//  Created by zhaolong on 16/8/29.
//
//

#ifndef month_card_config_h
#define month_card_config_h


#include "config_data.h"
#include <map>


class MonthCardCfgInfo
{
public:
    int _id;                //id
    int _rechargeIOS;       //IOS充值档
    int _rechargeAndroid;   //Android充值档
    int _rewardGem;         //充值获得
    int _dailyReward;       //每日奖励
    int _times;             //有效期

public:
    MonthCardCfgInfo();
    ~MonthCardCfgInfo();

};

class MonthCardConfig : public ConfigData
{
    
public:
    
    MonthCardConfig();
    
    ~MonthCardConfig();
    
    int parse();
    
    bool check();
    
    void clear();
    
    MonthCardCfgInfo* getCfg(int nIndex);               //根据配置id 获取配置信息
public:
    std::map<int, MonthCardCfgInfo*> _datas;
};


#endif /* month_card_config_h */
