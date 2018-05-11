//
//  system_mail_config.h
//  cserver
//
//  Created by zhaolong on 16/8/12.
//
//

#ifndef system_mail_config_h
#define system_mail_config_h

#include "config_data.h"
#include <map>


//邮件奖励道具模版
class MailRewardTemplate
{
public:
    int type;   //奖励道具类型 2 物品 ，3 装备
    int id;     //奖励道具id
    int num;    //奖励道具数量
};

typedef std::map<std::string, MailRewardTemplate> mMailRewardList;

class SystemMailCfgInfo
{
public:
    int _id;            //邮件配置id
    int _sendername;    //邮件发送者名字编号
    int _sendericon;    //邮件发送者头像id
    int _name;          //邮件名称
    int _describe;      //邮件描述
    int _content;       //邮件内容
    mMailRewardList _reward;    //奖励列表
    
public:
    SystemMailCfgInfo();
    ~SystemMailCfgInfo();
    
public:
    std::string getStrSenderName();
    std::string getStrName();
    std::string getStrDescribe();
    std::string getStrContent();
};

class SystemMailConfig : public ConfigData
{
    
public:
    
    SystemMailConfig();
    
    ~SystemMailConfig();
    
    int parse();
    
    void clear();
    
    SystemMailCfgInfo* getCfg(int nIndex);
public:
    std::map<int, SystemMailCfgInfo*> _datas;
};

#endif /* system_mail_config_h */
