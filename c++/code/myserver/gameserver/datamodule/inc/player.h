//
// Created by DGuco on 17-7-23.
//

#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include <string.h>
#include "playerbase.h"
#include "playercity.h"
#include "base.h"
#include "object.h"

//下行客户端数据包信息
struct Package
{
public:
    Package() 
    {
        Initialize();
    }
    ~Package() {}
    int Initialize()
    {
        m_unSerial = 0;
        m_unCmd = 0;
        m_unSeq = 0;
        // m_bIsEncrpy = false;
        m_bIsDeal = false;
        return 0;
    }
public:
    void SetSerial(unsigned short seq) {m_unSerial = seq;}
    unsigned short  GetSerial(){return m_unSerial;}
    void SetSeq(unsigned short seq) {m_unSeq = seq;}
    unsigned short  GetSeq(){return m_unSeq;}
    void SetCmd(unsigned short cmd) {m_unCmd = cmd; }
    unsigned short  GetCmd(){return m_unCmd;}

    // bool GetIsEncrpy() {return m_bIsEncrpy;}
    // void SetIsEncrpy(bool isEncrpy) {m_bIsEncrpy = isEncrpy;}
    char* GetMessBuff() {return m_acMessageBuff;}
    bool GetIsDeal() {return m_bIsDeal;}
    void SetDeal(bool isDeal) {m_bIsDeal = isDeal;}
private:
    unsigned short m_unSerial;                  //序列码
    unsigned short m_unSeq;                     //消息应答码    
    unsigned short m_unCmd;                     //消息编号
    // bool  m_bIsEncrpy                        //是否加密
    bool  m_bIsDeal;                            //当前消息是否处理完成
    char m_acMessageBuff[MAX_PACKAGE_LEN];      //下行消息缓冲区
};

class CPlayerBase;
class CPlayerCity;

class CPlayer : public CObj
{
public:
    CPlayer(OBJ_ID playerId);
    virtual ~CPlayer();
    virtual int Initialize();

public:
    Package& GetPackage() {return m_oPackage;}
    CPlayerBase* GetPlayerBase() {return m_spPlayerBase;}
    CPlayerCity* GetPlayerCity() {return m_spPlayerCity;}
    const OBJ_ID GetPlayerId() {return get_id();}
private:
    Package m_oPackage;         //处理消息状态
    CPlayerBase* m_spPlayerBase;    //玩家基础信息
    CPlayerCity* m_spPlayerCity;    //玩家城池信息
};

#endif //SERVER_PLAYER_H_H
