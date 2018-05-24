#ifndef __ALLIANCE_DONATE_CONFIG_H__
#define __ALLIANCE_DONATE_CONFIG_H__
#include "config_data.h"
#include <map>

class AllianceDonateItem
{
public:
    AllianceDonateItem()
    {
        ;
    }
    ~AllianceDonateItem()
    {
        ;
    }
public:
    int _id;
    int _alliancePoint;
    int _contribution;
    int _isClose;

    int _goldWeight;
    int _goldCost;
    int _goldUpgrade;
    int _goldCoolDown;

    int _gemWeight;
    int _gemCost;
    int _gemUpgrade;
    int _gemCoolDown;
};

class AllianceDonateConfig : public ConfigData
{
public:
    AllianceDonateConfig();
    ~AllianceDonateConfig();
public:
    int parse();
    void clear();
    AllianceDonateItem* item(int key);
public:
    std::map<int, AllianceDonateItem*> _datas;
};


#endif  //__ALLIANCE_DONATE_CONFIG_H__
