//
// Created by DGuco on 17-7-26.
// 结合数组和双向链表实现对象内存池（注:使用该内存的类必须有无参的构造函数）
//

#ifndef SERVER_MEMPOOL_H
#define SERVER_MEMPOOL_H

#include "base.h"
#include <clocale>
template<class Type>
class CMemoryPool
{
public:
	CMemoryPool(void)
		: m_pElementsSetList(NULL), m_pUnusedElementsList(NULL)
	{}
	virtual ~CMemoryPool(void)
	{}
private:
	//节点数据结构
	struct TagElement
	{
		Type Element;
		TagElement *pNext;
		TagElement *pBefore;
		bool bAlloc;
	};

	//对象链表
	struct TagElementsSet
	{
		//对象数组
		TagElement *aElementsSet;
		//如果初始化大小使用完，开辟另外一个同等大小的链表，并指向该链表
		TagElementsSet *pNext;
	};

	//对象池链表
	TagElementsSet *m_pElementsSetList;
	//使用数量
	int m_nNumOfAlloc;
	//当前可用的节点
	TagElement *m_pUnusedElementsList;
	//一个链表的最大对象数量
	int m_nNumOfElements;
	//链表数量
	int m_nNumOfElementsSet;

public:
	//开辟指定数量的内存池
	virtual bool Create(uint nNumOfElements)
	{
		m_nNumOfElements = nNumOfElements;
		m_nNumOfElementsSet = 1;

		m_pElementsSetList = new TagElementsSet;
		m_pElementsSetList->pNext = NULL;
		m_pElementsSetList->aElementsSet = new TagElement[m_nNumOfElements];

		for (int i = 0; i < m_nNumOfElements; i++) {
			if (i > 0) {
				m_pElementsSetList->aElementsSet[i].pBefore = &m_pElementsSetList->aElementsSet[i - 1];
				m_pElementsSetList->aElementsSet[i - 1].pNext = &m_pElementsSetList->aElementsSet[i];
			}
			m_pElementsSetList->aElementsSet[i].bAlloc = FALSE;
		}
		m_pElementsSetList->aElementsSet[0].pBefore = NULL;
		m_pElementsSetList->aElementsSet[m_nNumOfElements - 1].pNext = NULL;

		m_pUnusedElementsList = m_pElementsSetList->aElementsSet;
		m_nNumOfAlloc = 0;
		return TRUE;
	}

	//回收内存
	virtual void Destroy()
	{
		while (m_pElementsSetList) {
			if (m_pElementsSetList->aElementsSet) {
				delete[] m_pElementsSetList->aElementsSet;
				m_pElementsSetList->aElementsSet = NULL;
			}
			TagElementsSet *pOld = m_pElementsSetList;
			m_pElementsSetList = m_pElementsSetList->pNext;

			delete pOld;
		}
	}

	virtual Type *Alloc()
	{
		bool bTestAlloc = FALSE;
		//如果当前的链表使用完，开辟新的一条同等大小的链表
		if (m_pUnusedElementsList == NULL) {
			bTestAlloc = TRUE;
			TagElementsSet *pSet = new TagElementsSet;
			pSet->pNext = m_pElementsSetList;
			pSet->aElementsSet = new TagElement[m_nNumOfElements];

			for (int i = 0; i < m_nNumOfElements; i++) {
				if (i > 0) {
					pSet->aElementsSet[i].pBefore = &pSet->aElementsSet[i - 1];
					pSet->aElementsSet[i - 1].pNext = &pSet->aElementsSet[i];
				}
				pSet->aElementsSet[i].bAlloc = FALSE;
			}
			pSet->aElementsSet[0].pBefore = NULL;
			pSet->aElementsSet[m_nNumOfElements - 1].pNext = NULL;

			m_pUnusedElementsList = pSet->aElementsSet;

			m_pElementsSetList = pSet;

			m_nNumOfElementsSet++;
		}

		TagElement *pTagElement;

		pTagElement = m_pUnusedElementsList;
		m_pUnusedElementsList = m_pUnusedElementsList->pNext;
		if (m_pUnusedElementsList) {
			m_pUnusedElementsList->pBefore = NULL;

		}

		pTagElement->bAlloc = TRUE;

		m_nNumOfAlloc++;

		return (Type *) &(pTagElement->Element);
	}

	virtual void Free(Type *pElement)
	{
		TagElement *pTagElement = (TagElement *) pElement;
		pTagElement->pNext = m_pUnusedElementsList;
		pTagElement->pBefore = NULL;
		if (m_pUnusedElementsList)
			m_pUnusedElementsList->pBefore = pTagElement;
		m_pUnusedElementsList = pTagElement;
		m_nNumOfAlloc--;
		pTagElement->bAlloc = FALSE;
	}

	int GetAllocatedSize()
	{
		return m_nNumOfAlloc;
	}

	int GetPoolSize()
	{
		return m_nNumOfElements * m_nNumOfElementsSet;
	}

	bool IsCreated()
	{
		return m_pElementsSetList != NULL;
	}

};// class CMemoryPool

#endif //SERVER_MEMPOOL_H
