//
//  config_manage.cpp
//  cserver
//
//  Created by zhaolong on 16/9/8.
//
//

#include "config_manage.h"
#include "../global.h"


int ConfigManage::load()
{
    if (g_buildConfig.loadFromFile("building_config.json")!=0)
        return -1;
    
    if (g_parameterConfig.loadFromFile("parameter_config.json")!=0)
         return -1;
    
    if (g_playerExpConfig.loadFromFile("player_exp_config.json")!=0)
        return -1;
    
    if (g_shopConfig.loadFromFile("shop_config.json")!=0)
        return -1;

    if (g_shopquickConfig.loadFromFile("shop_quick_config.json")!=0)
        return -1;
    
    if (g_buildAmountConfig.loadFromFile("building_amount_config.json")!=0)
        return -1;
    
    if (g_compositeConfig.loadFromFile("composite_config.json")!=0)
        return -1;
    
    if (g_itemConfig.loadFromFile("item_config.json")!=0)
        return -1;
    
    if (g_armyConfig.loadFromFile("army_config.json")!=0)
       return -1;
    
    if (g_gridConfig.loadFromFile("grid_config.json")!=0)
       return -1;
    
    if (g_gridLimitConfig.loadFromFile("grid_limit_config.json")!=0)
        return -1;
    
    if (g_taskConfig.loadFromFile("task_config.json")!=0)
        return -1;
    
    if (g_dailyConfig.loadFromFile("daily_config.json")!=0)
        return -1;
    
    if (g_achievementConfig.loadFromFile("achievement_config.json")!=0)
        return -1;
    
    if (g_armyTypeConfig.loadFromFile("army_type_config.json")!=0)
        return -1;
    
    if (g_rechargeConfig.loadFromFile("recharge_config.json") != 0)
        return -1;
    
    if (g_pushMessageConfig.loadFromFile("push_message_config.json") != 0)
        return -1;
    
    if (g_mapFieldConfig.loadFromFile("map_field_config.json") != 0)
        return -1;
    
    if (g_areaLevelConfig.loadFromFile("area_level_config.json") != 0)
        return -1;
    
    if (g_mapConfig.loadFromFile("map_config.json")!=0)
        return -1;
    
    if (g_allianceflagConfig.loadFromFile("alliance_flag_config.json")!=0)
        return -1;
    
    if (g_allianceRewardConfig.loadFromFile("alliance_reward_config.json")!= 0)
        return -1;
    
    if (g_allianceDonateConfig.loadFromFile("alliance_donate_config.json")!=0)
        return -1;
    
    if (g_playerTalentConfig.loadFromFile("player_talent_config.json")!=0)
        return -1;
   
    if (g_onlineRewardConfig.loadFromFile("online_reward_config.json")!=0)
        return -1;
    //  酒馆悬赏任务配置表
    if (g_missionConfig.loadFromFile("mission_config.json")!=0)
        return -1;
    //  联盟商店配置表
    if (g_shopGuildConfig.loadFromFile("shop_guild_config.json")!=0)
        return -1;
    //  领主装备
    if (g_equipmentConfig.loadFromFile("equipment_config.json")!=0)
        return -1;
    //  铁匠铺
    if (g_smithyConfig.loadFromFile("smithy_config.json")!=0)
        return -1;
    
    if (g_areaLevelMonsterConfig.loadFromFile("area_level_monster_config.json")!=0)
        return -1;
    
    if (g_monsterConfig.loadFromFile("monster_config.json")!=0)
        return -1;
    //  联盟科技配置表
    if (g_allianceTechConfig.loadFromFile("alliance_tech_config.json")!=0)
        return -1;
    //  联盟建筑配置表
    if (g_allianceBuildingConfig.loadFromFile("alliance_building_config.json") != 0)
        return -1;
    //  联盟建筑数量配置表
    if (g_allianceBuildingAmountConfig.loadFromFile("alliance_building_amount_config.json") != 0)
        return -1;
    //  七日登陆奖励配置表
    if (g_sevenDayConfig.loadFromFile("seven_day_config.json") != 0)
        return -1;
    //  领主技能配置表
    if (g_playerSkillConfig.loadFromFile("player_skill_config.json")!=0)
        return -1;
    //  系统邮件配置表
    if (g_systemMailConfig.loadFromFile("system_mail_config.json") != 0)
        return -1;
    //  VIP 配置表
    if (g_vipConfig.loadFromFile("vip_config.json") != 0)
        return -1;
    //  市场配置表
    if (g_MarketConfig.loadFromFile("market_config.json") != 0)
        return -1;
    //  月卡配置表
    if (g_MonthCardConfig.loadFromFile("month_card_config.json") != 0)
        return -1;
    //  充值礼包
    if (g_GiftConfig.loadFromFile("gift_config.json") != 0)
        return -1;
    //  联盟礼包
    if (g_AllianceGiftConfig.loadFromFile("alliance_gift_config.json") != 0)
        return -1;
    //  转盘
    if (g_RoundleConfig.loadFromFile("roundle_config.json") != 0)
    {
        return -1;
    }
    //  宝箱
    if (g_ChestConfig.loadFromFile("chest_config.json") != 0)
    {
        return -1;
    }

    //  六天活动
    if (g_SixDayConfig.loadFromFile("sixday_config.json") != 0)
        return -1;
    
    //  龙崖BUFF
    if (g_DragonConfig.loadFromFile("dragon_config.json") != 0)
    {
        return -1;
    }
    
    //机器人城堡
    if (g_RobotConfig.loadFromFile("robot_config.json") != 0)
    {
        return -1;
    }
    
    //机器人名字库
    if (g_RobotNameConfig.loadFromFile("random_name_config.json") != 0)
    {
        return -1;
    }
    
    //  地图装饰
    if (g_MapObstacleConfig.loadFromFile("map_obstacle_config.json") != 0)
    {
        return -1;
    }
    
    //掉落配置表
    if (g_dropConfig.loadFromFile("drop_config.json") != 0)
    {
        return -1;
    }
    //机器人聊天配置表
    if (g_RobotTalkConfig.loadMe() != 0)
    {
        return -1;
    }
    
    //节日活动
    if (g_FestlActivityConfig.load() != 0)
    {
        return -1;
    }
    
    return ConfigManage::check();
}

int ConfigManage::reload()
{
    return ConfigManage::check();
}

int ConfigManage::check()
{
    if (!g_playerExpConfig.check())
        return -1;
    
    if (!g_sevenDayConfig.check())
        return -1;
    
    if (!g_MonthCardConfig.check())
        return -1;
    
    if (!g_GiftConfig.check())
        return -1;
    
    if (!g_AllianceGiftConfig.check())
        return -1;
    
    if (!g_taskConfig.check())
        return -1;
    
    if (!g_FestlActivityConfig.check())
        return -1;
    
    return 0;
}
