/*
 * client_comm_engine.cpp
 *
 *  Created on: 2016年11月26日
 *      Author: DGuco
 */

#include "client_comm_engine.h"
#include "my_assert.h"

unsigned char CClientCommEngine::tKey[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 2, 2, 4, 4, 5, 6};

unsigned char *CClientCommEngine::tpKey = &tKey[0];

void CClientCommEngine::CopyMesHead(MesHead *from, MesHead *to)
{
	if (from && to) {
		for (int i = 0; i < from->socketinfos_size(); ++i) {
			CSocketInfo *pSocketInfo = to->mutable_socketinfos()->Add();
			CSocketInfo socketInfo = from->socketinfos(i);
			pSocketInfo->set_socketid(socketInfo.socketid());
			pSocketInfo->set_createtime(socketInfo.createtime());
			pSocketInfo->set_state(socketInfo.state());
		}
		to->set_cmd(from->cmd());
		to->set_seq(from->seq());
		to->set_serial(from->serial());
	}

}

int CClientCommEngine::ParseClientStream(CByteBuff *byteBuff,
										 MesHead *pHead)
{
	if ((byteBuff == NULL) || (pHead == NULL)) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ParseClientStream Input param failed.");
	}

	//小于最小长度(包头长度 - 包总长度所占字节长度)
	if (byteBuff->ReadableDataLen() < MSG_HEAD_LEN - sizeof(unsigned short)) {
		MY_ASSERT_STR(0,
					  return -1,
					  "The package len is less than base len ,receive len %d",
					  byteBuff->ReadableDataLen());
	}
	pHead->set_serial(byteBuff->ReadUnShort());
	pHead->set_seq(byteBuff->ReadUnShort());
	pHead->set_cmd(byteBuff->ReadUnShort());
	return 0;
}

int CClientCommEngine::ConvertToGameStream(CByteBuff *convertBuff,
										   const void *pDataBuff,
										   unsigned short &unDataLen,
										   MesHead *pHead)
{
	if ((convertBuff == NULL) || (pDataBuff == NULL) || pHead == NULL) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConverGateToGame Input param failed.");
	}

	convertBuff->Reset();
	//预留总长度
	unsigned short unLength = 0;
	unLength += sizeof(unsigned short);
	//预留8字节对齐长度
	unLength += sizeof(unsigned short);

	// MesHead长度
	unsigned short headLen = (unsigned short) pHead->ByteSize();
	convertBuff->WriteUnShort(headLen, unLength);
	unLength += sizeof(unsigned short);

	// MesHead
	if (pHead->SerializeToArray(convertBuff->CanWriteData(), convertBuff->WriteableDataLen()) == false) {
		MY_ASSERT_STR(0, return -2, "CClientCommEngine::ConvertMsgToStream CTcpHead SerializeToArray failed.");
	}
	convertBuff->WriteLen(pHead->GetCachedSize());
	unLength += pHead->GetCachedSize();

	// // 开始消息解密
	// char tEncryBuff[MAX_PACKAGE_LEN] = {0};
	// char* tpEncryBuff = &tEncryBuff[0];
	// unsigned int tOutLen = MAX_PACKAGE_LEN;
	// if (tTmpIsEncry)
	// {
	//     // 加密的消息处理
	//     int tDecLen = tOutLen;
	//     //这里每次都创建一个CAes 对象保证函数的无状态，线程安全
	//     CAes tmpAes;
	//     tmpAes.init(tpKey,16);
	//     int outlen;
	//     tpEncryBuff = (unsigned char*)tmpAes.decrypt((const char*)pDataBuff,unDataLen,tDecLen);
	//     tOutLen = tDecLen;
	// }
	// else
	// {
	//     // 未加密的消息处理
	//     tpEncryBuff = (unsigned char*)pTemp1;
	//     tOutLen = unDataLen;
	// }

	// data长度
	convertBuff->WriteUnShort((unsigned short) unDataLen);
	unLength += sizeof(unsigned short);

	// 拷贝消息到发送缓冲区
	convertBuff->WriteBytes((char *) pDataBuff, unDataLen);
	unLength += unDataLen;

	//8字节对齐
	unsigned short iTmpAddlen = (unLength % 8);
	if (iTmpAddlen > 0) {
		iTmpAddlen = 8 - iTmpAddlen;
		memset(convertBuff->CanWriteData(), 0, iTmpAddlen);
	}
	unLength += iTmpAddlen;

	//回到消息起始地值补充数据从长度和字节补齐长度
	convertBuff->SetWriteIndex(0);
	//序列话消息总长度
	convertBuff->WriteUnShort(unLength);
	//序列话8字节对齐长度
	convertBuff->WriteUnShort(iTmpAddlen);
	//数据总长度
	convertBuff->SetWriteIndex(unLength);
	return 0;
}

