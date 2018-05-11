//
//  sixday_config.h
//  Created by mac on 16/10/18.
//  @author: DGuco
//  六天活动
//
//


#ifndef __SIXDAY_CONFIG_H__
#define __SIXDAY_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>
#include <string>

typedef std::map<int,std::map<std::string, std::vector<int> >> SixDayScoreReward; //积分奖励
typedef std::map<int,std::map<std::string, std::vector<int> >> SixDayTaskReward;  //任务奖励

//  酒吧悬赏任务配置表
class SixDayItem
{
    
public:
    SixDayItem();
    ~SixDayItem();
    void reset();
    
    
public:
    int miId;            //ID
    int miOrder;         //顺序
    int miName;          //名称
    int miType;          //活动类型
    int miIsMass;        //是否需要集结
    int miTargetName;    //目标名称
    int miIsAlliance;      //是否联盟共享

    std::map<int,int>mmTargetLevel; //条件
    std::map<int,int>mmScoreCondition; //积分条件1
    SixDayScoreReward mmScoreReward;    //积分奖励
    SixDayTaskReward  mmReward;         //  任务奖励

    std::vector<int>maMessageReward; //单次积分奖励
    std::vector<int>maTarget; //单次积分条件
    std::vector<int>maScore; //单次积分奖励
  };

class SixDayConfig : public ConfigData
{
public:
    SixDayConfig();
    
    ~SixDayConfig();
    
    int parse();
    
    void clear();
    
    SixDayItem* item(int key);
    
public:
    
    std::map<int, SixDayItem* > _datas;
};

#endif // __SIXDAY_CONFIG_H__