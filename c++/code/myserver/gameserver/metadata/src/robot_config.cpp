//
//  robot_config.cpp
//  cserver
//
//  Created by zhaolong on 16/11/15.
//
//

#include "robot_config.h"
#include "../global.h"
#include "../package/common_resp.h"

RobotCfgInfo::RobotCfgInfo()
{
    _id = -1;
    _level = -1;
    _totalLevel = -1;
    _dwellingsNum[0] = -1;
    _dwellingsNum[1] = -1;
    _sawmillNum[0] = -1;
    _sawmillNum[1] = -1;
    _dwellingsLevel[0] = -1;
    _dwellingsLevel[1] = -1;
    _sawmillLevel[0] = -1;
    _sawmillLevel[1] = -1;
    _totalarmy = -1;
    _playericon[0] = -1;
    _playericon[1] = -1;
    _vip[0] = -1;
    _vip[1] = -1;
}

RobotCfgInfo::~RobotCfgInfo()
{
    
}

//随机民居数量
int RobotCfgInfo::randDwellingsNum()
{
    int n = 2;
    int nMax = _dwellingsNum[1] + 1;
    if (_dwellingsNum[0] < nMax)
        n = crand_weight(_dwellingsNum[0], nMax);
    return n;
}

//随机民居等级
int RobotCfgInfo::randDwellingsLevel()
{
    int n = 1;
    int nMax = _dwellingsLevel[1] + 1;
    if (_dwellingsLevel[0] < nMax)
        n = crand_weight(_dwellingsLevel[0], nMax);
    return n;
}

//随机伐木场数量
int RobotCfgInfo::randSawmillNum()
{
    int n = 2;
    int nMax = _sawmillNum[1] + 1;
    if (_sawmillNum[0] < nMax)
        n = crand_weight(_sawmillNum[0], nMax);
    return n;
}

//随机伐木场等级
int RobotCfgInfo::randSawmillLevel()
{
    int n = 1;
    int nMax = _sawmillLevel[1] + 1;
    if (_sawmillLevel[0] < nMax)
        n = crand_weight(_sawmillLevel[0], nMax);
    return n;
}

//随机头像ID
int RobotCfgInfo::randIcon()
{
    int n = 4000000;
    int nMax = _playericon[1] + 1;
    if (_playericon[0] < nMax)
        n = crand_weight(_playericon[0], nMax);
    return n;
}

//随机VIP等级
int RobotCfgInfo::randVip()
{
    int n = 1;
    int nMax = _vip[1] + 1;
    if (_vip[0] < nMax)
        n = crand_weight(_vip[0], nMax);
    return n;
}

RobotConfig::RobotConfig()
{
    _datas.clear();
}

RobotConfig::~RobotConfig()
{
    clear();
}

int RobotConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        RobotCfgInfo* item = new RobotCfgInfo();
        
        auto item_obj = it->second.ToObject();
        
        item->_id = item_obj["id"].ToInt();
        item->_level = item_obj["level"].ToInt();
        item->_totalLevel = item_obj["totalLevel"].ToInt();
        
        auto ar1 = item_obj["dwellingsNum"].ToArray();
        item->_dwellingsNum[0] = ar1[0].ToInt();
        item->_dwellingsNum[1] = ar1[1].ToInt();
        
        auto ar2 = item_obj["sawmillNum"].ToArray();
        item->_sawmillNum[0] = ar2[0].ToInt();
        item->_sawmillNum[1] = ar2[1].ToInt();

        auto ar3 = item_obj["dwellingsLevel"].ToArray();
        item->_dwellingsLevel[0] = ar3[0].ToInt();
        item->_dwellingsLevel[1] = ar3[1].ToInt();

        auto ar4 = item_obj["sawmillLevel"].ToArray();
        item->_sawmillLevel[0] = ar4[0].ToInt();
        item->_sawmillLevel[1] = ar4[1].ToInt();
        
        item->_totalarmy = item_obj["totalarmy"].ToInt();
        
        auto ar5 = item_obj["playericon"].ToArray();
        item->_playericon[0] = ar5[0].ToInt();
        item->_playericon[1] = ar5[1].ToInt();
        
        auto ar6 = item_obj["vip"].ToArray();
        item->_vip[0] = ar6[0].ToInt();
        item->_vip[1] = ar6[1].ToInt();
        
        _datas[item->_id] = item;
        
    }
    return 0;
}

