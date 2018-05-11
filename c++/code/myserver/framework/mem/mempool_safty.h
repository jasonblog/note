//
// Created by DGuco on 17-7-26.
// 内存池安全操作接口,线程安全
//

#ifndef SERVER_MEMPOOLSAFTY_H
#define SERVER_MEMPOOLSAFTY_H

#include <mutex>
#include "mem_pool.h"

template<class Type>
class CMemoryPoolSafty: public CMemoryPool<Type>
{
private:
#ifdef _POSIX_MT_
	std::mutex	m_cs;
#endif

public:
	virtual bool Create(uint nNumOfElements)
	{
		return CMemoryPool<Type>::Create(nNumOfElements);
	}

	virtual void Destroy()
	{
		CMemoryPool<Type>::Destroy();
	}

	virtual Type *Alloc()
	{
#ifdef _POSIX_MT_
		std::lock_guard<std::mutex> guard(m_cs);
#endif
		Type *pType;
		pType = CMemoryPool<Type>::Alloc();
		return pType;
	}

	virtual void Free(Type *pElement)
	{
#ifdef _POSIX_MT_
		std::lock_guard<std::mutex> guard(m_cs);
#endif        CMemoryPool<Type>::Free( pElement );
	}

};
#endif //SERVER_MEMPOOLSAFTY_H
