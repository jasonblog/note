// 《自己动手写编译器、链接器》配套源代码

#include "scc.h"
int syntax_state;
int syntax_level;

/***********************************************************
 * 功能:	解析翻译单位
 *
 * <translation_unit>::={external_declaration}<TK_EOF>
 **********************************************************/
void translation_unit()
{
	while(token != TK_EOF)
	{
		external_declaration(SC_GLOBAL);
	}
}

/***********************************************************
 * 功能:	解析外部声明
 * l:		存储类型，局部的还是全局的
 *
 * <external_declaration>::=<function_definition>|<declaration>
 *
 * <function_definition>::= <type_specifier> <declarator><funcbody>
 *
 * <declaration>::= <type_specifier><TK_SEMICOLON>
 *		|<type_specifier>< init_declarator_list><TK_SEMICOLON>
 *
 * <init_declarator_list>::=
 *      <init_declarator>{<TK_COMMA> <init_declarator>}
 *
 * <init_declarator>::=
 *      <declarator>|<declarator> <TK_ASSIGN><initializer>
 *
 * 改写后文法：
 * <external_declaration>::=
 *  <type_specifier> (<TK_SEMICOLON> 
 *      |<declarator><funcbody>
 *	    |<declarator>[<TK_ASSIGN><initializer>]
 *	     {<TK_COMMA> <declarator>[<TK_ASSIGN><initializer>]}
 *		 <TK_SEMICOLON>
  **********************************************************/
void external_declaration(int l)
{
	Type btype, type;
	int v, has_init, r,addr;
	Symbol *sym;
	Section *sec=NULL;

	if (!type_specifier(&btype)) 
	{
		expect("<类型区分符>");
	}
	
	if (btype.t == T_STRUCT && token == TK_SEMICOLON) 
	{
		get_token();
		return;
	}
    while (1) 
	{
		type = btype;
		declarator (&type, &v,NULL);

		if (token == TK_BEGIN) //函数定义
		{
			if (l == SC_LOCAL)
				error("不支持函数嵌套定义");
			
			if ((type.t & T_BTYPE) != T_FUNC)
				expect("<函数定义>");	
			
			sym = sym_search(v);
			if (sym)	// 函数前面声明过，现在给出函数定义
			{
				if ((sym->type.t & T_BTYPE) != T_FUNC)
					error("'%s'重定义", get_tkstr(v));
				sym->type = type;
			}
			else
			{				
				sym = func_sym_push(v, &type);
			}
			sym->r = SC_SYM | SC_GLOBAL;
			funcbody(sym);
			break;
		}
		else
		{
			if ((type.t & T_BTYPE) == T_FUNC) // 函数声明
			{
				if (sym_search(v) == NULL)
				{
					sym_push(v, &type, SC_GLOBAL | SC_SYM, 0);
				}
			}
			else //变量声明
			{
				r=0;
				if (!(type.t & T_ARRAY))
					r |= SC_LVAL;
				
				r |= l;
				has_init = (token == TK_ASSIGN);

				if(has_init)
				{
					get_token(); //不能放到后面，char str[]="abc"情况，需要allocate_storage求字符串长度				    
				}
		
				sec = allocate_storage(&type,r, has_init, v, &addr); 
			    sym = var_sym_put(&type, r, v, addr);
				if(l == SC_GLOBAL)
					coffsym_add_update(sym, addr, sec->index,0,IMAGE_SYM_CLASS_EXTERNAL); 

				if(has_init)
				{
					initializer(&type, addr, sec);
				}
			}
		    if (token == TK_COMMA) 
			{
				get_token();
			}
			else
			{
			  syntax_state = SNTX_LF_HT;
			  skip(TK_SEMICOLON);
			  break;
			}			
		}
	}	
}


/*********************************************************** 
 * 功能:	解析初值符
 * type:	变量类型
 * c:		变量相关值
 * sec:		变量所在节
 *
 * < initializer>::=<assignment_expression>
 **********************************************************/
