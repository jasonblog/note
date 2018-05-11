#include "smithy_config.h"
#include "../common_err.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "../../framework/framework.h"

SmithyData::SmithyData()
{
    _forging.clear();
    _greenType.clear();
    _blueType.clear();
    _purpleType.clear();
    _orangeType.clear();
    _blueNum.clear();
    _purpleNum.clear();
    _orangeNum.clear();
}

SmithyData::~SmithyData()
{
    _forging.clear();
    _greenType.clear();
    _blueType.clear();
    _purpleType.clear();
    _orangeType.clear();
    _blueNum.clear();
    _purpleNum.clear();
    _orangeNum.clear();
}

SmithyConfig::SmithyConfig()
{
    _datas.clear();
}

SmithyConfig::~SmithyConfig()
{
    for (auto it = _datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

int SmithyConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        auto item = new SmithyData();
        
        auto item_obj = it->second.ToObject();
        auto oid = item_obj["id"].ToInt();
        auto name = item_obj["name"].ToInt();
        auto expend = item_obj["expend"].ToInt();
        auto num = item_obj["num"].ToInt();
        auto forging = item_obj["forging"].ToArray();
        auto greenType = item_obj["greenType"].ToArray();
        auto blueType = item_obj["blueType"].ToArray();
        auto purpleType = item_obj["purpleType"].ToArray();
        auto orangeType = item_obj["orangeType"].ToArray();
        auto blueNum = item_obj["blueNum"].ToArray();
        auto purpleNum = item_obj["purpleNum"].ToArray();
        auto orangeNum = item_obj["orangeNum"].ToArray();


        item->_id = oid;
        item->_name = name;
        item->_expend = expend;
        item->_num = num;
        for (auto l=forging.begin(); l!=forging.end(); ++l) {
            auto y = l->ToInt();
            item->_forging.push_back(y);
        }
        for (auto l=greenType.begin(); l!=greenType.end(); ++l) {
            auto y = l->ToInt();
            item->_greenType.push_back(y);
        }
        for (auto l=blueType.begin(); l!=blueType.end(); ++l) {
            auto y = l->ToInt();
            item->_blueType.push_back(y);
        }
        for (auto l=purpleType.begin(); l!=purpleType.end(); ++l) {
            auto y = l->ToInt();
            item->_purpleType.push_back(y);
        }

        for (auto l=orangeType.begin(); l!=orangeType.end(); ++l) {
            auto y = l->ToInt();
            item->_orangeType.push_back(y);
        }
        for (auto l=blueNum.begin(); l!=blueNum.end(); ++l) {
            auto y = l->ToInt();
            item->_blueNum.push_back(y);
        }
        for (auto l=purpleNum.begin(); l!=purpleNum.end(); ++l) {
            auto y = l->ToInt();
            item->_purpleNum.push_back(y);
        }
        for (auto l=orangeNum.begin(); l!=orangeNum.end(); ++l) {
            auto y = l->ToInt();
            item->_orangeNum.push_back(y);
        }

        _datas[oid] = item;
        
    }
    return 0;
}

void SmithyConfig::clear()
{
    for (auto it = _datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
    
}

SmithyData* SmithyConfig::item(int key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("SmithyConfig::item:{}", key);
}