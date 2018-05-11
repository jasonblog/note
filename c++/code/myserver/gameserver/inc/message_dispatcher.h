//
// Created by DGuco on 17-7-27.
//

#ifndef SERVER_MESSAGEDISPATCHER_H
#define SERVER_MESSAGEDISPATCHER_H

#include "server_tool.h"
#include "message.pb.h"

class CMessageDispatcher: public CSingleton<CMessageDispatcher>
{
public:
	CMessageDispatcher();
	~CMessageDispatcher();

	// 客户端上传的消息派发
	static int ProcessClientMessage(std::shared_ptr<CMessage> pMsg);
	// 服务器消息派发
	static int ProcessServerMessage(CProxyMessage *pMessage);
};

#endif //SERVER_MESSAGEDISPATCHER_H
