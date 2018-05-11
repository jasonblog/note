//
// Created by DGuco on 17-7-7.
//

#ifndef SERVER_OBJECTMANAGER_H
#define SERVER_OBJECTMANAGER_H
#include "server_tool.h"

class CObjectManager{
public:
    CObjectManager(EnObjType objType);
    ~CObjectManager();
    OBJ_ID  GetValidId();

private:
    void SetLastUsedId(OBJ_ID m_ulLastUsedId);
    void SetLastGetTIme(time_t time);
private:
    time_t m_tLastGetTime;      //上次获取id的时间
    int m_ulLastUsedId;      //上次获取同一时间的id
    EnObjType m_enObjType;
};


#endif //SERVER_OBJECTMANAGER_H