void initializer(Type *type, int c, Section *sec)
{
	if (type->t & T_ARRAY && sec) 
	{
        memcpy(sec->data + c , tkstr.data, tkstr.count);
		get_token();
	}
	else
	{
		assignment_expression();
		init_variable(type, sec, c, 0);
	}
}

/***********************************************************
 * 功能:		类型区分符
 * type(输出):	数据类型
 * 返回值:		是否发现合法的类型区分符
 *
 * <type_specifier>::= <KW_INT> 
 *		| <KW_CHAR> 
 *		| <KW_SHORT> 
 *		| <KW_VOID >
 *		| <struct_specifier>
 **********************************************************/
int type_specifier(Type *type)
{		
	int t,type_found;
	Type type1;
	t= 0;
	type_found = 0;
    switch(token) 
	{
    case KW_CHAR:
		t = T_CHAR;
		type_found = 1;
		syntax_state = SNTX_SP;
		get_token();
		break;
	case KW_SHORT:
		t = T_SHORT;
		type_found = 1;
		syntax_state = SNTX_SP;
		get_token();
		break;
	case KW_VOID:
		t = T_VOID;
		type_found = 1;
		syntax_state = SNTX_SP;
		get_token();
		break;
	case KW_INT:
		t = T_INT;
		syntax_state = SNTX_SP;
		type_found = 1;
		get_token();
		break;
	case KW_STRUCT:
		syntax_state = SNTX_SP;
		struct_specifier(&type1);
        type->ref = type1.ref;
        t = T_STRUCT;
        type_found = 1;
		break;
	default:		
		break;
	}
	type->t = t;
	return type_found;
}

/***********************************************************
 * 功能:		解析结构区分符
 * type(输出):	结构类型
 *
 * <struct_specifier>::= 
 *	<KW_STRUCT><IDENTIFIER><TK_BEGIN><struct_declaration_list><TK_END> 
 *		| <KW_STRUCT><IDENTIFIER>
 **********************************************************/
void struct_specifier(Type *type)
{
    int v;
    Symbol *s;
    Type type1;
	
    get_token();	
	v = token;
	
	syntax_state = SNTX_DELAY;      // 新取单词不即时输出，延迟到取出单词后根据单词类型判断输出格式
	get_token();
	
	if(token == TK_BEGIN)			// 适用于结构体定义
		syntax_state = SNTX_LF_HT;
	else if(token == TK_CLOSEPA)	// 适用于 sizeof(struct struct_name)
		syntax_state = SNTX_NUL;
	else							// 适用于结构变量声明
		syntax_state = SNTX_SP;
	syntax_indent();	
	
	if (v < TK_IDENT)				// 关键字不能作为结构名称
		expect("结构体名");
	s = struct_search(v);
	if (!s) 
	{		
		type1.t = KW_STRUCT;
		// -1表示结构体未定义
		s = sym_push(v | SC_STRUCT, &type1, 0, -1);
		s->r = 0;
	} 
	
    type->t = T_STRUCT;
    type->ref = s;
    
    if (token == TK_BEGIN) 
	{
		struct_declaration_list(type);
    }
}

/***********************************************************
 * 功能:		解析结构声明符表
 * type(输出):	结构类型
 *
 * <struct_declaration_list>::=<struct_declaration>{<struct_declaration>}
 **********************************************************/
void struct_declaration_list(Type *type)
{
    int maxalign, offset;
    Symbol *s,**ps;
	s = type->ref;
	syntax_state = SNTX_LF_HT;   // 第一个结构体成员与'{'不写在一行
	syntax_level++;              // 结构体成员变量声明，缩进增加一级
	get_token();
	if (s->c != -1)
		error("结构体已定义");
	maxalign = 1;
	ps = &s->next;
	offset = 0;
	while (token != TK_END) 
	{
		struct_declaration(&maxalign, &offset,&ps);
	}			
	skip(TK_END);
	syntax_state = SNTX_LF_HT;
	
	s->c = calc_align(offset,maxalign);//结构体大小
	s->r = maxalign; //结构体对齐
}

