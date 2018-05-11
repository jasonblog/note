#include "../inc/parameter_config.h"

ParameterItem::ParameterItem()
{
    _common.clear();
    _vi.clear();
}

ParameterItem::~ParameterItem()
{
    
}

ParameterConfig::ParameterConfig()
{
    _datas.clear();
}

ParameterConfig::~ParameterConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int ParameterConfig::parse()
{
    auto obj = m_Obj.GetArray();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        auto v = it->GetObject();
        std::string key = v["id"].GetString();
        Value& value = v["value"];
        ParameterItem* item = new ParameterItem;
        if (key == "initialBuilding") {
            auto bidList = value.GetObject();
            auto noMap = item->GetiiMap();
            for (auto it = bidList.begin(); it != bidList.end(); ++it){
                noMap[atoi(it->name.GetString())] = it->value.GetInt();
            }
        }
        else if (key == "initialBuildingTest") {
            auto obj = value.GetObject();
            auto noMap = item->GetiiMap();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                noMap[atoi(it->name.GetString())] = it->value.GetInt();
            }
        }
        else if (key == "MonsterDailyMax")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "monsterMaxWeek")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "massedCastleLevel")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "massTeamNum")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "massTime")
        {
            auto ar = value.GetArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "ChainMilitaryTent_3")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "ChainMilitaryTent_5")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "ChainMilitaryTent_7")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "ChainMilitaryTent_9")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "ChainMilitaryTent_11")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "ChainRes_3")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "ChainRes_5")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "ChainRes_7")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "ChainRes_9")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "ChainRes_11")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "AllianceInviteRefreshTime")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "LikeInRanking")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "DataVersion")
        {
            item->_str = value.GetString();
        }
        else if (key == "JoinAllianceHonor")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "initialLand")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "spyPlayerSkillNeedWatchTowerLevel")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "initialBuildTime")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "version")
        {
            item->_str = value.GetString();
        }
        else if (key == "worldMapLimitPerArea")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "initTaskFirst")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "initialHead")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "ChangeTalentCost")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "initialPlayerLevel")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "resource_for_InitUser")
        {
            auto obj = value.GetObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_iimap[atoi(it->name.GetString())] = it->value.GetInt();
            }
        }
        else if (key == "initialItem")
        {
            auto obj = value.GetObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_iimap[atoi(it->name.GetString())] = it->value.GetInt();
            }
        }
        else if (key == "playerHead")
        {
            auto ar = value.GetArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "worldMapGridModel")
        {
            auto ar = value.GetArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "worldMapAreaOrder")
        {
            auto ar = value.GetArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "hospitalSoldier")
        {
            auto ar = value.GetArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "dailyScore")
        {
            auto ar = value.GetArray();
            for (auto it=ar.begin(); it!=ar.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if(key == "changeGuildNamePrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "changeGuildAbbreviationPrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "replaceGuildOwner")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "buildGuildCastleLevel")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "freeBuildGuildCastleLevel")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "buildGuildPrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "joinGuildReward")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "joinGuildReward")
        {
           item->_integer = value.GetInt();
        }
        else if (key == "recommendMemberRefreshPrice")
        {
            item->_integer = value.GetInt();

        }
        else if (key == "trapInitial")/////
        {
            auto obj = value.GetObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_iimap[atoi(it->name.GetString())] = it->value.GetInt();
            }
        }
        else if (key == "changePositionPrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "initArmyFirst") ////
        {
            auto obj =value.GetObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_iimap[atoi(it->name.GetString())] = it->value.GetInt();
            }
        }
        else if (key == "changeNamePrice")
        {
            item->_integer = value.GetInt();
        }else if (key == "fristChangeNameRewards")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "changeHeadPrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "changeHeadTime")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "farmOutputTime")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "NumberTroopDispatchFirst")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "outfirePrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "wallRepairValue")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "wallRepairTime")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "wishFreeTimes")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "wishCri")
        {
            auto obj = value.GetObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_iimap[atoi(it->name.GetString())] = it->value.GetInt();
            }
        }
        else if (key == "wishStoneCastleLevel")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "wishIronCastleLevel")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "resourceReturn")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "initialTraining")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "worldMapTimePerGrid")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "worldMapSpeedUpTimes")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "TowerUnlockLevel")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "worldMapSpeedUp")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "woundOreder")
        {
            auto obj = value.GetArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "ResParam")
        {
            auto obj = value.GetArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "initBuildFirst")
        {
            auto obj = value.GetObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_iimap[atoi(it->name.GetString())] = it->value.GetInt();
            }
        }
        else if (key == "farmOutputPer")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "skillInitial") ////
        {
            auto obj = value.GetObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_iimap[atoi(it->name.GetString())] = it->value.GetInt();
            }
        }
        else if (key == "DefaultArmy") ////
        {
            auto obj = value.GetObject();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_iimap[atoi(it->name.GetString())] = it->value.GetInt();
            }
        }
        else if (key == "addSpeedPrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "massAddSpeedPrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "returnPrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "massReturnPrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "moveCityPrice")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "timeContributeJoinGuild")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "GuildShopLimit")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "BarUnlockLevel")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "GuildBenefitsTimes")
        {
            item->_integer = value.GetInt();
        }
        else if(key == "AllianceConflict")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "MarketTime")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "MarketSpeed")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "MarketProfit")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "MarketAdd")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "MarketAddParam")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "MarketReduce")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "MarketReduceParam")
        {
            item->_floater = value.GetFloat();
        }
        else if (key == "GiftWeight")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "SeasonReward")
        {
            auto obj = value.GetArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "DepotSpeed")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "collectLimit")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "PriceofSearch")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "ColourWeight")
        {
            auto obj = value.GetArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "TypeWeight")
        {
            auto obj = value.GetArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "IsPositiveWeight")
        {
            auto obj = value.GetArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "BeginTime")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "DragonBrowTime")
        {
            item->_integer = value.GetInt();
        }
        else if(key == "RobotLimitPerArea")
        {
            //机器人饱和度
            item->_floater = value.GetFloat();
        }
        else if(key == "RobotNum")
        {
            //初始机器人数量
            item->_integer = value.GetInt();
        }
        else if (key == "ObstacleNum")
        {
            auto obj = value.GetArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        else if (key == "RobotTalkInterval")
        {
            item->_integer = value.GetInt();
        }
        else if (key == "talent")
        {
            auto obj = value.GetArray();
            for (auto it=obj.begin(); it!=obj.end(); ++it) {
                item->_vi.push_back(it->GetInt());
            }
        }
        
        _datas[key] = item;
    }
    return 0;
}

void ParameterConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

const ParameterItem* ParameterConfig::item(const std::string& key) const
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
}
