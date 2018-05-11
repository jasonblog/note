//
// Created by dguco on 17-9-6.
//

#include "../inc/player.h"

CPlayerData::CPlayerData(CPlayer* pPlayer) : m_pPlayer(pPlayer)
{

}

CPlayerData::~CPlayerData() {
    //这里不释放指针指向的空间，为了方便拿到其他模块的数据这里保存了一个指向玩家数据的
    //指针，在真正销毁玩家数据的时候释放=>CSceneObjManager::DestroyPlayer
    m_pPlayer = NULL;
}

const CPlayer* CPlayerData::GetPlayer()
{
    if (m_pPlayer == NULL) {
        char msg[128];
        sprintf(msg, "[%s : %d : %s] Get player data error.,player id = %d",
                __MY_FILE__, __LINE__, __FUNCTION__);
        throw std::logic_error(msg);
    }
    return m_pPlayer;
}

OBJ_ID CPlayerData::GetPlayerId()
{
    return GetPlayer()->get_id();
}