/***********************************************************
 * 功能:				解析结构声明
 * maxalign(输入,输出):	成员最大对齐粒度
 * offset(输入,输出):	偏移量
 * ps(输出):			结构定义符号
 *
 * <struct_declaration>::=
 *		<type_specifier><struct_declarator_list><TK_SEMICOLON>
 *
 * <struct_declarator_list>::=<declarator>{<TK_COMMA><declarator>}
**********************************************************/
void struct_declaration(int *maxalign,int *offset,Symbol ***ps)
{
	int v, size, align;
    Symbol *ss;
    Type type1, btype;
	int force_align;
	type_specifier(&btype);
	while (1) 
	{
		v = 0;
		type1 = btype;
		declarator (&type1, &v,&force_align);
		size = type_size(&type1, &align);
		
		if(force_align & ALIGN_SET)
			align = force_align & ~ALIGN_SET;
		
		*offset = calc_align(*offset,align);
		
		if (align > *maxalign)
			*maxalign = align;                       
		ss = sym_push(v | SC_MEMBER, &type1, 0, *offset);
		*offset += size;
		**ps = ss;
		*ps = &ss->next;
		
		if (token == TK_SEMICOLON || token == TK_EOF)
			break;
		skip(TK_COMMA);
	}
	syntax_state = SNTX_LF_HT;
	skip(TK_SEMICOLON);
}


/***********************************************************
 * 功能:				解析声明符
 * type:				数据类型
 * v(输出):				单词编号
 * force_align(输出):	强制对齐粒度
 *
 * <declarator>::={<pointer>}{<function_calling_convention>}
 *	{<struct_member_alignment>}<direct_declarator>
 *
 * <pointer>::=<TK_STAR>	
 **********************************************************/
void declarator(Type *type, int *v, int *force_align)
{
    int fc;
    while (token == TK_STAR) 
	{
        mk_pointer(type);
		get_token();
    }
	function_calling_convention (&fc);
	if(force_align)
	struct_member_alignment(force_align);
	direct_declarator(type,v,fc);  
}

/***********************************************************
 * 功能:	解析函数调用约定
 * fc(输出):调用约定
 *
 * <function_calling_convention>::=<KW_CDECL>|<KW_STDCALL>
 * 用于函数声明上，用在数据声明上忽略掉
 **********************************************************/
void function_calling_convention (int *fc)
{
	*fc = KW_CDECL;
	if(token == KW_CDECL || token == KW_STDCALL)
	{
		*fc = token;
		syntax_state = SNTX_SP;
		get_token();		
	}
}

/***********************************************************
 * 功能:				解析结构成员对齐
 * force_align(输出):	强制对齐粒度
 *
 * <struct_member_alignment>::=<KW_ALIGN><TK_OPENPA><TK_CINT><TK_CLOSEPA>
 **********************************************************/
void struct_member_alignment(int *force_align)
{
	int align = 1;
	if(token == KW_ALIGN)
	{
		get_token();
		skip(TK_OPENPA);
		if(token == TK_CINT)
		{
			 get_token();
			 align = tkvalue;
		}
		else
			expect("整数常量");
		skip(TK_CLOSEPA);
		if(align!=1 && align!=2 && align!=4)
		  align = 1;
	    align |= ALIGN_SET;	
		*force_align = align;
	}
	else
	  *force_align = 1;	
}

/***********************************************************
 * 功能:			解析直接声明符
 * type(输入,输出):	数据类型
 * v(输出):			单词编号	
 * func_call:		函数调用约定
 *
 * <direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
 **********************************************************/
void direct_declarator(Type *type,int *v,int func_call)
{
	if (token >= TK_IDENT) 
	{		 
		*v = token;
		get_token();
	} 
	else
	{
		expect("标识符");
	}
	direct_declarator_postfix(type,func_call);
}

/***********************************************************
 * 功能:			直接声明符后缀
 * type(输入,输出):	数据类型
 * func_call:		函数调用约定
 *
 * <direct_declarator_ postfix>::= {<TK_OPENBR><TK_CINT><TK_CLOSEBR>
 * 		|<TK_OPENBR><TK_CLOSEBR>
 *		|<TK_OPENPA><parameter_type_list><TK_CLOSEPA> 
 *		|<TK_OPENPA><TK_CLOSEPA>}
 **********************************************************/
