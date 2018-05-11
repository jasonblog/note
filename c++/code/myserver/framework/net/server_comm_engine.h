#ifndef SERVER_COMM_ENGINE_H_
#define SERVER_COMM_ENGINE_H_


#include "message.pb.h"
#include "message_interface.h"

void pbmsg_setproxy(CProxyHead* pHead, int iSrcFE, int iSrcID, int iDstFE, int iDstID, time_t tTimestamp, enMessageCmd eCmd);
void pbmsg_setmessagehead(CProxyHead* pHead, int iMsgID);


class CServerCommEngine
{
public:
	// --------------------------------------------------------------------------------
	// Function:	ConvertStreamToMsg
	// Description:	把服务器的二进制流转换成CProxyHead + CMessage.msgpara
	// 				消息体指针已经在内部赋值给了CMessage.msgpara
	// 				注意: 需要在外部delete msgpara
	// --------------------------------------------------------------------------------
	static int ConvertStreamToMsg(const void* pBuff,
								  unsigned short unBuffLen,
								  CProxyMessage* pMsg,
								  CFactory* pMsgFactory = NULL);
	// --------------------------------------------------------------------------------
	// Function:	ConvertMsgToStream
	// Description:	序列化CProxyHead + CMessageHead + CMessage.msgpara
	// --------------------------------------------------------------------------------
	static int ConvertMsgToStream(CProxyMessage* pMsg,
								  void* pBuff,
								  unsigned short& unBuffLen);
	// --------------------------------------------------------------------------------
	// Function:	ConvertStreamToProxy
	// Description:	把服务器的二进制流转换成CProxyHead 
	// --------------------------------------------------------------------------------
	static int ConvertStreamToProxy(const void* pBuff,
									unsigned short unBuffLen,
									CProxyHead* pProxyHead);

};

#endif // SERVER_COMM_ENGINE_H_
