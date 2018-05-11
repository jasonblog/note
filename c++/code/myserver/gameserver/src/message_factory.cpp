/*
 *  message_factory.cpp
 *
 *  Created on  : 2017年8月8日
 *  Author      : DGuco
 */

#include "player.pb.h"
#include "../inc/message_factory.h"

// 单件定义
template<> CMessageFactory *CSingleton<CMessageFactory>::spSingleton = NULL;

// 根据消息ID，创建消息
Message *CMessageFactory::CreateMessage(unsigned int uiMessageID)
{
	Message *pTmpMessage = CreateClientMessage(uiMessageID);

	if (pTmpMessage == NULL) {
		pTmpMessage = CreateServerMessage(uiMessageID);
	}

	if (pTmpMessage == NULL) {
		LOG_ERROR("default", "[{} : {} : {}] invalid message id {}.", __MY_FILE__, __LINE__, __FUNCTION__, uiMessageID);
	}
	else {
		LOG_DEBUG("default", "create message id {} succeed.", uiMessageID);
	}

	return pTmpMessage;
}

Message *CMessageFactory::CreateClientMessage(unsigned int uiMessageID)
{
	Message *pTmpMessage = NULL;

	switch (uiMessageID) {
		// 只有客户端上行的消息需要在这里生成，下行的消息都是临时变量，不需要在MessageFactory中创建
	CASE_NEW_MSG(UserAccountLoginRequest, 100);
	CASE_NEW_MSG(PlayerLoginRequest, 101);
	default: {
		break;
	}
	}

	return pTmpMessage;
}

Message *CMessageFactory::CreateServerMessage(unsigned int uiMessageID)
{
	Message *pTmpMessage = NULL;

	switch (uiMessageID) {
		// 服务器内部的消息都需要在这里创建
		{
			break;
		}
	}

	return pTmpMessage;
}

