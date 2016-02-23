// 《自己动手写编译器、链接器》配套源代码

#include "scc.h"

/***********************************************************
 *  功能:		重新分配动态数组容量
 *  parr:		动态数组指针
 *  new_size:	动态数组最新元素个数
 **********************************************************/
void dynarray_realloc(DynArray *parr, int new_size)
{
    int capacity;
    void *data;

    capacity = parr->capacity;
    while (capacity < new_size)
        capacity = capacity * 2;
    data = realloc(parr->data, capacity);
    if (!data)
        error("内存分配失败");
    parr->capacity = capacity;
    parr->data = data;
}

/***********************************************************
 *  功能:	追加动态数组元素
 *  parr:	动态数组指针
 *  data:	所要追加的新元素
 **********************************************************/
void dynarray_add(DynArray *parr, void *data)
{
 	int count;
    count = parr->count + 1;
    if (count*sizeof(void*) > parr->capacity)
        dynarray_realloc(parr, count*sizeof(void*));
    parr->data[count - 1] = data;
    parr->count = count;  
}

/***********************************************************
 * 功能:		初始化动态数组储存容量
 * parr:		动态数组指针
 * initsize:	动态数组初始化分配空间
 **********************************************************/
void dynarray_init(DynArray *parr, int initsize)
{
	if(parr != NULL)
	{
		parr->data = (void**)malloc(sizeof(void*)*initsize);
		parr->count = 0;
		parr->capacity = initsize;
	}
}

/***********************************************************
 * 功能:	删除动态数组中第i个元素
 * parr:	动态数组指针
 * i:		第不个元素	
 **********************************************************/
void dynarray_delete(DynArray *parr,int i)
{
	if(parr->data[i])
		free(parr->data[i]);
    memcpy(parr->data+i,parr->data+i+1,sizeof(void*)*(parr->count-i-1));
    free(parr->data);
    parr->data = NULL;
}

/***********************************************************
 *  功能:	释放动态数组使用的内存空间
 *  parr:	动态数组指针
 **********************************************************/
void dynarray_free(DynArray *parr)
{
    void **p;
    for (p = parr->data; parr->count; ++p, --parr->count)
        if (*p)
            free(*p);
    free(parr->data);
    parr->data = NULL;
}

/***********************************************************
 *  功能:	动态数组元素查找
 *  parr:	动态数组指针
 *  key:	要查找的元素
 **********************************************************/                                             
int dynarray_search(DynArray *parr, int key)
{                                            
    int i;
   	int **p;
	p = (int**)parr->data;
    for (i = 0; i < parr->count; ++i, p++)            
    if (key == **p)                
        return i;                            
    return -1;                               
}         