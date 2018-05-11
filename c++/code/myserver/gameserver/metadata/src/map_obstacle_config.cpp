#include "map_obstacle_config.h"

MapObstacleItem::MapObstacleItem()
{
    _id = 0;
    _listOdd.clear();
    _listEven.clear();
}

MapObstacleItem::~MapObstacleItem()
{
    _id = 0;
    _listOdd.clear();
    _listEven.clear();
}

MapObstacleConfig::MapObstacleConfig()
{
    _datas.clear();
}

MapObstacleConfig::~MapObstacleConfig()
{
    clear();
}

int MapObstacleConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto one = new MapObstacleItem;
        auto one_obj = it->second.ToObject();
        one->_id = one_obj["id"].ToInt();
        auto area = one_obj["area"].ToArray();
        
        auto odd = one_obj["coordinateOdd"].ToArray();
        auto even = one_obj["coordinateEven"].ToArray();
        
        for (int i = 0; i < odd.size(); ++i)
        {
            one->_listOdd.push_back(odd[i]);
        }
        
        for (int i = 0; i < even.size(); ++i)
        {
            one->_listEven.push_back(even[i]);
        }
        
        _datas[one->_id] = one;
        
        for (int i = 0; i < area.size(); ++i)
        {
            _dataArea[area[i]].push_back(one->_id);
        }
    }
    
    return 0;
}

void MapObstacleConfig::clear()
{
    CONFIG_DATA_CLEAR(_datas)
}

MapObstacleItem* MapObstacleConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    
    return NULL;
}


