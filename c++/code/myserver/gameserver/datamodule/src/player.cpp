//
// Created by dguco on 17-7-23.
//
#include "../inc/player.h"

CPlayer::CPlayer(OBJ_ID playerId)
{
    set_id(playerId);
    Initialize();
}

CPlayer::~CPlayer()
{
}

int CPlayer::Initialize()
{
    m_oPackage.Initialize();
    m_spPlayerBase = new CPlayerBase(this);
    m_spPlayerCity = new CPlayerCity(this);
    return 0;
}