bool RobotConfig::check()
{
    return true;
}

void RobotConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

RobotCfgInfo* RobotConfig::getCfgByID(int nIndex)
{
    auto it=_datas.find(nIndex);
    if (it!=_datas.end())
    {
        return it->second;
    }
    return NULL;
}

RobotCfgInfo* RobotConfig::randRobotCfg()
{
    return getCfgByID(randLevel());
}

int RobotConfig::randLevel()
{
    int nRand = crand_weight(0, _datas.size());
    int i = 0;
    for (auto it = _datas.begin(); it != _datas.end(); ++it)
    {
        if (i == nRand)
            return it->first;
        i++;
    }
    return 1;
}

RobotRandNameInfo::RobotRandNameInfo()
{
    _name.clear();
    _id = 0;
    _country = -1;
}

void RobotRandNameInfo::init(const std::string& country, const std::string& name)
{
    static int n = 0;
    _id = ++n;
    _country = country;
    _name = name;
}

RobotNameCfg::RobotNameCfg()
{
    clear();
}

RobotNameCfg::~RobotNameCfg()
{
    clear();
}

int RobotNameCfg::parse()
{
    auto obj = m_obj.ToObject();

    //姓名存储临时列表 这里用静态的变量 防止把栈挤爆
    static std::vector<std::string> cnsurlist;
    cnsurlist.clear();
    static std::vector<std::string> cnnamelist;
    cnnamelist.clear();
    static std::vector<std::string> ensurlist;
    ensurlist.clear();
    static std::vector<std::string> ennamelist;
    ennamelist.clear();
  
    RobotRandNameInfo info;
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto item_obj = it->second.ToObject();
        
        std::string cnsur = item_obj["chineseFamilyName"].ToString();
        if(cnsur.length() > 0)
        {
            cnsurlist.push_back(cnsur);
            initList(cnnamelist, cnsur, info, "CN", true);
        }
        
        std::string cnname = item_obj["chineseName"].ToString();
        if (cnname.length() > 0)
        {
            cnnamelist.push_back(cnname);
            initList(cnsurlist, cnname, info, "CN", false);
        }

        std::string ensur = item_obj["englishFamilyName"].ToString();
        if (ensur.length() > 0)
        {
            ensurlist.push_back(ensur);
            initList(ennamelist, ensur, info, "EN", true);
        }
        
        std::string enname = item_obj["englishName"].ToString();
        if (enname.length() > 0)
        {
            ennamelist.push_back(enname);
            initList(ensurlist, enname, info, "EN", false);
        }
    }
    initRandNameList();
    cnsurlist.clear();
    cnnamelist.clear();
    ensurlist.clear();
    ennamelist.clear();
    return 0;
}

bool RobotNameCfg::check()
{
    return true;
}

void RobotNameCfg::clear()
{
    _namelist.clear();
    _randlist.clear();
}

bool RobotNameCfg::randRobotName(std::string& name, std::string& country)
{
    if (_randlist.empty())
        return false;
    
    name.clear();

    //把最后一个弹出来
    int id = _randlist.back();
    auto it = _namelist.find(id);
    if (it != _namelist.end())
    {
        country = it->second._country;
        name = it->second._name;
        _randlist.pop_back();
        return true;
    }
    else
    {
        _randlist.pop_back();
        return false;
    }
}

bool RobotNameCfg::randRobotNameByCountry(std::string& name, std::string& country)
{
    if (_randlist.empty())
        return false;

    name.clear();
    for (auto it = _randlist.begin(); it != _randlist.end(); ++it)
    {
        RobotRandNameInfo* pName = getNameByID((*it));
        if (pName && country == pName->_country)
        {
            name = pName->_name;
            _randlist.erase(it);
            return true;
        }
    }
    return false;
}

void RobotNameCfg::randCountry(std::string& country)
{
    int nRand = rand() % 5;
    switch (nRand)
    {
        case 0:
            country = "CN";
            break;
        default:
            country = "EN";
            break;
    }
}

