#include <string.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "share_mem.h"
#include "log.h"

/**
  *函数名          : MakeKey
  *功能描述        : 生成共享内存块唯一标识key
  *参数			  : pFile文件路径 vId：
  *返回值         ： 共享内存块唯一标识key
**/
key_t MakeKey(const char *pFile, int vId)
{
	return ftok(pFile, vId);
}

/**
  *函数名          : CreateShareMem
  *功能描述        : 创建共享内存块
  *参数			 :  iKey：共享内存块唯一标识key vSize：大小
  *返回值         ： 共享内存块地址
**/
BYTE *CreateShareMem(key_t iKey, long vSize)
{
	size_t iTempShmSize;
	int iShmID;

	if (iKey < 0) {
		LOG_ERROR("default", "CreateShareMem failed. errno:{}.", strerror(errno));
		exit(-1);
	}

	iTempShmSize = (size_t) vSize;
	//iTempShmSize += sizeof(CSharedMem);

	LOG_INFO("default", "Try to malloc share memory of {} bytes...", iTempShmSize);

	iShmID = shmget(iKey, iTempShmSize, IPC_CREAT | IPC_EXCL | 0666);

	if (iShmID < 0) {
		if (errno != EEXIST) {
			LOG_ERROR("default", "Alloc share memory failed, iKey:{}, size:{}, error:{}",
					  iKey, iTempShmSize, strerror(errno));
			exit(-1);
		}

//		LOG_NOTICE( "default", "Same shm seg (key=%08X) exist, now try to destroy it...", iKey);
//        DestroyShareMem(iKey);
//        iShmID = shmget( iKey, iTempShmSize, IPC_CREAT|IPC_EXCL|0666 );
//        if (iShmID < 0)
//        {
//            LOG_ERROR( "default", "Fatal error, alloc share memory failed, {}", strerror(errno));
//            exit(-1);
//        }
		LOG_INFO("default", "Same shm seg (key={}) exist, now try to attach it...", iKey);

		iShmID = shmget(iKey, iTempShmSize, 0666);
		if (iShmID < 0) {
			LOG_INFO("default", "Attach to share memory {} failed, {}. Now try to touch it", iShmID, strerror(errno));
			iShmID = shmget(iKey, 0, 0666);
			if (iShmID < 0) {
				LOG_ERROR("default", "Fatel error, touch to shm failed, {}.", strerror(errno));
				exit(-1);
			}
			else {
				LOG_INFO("default", "First remove the exist share memory {}", iShmID);
				if (shmctl(iShmID, IPC_RMID, NULL)) {
					LOG_ERROR("default", "Remove share memory failed, {}", strerror(errno));
					exit(-1);
				}
				iShmID = shmget(iKey, iTempShmSize, IPC_CREAT | IPC_EXCL | 0666);
				if (iShmID < 0) {
					LOG_ERROR("default", "Fatal error, alloc share memory failed, {}", strerror(errno));
					exit(-1);
				}
			}
		}
		else {
			LOG_INFO("default", "Attach to share memory succeed.");
		}
	}

	LOG_INFO("default", "Successfully alloced share memory block, (key={}), id = {}, size = {}",
			 iKey, iShmID, iTempShmSize);
	BYTE *tpShm = (BYTE *) shmat(iShmID, NULL, 0);

	if ((void *) -1 == tpShm) {
		LOG_ERROR("default", "create share memory failed, shmat failed, iShmID = {}, error = {}.",
				  iShmID, strerror(errno));
		exit(0);
	}

	return tpShm;
}

/************************************************
  函数名          : DestroyShareMem
  功能描述        : 销毁共享内存块
  参数			 :  iKey：共享内存块唯一标识key
  返回值         ： 成功0 错误：错误码
************************************************/
int DestroyShareMem(key_t iKey)
{
	int iShmID;

	if (iKey < 0) {
		LOG_ERROR("default", "Error in ftok, {}.", strerror(errno));
		return -1;
	}
	LOG_INFO("default", "Touch to share memory key = {}...", iKey);
	iShmID = shmget(iKey, 0, 0666);
	if (iShmID < 0) {
		LOG_ERROR("default", "Error, touch to shm failed, {}", strerror(errno));
		return -1;
	}
	else {
		LOG_INFO("default", "Now remove the exist share memory {}", iShmID);
		if (shmctl(iShmID, IPC_RMID, NULL)) {
			LOG_ERROR("default", "Remove share memory failed, {}", strerror(errno));
			return -1;
		}
		LOG_INFO("default", "Remove shared memory(id = {}, key = {}) succeed.", iShmID, iKey);
	}
	return 0;
}
