//
//  alliance_gift_config.h
//  cserver
//
//  Created by zhaolong on 16/9/2.
//
//

#ifndef alliance_gift_config_h
#define alliance_gift_config_h

#include "config_data.h"
#include <map>
#include <string>
#include <vector>

class AllianceGiftCfgInfo
{
public:
    int _id;                                        //id
    int _rechargeIOS;                               //IOS充值档
    int _rechargeAndroid;                           //Android充值档
    int _alliancePoint;                             //联盟积分
    int _contribution;                              //购买人贡献度
    int _rewardGem;                                 //充值获得钻石
    std::map<int, std::vector<int>> _rewardItem;    //购买人道具奖励
    std::map<int, std::vector<int>> _playerReward;  //成员道具奖励
    int _mail;                                      //邮件id
    
public:
    AllianceGiftCfgInfo();
    ~AllianceGiftCfgInfo();
    
};

class AllianceGiftConfig : public ConfigData
{
    
public:
    
    AllianceGiftConfig();
    
    ~AllianceGiftConfig();
    
    int parse();
    
    bool check();               //检测配置正确性
    
    void clear();
    
    AllianceGiftCfgInfo*    getCfg(int nIndex);                                     //根据配置id 获取配置信息

private:
    std::map<int, AllianceGiftCfgInfo*>     _datas;             //ID = 配置信息
    
};



#endif /* alliance_gift_config_h */
