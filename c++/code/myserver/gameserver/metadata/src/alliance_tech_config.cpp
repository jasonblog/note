#include "alliance_tech_config.h"
#include "../common_err.h"

//  联盟科技配置表
AllianceTechItem::AllianceTechItem()
{
    
}

AllianceTechItem::~AllianceTechItem()
{
    
}

//------------------------------------------
//------------------------------------------

AllianceTechConfig::AllianceTechConfig()
{
    _datas.clear();
}

AllianceTechConfig::~AllianceTechConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int AllianceTechConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto item = new AllianceTechItem();
        
        auto item_obj   = it->second.ToObject();
        
        auto id         = item_obj["id"].ToInt();           //  ID
        auto type       = item_obj["type"].ToInt();         //  科技效果
        auto level      = item_obj["level"].ToInt();        //  等级
        auto param      = item_obj["param"].ToFloat();      //  加成
        auto point      = item_obj["point"].ToInt();        //  需要积分
        auto time       = item_obj["time"].ToInt();         //  升级时间
        auto next       = item_obj["next"].ToInt();         //  升级后的ID
        
        item->_id       = id;
        item->_type     = type;
        item->_level    = level;
        item->_param    = param;
        item->_point    = point;
        item->_time     = time;
        item->_next     = next;
        
        _datas[id] = item;
    }

    
    return 0;
}

void AllianceTechConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

AllianceTechItem* AllianceTechConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("AllianceTechConfig::item:{}", key);
}