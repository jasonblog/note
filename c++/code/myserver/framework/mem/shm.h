//
//  shm.h
//  共享内存块管理类
//  Created by DGuco on 17/03/23.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _SHM_H_
#define _SHM_H_

#include "base.h"

#define YEARSEC 31536000

typedef enum
{
	SHM_INIT = 1,
	SHM_RECOVER = 0
} EIMode;

class CSharedMem
{
	//禁止在栈上生成对象
protected:
    //构造函数
	CSharedMem();
    //构造函数
	CSharedMem(unsigned int nKey, size_t nSize);
    //构造函数
	CSharedMem(unsigned int nKey, size_t nSize, int nInitFlag);

public:
    //析构函数
    ~CSharedMem();
    //初始化
	int Initialize(unsigned int nKey, size_t nSize);
    //设置共享内存创建时间戳
	void SetStamp();
    //new操作符重载将类对象定义到共享内存区地址
	void* operator new( size_t nSize);
    //delete操作符重载
	void  operator delete(void* pMem);
    //获取对象创建类型
	EIMode GetInitMode();
    //设置对象创建类型
	void SetInitMode( EIMode emMode );
    //在创建好的共享内存块上划分内存段
	void*  CreateSegment( size_t nSize);
    static CSharedMem* CreateInstance();
    static CSharedMem* CreateInstance(unsigned int nKey, size_t nSize);
    static CSharedMem* CreateInstance(unsigned int nKey, size_t nSize, int nInitFlag);

    //当前共享内存块地址大小通常为sizeof(CSharedMem) + sizeof(CCodeQueue) + PIPE_SIZE（可变）
	static BYTE* pbCurrentShm;

protected:

private:
    //共享内存key
	unsigned int	m_nShmKey;
    //共享内存大小
	size_t			m_nShmSize;
    //共享内存创建时间戳
	time_t			m_tCreateTime;
	time_t			m_tLastStamp;
    //crc校验码
	unsigned int	m_nCRC;
    //当前可用空闲内存去的起始地址
	BYTE*			m_pbCurrentSegMent; 
    //对象初始化类型
	EIMode			m_InitMode;
};


#endif
