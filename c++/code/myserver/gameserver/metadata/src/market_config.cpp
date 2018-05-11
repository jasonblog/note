//
//  market_config.cpp
//  cserver
//
//  Created by zhaolong on 16/8/18.
//
//

#include "market_config.h"

MarketConfig::MarketConfig()
{
    _datas.clear();
}

MarketConfig::~MarketConfig()
{
    clear();
}

int MarketConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        MarketCfgInfo* item = new MarketCfgInfo();
        
        auto item_obj = it->second.ToObject();
        int oid = item_obj["id"].ToInt();
        item->nID = oid;
        item->nCastle = item_obj["castleLevel"].ToInt();
        item->fAdd = item_obj["parameter"].ToFloat();
        item->nBuy = item_obj["expend"].ToInt();
        item->nWeight = item_obj["weight"].ToFloat();
        
        _datas[oid] = item;
    }
    
    return 0;
}

void MarketConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

MarketCfgInfo* MarketConfig::getCfg(int nIndex)
{
    auto it=_datas.find(nIndex);
    if (it!=_datas.end())
    {
        return it->second;
    }
    return NULL;
}

bool MarketConfig::randomId(std::vector<int>& list, int nNum)
{
    std::map<int, int> mTempList;
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        //有随机权重 且没有城堡等级限制 才加入随机队列
        if (it->second->nWeight > 0 && it->second->nCastle == 1)
        {
            mTempList[it->second->nID] = it->second->nWeight;
        }
    }
    //开始随机
    int i = 0;
    do
    {
        if (mTempList.size() <= 0)
        {
            break;
        }
        int nBase = 0;
        for (auto it = mTempList.begin(); it != mTempList.end(); ++it)
        {
            nBase += it->second;
        }
        if (0 == nBase)
        {
            return false;
        }
        int nRand = rand() % nBase;
        int nCur = 0;
        for (auto it = mTempList.begin(); it != mTempList.end(); ++it)
        {
            nCur += it->second;
            if (nRand < nCur) {
                list.push_back(it->first);
                mTempList.erase(it);
                break;
            }
        }
        i++;
    } while (i < nNum);
    
    return nNum == list.size();
}





