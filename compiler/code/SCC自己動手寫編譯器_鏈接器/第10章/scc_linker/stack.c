// 《自己动手写编译器、链接器》配套源代码

#include "scc.h"
/***********************************************************
 *  功能:		初始化栈储存容量
 *  stack:		栈存储结构
 *  initsize:	栈初始化分配空间
 **********************************************************/
void stack_init(Stack *stack,int initsize)
{
	stack->base = (void **) malloc(sizeof(void **) * initsize);
	if (!stack->base) 
	{
		error("内存分配失败");;
	} 
	else 
	{
		stack->top = stack->base;
		stack->stacksize = initsize;
	}
}

/***********************************************************
 *  功能:		插入元素element为新的栈顶元素
 *  stack:		栈存储结构
 *  element:	要插入栈顶的新元素
 *  size:		栈元素实际数据尺寸
 *  返回值:		栈顶元素
 **********************************************************/
void* stack_push(Stack *stack, void *element, int size)
{
	int newsize;
	if (stack->top >= stack->base + stack->stacksize) 
	{	
		newsize = stack->stacksize * 2;
		stack->base = (void **) realloc(stack->base,
				   (sizeof(void **) * newsize));
		if (!stack->base) 
		{
			return NULL;
		}
		stack->top = stack->base + stack->stacksize;
		stack->stacksize = newsize;
	}
	*stack->top = (void **) malloc(size);
	memcpy(*stack->top, element, size);
    stack->top++;
	return *(stack->top-1);
}

/***********************************************************
 *  功能:	弹出栈顶元素
 *  stack:	栈存储结构
 **********************************************************/
void stack_pop(Stack *stack)
{
	if (stack->top > stack->base) 
	{
		free(*(--stack->top));
	}
}

/***********************************************************
 *  功能:	得到栈顶元素
 *  stack:	栈存储结构
 *  返回值: 栈顶元素
 **********************************************************/
void* stack_get_top(Stack *stack)
{
	void **element;
    if (stack->top > stack->base) 
	{
		element = stack->top - 1;
		return *element;
	} 
	else 
	{
		return NULL;
	}
}

/***********************************************************
 *  功能:	判断栈是否为空
 *  stack:	栈存储结构
 *  返回值: 1表示栈为空,0表示栈非空
 **********************************************************/
int stack_is_empty(Stack *stack)
{
	if (stack->top == stack->base) {
		return 1;
	} 
	else 
	{
		return 0;
	}
}

/***********************************************************
 *  功能:	销毁栈
 *  stack:	栈存储结构
 **********************************************************/
void stack_destroy(Stack *stack)
{
	void **element;

	for (element = stack->base; element < stack->top; element++) 
	{
		free(*element);
	}
	if (stack->base) 
	{
		free(stack->base);
	}
	stack->base=NULL;
	stack->top=NULL;
	stack->stacksize=0;
}
