// 《自己动手写编译器、链接器》配套源代码

#include "scc.h"
Stack global_sym_stack,		// 全局符号栈
	  local_sym_stack;		// 局部符号栈
Type char_pointer_type,		// 字符串指针				
	 int_type,				// int类型
	 default_func_type;		// 缺省函数类型
/***********************************************************
 * 功能:	查找结构定义
 * v:		符号编号
 **********************************************************/
Symbol *struct_search(int v)
{
    if (v >= tktable.count)
        return NULL;
	else
		return ((TkWord*)tktable.data[v])->sym_struct;
}

/***********************************************************
 * 功能:	查找结构定义 
 * v:		符号编号
 **********************************************************/
Symbol *sym_search(int v)
{
    if (v >= tktable.count)
        return NULL;
	else
		return ((TkWord*)tktable.data[v])->sym_identifier;
}

/***********************************************************
 * 功能:	将符号放在符号栈中 
 * v:		符号编号
 * type:	符号数据类型
 * c:		符号关联值 
 **********************************************************/
Symbol *sym_direct_push(Stack *ss, int v, Type *type, int c)
{
	Symbol s,*p;
    s.v = v;
	s.type.t = type->t;
	s.type.ref = type->ref;
    s.c = c;
    s.next = NULL;
	p = (Symbol*)stack_push(ss,&s,sizeof(Symbol));
    return p;
}

/*********************************************************** 
 * 功能:	将符号放在符号栈中,动态判断是放入全局符号栈还是局部符号栈
 * v:		符号编号
 * type:	符号数据类型
 * r:		符号存储类型
 * c:		符号关联值
 **********************************************************/
Symbol *sym_push(int v, Type *type, int r, int c)
{
    Symbol *ps, **pps;
    TkWord *ts;
	Stack *ss;

    if (stack_is_empty(&local_sym_stack) == 0)
	{
		ss = &local_sym_stack;
	}
    else
	{
		ss = &global_sym_stack;
	}
    ps = sym_direct_push(ss,v,type,c);
	ps->r = r;    

    // 不记录结构体成员及匿名符号
	if((v & SC_STRUCT) || v < SC_ANOM)
	{
        // 更新单词sym_struct或sym_identifier字段
        ts = (TkWord*)tktable.data[(v & ~SC_STRUCT)];
        if (v & SC_STRUCT)
            pps = &ts->sym_struct;
        else
            pps = &ts->sym_identifier;
        ps->prev_tok = *pps;
        *pps = ps;
    }
    return ps;
}

/*********************************************************** 
 * 功能:	弹出栈中符号直到栈顶符号为'b'
 * ptop:	符号栈栈顶
 * b:		符号指针
 **********************************************************/
void sym_pop(Stack *ptop, Symbol *b)
{
    Symbol *s, **ps;
    TkWord *ts;
    int v;

    s = (Symbol*)stack_get_top(ptop);
    while(s != b) 
	{
        v = s->v;
        // 更新单词表中sym_struct sym_identifier
		if((v & SC_STRUCT) || v < SC_ANOM)
		{
            ts = (TkWord*)tktable.data[(v & ~SC_STRUCT)];
            if (v & SC_STRUCT)
                ps = &ts->sym_struct;
            else
                ps = &ts->sym_identifier;
            *ps = s->prev_tok;
        }
		stack_pop(ptop);
        s = (Symbol*)stack_get_top(ptop);  	
    }
}

/*********************************************************** 
 * 功能:	生成指针类型
 * t:		原数据类型
 **********************************************************/
void mk_pointer(Type *t)
{
	Symbol *s;
    s = sym_push(SC_ANOM, t, 0, -1);
    t->t = T_PTR ;
    t->ref = s;
}


/***********************************************************
 * 功能:	返回类型长度
 * t:		数据类型指针
 * a:		对齐值
 **********************************************************/
int type_size(Type *t, int *a)
{
    Symbol *s;
    int bt;
	// 指针类型长度为4个字节
    int PTR_SIZE = 4;

    bt = t->t & T_BTYPE;
	switch(bt)
	{
    case T_STRUCT: 		
        s = t->ref;
        *a = s->r;
        return s->c;
		
	case T_PTR:
        if (t->t & T_ARRAY)
	{
            s = t->ref;
            return type_size(&s->type, a) * s->c;
        } 
		else 
		{
            *a = PTR_SIZE;
            return PTR_SIZE;
        }
		
	case T_INT:
        *a = 4;
        return 4;
		
	case T_SHORT:
        *a = 2;
        return 2;
		
	default:			// char, void, function       
        *a = 1;
        return 1;
    }
}

/*********************************************************** 
 * 功能:	将函数符号放入全局符号表中
 * v:		符号编号
 * type:	符号数据类型
 **********************************************************/
Symbol *func_sym_push(int v, Type *type)
{
    Symbol *s, **ps;
    s = sym_direct_push(&global_sym_stack, v, type, 0);
	
	ps = &((TkWord*)tktable.data[v])->sym_identifier;
	// 同名符号，函数符号放在最后-> ->...s
	while (*ps != NULL)
		ps = &(*ps)->prev_tok;
	s->prev_tok = NULL;
	*ps = s;
    return s;
}

Symbol *var_sym_put(Type *type, int r, int v, int addr)
{
	Symbol *sym = NULL;
	if ((r & SC_VALMASK) == SC_LOCAL)			// 局部变量
	{  
		
        sym = sym_push(v, type, r, addr);
    } 
	else if (v && (r & SC_VALMASK) == SC_GLOBAL) // 全局变量
	{
		sym = sym_search(v);
		if (sym)
			error("%s重定义\n",((TkWord*)tktable.data[v])->spelling);
		else
		{
			sym = sym_push(v, type, r | SC_SYM, 0);
		}
	}
	//else 字符串常量符号
	return sym;
}

/*********************************************************** 
 * 功能:	将节名称放入全局符号表
 * sec:		节名称
 * c:		符号关联值
 **********************************************************/
Symbol *sec_sym_put(char *sec,int c)
{
    TkWord* tp;
	Symbol *s;
	Type type;
	type.t = T_INT;
	tp = tkword_insert(sec);
	token = tp->tkcode;
	s = sym_push(token,&type,SC_GLOBAL,c); 
	return s;	
}