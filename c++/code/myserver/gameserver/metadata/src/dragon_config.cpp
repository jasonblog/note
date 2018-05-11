#include "dragon_config.h"

DragonItem::DragonItem()
{
    
}
DragonItem::~DragonItem()
{
    
}
//------------------------------------------
//------------------------------------------

DragonConfig::DragonConfig()
{
    _datas.clear();
}

DragonConfig::~DragonConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int DragonConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto dragonItem = new DragonItem();
        
        auto item_obj   = it->second.ToObject();
        
        auto id         = item_obj["id"].ToInt();
        auto type       = item_obj["type"].ToInt();
        auto colour     = item_obj["colour"].ToInt();
        
        auto gold       = item_obj["gold"].ToArray();
        auto wood       = item_obj["wood"].ToArray();
        auto stone      = item_obj["stone"].ToArray();
        auto iron       = item_obj["iron"].ToArray();
        auto resource   = item_obj["resource"].ToArray();
        auto atk        = item_obj["atk"].ToArray();
        auto def        = item_obj["def"].ToArray();
        auto troop      = item_obj["troop"].ToArray();
        auto mSpeed     = item_obj["mSpeed"].ToArray();
        
        dragonItem->iID     = id;
        dragonItem->iType   = type;
        dragonItem->iColour = colour;
        
        for (auto itm = gold.begin(); itm != gold.end(); ++itm)
        {
            dragonItem->vGold.push_back(itm->ToInt());
        }
        for (auto itm = wood.begin(); itm != wood.end(); ++itm)
        {
            dragonItem->vWood.push_back(itm->ToInt());
        }
        for (auto itm = stone.begin(); itm != stone.end(); ++itm)
        {
            dragonItem->vStone.push_back(itm->ToInt());
        }
        for (auto itm = iron.begin(); itm != iron.end(); ++itm)
        {
            dragonItem->vIron.push_back(itm->ToInt());
        }
        for (auto itm = resource.begin(); itm != resource.end(); ++itm)
        {
            dragonItem->vResource.push_back(itm->ToInt());
        }
        for (auto itm = atk.begin(); itm != atk.end(); ++itm)
        {
            dragonItem->vAtk.push_back(itm->ToInt());
        }
        for (auto itm = def.begin(); itm != def.end(); ++itm)
        {
            dragonItem->vDef.push_back(itm->ToInt());
        }
        for (auto itm = troop.begin(); itm != troop.end(); ++itm)
        {
            dragonItem->vTroop.push_back(itm->ToInt());
        }
        for (auto itm = mSpeed.begin(); itm != mSpeed.end(); ++itm)
        {
            dragonItem->vSpeed.push_back(itm->ToInt());
        }
        
        _datas[id] = dragonItem;
    }
    
    return 0;
}

void DragonConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

DragonItem* DragonConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    
    return NULL;
}