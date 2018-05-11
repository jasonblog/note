//
//  robot_config.h
//  cserver
//
//  Created by zhaolong on 16/11/15.
//
//

#ifndef robot_config_h
#define robot_config_h


#include "config_data.h"
#include <map>
#include <string>
#include <vector>
#include <list>


class RobotCfgInfo
{
public:
    int _id;                //id
    int _level;             //城堡等级
    int _totalLevel;        //功能建筑总等级
    int _dwellingsNum[2];   //民居数量随机区间 [0]最小数量 [1]最大数量
    int _sawmillNum[2];     //伐木场数量随机区间 [0]最小数量 [1]最大数量
    int _dwellingsLevel[2]; //民居等级随机区间 [0]最小等级 [1]最大的等级
    int _sawmillLevel[2];   //伐木场等级随机区间 [0]最小等级 [1]最大的等级
    int _totalarmy;         //总士兵数
    int _playericon[2];     //机器人头像选择区间
    int _vip[2];            //机器人vip等级区间
    
public:
    RobotCfgInfo();
    ~RobotCfgInfo();
    
public:
    //随机民居数量
    int randDwellingsNum();
    
    //随机民居等级
    int randDwellingsLevel();
    
    //随机伐木场数量
    int randSawmillNum();
    
    //随机伐木场等级
    int randSawmillLevel();
    
    //随机头像ID
    int randIcon();
    
    //随机VIP等级
    int randVip();
    
};

class RobotConfig : public ConfigData
{
    
public:
    
    RobotConfig();
    
    ~RobotConfig();
    
    int parse();
    
    bool check();
    
    void clear();
    
    RobotCfgInfo* getCfgByID(int nIndex);               //根据配置id 获取配置信息
    RobotCfgInfo* randRobotCfg();                       //随机获取一个配置信息
    
    int randLevel();    //随机一个城堡等级
public:
    // key cfgid value 配置信息指针
    std::map<int, RobotCfgInfo*> _datas;
};

//机器人随机名字对象
class RobotRandNameInfo
{
public:
    RobotRandNameInfo();
    
    void init(const std::string& country, const std::string& name);
public:
    std::string _name;
    std::string _country;       //语言类型 CN代表中文 EN代表英文
    int _id;                    //生成的ID
    
};


class RobotNameCfg : public ConfigData
{
public:
    
    RobotNameCfg();
    
    ~RobotNameCfg();
    
    int parse();
    
    bool check();
    
    void clear();
    
    //随机获取函数 仅保证本次启动服务器后 获取的名字不会重复
    //名字用完以后 再获取 将不会获取新的名字
public:
    //从随机库里随机  随机一个 机器人名字
    //country 语言类型 CN代表中文 EN代表英文
    //返回值
    //失败  代表随机库空了 以后也不再产生随机名字
    bool randRobotName(std::string& name, std::string& country);
    
    //指定语言类型 随机一个名字
    //country CN代表中文 EN代表英文
    //没有找到指定类型 就返回false
    bool randRobotNameByCountry(std::string& name, std::string& country);
    
    //只是单纯的随机一个国家
    void randCountry(std::string& country);
    
private:
    //初始化配置表 私用接口
    void initList(std::vector<std::string> list, std::string& name, RobotRandNameInfo& info, const std::string& country, bool bSur);
    //初始化随机名字库
    void initRandNameList();
    //根据生成的ID 获取对象
    RobotRandNameInfo* getNameByID(int id);

private:
    //名字配置表 key 随机名字ID
    std::map<int, RobotRandNameInfo> _namelist;

    //随机名字库 value 随机名字ID
    std::list<int> _randlist;
};

//单个机器人聊天内容
struct RobotOnceTalkCfgInfo
{
    int _cfgid;         //配置表ID
    int _id;            //配置给的发言者ID 相同的发言ID 代表是一个人;
    std::string _cn;
    std::string _en;
    
    std::string& getTalkInfo(const std::string country);
};

//机器人聊天组
class RobotTalkListCfgInfo
{
public:
    RobotTalkListCfgInfo();
    void add(int team, const RobotOnceTalkCfgInfo& value);
    
public:
    std::map<int, std::vector<RobotOnceTalkCfgInfo>> _mMap;  //key team; value 内容
};

//机器人聊天配置表
//这个配置表 是机器人聊天4个配置表的集合 重新组装起来的
class RobotTalkConfig : public ConfigData
{
public:
    
    RobotTalkConfig();
    
    ~RobotTalkConfig();
    
    int parse();
    
    bool check();
    
    void clear();
    
    //加载
    int loadMe();
    
    //随机获取一个机器人聊天
    //里面包含不重复逻辑
    bool randCfg(std::vector<RobotOnceTalkCfgInfo>& vec);
    
private:
    std::vector<RobotTalkListCfgInfo> _datas;
    
};


#endif /* robot_config_h */
