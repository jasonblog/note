#include "drop_config.h"
#include "../common_err.h"
#include "../common_def.h"
#include "../global.h"

//掉落配置表
DropItem::DropItem()
{
    reset();
}
DropItem::~DropItem()
{
    reset();
}

void DropItem::reset()
{
    miId = 0;
    miNum= 0;
    isRepeat = 0;
    for (auto it : mmRandomItem)
    {
        it.second.clear();
    }
    mmRandomItem.clear();
}
//------------------------------------------
//------------------------------------------

DropConfig::DropConfig()
{
    _datas.clear();
}

DropConfig::~DropConfig()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

int DropConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it)
    {
        auto item = new DropItem();
        
        auto item_obj   = it->second.ToObject();
        
        item->miId              = item_obj["id"].ToInt();
        item->miNum             = item_obj["num"].ToInt();
        item->isRepeat          = item_obj["isRepeat"].ToInt();
        
        auto reward = item_obj["randomItem"].ToObject();
        for (auto it=reward.begin(); it!=reward.end(); ++it)
        {
            std::vector<int> v;
            auto vl = it->second.ToArray();
            for (auto xit=vl.begin(); xit!=vl.end(); ++xit)
            {
                v.push_back(xit->ToInt());
            }
            item->mmRandomItem[it->first] = v;
        }
        
        _datas[item->miId] = item;
    }
    
    if(check() == true)
    {
        return 0;
    }
    return -1;
}

void DropConfig::clear()
{
    for (auto it=_datas.begin(); it!=_datas.end(); ++it)
    {
        delete it->second;
    }
    _datas.clear();
}

DropItem* DropConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end())
    {
        return it->second;
    }
    return NULL;
    //    throw ERROR::MSG("MissionConfig::item:{}", key);
}

bool DropConfig::check()
{
    for (auto it : _datas)
    {
        for (auto itx : it.second->mmRandomItem)
        {
            bool isRight = false;
            for (auto ity : g_itemConfig._datas)
            {
                if (ity.second->_id == itx.second[1])
                {
                    isRight = true;
                }
            }
            
            if(isRight == false)
            {
                ERROR_LOG("Dropconfig check error,itemid = {}",itx.second[1]);
                return isRight;
            }
        }
    }
    return true;
}

void DropConfig::drop(int iId,std::map<int,std::vector<int>> & dropResult)
{
    DropItem* dropitem = item(iId);
    if (dropitem == NULL)
    {
        return;
    }
    
    //key id value:是否已掉落过
    std::map<int,bool> hasdrop;
    std::vector<int> dropitems;
    for (int i = 0; i < dropitem->miNum; ++i)
    {
        int iTemp = rand() % THOUSAND_BASE + 1;
        int iBase = 0;
        dropitems.clear();
        for (auto it = dropitem->mmRandomItem.begin();it != dropitem->mmRandomItem.end();it++)
        {
            //设为没有掉落过
            auto iteator = hasdrop.find(it->second[1]);
            if (iteator == hasdrop.end())
            {
                hasdrop[it->second[1]] = false;
            }
            
            iBase += it->second[3];
            if (iTemp <= iBase)
            {
                //不允许重复掉落
                if (dropitem->isRepeat == 0)
                {
                    if(hasdrop[it->second[1]] == false)
                    {
                        dropitems.push_back(it->second[0]);
                        dropitems.push_back(it->second[1]);
                        dropitems.push_back(it->second[2]);
                        dropResult[i] = dropitems;
                        //设置已经掉落过
                        hasdrop[it->second[1]] = true;
                        break;
                    }
                    //重新掉落
                    it = dropitem->mmRandomItem.begin();
                    iBase = 0;
                    iTemp = rand() % THOUSAND_BASE + 1;
                }else
                {
                    bool flag = false;
                    for (auto itresult = dropResult.begin(); itresult != dropResult.end();itresult++)
                    {
                        if (itresult->second[1] == it->second[1])
                        {
                            itresult->second[2] += it->second[2];
                            flag = true;
                            break;
                        }
                    }
                    if (flag == true)
                    {
                        break;
                    }
                    
                    dropitems.push_back(it->second[0]);
                    dropitems.push_back(it->second[1]);
                    dropitems.push_back(it->second[2]);
                    dropResult[i] = dropitems;
                    break;
                }
            }
        }
    }
}