void direct_declarator_postfix(Type *type,int func_call)
{
	int n;
    Symbol *s;

    if (token == TK_OPENPA) 
	{ 
		parameter_type_list(type,func_call);
	}
	else if (token == TK_OPENBR)
	{
		get_token();
		n = -1;
		if(token == TK_CINT)
		{
			get_token();
			n = tkvalue;
		}
		skip(TK_CLOSEBR);
		direct_declarator_postfix(type,func_call);
		s = sym_push(SC_ANOM, type, 0, n);
		type->t = T_ARRAY | T_PTR;
		type->ref = s;
	}
}

/***********************************************************
 * 功能:			解析形参类型表
 * type(输入,输出):	数据类型
 * func_call:		函数调用约定
 *
 * <parameter_type_list>::=<parameter_list>
 *        |<parameter_list><TK_COMMA><TK_ELLIPSIS>
 *
 *  <parameter_list>::=
 *		<parameter_declaration>{<TK_COMMA ><parameter_declaration>}
 **********************************************************/
void parameter_type_list(Type *type,int func_call)
{
	int n;
    Symbol **plast, *s, *first;
    Type pt;

	get_token();
	first = NULL;
	plast = &first;    
	
	while(token != TK_CLOSEPA)
	{
		if (!type_specifier(&pt)) 
		{
			error("无效类型标识符");
		}
		declarator(&pt, &n,NULL);
		s = sym_push(n | SC_PARAMS, &pt, 0, 0);
		*plast = s;
		plast = &s->next;
		if (token == TK_CLOSEPA)
			break;
		skip(TK_COMMA);
		if (token == TK_ELLIPSIS) 
		{
			func_call = KW_CDECL;
			get_token();
			break;
		}
	}
	syntax_state = SNTX_DELAY;
	skip(TK_CLOSEPA);
	if(token == TK_BEGIN)			// 函数定义
		syntax_state = SNTX_LF_HT;
	else							// 函数声明
		syntax_state = SNTX_NUL;
	syntax_indent();
	
	// 此处将函数返回类型存储，然后指向参数，最后将type设为函数类型，引用的相关信息放在ref中
	s = sym_push(SC_ANOM, type, func_call, 0);
	s->next = first;
	type->t = T_FUNC;
	type->ref = s;
}

/***********************************************************
 * 功能:	解析函数体
 * sym:		函数符号
 *
 * <funcbody>::=<compound_statement>
 **********************************************************/
void funcbody(Symbol *sym)
{
	ind = sec_text->data_offset;
    coffsym_add_update(sym, ind, sec_text->index,CST_FUNC,IMAGE_SYM_CLASS_EXTERNAL);
	/* 放一匿名符号在局部符号表中 */
    sym_direct_push(&local_sym_stack, SC_ANOM, &int_type, 0);
	gen_prolog(&sym->type);
    rsym = 0;
    compound_statement(NULL,NULL); 
	backpatch(rsym,ind);
	gen_epilog();
	sec_text->data_offset = ind;
	sym_pop(&local_sym_stack, NULL); /* 清空局部符号栈*/
}

/***********************************************************
 * 功能:	判断是否为类型区分符
 * v:		单词编号
 **********************************************************/
int is_type_specifier(int v)
{
	switch(v)
	{
	case KW_CHAR:		
	case KW_SHORT: 
	case KW_INT:		
	case KW_VOID:		
	case KW_STRUCT:
		return 1;
	default:
		break;
	}
	return 0;
}

/***********************************************************
 * 功能:	解析语句
 * bsym:	break跳转位置
 * csym:	continue跳转位置
 *
 * <statement >::=<compound_statement> 
 *		|<if_statement> 
 *		| <return_statement> 
 *		| <break_statement> 
 *		| <continue_statement> 
 *		| <for_statement> 
 *		| <expression_statement>
 **********************************************************/
