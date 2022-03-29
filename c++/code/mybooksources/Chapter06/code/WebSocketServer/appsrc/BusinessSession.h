/***************************************************************
 * ��Ȩ���� :
 * �ļ���   : BusinessSession.h
 * ����     : zhangyl
 * �汾     : 1.0.0.0
 * �������� : 2019.03.29
 * ����     :
 ***************************************************************/

#ifndef __BUSINESS_SESSION_H__
#define __BUSINESS_SESSION_H__

#include <string>

#include "../websocketsrc/MyWebSocketSession.h"

/** 
 * ʹ�÷������������Լ���BusinessSession�࣬������רע��ҵ����
 * BussinessSession��רע��ҵ����WebSocketSession��רע������ͨ�ű���
 */
class BusinessSession : public MyWebSocketSession
{
public:
    BusinessSession(std::shared_ptr<TcpConnection>& conn);
    virtual ~BusinessSession() = default;

public:
    void onConnect() override;
    bool onMessage(const std::string& strClientMsg) override;
 
private:
    void sendWelcomeMsg();

private:
    static std::string          m_strWelcomeMsg;        //��ӭ��Ϣ
};

#endif //!__BUSINESS_SESSION_H__