int CClientCommEngine::ConvertToGameStream(CByteBuff *convertBuff,
										   MesHead *pHead,
										   Message *pMsg)
{
	if (convertBuff == NULL || pHead == NULL) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	convertBuff->Reset();

	unsigned short unLength = 0;
	//预留总长度
	unLength += sizeof(unsigned short);
	//预留8字节对齐长度
	unLength += sizeof(unsigned short);

	// MesHead长度
	unsigned short headLen = (unsigned short) pHead->ByteSize();
	unLength += sizeof(unsigned short);
	convertBuff->WriteUnShort(headLen, unLength);

	// MesHead
	if (pHead->SerializeToArray(convertBuff->CanWriteData(), convertBuff->WriteableDataLen() - unLength) == false) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertToGateStream CTcpHead SerializeToArray failed.");
	}
	convertBuff->WriteLen(pHead->GetCachedSize());
	unLength += pHead->GetCachedSize();

	if (pMsg) {
		char tEncryBuff[MAX_PACKAGE_LEN] = {0};
		char *tpEncryBuff = &tEncryBuff[0];
		unsigned short iMsgParaLen = MAX_PACKAGE_LEN;
		if (pMsg->SerializeToArray(tpEncryBuff, iMsgParaLen) == false) {
			MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertToGateStream MsgPara SerializeToArray failed.");
		}
		// 消息长度
		convertBuff->WriteUnShort(pMsg->GetCachedSize());
		unLength += sizeof(unsigned short);

		//拷贝消息到缓冲区
		convertBuff->WriteBytes(tpEncryBuff, pMsg->GetCachedSize());
		unLength += pMsg->GetCachedSize();
	}

	//8字节对齐
	unsigned short iTmpAddlen = (unLength % 8);
	if (iTmpAddlen > 0) {
		iTmpAddlen = 8 - iTmpAddlen;
	}
	unLength += iTmpAddlen;

	//回到消息起始地值补充数据从长度和字节补齐长度
	convertBuff->SetWriteIndex(0);
	//序列话消息总长度
	convertBuff->WriteUnShort(unLength);
	//序列话8字节对齐长度
	convertBuff->WriteUnShort(iTmpAddlen);
	//数据总长度
	convertBuff->SetWriteIndex(unLength);
	return 0;
}

int CClientCommEngine::ConvertToGateStream(CByteBuff *pBuff,
										   MesHead *pHead,
										   Message *pMsg,
										   unsigned short cmd,
										   unsigned short serial,
										   unsigned short seq)
{
	if ((pBuff == NULL) || (pMsg == NULL) || pHead == NULL) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertMsgToStream Input param failed.");
	}

	unsigned short unLength = 0;
	//预留总长度
	unLength += sizeof(unsigned short);
	//预留8字节对齐长度
	unLength += sizeof(unsigned short);

	// MesHead长度
	unsigned short tmpHeadLen = (unsigned short) pHead->ByteSize();
	pBuff->WriteUnShort(tmpHeadLen, unLength);
	unLength += sizeof(unsigned short);

	// MesHead
	if (pHead->SerializeToArray(pBuff->CanWriteData(), pBuff->WriteableDataLen()) == false) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertToGateStream CTcpHead SerializeToArray failed.");
	}
	pBuff->WriteLen(pHead->GetCachedSize());
	unLength += pHead->GetCachedSize();

	//写入数据总长度(sizeof(总长度) + sizeof(cmd) + sizeof(serial) + sizeof(seq) + 消息长度)
	int dataLen = sizeof(unsigned short) + sizeof(cmd) + sizeof(serial) + sizeof(seq) + pMsg->GetCachedSize();
	pBuff->WriteUnShort(dataLen);
	unLength += sizeof(unsigned short);

	pBuff->WriteUnShort(serial);
	unLength += sizeof(unsigned short);

	pBuff->WriteUnShort(seq);
	unLength += sizeof(unsigned short);

	pBuff->WriteUnShort(cmd);
	unLength += sizeof(unsigned short);

	if (pMsg->SerializeToArray(pBuff->CanWriteData(), pBuff->WriteableDataLen()) == false) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertToGateStream MsgPara SerializeToArray failed.");
	}
	pBuff->WriteLen(pMsg->GetCachedSize());
	unLength += pMsg->GetCachedSize();
	// if (pHead->isencry())
	// {
	//     //这里每次都创建一个CAes 对象保证函数的无状态，线程安全
	//     CAes tmpAes;
	//     tmpAes.init(tpKey,16);
	//     tpEncryBuff = tmpAes.encrypt(tpEncryBuff,pMsg->ByteSize(), iMsgParaLen);
	// }
	// else
	// {
	//     iMsgParaLen = pMsg->ByteSize();
	// }

	//8字节对齐
	unsigned short iTmpAddlen = (unLength % 8);
	if (iTmpAddlen > 0) {
		iTmpAddlen = 8 - iTmpAddlen;
		memset(pBuff->CanWriteData(), 0, iTmpAddlen);
	}
	unLength += iTmpAddlen;

	//回到消息起始地值补充数据从长度和字节补齐长度
	pBuff->SetWriteIndex(0);
	//序列话消息总长度
	pBuff->WriteUnShort(unLength);
	//序列话8字节对齐长度
	pBuff->WriteUnShort(iTmpAddlen);

	pBuff->SetWriteIndex(unLength);
	return 0;
}

