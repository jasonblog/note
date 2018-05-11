#include "player.pb.h"
#include "my_assert.h"
#include "../inc/core_module.h"
#include "../inc/db_module.h"
#include "../inc/module_manager.h"
#include "../../datamodule/inc/sceneobjmanager.h"

template<> CCoreModule *CSingleton<CCoreModule>::spSingleton = NULL;

CCoreModule::CCoreModule()
{
}

CCoreModule::~CCoreModule()
{
}

int CCoreModule::Initialize()
{
	return 0;
}

int CCoreModule::OnLaunchServer()
{
	return 0;
}

int CCoreModule::OnExitServer()
{
	return 0;
}

void CCoreModule::OnRouterMessage(CProxyMessage *pMsg)
{

}

void CCoreModule::OnClientMessage(CPlayer *pTeam, CMessage *pMsg)
{

}

int CCoreModule::OnCreateEntity(CPlayer *pTeam)
{
	return 0;
}

void CCoreModule::OnDestroyEntity(CPlayer *pTeam)
{

}

void CCoreModule::OnMsgUserLoginRequest(CMessage *pMsg)
{
	MY_ASSERT_LOG("core", pMsg != NULL,
				  return);
	UserAccountLoginRequest *request = (UserAccountLoginRequest *) pMsg->msgpara();
	CDbModule::GetSingletonPtr()
		->FindOrCreateUserRequest(request->platform(), request->puid(), pMsg->mutable_msghead());
}

void CCoreModule::OnMsgPlayerLoginRequest(CMessage *pMsg)
{
	MY_ASSERT_LOG("core", pMsg != NULL,
				  return);
	PlayerLoginRequest *request = (PlayerLoginRequest *) pMsg->msgpara();
	CPlayer *pPlayer = CSceneObjManager::GetSingletonPtr()->GetPlayer(request->playerid());
	if (pPlayer == NULL) {
		pPlayer = new CPlayer(request->playerid());
		CSceneObjManager::GetSingletonPtr()->AddNewPlayer(pPlayer);
		CModuleManager::GetSingletonPtr()->OnCreateEntity(pPlayer);
	}
	CModuleManager::GetSingletonPtr()->OnPlayerLogin(pPlayer);
}

bool CCoreModule::CheckOnlineIsFull()
{
	return false;
}
