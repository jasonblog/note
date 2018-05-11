//
// Created by DGuco on 17-7-27.
//

#include "../inc/sceneobjmanager.h"

template<> CSceneObjManager *CSingleton<CSceneObjManager>::spSingleton = NULL;

CSceneObjManager::CSceneObjManager()
{
	Initialize();
}

CSceneObjManager::~CSceneObjManager()
{
	if (m_pPlayerManager != NULL) {
		delete m_pPlayerManager;
		m_pPlayerManager = NULL;
	}
}

int CSceneObjManager::Initialize()
{
	m_pPlayerManager = new CObjectManager(EnObjType::OBJ_PLAYER_ENTRY);
	m_mPlayerMap.clear();
	m_mSocketMap.clear();
	return 0;
}

CPlayer *CSceneObjManager::GetPlayer(OBJ_ID ulPlayerid)
{
	MAP_SAFE_FIND(m_mPlayerMap, ulPlayerid);
}

CPlayer *CSceneObjManager::GetPlayerBySocket(int socket)
{
	return NULL;
}

//删除玩家
int CSceneObjManager::DestroyPlayer(OBJ_ID iObjID)
{
	auto it = m_mPlayerMap.find(iObjID);
	if (it != m_mPlayerMap.end()) {
		delete it->second;
	}
	m_mPlayerMap.erase(iObjID);
}

void CSceneObjManager::AddNewPlayer(CPlayer *pPlayer)
{
	if (pPlayer == NULL) {
		return;
	}
	m_mPlayerMap[pPlayer->GetPlayerId()] = pPlayer;
	m_mSocketMap[pPlayer->GetPlayerBase()->GetSocket()] = pPlayer->GetPlayerId();
}

CObjectManager *CSceneObjManager::GetPlayerManager()
{
	return m_pPlayerManager;
}
