#include "chest_config.h"

ChestItem::ChestItem()
{
    
}
ChestItem::~ChestItem()
{
    
}
//------------------------------------------
//------------------------------------------

ChestConfig::ChestConfig()
{
    _datas.clear();
}

ChestConfig::~ChestConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int ChestConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto chestItem = new ChestItem();
        
        auto item_obj   = it->second.ToObject();
        
        auto id                     = item_obj["id"].ToInt();
        auto team                   = item_obj["team"].ToInt();
        auto type                   = item_obj["type"].ToInt();
        auto item                   = item_obj["item"].ToInt();
        auto num                    = item_obj["num"].ToInt();
        auto overTimes              = item_obj["overTimes"].ToInt();
        auto price                  = item_obj["price"].ToInt();
        auto chance                 = item_obj["chance"].ToInt();
        
        chestItem->iID              = id;
        chestItem->iTeam            = team;
        chestItem->iType            = type;
        chestItem->iItem            = item;
        chestItem->iNum             = num;
        chestItem->iOverTimes       = overTimes;
        chestItem->iPrice           = price;
        chestItem->iChance          = chance;
        
        _datas[id] = chestItem;
        auto it1 = _dataTeam.find(team);
        if (it1 == _dataTeam.end())
        {
            std::vector<ChestItem*> tmp;
            tmp.push_back(chestItem);
            _dataTeam[team] = tmp;
        }
        else
        {
            it1->second.push_back(chestItem);
        }
    }
    
    return 0;
}

void ChestConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

ChestItem* ChestConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    
    return NULL;
}

