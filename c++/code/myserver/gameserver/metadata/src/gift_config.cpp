//
//  gift_config.cpp
//  cserver
//
//  Created by zhaolong on 16/8/31.
//
//

#include "gift_config.h"
#include "../package/common_resp.h"
#include "../global.h"

GiftCfgInfo::GiftCfgInfo()
{
    _id = 0;
    _type = 0;
    _rechargeIOS = 0;
    _rechargeAndroid = 0;
    _rewardGem = 0;
    _times = 0;
    _reward.clear();
}

GiftCfgInfo::~GiftCfgInfo()
{
    
}

GiftTypeList::GiftTypeList()
{
    _list.clear();
}

GiftTypeList::~GiftTypeList()
{
    _list.clear();
}

void GiftTypeList::add(int nCfgID)
{
    _list.push_back(nCfgID);
}

int GiftTypeList::randomOnce(const std::map<int, int>&  mList)
{
    std::vector<int> aRandList;
    aRandList.clear();
    for (auto it = _list.begin(); it != _list.end(); ++it)
    {
        //列表里面没有 就加入到随机表内  新手礼包不能被随机到
        int nID = *it;
        if (GIFT_NEW_PLAYER_ID != nID && mList.find(nID) == mList.end())
        {
            aRandList.push_back(nID);
        }
    }
    auto it = aRandList.begin();
    
    if (it == aRandList.end())
        return -1;

    int nID = *it;
    for (; it != aRandList.end(); ++it)
    {
        if (rand() % 2 == 0)
        {
            nID = *it;
            break;
        }
    }
    return nID;
}

GiftConfig::GiftConfig()
{
    _datas.clear();
    _datatype.clear();
    _maxtype = 0;
}

GiftConfig::~GiftConfig()
{
    clear();
}

int GiftConfig::parse()
{
    auto obj = m_obj.ToObject();
    _maxtype = 0;
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        GiftCfgInfo* item = new GiftCfgInfo();
        
        auto item_obj = it->second.ToObject();
        int oid = item_obj["id"].ToInt();
        item->_id = oid;
        item->_type = item_obj["type"].ToInt();
        int rechargeIOS = item_obj["rechargeIOS"].ToInt();
        int rechargeAndroid = item_obj["rechargeAndroid"].ToInt();
        item->_rechargeIOS = rechargeIOS;
        item->_rechargeAndroid = rechargeAndroid;
        item->_rewardGem = item_obj["rewardGem"].ToInt();
        item->_times = item_obj["duration"].ToInt();
        
        std::string itemList = item_obj["rewardItem"].ToString();
        fillStrToMap(itemList.c_str(), item->_reward);
        
        _datas[oid] = item;
        int type = makeTypeByCfgID(rechargeIOS, rechargeAndroid);
        addTypeList(type, oid);
        if (type > _maxtype)
            _maxtype = type;
    }
    return 0;
}

bool GiftConfig::check()
{
    for (auto it = _datas.begin(); it != _datas.end(); ++it)
    {
        //去充值配置表 查看充值的正确性
        if (NULL == g_rechargeConfig.item(it->second->_rechargeIOS))
        {
            ERROR_LOG("ERR: check GiftConfig _rechargeIOS is err!!! id = {}, _rechargeIOS = {}", it->first, it->second->_rechargeIOS);
            return false;
        }
        if (NULL == g_rechargeConfig.item(it->second->_rechargeAndroid))
        {
            ERROR_LOG("ERR: check GiftConfig _rechargeAndroid is err!!! id = {}, _rechargeAndroid = {}", it->first, it->second->_rechargeAndroid);
            return false;
        }
        
        //检测配置的奖品正确性
        if(!checkVectorIsItem(it->second->_reward))
        {
            ERROR_LOG("ERR: check GiftConfig _reward item id is err!!! id = {}", it->first);
            return false;
        }
    }
    return true;
}

void GiftConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
    
    for (auto it=_datatype.begin(); it!=_datatype.end(); ++it)
    {
        delete it->second;
    }
    _datatype.clear();
}

GiftCfgInfo* GiftConfig::getCfg(int nIndex)
{
    auto it=_datas.find(nIndex);
    if (it!=_datas.end())
    {
        return it->second;
    }
    return NULL;
}

GiftTypeList* GiftConfig::getIdByType(int nType)
{
    auto it = _datatype.find(nType);
    if (it == _datatype.end())
        return NULL;
    else
        return it->second;
}

void GiftConfig::addTypeList(int type, int id)
{
    auto it = _datatype.find(type);
    if (it == _datatype.end())
    {
        GiftTypeList* list = new GiftTypeList();
        list->add(id);
        _datatype[type] = list;
    }
    else
    {
        it->second->add(id);
    }
}

int GiftConfig::makeTypeByCfgID(int nIndex)
{
    GiftCfgInfo* pCfg = getCfg(nIndex);
    if (pCfg)
    {
        return makeTypeByCfgID(pCfg->_rechargeIOS, pCfg->_rechargeAndroid);
    }
    return -1;
}


int GiftConfig::makeTypeByCfgID(int rechargeIOS, int rechargeAndroid)
{
    return rechargeIOS * GIFT_MAKE_TYPE_VALUE + rechargeAndroid;
}

bool GiftConfig::initRate(std::map<int, int>& mRate, int nRate)
{
    mRate.clear();
    for (auto it = _datatype.begin(); it != _datatype.end(); ++it)
    {
        int type = it->first;
        mRate[type] = nRate;
    }
    return true;
}

int GiftConfig::getMaxType()
{
    return _maxtype;
}

int GiftConfig::getNextType(int nType)
{
    int rechargeIOS = nType / GIFT_MAKE_TYPE_VALUE;
    int rechargeAndroid = nType % GIFT_MAKE_TYPE_VALUE;
    rechargeIOS++;
    rechargeAndroid++;
    return makeTypeByCfgID(rechargeIOS, rechargeAndroid);
}