void statement(int *bsym, int *csym)
{
	switch(token)
	{
	case TK_BEGIN:
		compound_statement(bsym, csym);
		break;
	case KW_IF:
		if_statement(bsym, csym);
		break;
	case KW_RETURN:
		return_statement();
		break;
	case KW_BREAK:
		break_statement(bsym);
		break;
	case KW_CONTINUE:
		continue_statement(csym);
		break;
	case KW_FOR:
		for_statement(bsym,csym);
		break;
	default:
		expression_statement();
		break;
	}	
}

/***********************************************************
 * 功能:	解析复合语句
 * bsym:	break跳转位置
 * csym:	continue跳转位置
 * 
 * <compound_statement>::=<TK_BEGIN>{<declaration>}{<statement>}<TK_END>
 **********************************************************/
void compound_statement(int *bsym, int *csym)
{ 	
	Symbol *s;
	s = (Symbol*)stack_get_top(&local_sym_stack);
	syntax_state = SNTX_LF_HT;
	syntax_level++;						// 复合语句，缩进增加一级
	
	get_token();
	while(is_type_specifier(token))
	{
		external_declaration(SC_LOCAL);
	}
	while (token != TK_END) 
	{		
		statement(bsym, csym);
	}		
	sym_pop(&local_sym_stack, s);
	syntax_state = SNTX_LF_HT;
	get_token();	
}

/***********************************************************
 * 功能:	解析if语句
 * bsym:	break跳转位置
 * csym:	continue跳转位置
 *
 * <if_statement>::=<KW_IF><TK_OPENPA><expression>
 *	<TK_CLOSEPA><statement>[<KW_ELSE><statement>]
 **********************************************************/
void if_statement(int *bsym, int *csym)
{
	int a, b;
	syntax_state = SNTX_SP;
	get_token();
	skip(TK_OPENPA);
	expression();
	syntax_state = SNTX_LF_HT;		
	skip(TK_CLOSEPA);
	a = gen_jcc(0);
	statement(bsym, csym);		
	if (token == KW_ELSE) 
	{
		syntax_state = SNTX_LF_HT;
		get_token();
		b = gen_jmpforward(0);
		backpatch(a,ind);
		statement(bsym, csym);
		backpatch(b,ind); /* 反填else跳转 */
	}
	else
		backpatch(a,ind);
}

/***********************************************************
 * 功能:	解析for语句
 * bsym:	break跳转位置
 * csym:	continue跳转位置
 * 
 * <for_statement>::=<KW_FOR><TK_OPENPA><expression_statement>
 *	<expression_statement><expression><TK_CLOSEPA><statement>
 **********************************************************/
void for_statement(int *bsym, int *csym)
{
	int a, b, c, d, e;
	get_token();
	skip(TK_OPENPA);
	if (token != TK_SEMICOLON) 
	{
		expression();
		operand_pop();
	}
	skip(TK_SEMICOLON);
	d = ind;
	c = ind;
	a = 0;
	b = 0;
	if (token != TK_SEMICOLON) 
	{
		expression();
		a = gen_jcc(0);
	}
	skip(TK_SEMICOLON);
	if (token != TK_CLOSEPA) 
	{
		e = gen_jmpforward(0);
		c = ind;
		expression();
		operand_pop();
		gen_jmpbackword(d);
		backpatch(e,ind);
	}
	syntax_state = SNTX_LF_HT;
	skip(TK_CLOSEPA);
	statement(&a, &b);//只有此处用到break,及continue,一个循环中可能有多个break,或多个continue,故需要拉链以备反填
	gen_jmpbackword(c);
	backpatch(a,ind);
	backpatch(b, c);
}

/***********************************************************
 * 功能:	解析continue语句
 * csym:	continue跳转位置
 * 
 * <continue_statement>::=<KW_CONTINUE><TK_SEMICOLON>
 **********************************************************/
void continue_statement(int *csym)
{
	if (!csym)
		error("此处不能用continue");
	*csym = gen_jmpforward(*csym);
	get_token();
	syntax_state = SNTX_LF_HT;
	skip(TK_SEMICOLON);	
}

/***********************************************************
 * 功能:	解析break语句
 * bsym:	break跳转位置
 * 
 * <break_statement>::=<KW_BREAK><TK_SEMICOLON>
 **********************************************************/
