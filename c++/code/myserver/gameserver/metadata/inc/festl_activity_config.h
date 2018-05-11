//
//  festl_activity_config.h
//  cserver
//
//  Created by zhaolong on 16/12/10.
//
//

#ifndef festl_activity_config_h
#define festl_activity_config_h

#include <map>
#include <string>
#include <vector>
#include "config_data.h"

class FestlActivityChildCfg
{
public:
    int _id;                                    //id
    int _type;                                  //子活动类别
    int _drop;                                  //掉落ID
    int _target;                                //条件
    std::map<int, std::vector<int>> _targetStr; //多条件
    std::map<int, std::vector<int>> _reward;    //奖励的道具
    
public:
    FestlActivityChildCfg();
    ~FestlActivityChildCfg();
    
};

class FestlActivityInfo
{
public:
    FestlActivityInfo();
    ~FestlActivityInfo();
    
public:
    int _id;                    //编号
    int _aheadTime;             //提前显示时间
    int _openCondition;         //开启条件
    int _startDate;             //开始日期
    int _yearMode;              //记年方法  这个暂时不用 都用公历纪年
    int _duration;              //持续时间
    int _period;                //周期
    bool _isOpen;               //是否开启
    std::vector<int> _child;    //子活动ID列表
    
};

class FestlActivityConfig : public ConfigData
{
    
public:
    
    FestlActivityConfig();
    
    ~FestlActivityConfig();
    
    int parse();
    
    bool check();               //检测配置正确性
    
    void clear();
    
    int load();
    
    FestlActivityInfo*      getCfg(int nIndex);                                     //根据配置id 获取配置信息
    FestlActivityChildCfg*  getChildCfg(int nChildID);

public:
    //这个变量public 方便使用
    std::map<int, FestlActivityInfo>        _base;     //ID = 活动信息
    
private:
    std::map<int, FestlActivityChildCfg>   _child;    //ID = 子活动信息
    int mnLoadType;                                    //记录当前加载的配置表
    
};



#endif /* festl_activity_config_h */
