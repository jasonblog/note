#ifndef __DAILY_REWARD_CONFIG_H__
#define __DAILY_REWARD_CONFIG_H__

class DailyRewardItem
{
public:
  
    DailyRewardItem();
    
    ~DailyRewardItem();
    
public:
    
    int _tid;
    
    int _status;
};

class DailyRewardConfig : public ConfigData
{
    
public:
    
    DailyRewardConfig();
    
    ~DailyRewardConfig();
    
    
    
};

#endif // __DAILY_REWARD_CONFIG_H__