void break_statement(int *bsym)
{
	if (!bsym)
		error("此处不能用break");
	*bsym = gen_jmpforward(*bsym);
	get_token();
	syntax_state = SNTX_LF_HT;
	skip(TK_SEMICOLON);	
}
/***********************************************************
 * 功能:	解析return语句
 *  
 * <return_statement>::=<KW_RETURN><TK_SEMICOLON>
 *			|<KW_RETURN><expression><TK_SEMICOLON>
 **********************************************************/
void return_statement()
{
	syntax_state = SNTX_DELAY;
	get_token();
	if(token == TK_SEMICOLON)	// 适用于 return;
		syntax_state = SNTX_NUL;
	else						// 适用于 return <expression>;
		syntax_state = SNTX_SP;
	syntax_indent();
	
	if (token != TK_SEMICOLON) 
	{
		expression();
		load_1(REG_IRET,optop);
		operand_pop();
	}
	syntax_state = SNTX_LF_HT;
	skip(TK_SEMICOLON);		
	rsym = gen_jmpforward(rsym);
}

/***********************************************************
 * 功能:	解析表达式语句
 *  
 * <expression_statement>::= <TK_SEMICOLON>|<expression><TK_SEMICOLON> 
 **********************************************************/
void expression_statement()
{	
	if (token != TK_SEMICOLON) 
	{
		expression();
		operand_pop();
	}
	syntax_state = SNTX_LF_HT;
	skip(TK_SEMICOLON);
}

/***********************************************************
 * 功能:	解析表达式
 * 
 * <expression>::=<assignment_expression>{<TK_COMMA><assignment_expression>}
 **********************************************************/
void expression()
{
    while (1) 
	{
        assignment_expression();
        if (token != TK_COMMA)
            break;
        operand_pop();
        get_token();
    }
}


/***********************************************************
 * 功能:	解析赋值表达式
 *
 * <assignment_expression>::= <equality_expression>
 *		|<unary_expression><TK_ASSIGN> <equality_expression> 
 **********************************************************/
//这里有左递归，可以提取公因子
void assignment_expression()
{
    equality_expression();
    if (token == TK_ASSIGN) 
	{
        check_lvalue();
        get_token();
        assignment_expression();
        store0_1();
    }
}

/***********************************************************
 * 功能:	解析相等类表达式
 *
 * < equality_expression >::=<relational_expression>
 *		{<TK_EQ> <relational_expression>
 *		|<TK_NEQ><relational_expression>}
 **********************************************************/
void equality_expression()
{

    int t;
    relational_expression();
    while (token == TK_EQ || token == TK_NEQ) 
	{
        t = token;
        get_token();
        relational_expression();
        gen_op(t);
    }
}

/***********************************************************
 * 功能:	解析关系表达式
 *
 * <relational_expression>::=<additive_expression>{
 *		<TK_LT><additive_expression> 
 *		|<TK_GT><additive_expression> 
 *		|<TK_LEQ><additive_expression> 
 *		|<TK_GEQ><additive_expression>}
 **********************************************************/
void relational_expression()
{
    int t;
    additive_expression();
    while ((token == TK_LT || token == TK_LEQ) ||
           token == TK_GT || token == TK_GEQ) 
	{
        t = token;
        get_token();
        additive_expression();
        gen_op(t);
    }
}

/***********************************************************
 * 功能:	解析加减类表达式
 *
 * <additive_expression>::=< multiplicative_expression> 
 *		{<TK_PLUS> <multiplicative_expression>
 *		<TK_MINUS>< multiplicative_expression>}
 **********************************************************/
void additive_expression()
{
    int t;
    multiplicative_expression();
    while (token == TK_PLUS || token == TK_MINUS)
	{
        t = token;
        get_token();
        multiplicative_expression();
        gen_op(t);
    }
}

/***********************************************************
 * 功能:	解析乘除类表达式
 *
 * <multiplicative_expression>::=<unary_expression>
 *		{<TK_STAR>  < unary_expression >
 *		|<TK_DIVIDE>< unary_expression > 
 *		|<TK_MOD>  < unary_expression >}
 **********************************************************/
