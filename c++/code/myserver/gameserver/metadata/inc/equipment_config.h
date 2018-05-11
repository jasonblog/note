#ifndef __EQUIPMENT_CONFIG_H__
#define __EQUIPMENT_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>

class EquipmentItem
{
public:
    
    EquipmentItem();
    
    ~EquipmentItem();
    
public:
    
    int _iId;
    int _iEquipId;
    int _iType;
    int _iColor;
    int _iLevel;
    int _iExp;
    int _iNext;
    
    float _fZsAtk;
    float _fZsHp;
    float _fGsAtk;
    float _fGsHp;
    float _fQbAtk;
    float _fQbHp;
    float _fFlAtk;
    float _fFlHp;
    float _fFsAtk;
    float _fFsHp;
    float _fGold;
    float _fWood;
    float _fStone;
    float _fIron;
    float _fResource;
    float _fAtk;
    float _fDef;
    
    float _fTroop;
    float _fHospital;
    float _fLoad;
    float _fTspeed;
    float _fNjatk;
    float _fMspeed;
};

class EquipmentConfig : public ConfigData
{
    
public:
    
    EquipmentConfig();
    
    ~EquipmentConfig();
    
    int parse();
    
    void clear();
    
    const EquipmentItem* item(const int key) const;
private:
    void getSmithyRewardData();
    
public:
    
    std::map<int, EquipmentItem*> _datas;
    //key:装备品质 value:key:装备类型 value:装备configid 供铁匠铺抽奖使用
    std::map<int,std::map<int,std::vector<int> > >* _colors;
};

#endif // __EQUIPMENT_CONFIG_H__