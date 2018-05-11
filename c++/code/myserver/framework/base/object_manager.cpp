//
// Created by DGuco on 17-7-7.
//

#include "object_manager.h"

CObjectManager::CObjectManager(EnObjType objType)
{
    m_enObjType = objType;
    m_tLastGetTime = GetSecondTime();
    m_ulLastUsedId = OBJ_ID_START(objType);
}

CObjectManager::~CObjectManager()
{
}

OBJ_ID CObjectManager::GetValidId()
{
    int timeNow = GetSecondTime();
    OBJ_ID id;

    if (m_tLastGetTime == timeNow) {
        m_ulLastUsedId++;
    }else
    {
        SetLastGetTIme(timeNow);
        SetLastUsedId(OBJ_ID_START(m_enObjType));
    }
    if (m_ulLastUsedId > OBJ_ID_END(m_enObjType))
    {
        LOG_ERROR("default" ,"Id out of range,type = {}, id_start = {}, id_end = {}.",m_enObjType, OBJ_ID_START(m_enObjType), OBJ_ID_START(m_enObjType));
        return INVALID_OBJ_ID;
    }
    id = (unsigned long)timeNow << 32 | m_ulLastUsedId;
    return id;
}

void CObjectManager::SetLastUsedId(OBJ_ID ulLastUsedId)
{
    m_ulLastUsedId = ulLastUsedId;
}

void CObjectManager::SetLastGetTIme(time_t time)
{
    m_tLastGetTime = time;
}