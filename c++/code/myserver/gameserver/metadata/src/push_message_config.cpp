#include "push_message_config.h"
#include "../common_err.h"

PushMessageItem::PushMessageItem()
{
    _id.clear();
    _cn.clear();
    _en.clear();
    _de.clear();
}

PushMessageItem::~PushMessageItem()
{
    _id.clear();
    _cn.clear();
    _en.clear();
    _de.clear();
}

const std::string& PushMessageItem::getByLanguage(const std::string& language)
{
    if (language == "CN")
        return _cn;
    else if (language == "DE")
        return _de;
    else
        return _en;
}

PushMessageConfig::PushMessageConfig()
{
    _datas.clear();
}

PushMessageConfig::~PushMessageConfig()
{
    clear();
}

int PushMessageConfig::parse()
{
    auto obj = m_obj.ToObject();
    for (auto it=obj.begin(); it!=obj.end(); ++it) {
        PushMessageItem* item = new PushMessageItem();
        auto item_obj = it->second.ToObject();
        std::string oid = item_obj["id"].ToString();
        std::string cn = item_obj["CN"].ToString();
        std::string en = item_obj["EN"].ToString();
        std::string de = item_obj["DE"].ToString();
        item->_id = oid;
        item->_cn = cn;
        item->_en = en;
        item->_de = de;
        _datas[oid] = item;
    }
    return 0;
}

void PushMessageConfig::clear()
{
    for (auto it = _datas.begin(); it!=_datas.end(); ++it) {
        delete it->second;
    }
    _datas.clear();
}

PushMessageItem* PushMessageConfig::item(const std::string &key)
{
    auto it = _datas.find(key);
    if (it!=_datas.end()) {
        return it->second;
    }
    return NULL;
//    throw ERROR::MSG("PushMessageConfig::item:{}", key.c_str());
}