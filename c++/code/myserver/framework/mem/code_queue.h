//
//  codequeue.h
//  管理共享内存
//  Created by DGuco on 17/03/23.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _CODE_QUEUE_H_
#define _CODE_QUEUE_H_
#include "shm.h"

#define QUEUERESERVELENGTH 8        //预留长度

class CCodeQueue
{
	//禁止在栈上生成对象
protected:
	//构造函数
	CCodeQueue();
	//构造函数
	CCodeQueue(int nTotalSize, int iLockIdx = -1);

public:
	//析构函数
	~CCodeQueue();
	//new操作符重载将类,在指定的内存地址(共享内存)上生成对象
	void *operator new(size_t nSize);
	//delete操作符重载
	void operator delete(void *pBase);
	//初始化
	int Initialize(int nTotalSize);
	//恢复
	int Resume(int nTotalSize);
	//向进程间共享内存管道插入数据
	int AppendOneCode(const BYTE *pInCode, int sInLength);
	//从进程间共享内存管道获取数据
	int GetHeadCode(BYTE *pOutCode, int &psOutLength);
	//从进程间共享内存管道获取数据
	int PeekHeadCode(BYTE *pOutCode, int &psOutLength);
	//从进程间共享内存管道删除数据
	int DeleteHeadCode();
	//从进程间共享内存管道获取数据
	int GetOneCode(int iCodeOffset, int nCodeLength, BYTE *pOutCode, int &psOutLength);
	//共享内存管道是否空闲
	bool IsQueueEmpty();
	//向文件中备份数据
	int DumpToFile(const char *szFileName);
	//从文件中加载数据
	int LoadFromFile(const char *szFileName);
	//清除进程间共享内存管道数据
	int CleanQueue();

	static CCodeQueue *CreateInstance();
	static CCodeQueue *CreateInstance(int nTotalSize, int iLockIdx = -1);
	//codequeue大小
	static size_t CountQueueSize(int iBufSize);
	//获取共享内管道的其实地址
	BYTE *GetPipeAddr();
	static CSharedMem *pCurrentShm;

	void GetCriticalData(int &iBegin, int &iEnd, int &iLeft);

private:
	int IsQueueFull();
	int SetFullFlag(int iFullFlag);

	//获取数据读写索引
	void GetCriticalData(int &iBeginIdx, int &iEndIdx);
	//设置数据读写索引
	int SetCriticalData(int iReadIndex, int iWriteIndex);
	//获取可读数据长度
	int GetCanReadLen();
	//获取可写数据长度
	int GetCanWriteLen();

	struct _tagHead
	{
		int m_iSize;                //共享内存管道大小
		int m_iCodeBufOffSet;       //共享内存管道地址在CCodeQueue对象中的偏移地址
		int m_iReadIndex;           //读数据索引
		int m_iWriteIndex;          //写数据索引
		int m_iLockIdx;             //数据锁id
	} m_stQueueHead;

	//共享内存管道起始地址
	BYTE *m_pbyCodeBuffer;
};

#endif
