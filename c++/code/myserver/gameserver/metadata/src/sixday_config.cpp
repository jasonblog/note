#include "sixday_config.h"
#include "../common_err.h"

//  酒吧悬赏任务配置表
SixDayItem::SixDayItem()
{
    reset();
}
SixDayItem::~SixDayItem()
{
    reset();
}

void SixDayItem::reset()
{
    miId = 0;            //ID
    miOrder= 0;         //顺序
    miName = 0;          //名称
    miType = 0;          //活动类型
    miIsMass = 0;        //是否需要集结
    miTargetName =0;    //目标名称
    miIsAlliance = 0;      //是否联盟共享
    mmTargetLevel.clear();
    mmScoreCondition.clear();
    mmScoreReward.clear();
    mmReward.clear();
    maScore.clear();
    maTarget.clear();
}
//------------------------------------------
//------------------------------------------

SixDayConfig::SixDayConfig()
{
    _datas.clear();
}

SixDayConfig::~SixDayConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int SixDayConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto item = new SixDayItem();
        
        auto item_obj   = it->second.ToObject();
        
        item->miId              = item_obj["id"].ToInt();           //  ID;
        item->miOrder           = item_obj["order"].ToInt();         //顺序
        item->miName            = item_obj["name"].ToInt();          //名称
        item->miType            = item_obj["type"].ToInt();          //活动类型
        item->miIsMass          = item_obj["isMass"].ToInt();        //是否需要集结
        item->miTargetName      = item_obj["targetName"].ToInt();    //目标名称
        item->mmTargetLevel[1]    = item_obj["targetLevel1"].ToInt();    //条件1
        item->mmTargetLevel[2]   = item_obj["targetLevel2"].ToInt();    //条件2
        item->mmTargetLevel[3]    = item_obj["targetLevel3"].ToInt();    //条件3
        item->miIsAlliance      = item_obj["isAlliance"].ToInt();      //是否联盟共享
        item->mmScoreCondition[1] = item_obj["scorecondition1"].ToInt(); //积分条件1
        item->mmScoreCondition[2] = item_obj["scorecondition2"].ToInt(); //积分条件1
        item->mmScoreCondition[3] = item_obj["scorecondition3"].ToInt(); //积分条件1

        auto reward1 = item_obj["reward1"].ToObject();
        for (auto it=reward1.begin(); it!=reward1.end(); ++it)
        {
            std::vector<int> v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit)
            {
                v.push_back(xit->ToInt());
            }
            item->mmReward[1][it->first] = v;
        }
        
        
        auto reward2 = item_obj["reward2"].ToObject();
        for (auto it=reward2.begin(); it!=reward2.end(); ++it)
        {
            std::vector<int> v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit)
            {
                v.push_back(xit->ToInt());
            }
            item->mmReward[2][it->first] = v;
        }
        
        auto reward3 = item_obj["reward3"].ToObject();
        for (auto it=reward3.begin(); it!=reward3.end(); ++it)
        {
            std::vector<int> v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit)
            {
                v.push_back(xit->ToInt());
            }
            item->mmReward[3][it->first] = v;
        }
        
        auto scoreReward1 = item_obj["scoreReward1"].ToObject();
        for (auto it=scoreReward1.begin(); it!=scoreReward1.end(); ++it)
        {
            std::vector<int> v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit)
            {
                v.push_back(xit->ToInt());
            }
            item->mmScoreReward[1][it->first] = v;
        }
        
        auto scoreReward2 = item_obj["scoreReward2"].ToObject();
        for (auto it=scoreReward2.begin(); it!=scoreReward2.end(); ++it)
        {
            std::vector<int> v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit)
            {
                v.push_back(xit->ToInt());
            }
            item->mmScoreReward[2][it->first] = v;
        }
        
        auto scoreReward3 = item_obj["scoreReward3"].ToObject();
        for (auto it=scoreReward3.begin(); it!=scoreReward3.end(); ++it)
        {
            std::vector<int> v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit)
            {
                v.push_back(xit->ToInt());
            }
            item->mmScoreReward[3][it->first] = v;
        }
        
        auto messageReward = item_obj["messageReward"].ToArray();
        for (auto it = messageReward.begin(); it != messageReward.end(); ++it)
        {
            item->maMessageReward.push_back(it->ToInt());
        }
        
        auto target = item_obj["target"].ToArray();
        for (auto it = target.begin(); it != target.end(); ++it)
        {
            item->maTarget.push_back(it->ToInt());
        }
        
        auto score = item_obj["score"].ToArray();
        for (auto it = score.begin(); it != score.end(); ++it)
        {
            item->maScore.push_back(it->ToInt());
        }
        
        _datas[item->miId] = item;
    }
    
    return 0;
}

void SixDayConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

SixDayItem* SixDayConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    return NULL;
    //    throw ERROR::MSG("MissionConfig::item:{}", key);
}