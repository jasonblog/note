//
// Created by server on 16-8-3.
//

#include "seven_day_config.h"

SevenDayCfgInfo::SevenDayCfgInfo()
{
    _id = 0;
    _reward.clear();
}

SevenDayCfgInfo::~SevenDayCfgInfo()
{

}

SevenDayConfig::SevenDayConfig()
{
    _datas.clear();
}

SevenDayConfig::~SevenDayConfig()
{
    clear();
}

int SevenDayConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        SevenDayCfgInfo* item = new SevenDayCfgInfo();

        auto item_obj = it->second.ToObject();
        int oid = item_obj["id"].ToInt();
        auto reward = item_obj["reward"].ToObject();
        item->_id = oid;
        int iIndex = 0;
        for (auto it1=reward.begin(); it1!=reward.end(); ++it1) {
            std::vector<int> v;
            auto ar = it1->second.ToArray();
            for (auto xit=ar.begin(); xit!=ar.end(); ++xit) {
                v.push_back(xit->ToInt());
            }
            item->_reward[iIndex++] = v;

        }
        _datas[oid] = item;
    }
    return 0;
}

void SevenDayConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

bool SevenDayConfig::check()
{
    //检查配置表正确性
    for (int i = SignDAys_First; i < SignDAys_Max; ++i) {
        SevenDayCfgInfo* pCfg = getCfg(i);
        if (NULL == pCfg)
        {
            printf("[server] seven_day_config fail !!! {} day sign id is err !!!", i);
            return false;
        }
    }
    return true;
}

SevenDayCfgInfo* SevenDayConfig::getCfg(int iIndex)
{
    auto it=_datas.find(iIndex);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
}