int CClientCommEngine::ConvertStreamToMessage(CByteBuff *pBuff,
											  unsigned short unBuffLen,
											  CMessage *pMessage,
											  CFactory *pMsgFactory)
{
	if ((pBuff == NULL)) {
		MY_ASSERT_STR(0, return -1, "CClientCommEngine::ConvertStreamToClientMsg Input param failed.");
	}

	unsigned short unTmpUseLen = 0;
	//取出数据总长度
	unsigned short unTmpTotalLen = pBuff->ReadUnShort();
	unTmpUseLen += sizeof(unsigned short);

	// 总长度不匹配
	if (unTmpTotalLen != unBuffLen) {
		MY_ASSERT_STR(0,
					  return -1,
					  "The package len is not equal to data len %d ,package len %d",
					  unBuffLen,
					  unTmpTotalLen);
	}

	// 字节对齐补充长度（采用8字节对齐）
	unsigned short unTmpAddlLen = pBuff->ReadUnShort();
	unTmpUseLen += sizeof(unsigned short);

	// MesHead长度
	unsigned short tmpHeadLen = pBuff->ReadUnShort();
	unTmpUseLen += sizeof(unsigned short);

	MesHead *pHead = pMessage->mutable_msghead();
	//反序列化失败
	if (pHead->ParseFromArray(pBuff->CanReadData(), tmpHeadLen) == false) {
		MY_ASSERT_STR(0, return -1, "MesHead ParseFromArray failed");
	}
	pBuff->ReadLen(pHead->GetCachedSize());
	unTmpUseLen += pHead->GetCachedSize();

	//消息长度
	unsigned short tmpDataLen = pBuff->ReadUnShort();;
	unTmpUseLen += sizeof(unsigned short);

	//扔掉字节对齐长度(字节补齐部分早尾部)
	unTmpUseLen += unTmpAddlLen;

	//序列化消息,消息
	if ((unTmpTotalLen - unTmpUseLen) > 0 && pMsgFactory != NULL && pMessage != NULL) {
		// MessagePara
		// 使用消息工厂
		Message *tpMsgPara = pMsgFactory->CreateMessage(pHead->cmd());
		if (tpMsgPara == NULL) {
			MY_ASSERT_STR(0, return -1, "Message CreateMessage failed");
		}

		if (tpMsgPara->ParseFromArray(pBuff->CanReadData(), tmpDataLen) == false) {
//			// 因为使用placement new，new在了一块静态存储的buffer上，只能析构，不能delete
//			// 并且是非线程安全的
//			pMessage->~Message();
			delete tpMsgPara;
			tpMsgPara = NULL;
			MY_ASSERT_STR(0, return -1, "Message ParseFromArray failed");
		}
		pBuff->ReadLen(tpMsgPara->GetCachedSize());
		pMessage->set_msgpara((unsigned long) tpMsgPara);
	}
	return 0;
}  
