#include "roundle_config.h"
#include "../common_err.h"

RoundleItem::RoundleItem()
{
    vRemove.clear();
}
RoundleItem::~RoundleItem()
{
    vRemove.clear();
}
//------------------------------------------
//------------------------------------------

RoundleConfig::RoundleConfig()
{
    _datas.clear();
}

RoundleConfig::~RoundleConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int RoundleConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto roundleItem = new RoundleItem();
        
        auto item_obj   = it->second.ToObject();
        
        auto id                 = item_obj["id"].ToInt();
        auto type               = item_obj["type"].ToInt();
        auto output             = item_obj["output"].ToInt();
        auto item               = item_obj["item"].ToInt();
        auto chances            = item_obj["chances"].ToInt();
        auto remove             = item_obj["remove"].ToArray();
        
        roundleItem->iID        = id;
        roundleItem->iType      = type;
        roundleItem->iOutput    = output;
        roundleItem->iItem      = item;
        roundleItem->iChances   = chances;
        
        for (auto it=remove.begin(); it!=remove.end(); ++it)
        {
            roundleItem->vRemove.push_back(it->ToInt());
        }
        
        _datas[id] = roundleItem;
    }
    
    return 0;
}

void RoundleConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

RoundleItem* RoundleConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    
    return NULL;
}

