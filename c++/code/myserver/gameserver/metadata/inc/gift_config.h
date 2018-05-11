//
//  gift_config.h
//  cserver
//
//  Created by zhaolong on 16/8/31.
//
//

#ifndef gift_config_h
#define gift_config_h

#include "config_data.h"
#include <map>
#include <string>
#include <vector>

#define GIFT_NEW_PLAYER_ID 1320000              //新手礼包配置ID

class GiftCfgInfo
{
public:
    int _id;                                    //id
    int _type;                                  //礼包类别
    int _rechargeIOS;                           //IOS充值档
    int _rechargeAndroid;                       //Android充值档
    int _rewardGem;                             //充值获得
    int _times;                                 //有效期
    std::map<int, std::vector<int>> _reward;    //奖励的道具
    
public:
    GiftCfgInfo();
    ~GiftCfgInfo();

};

class GiftTypeList
{
private:
    std::vector<int> _list;       //配置ID
    
public:
    GiftTypeList();
    ~GiftTypeList();
    
public:
    void add(int nCfgID);
    //随机出一个ID 且不能和列表内的id重复
    int randomOnce(const std::map<int, int>&  mList);
};

class GiftConfig : public ConfigData
{
    
public:
    
    GiftConfig();
    
    ~GiftConfig();
    
    int parse();
    
    bool check();               //检测配置正确性
    
    void clear();
    
    GiftCfgInfo*    getCfg(int nIndex);                                     //根据配置id 获取配置信息
    GiftTypeList*   getIdByType(int nType);                                 //根据礼包档位 获取配置ID列表
    int             makeTypeByCfgID(int nIndex);                            //根据配置id 生成一个档位key 必须保证nIndex 的合法性 不然就返回-1
    int             makeTypeByCfgID(int rechargeIOS, int rechargeAndroid);  //根据配置的充值档id 生成一个档位key
    bool            initRate(std::map<int, int>& mRate, int nRate);         //用_datatype 初始化一个权重表
    int             getMaxType();                                           //获取最高充值档档key
    int             getNextType(int nType);                                 //获取下一档充值档key 不做最高充值档判断

private:
    std::map<int, GiftCfgInfo*>     _datas;             //ID = 配置信息
    std::map<int, GiftTypeList*>    _datatype;          //礼包档位 ＝ 对应的礼包id 列表
    int                             _maxtype;           //最高充值档
    
    enum
    {
        GIFT_MAKE_TYPE_VALUE = 10000,
    };
    
private:
    void addTypeList(int type, int id);
};


#endif /* gift_config_h */
