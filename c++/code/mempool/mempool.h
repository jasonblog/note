//
//  mempool.h
//  Created by DGuco on 17/7/29.
//  Copyright © 2017年 DGuco. All rights reserved.
//  结合数组和双向链表实现对象内存池（注:使用该内存的类必须有无参的构造函数）
//

#ifndef SERVER_MEMPOOL_H
#define SERVER_MEMPOOL_H

#include <clocale>
template<class Type>
class CMemoryPool
{
public:
    CMemoryPool(void) :	m_pElementsSetList(NULL),m_pUnusedElementsList(NULL)
    {
        
    }
    ~CMemoryPool(void)
    {
        if (GetPoolSize() > 0 || IsCreated()){
            //注意这里一般手动调用Destroy，在析构函数中调用是为了防止我们忘记手动调用｀
            Destroy();
        }
    }
    
    //简直拷贝和赋值
    CMemoryPool(const CMemoryPool& other) = delete ;
    CMemoryPool&operator=(CMemoryPool& other) = delete;
private:
    //节点数据结构
    struct TagElement
    {
        Type		Element;
        TagElement*	pNext;
        TagElement*	pBefore;
    };
    
    //对象数组，注意这里的next的用途，当内存池的容量不够使，会另外开辟同等大小的空间来存储对象，
    //新数组的next会指向旧的数组，其实是一个链表，每个节点是一个数组
    struct TagElementsSet
    {
        //对象数组
        TagElement*		aElementsSet;
        TagElementsSet*	pNext;
    };
    
    //对象池数组
    TagElementsSet* m_pElementsSetList;
    //使用数量
    int			m_nNumOfAlloc;
    //当前可用的节点
    TagElement*	m_pUnusedElementsList;
    //一个数组的最大对象数量也是内存池的初始大小
    int			m_nNumOfElements;
    //数组数量
    int			m_nNumOfElementsSet;
    
public:
    //开辟指定数量的内存池
    bool	Create( uint nNumOfElements )
    {
        m_nNumOfElements	= nNumOfElements;
        m_nNumOfElementsSet	= 1;
        
        m_pElementsSetList					= new TagElementsSet;
        m_pElementsSetList->pNext			= NULL;
        //开辟数组
        m_pElementsSetList->aElementsSet	= new TagElement[m_nNumOfElements];
    
        //初始化链表结构
        for( int i = 0; i < m_nNumOfElements; i++ )
        {
            if( i > 0 )
            {
                m_pElementsSetList->aElementsSet[i].pBefore	= &m_pElementsSetList->aElementsSet[i-1];
                m_pElementsSetList->aElementsSet[i-1].pNext	= &m_pElementsSetList->aElementsSet[i];
            }
        }
        m_pElementsSetList->aElementsSet[0].pBefore					= NULL;
        m_pElementsSetList->aElementsSet[m_nNumOfElements-1].pNext	= NULL;
        
        //当前可用的节点信息
        m_pUnusedElementsList	= m_pElementsSetList->aElementsSet;
        m_nNumOfAlloc			= 0;
        return true;
    }
    
    //回收内存池
    void Destroy()
    {
        while( m_pElementsSetList )
        {
            if( m_pElementsSetList->aElementsSet )
            {
                delete[] m_pElementsSetList->aElementsSet;
                m_pElementsSetList->aElementsSet = NULL;
            }
            TagElementsSet* pFirst = m_pElementsSetList;
            m_pElementsSetList = m_pElementsSetList->pNext;
            
            delete pFirst;
        }
        m_nNumOfAlloc = 0;
        m_nNumOfAlloc = 0;
        m_nNumOfElementsSet = 0;
    }
    
    Type* Alloc()
    {
        //如果当前的链表使用完，开辟新的一条同等大小的链表
        if( m_pUnusedElementsList == NULL )
        {
            TagElementsSet* pSet	= new TagElementsSet;
            //把next之乡旧的数组
            pSet->pNext				= m_pElementsSetList;
            pSet->aElementsSet		= new TagElement[m_nNumOfElements];
            
            //初始化链表信息
            for( int i = 0; i < m_nNumOfElements; i++ )
            {
                if( i > 0 )
                {
                    pSet->aElementsSet[i].pBefore	= &pSet->aElementsSet[i-1];
                    pSet->aElementsSet[i-1].pNext	= &pSet->aElementsSet[i];
                }
            }
            pSet->aElementsSet[0].pBefore					= NULL;
            pSet->aElementsSet[m_nNumOfElements-1].pNext	= NULL;
            
            //把当前可用节点指向新数组的第一个元素
            m_pUnusedElementsList	= pSet->aElementsSet;
        
            m_pElementsSetList = pSet;
            m_nNumOfElementsSet++;
        }
        
        TagElement* pTagElement;
        pTagElement = m_pUnusedElementsList;
        m_pUnusedElementsList = m_pUnusedElementsList->pNext;
        if( m_pUnusedElementsList )
        {
            m_pUnusedElementsList->pBefore = NULL;
            
        }
    
        m_nNumOfAlloc++;
        return &(pTagElement->Element);
    }
    
    //回收一个对象
    void Free( Type* pElement )
    {
        TagElement* pTagElement = (TagElement*)pElement;
        pTagElement->pNext					= m_pUnusedElementsList;
        pTagElement->pBefore				= NULL;
        if( m_pUnusedElementsList )
            m_pUnusedElementsList->pBefore	= pTagElement;
        m_pUnusedElementsList				= pTagElement;
        m_nNumOfAlloc--;
    }
    
    int GetAllocatedSize()
    {
        return m_nNumOfAlloc;
    }
    
    int	GetPoolSize()
    {
        return m_nNumOfElements * m_nNumOfElementsSet;
    }
    
    bool IsCreated()
    {
        return m_pElementsSetList != NULL;
    }
    
};// class CMemoryPool

#endif //SERVER_MEMPOOL_H
