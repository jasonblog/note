#include "server_comm_engine.h"
#include "my_assert.h"

void pbmsg_setproxy(CProxyHead *pHead,
					int iSrcFE,
					int iSrcID,
					int iDstFE,
					int iDstID,
					time_t tTimestamp,
					enMessageCmd eCmd)
{
	pHead->set_srcfe(iSrcFE);
	pHead->set_srcid(iSrcID);
	pHead->set_dstfe(iDstFE);
	pHead->set_dstid(iDstID);
	pHead->set_opflag(eCmd);
	pHead->set_timestamp(tTimestamp);
}

void pbmsg_setmessagehead(CProxyHead *pMsg, int iMsgID)
{
	pMsg->set_messageid(iMsgID);
}

int CServerCommEngine::ConvertStreamToMsg(const void *pBuff,
										 unsigned short unBuffLen,
										 CProxyMessage *pMsg,
										 CFactory *pMsgFactory)
{
	if ((pBuff == NULL) || (pMsg == NULL)) {
		MY_ASSERT_STR(0, return -1, "CServerCommEngine::ConvertStreamToMsg Input param failed.");
	}
	char *tpBuff = (char *) pBuff;

	// 总长度
	unsigned short tTotalLen = unBuffLen;

	// 字节对齐补充长度（采用8字节对齐）
	unsigned short tAddLen = *(unsigned short *) tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	// 补齐的长度一定小于8字节
	if (tAddLen >= 8) {
		MY_ASSERT_STR(0, return -3, "CServerCommEngine::ConvertStreamToMsg tAddLen = %d impossibility.", tAddLen);
	}
	// 直接抛掉补充长度
	tTotalLen -= tAddLen;

	// CProxyHead长度
	unsigned short tTmpLen = 0;
	tTmpLen = *(unsigned short *) tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	CProxyHead *pProxyHead = pMsg->mutable_msghead();
	// CProxyHead
	if (pProxyHead->ParseFromArray(tpBuff, tTmpLen) != true) {
		MY_ASSERT_STR(0, return -4, "CServerCommEngine::ConvertStreamToMsg CProxyHead ParseFromArray failed.");
	}
	tpBuff += tTmpLen;
	tTotalLen -= tTmpLen;

	if (tTotalLen < 0) {
		MY_ASSERT_STR(0, return -5, "CServerCommEngine::ConvertStreamToMsg tTotalLen = %d impossibility.", tTotalLen);
	}

	if (tTotalLen == 0) {
		return 0;
	}

	// MessagePara长度
	tTmpLen = *(unsigned short *) tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	if (tTotalLen != tTmpLen) {
		// msgpara长度不匹配
		MY_ASSERT_STR(0,
					  return -7,
					  "CServerCommEngine::ConvertStreamToMsg failed, tTotalLen(%d) != tTmpLen.(%d)",
					  tTotalLen,
					  tTmpLen);
	}

	if (pMsgFactory) {
		// MessagePara
		// 使用消息工厂
		Message *tpMsgPara = NULL;

		tpMsgPara = pMsgFactory->CreateMessage(pMsg->msghead().messageid());

		if (tpMsgPara == NULL) {
			MY_ASSERT_STR(0,
						  return -8,
						  "CServerCommEngine::ConvertStreamToMsg CMessageFactory can't create msg id = %d.",
						  pMsg->msghead().messageid());
		}

		if (tpMsgPara->ParseFromArray(tpBuff, tTmpLen) != true) {
			// 因为使用placement new，new在了一块静态存储的buffer上，只能析构，不能delete
			// 并且是非线程安全的
			tpMsgPara->~Message();
			MY_ASSERT_STR(0, return -9, "CServerCommEngine::ConvertStreamToMsg CMessage.msgpara ParseFromArray failed.");
		}

		pMsg->set_msgpara((unsigned long) tpMsgPara);
	}

	return 0;
}