void multiplicative_expression()
{
    int t;
    unary_expression();
    while (token == TK_STAR || token == TK_DIVIDE || token == TK_MOD) 
	{
        t = token;
        get_token();
        unary_expression();
        gen_op(t);
    }
}

/***********************************************************
 * 功能:	解析一元表达式
 *
 * <unary_expression>::= <postfix_expression> 
 *			|<TK_AND><unary_expression> 
 *			|<TK_STAR><unary_expression> 
 *			|<TK_PLUS><unary_expression> 
 *			|<TK_MINUS><unary_expression> 
 *			|<KW_SIZEOF><TK_OPENPA><type_specifier><TK_ CLOSEPA> 
 **********************************************************/
void unary_expression()
{
    switch(token) 
	{
   	case TK_AND:
        get_token();
        unary_expression();
		if ((optop->type.t & T_BTYPE) != T_FUNC &&
            !(optop->type.t & T_ARRAY))
		    cancel_lvalue();
		mk_pointer(&optop->type);        
        break;
	case TK_STAR:
        get_token();
		unary_expression();
        indirection();
        break;
	case TK_PLUS:
        get_token();       
        unary_expression();
        break;
	case TK_MINUS:
        get_token();
        operand_push(&int_type, SC_GLOBAL, 0);
        unary_expression();
        gen_op(TK_MINUS);
        break;
    case KW_SIZEOF:
		sizeof_expression();
        break;
	default:
		postfix_expression();
		break;
	}

}

/***********************************************************
 * 功能:	解析sizeof表达式
 *
 * <sizeof_expression>::= 
 *		<KW_SIZEOF><TK_OPENPA><type_specifier><TK_ CLOSEPA>
 **********************************************************/
void sizeof_expression()
{
	int align, size;
	Type type;
	
	get_token();
	skip(TK_OPENPA);
	type_specifier(&type);
	skip(TK_CLOSEPA);

	size = type_size(&type, &align);
	if (size < 0)
		error("sizeof计算类型尺寸失败");
	operand_push(&int_type, SC_GLOBAL, size);	
}

/***********************************************************
 * 功能:	解析后缀表达式
 *
 * <postfix_expression>::=  <primary_expression> 
 *		{<TK_OPENBR><expression> <TK_CLOSEBR> 
 *		|<TK_OPENPA><TK_CLOSEPA>
 *		|<TK_OPENPA><argument_expression_list><TK_CLOSEPA>
 *		|<TK_DOT><IDENTIFIER> 
 *		|<TK_POINTSTO><IDENTIFIER>}
 **********************************************************/
void postfix_expression()
{
	Symbol *s;
	primary_expression();
    while (1) 
	{
		if (token == TK_DOT || token == TK_POINTSTO) 
		{		
			if (token == TK_POINTSTO) 
                indirection();
            cancel_lvalue();
            get_token();
            if ((optop->type.t & T_BTYPE) != T_STRUCT)
                expect("结构体变量");
            s = optop->type.ref;
            token |= SC_MEMBER;
            while ((s = s->next) != NULL) 
			{
                if (s->v == token)
                    break;
            }
            if (!s)
                error("没有此成员变量: %s",  get_tkstr(token & ~SC_MEMBER));
            /* 成员变量地址 = 结构变量指针 + 成员变量偏移 */
            optop->type = char_pointer_type; /* 成员变量的偏移是指相对于结构体首地址的字节偏移，因此此处变换类型为字节变量指针 */
            operand_push(&int_type, SC_GLOBAL, s->c);
            gen_op(TK_PLUS);  //执行后optop->value记忆了成员地址
            /* 变换类型为成员变量数据类型 */
            optop->type = s->type;
            /* 数组变量不能充当左值 */
            if (!(optop->type.t & T_ARRAY)) 
			{
                optop->r |= SC_LVAL;
            }
            get_token();
		} 
		else if (token == TK_OPENBR) 
		{
            get_token();
            expression();
            gen_op(TK_PLUS);
            indirection();
            skip(TK_CLOSEBR);
        } 
		else if (token == TK_OPENPA) 
		{
			argument_expression_list();
		}
		else
			break;
	}

}

