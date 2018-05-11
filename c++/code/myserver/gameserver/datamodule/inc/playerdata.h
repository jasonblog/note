//
// Created by DGuco on 17-9-6.
//

#ifndef SERVER_PLAYERDATA_H
#define SERVER_PLAYERDATA_H

#include <memory>
#include "object.h"

class CPlayer;
class CPlayerData
{
public:
    CPlayerData(CPlayer* pPlayer);
    ~CPlayerData();
private:
    CPlayer* m_pPlayer;
public:
    const CPlayer* GetPlayer();
    OBJ_ID GetPlayerId();
};

#endif //SERVER_PLAYERDATA_H