int CServerCommEngine::ConvertMsgToStream(CProxyMessage *pMsg,
										 void *pBuff,
										 unsigned short &unBuffLen)
{
	if ((pBuff == NULL) || (unBuffLen < 8)) {
		MY_ASSERT_STR(0, return -1, "CServerCommEngine::ConvertMsgToStream Input impossibility.");
	}

	char *tpBuff = (char *) pBuff;
	unsigned short tLen = 0;
	int tAddLen = 0;

	// 序列化总长度
	// 暂时跳过，后面补充
	tpBuff += sizeof(unsigned short);
	tLen += sizeof(unsigned short);

	// 序列化8字节对齐补充长度
	// 暂时跳过后面补充
	tpBuff += sizeof(unsigned short);
	tLen += sizeof(unsigned short);

	CProxyHead *pProxyHead = pMsg->mutable_msghead();
	// 序列化CProxyHead长度
	*(unsigned short *) tpBuff = pProxyHead->ByteSize();
	tpBuff += sizeof(unsigned short);
	tLen += sizeof(unsigned short);

	// 序列化CProxyHead
	if (pProxyHead->SerializeToArray(tpBuff, unBuffLen - tLen) == false) {
		MY_ASSERT_STR(0, return -2, "CServerCommEngine::ConvertMsgToStream CProxyHead SerializeToArray failed.");
	}
	tpBuff += pProxyHead->GetCachedSize();
	tLen += pProxyHead->GetCachedSize();

	// 如果没有消息
	// 发送给proxy或者proxy发出的消息，只有CProxyHead，所以到这里就OK了
	if (!pMsg->has_msgpara()) {
		// 主要是注册和心跳
		tAddLen = (tLen % 8);
		if (tAddLen > 0) {
			tAddLen = (8 - tAddLen);
		}

		tpBuff = (char *) pBuff;
		// 序列化总长度
		*(unsigned short *) tpBuff = (tLen + tAddLen);
		tpBuff += sizeof(unsigned short);

		// 序列化8字节对齐补充长度
		*(unsigned short *) tpBuff = tAddLen;

		unBuffLen = (tLen + tAddLen);
		return 0;
	}

	// 获取msgpara
	Message *tpMsgPara = (Message *) pMsg->msgpara();
	if (tpMsgPara == NULL) {
		MY_ASSERT_STR(0, return -4, "CServerCommEngine::ConvertMsgToStream CMessagePara is NULL.");
	}

	// msgpara长度
	*(unsigned short *) tpBuff = tpMsgPara->ByteSize();
	tpBuff += sizeof(unsigned short);
	tLen += sizeof(unsigned short);

	// 序列化msgpara
	if (tpMsgPara->SerializeToArray(tpBuff, unBuffLen - tLen) == false) {
		MY_ASSERT_STR(0, return -5, "CServerCommEngine::ConvertMsgToStream msgpara SerializeToArray failed.");
	}
	tpBuff += tpMsgPara->GetCachedSize();
	tLen += tpMsgPara->GetCachedSize();

	// 计算补充长度
	tAddLen = (tLen % 8);
	if (tAddLen > 0) {
		tAddLen = (8 - tAddLen);
	}

	tpBuff = (char *) pBuff;
	// 序列化总长度
	*(unsigned short *) tpBuff = (tLen + tAddLen);
	tpBuff += sizeof(unsigned short);

	// 序列化8字节对齐补充长度
	*(unsigned short *) tpBuff = tAddLen;

	unBuffLen = (tLen + tAddLen);

	return 0;
}

int CServerCommEngine::ConvertStreamToProxy(const void *pBuff,
										   unsigned short unBuffLen,
										   CProxyHead *pProxyHead)
{
	if ((pBuff == NULL) || (unBuffLen < (sizeof(unsigned short) * 3)) || (pProxyHead == NULL)) {
		MY_ASSERT_STR(0, return -1, "CServerCommEngine::ConvertStreamToMsg Input param failed.");
	}
	char *tpBuff = (char *) pBuff;

	// 总长度
	unsigned short tTotalLen = unBuffLen;

	// 字节对齐补充长度（采用8字节对齐）
	unsigned short tAddLen = *(unsigned short *) tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	// 补齐的长度一定小于8字节
	if (tAddLen >= 8) {
		MY_ASSERT_STR(0, return -3, "CServerCommEngine::ConvertStreamToMsg tAddLen = %d impoosibility.", tAddLen);
	}
	// 直接抛掉补充长度
	tTotalLen -= tAddLen;

	// CProxyHead长度
	unsigned short tHeadTmpLen = 0;
	tHeadTmpLen = *(unsigned short *) tpBuff;
	tpBuff += sizeof(unsigned short);
	tTotalLen -= sizeof(unsigned short);

	// CProxyHead
	if (pProxyHead->ParseFromArray(tpBuff, tHeadTmpLen) != true) {
		MY_ASSERT_STR(0, return -4, "CServerCommEngine::ConvertStreamToMsg CProxyHead ParseFromArray failed.");
	}
	tpBuff += tHeadTmpLen;
	tTotalLen -= tHeadTmpLen;

	if (tTotalLen < 0) {
		MY_ASSERT_STR(0, return -5, "CServerCommEngine::ConvertStreamToMsg tTotalLen = %d impossibility.", tTotalLen);
	}

	return 0;
}