void RobotNameCfg::initList(std::vector<std::string> list, std::string& name, RobotRandNameInfo& info, const std::string& country, bool bSur)
{
    std::string fullname;
    for (int i = 0; i < list.size(); ++i)
    {
        if (bSur)
            fullname = name + list[i];
        else
            fullname = list[i] + name;
        
        info.init(country, fullname);
        _namelist[info._id] = info;
    }
}

void RobotNameCfg::initRandNameList()
{
    for (auto it = _namelist.begin(); it != _namelist.end(); ++it)
    {
        //随机位置添加
        insertRandPosToList(_randlist, it->first);
    }
}

RobotRandNameInfo* RobotNameCfg::getNameByID(int id)
{
    auto it = _namelist.find((id));
    if (it != _namelist.end())
        return &(it->second);
    else
        return NULL;
}

std::string& RobotOnceTalkCfgInfo::getTalkInfo(const std::string country)
{
    if (country == "CN")
        return _cn;
    else
        return _en;
}

RobotTalkListCfgInfo::RobotTalkListCfgInfo()
{
    _mMap.clear();
}

void RobotTalkListCfgInfo::add(int team, const RobotOnceTalkCfgInfo& value)
{
    auto it = _mMap.find(team);
    if (it == _mMap.end())
    {
        std::vector<RobotOnceTalkCfgInfo> aVec;
        aVec.push_back(value);
        _mMap[team] = aVec;
    }
    else
    {
        //根据配置ID 保证发言顺心
        auto itVec = it->second.begin();
        for (int i = 0; i < it->second.size(); i++)
        {
            if (it->second[i]._cfgid > value._cfgid)
            {
                it->second.insert(itVec + i, value);
                return;
            }
        }
        it->second.push_back(value);
    }
}

RobotTalkConfig::RobotTalkConfig()
{
    _datas.clear();
}

RobotTalkConfig::~RobotTalkConfig()
{
    
}

int RobotTalkConfig::parse()
{
    auto obj = m_obj.ToObject();
    RobotTalkListCfgInfo item;
    int i = 0;
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto item_obj = it->second.ToObject();
        
        //单人聊天配置表 没有 team_id talker_id 字段 这里给它模拟一个
        int teamid = i++;
        auto it_team = item_obj.find("team_id");
        if (it_team != item_obj.end())
            teamid = it_team->second.ToInt();
        
        int talkid = 1;
        auto it_talk = item_obj.find("talker_id");
        if (it_talk != item_obj.end())
            talkid = it_talk->second.ToInt();
        
        RobotOnceTalkCfgInfo info;
        
        info._cfgid = item_obj["id"].ToInt();
        info._id = talkid;
        info._cn = item_obj["CN"].ToString();
        info._en = item_obj["EN"].ToString();
        
        item.add(teamid, info);

    }
    _datas.push_back(item);
    return 0;
}

bool RobotTalkConfig::check()
{
    return true;
}

void RobotTalkConfig::clear()
{
    
}

int RobotTalkConfig::loadMe()
{
    if (loadFromFile("only_config.json") != 0)
        return -1;
    if (loadFromFile("more_2_config.json") != 0)
        return -1;
    if (loadFromFile("more_3_config.json") != 0)
        return -1;
    if (loadFromFile("more_4_config.json") != 0)
        return -1;
    
    return 0;
}

bool RobotTalkConfig::randCfg(std::vector<RobotOnceTalkCfgInfo>& vec)
{
    if (_datas.empty())
        return false;
    
    static list<std::vector<int>> s_list;
    if (s_list.empty())
    {
        for (int i = 0; i < _datas.size(); i++)
        {
            for (auto it = _datas[i]._mMap.begin(); it != _datas[i]._mMap.end(); it++)
            {
                std::vector<int> templist;
                templist.push_back(i);
                templist.push_back(it->first);
                //随机位置添加
                insertRandPosToList(s_list, templist);
            }
        }
        if (s_list.empty())
            return false;
    }
    
    bool bOK = false;
    do
    {
        //把最后一个弹出来
        std::vector<int> templist = s_list.back();
        if (templist.size() != 2)
            break;
        
        int nType = templist[0];
        int nTeam = templist[1];
        
        if (nType >= _datas.size())
            break;
        
        auto it = _datas[nType]._mMap.find(nTeam);
        if (it == _datas[nType]._mMap.end())
            break;
        
        vec = it->second;
        bOK = true;
        
    } while (0);

    s_list.pop_back();
    return bOK;
}







