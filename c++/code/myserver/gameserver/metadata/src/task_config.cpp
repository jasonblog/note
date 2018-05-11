#include "task_config.h"
#include "../common_err.h"
#include "../global.h"

TaskConfItem::TaskConfItem()
{
    _id = 0;
    _cond = 0;
    _buildingType = 0;
    _num = 0;
    _next = 0;
}

TaskConfItem::~TaskConfItem()
{
    _id = 0;
    _cond = 0;
    _buildingType = 0;
    _num = 0;
    _next = 0;
}

TaskConfig::TaskConfig()
{
    _datas.clear();
}

TaskConfig::~TaskConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int TaskConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new TaskConfItem();
        
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto condition = item_obj["condition"].ToInt();
        auto buildingType = item_obj["buildingType"].ToInt();
        auto num = item_obj["num"].ToInt();
        auto next = item_obj["next"].ToInt();
        auto reward = item_obj["reward"].ToObject();
        item->_id = oid;
        item->_cond = condition;
        item->_buildingType = buildingType;
        item->_num = num;
        item->_next = next;
        for (auto it=reward.begin(); it!=reward.end(); ++it) {
            auto v = new std::vector<int>;
            item->_reward[it->first] = v;
            auto ar = it->second.ToArray();
            for (auto xit=ar.begin(); xit!=ar.end(); ++xit) {
                v->push_back(xit->ToInt());
            }
        }
        _datas[oid] = item;
    }
    return 0;
}

void TaskConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

bool TaskConfig::check()
{
    bool bOK = true;
    std::map<int, int> tempList;
    tempList.clear();
    
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        int nNext = it->second->_next;
        if (tempList.find(nNext) == tempList.end())
        {
            tempList[nNext] = it->second->_id;
        }
        else
        {
            ERROR_LOG("ERR: TaskConfig check err !!! task id = {} data: next task id = {} is repeat !!!", it->second->_id, nNext);
            bOK = false;
        }
    }
    
    for (auto it = tempList.begin(); it != tempList.end(); ++it)
    {
        if (_datas.find(it->second) == _datas.end())
        {
            ERROR_LOG("ERR: TaskConfig check err !!! task id = {} data: next task id = {} is illegal !!!", it->first, it->second);
            bOK = false;
        }
    }
    
    return bOK;
}

TaskConfItem* TaskConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("TaskConfig::item:{}", key);
}
