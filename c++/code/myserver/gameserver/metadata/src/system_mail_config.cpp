//
//  system_mail_config.cpp
//  cserver
//
//  Created by zhaolong on 16/8/12.
//
//

#include "system_mail_config.h"
#include "../package/common_resp.h"

SystemMailCfgInfo::SystemMailCfgInfo()
{
    _id = -1;            //邮件配置id
    _sendername = -1;    //邮件发送者名字编号
    _sendericon = -1;    //邮件发送者头像id
    _name = -1;          //邮件名称
    _describe = -1;      //邮件描述
    _content = -1;       //邮件内容
    _reward.clear();
}

SystemMailCfgInfo::~SystemMailCfgInfo()
{
    
}

std::string SystemMailCfgInfo::getStrSenderName()
{
    return intToSting(_sendername);
}

std::string SystemMailCfgInfo::getStrName()
{
    return intToSting(_name);
}

std::string SystemMailCfgInfo::getStrDescribe()
{
    return intToSting(_describe);
}

std::string SystemMailCfgInfo::getStrContent()
{
    return intToSting(_content);
}

SystemMailConfig::SystemMailConfig()
{
    _datas.clear();
}

SystemMailConfig::~SystemMailConfig()
{
    clear();
}

int SystemMailConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        SystemMailCfgInfo* item = new SystemMailCfgInfo();
        
        auto item_obj = it->second.ToObject();
        int oid = item_obj["id"].ToInt();
        item->_id = oid;
        item->_sendername = item_obj["player"].ToInt();
        item->_sendericon = item_obj["icon"].ToInt();
        item->_name = item_obj["name"].ToInt();
        item->_describe = item_obj["shortDesc"].ToInt();
        item->_content = item_obj["desc"].ToInt();
        
        auto reward = item_obj["reward"].ToObject();
        for (auto it1=reward.begin(); it1!=reward.end(); ++it1) {
            MailRewardTemplate oMailReward;
            auto ar = it1->second.ToArray();
            oMailReward.type = ar[0].ToInt();
            oMailReward.id = ar[1].ToInt();
            oMailReward.num = ar[2].ToInt();
 
            item->_reward[it1->first] = oMailReward;
            
        }
        _datas[oid] = item;
    }
    return 0;
}

void SystemMailConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

SystemMailCfgInfo* SystemMailConfig::getCfg(int nIndex)
{
    auto it=_datas.find(nIndex);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
}