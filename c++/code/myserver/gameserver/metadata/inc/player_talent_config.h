#ifndef __PLAYER_TALENT_CONFIG_H__
#define __PLAYER_TALENT_CONFIG_H__


#include "config_data.h"

#include <map>
#include <vector>

enum TALENT_CONFIG_ID
{
    TALENT_5_1 = 1080001,
    TALENT_5_2 = 1080002,
    TALENT_5_3 = 1080003,
    
    TALENT_15_1 = 1080004,
    TALENT_15_2 = 1080005,
    TALENT_15_3 = 1080006,
    
    TALENT_25_1 = 1080007,
    TALENT_25_2 = 1080008,
    TALENT_25_3 = 1080009,

    TALENT_35_1 = 1080010,
    TALENT_35_2 = 1080011,
    TALENT_35_3 = 1080012,

    TALENT_45_1 = 1080013,
    TALENT_45_2 = 1080014,
    TALENT_45_3 = 1080015,

    TALENT_55_1 = 1080016,
    TALENT_55_2 = 1080017,
    TALENT_55_3 = 1080018,
};

class PlayerTalent
{
    
public:
    
    PlayerTalent();
    
    ~PlayerTalent();
    
public:
    
    int _id;
    int _unlock;
    std::vector<float> _para;
};

class PlayerTalentConfig : public ConfigData
{
    
public:
    
    PlayerTalentConfig();
    
    ~PlayerTalentConfig();
    
    int parse();
    
    void clear();
    
    PlayerTalent* item(int key);
    
public:
    
    std::map<int, PlayerTalent*> _datas;
};

#endif // __PLAYER_TALENT_CONFIG_H__

