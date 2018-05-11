//
//  sharemem.h
//  调用系统共享内存接口创建共享内存
//  Created by DGuco on 17/03/23.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef __SHARE_MEM_H__
#define __SHARE_MEM_H__

#include "base.h"

//生成共享内存的唯一标识key
key_t MakeKey( const char* pFile, int vId );

//创建共享内存块
BYTE* CreateShareMem( key_t iKey, long vSize );

//销毁共享内存块
int DestroyShareMem( key_t iKey );

#endif // __SHARE_MEM_H__