/***********************************************************
 * 功能:	解析初等表达式
 *
 * <primary_expression>::=<IDENTIFIER>
 *		|<TK_CINT>
 *		|<TK_CSTR>
 *		|<TK_CCHAR>
 *		|<TK_OPENPA><expression><TK_CLOSEPA>
 **********************************************************/
void primary_expression()
{
	int t, r,addr;
    Type type;
    Symbol *s;
	Section *sec=NULL;
	switch(token) 
	{
    case TK_CINT: 
    case TK_CCHAR:
        operand_push(&int_type, SC_GLOBAL, tkvalue);
		get_token();
        break;
	case TK_CSTR:
		t = T_CHAR;
        type.t = t;
        mk_pointer(&type);
        type.t |= T_ARRAY;
        sec = allocate_storage(&type,SC_GLOBAL, 2, 0, &addr);
		var_sym_put(&type, SC_GLOBAL, 0, addr);
        initializer(&type, addr, sec);  
        break;
	case TK_OPENPA:
		get_token();
		expression();
		skip(TK_CLOSEPA);
		break;
	default:
		 t = token;		
		 get_token();
		 if(t < TK_IDENT)
			expect("标识符或常量");
		 s = sym_search(t);
		 if (!s) 
		 { 
			 if (token != TK_OPENPA)
				 error("'%s'未声明\n", get_tkstr(t));
			
			 s = func_sym_push(t, &default_func_type);//允许函数不声明，直接引用
			 s->r = SC_GLOBAL | SC_SYM;
		 }
		r = s->r;
		operand_push(&s->type, r, s->c);
        /* 符号引用，操作数必须记录符号地址 */		
        if (optop->r & SC_SYM) 
		{   
			optop->sym = s;      
            optop->value = 0;  //用于函数调用，及全局变量引用 printf("g_cc=%c\n",g_cc);
        }
		break;
	}
}

/***********************************************************
 * 功能:	解析实参表达式表
 *
 * <argument_expression_list >::=<assignment_expression>
 *		{<TK_COMMA> <assignment_expression>}
 **********************************************************/
void argument_expression_list()
{
	Operand ret;
	Symbol *s,*sa;
	int nb_args;
	s = optop->type.ref;
	get_token();
	sa = s->next; 
	nb_args = 0;
	ret.type = s->type;
	ret.r = REG_IRET;
	ret.value = 0;
	if (token != TK_CLOSEPA) 
	{
		for(;;) 
		{
			assignment_expression();
			nb_args++;
			if (sa)
				sa = sa->next;
			if (token == TK_CLOSEPA)
				break;
			skip(TK_COMMA);
		}
	}   
	if (sa)
		error("实参个数少于函数形参个数"); //讲一下形参，实参
	skip(TK_CLOSEPA);
	gen_invoke(nb_args);

	operand_push(&ret.type, ret.r, ret.value);
}

/***********************************************************
 * 功能:	缩进n个tab键
 * n:		缩进个数
 **********************************************************/
void print_tab(int n)
{
	int i = 0;
	for(; i < n; i++)
	    printf("\t");
}

/***********************************************************
 * 功能:	语法缩进
 * 通常情况在get_token最后调用，
 * 如果必须根据新取单词类型判断输出格式，则在取该单词前设置syntax_state = SNTX_DELAY，暂不输出，
 * 待取出新单词后，根据单词类型设置syntax_state，重新调用该函数根据syntax_state，进行适当输出
 **********************************************************/
void syntax_indent()
{
	switch(syntax_state)
	{
		case SNTX_NUL:
			color_token(LEX_NORMAL);
			break;	
		case SNTX_SP:
			printf(" ");
			color_token(LEX_NORMAL);
			break;	
		case SNTX_LF_HT:
			{	
				if(token == TK_END)		// 遇到'}',缩进减少一级
				syntax_level--;
				printf("\n");
				print_tab(syntax_level);				
			}
			color_token(LEX_NORMAL);
			break;		
		case SNTX_DELAY:
			break;
	}	
	syntax_state = SNTX_NUL;
}