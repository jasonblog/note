// 《自己动手写编译器、链接器》配套源代码

#include <stdio.h>
#include <windows.h>

/*******************************dynstring.h begin****************************/
/*动态字符串定义*/
typedef struct DynString 
{
    int count;		// 字符串长度  
	int capacity;	// 包含该字符串的缓冲区长度
    char *data;		// 指向字符串的指针
} DynString;

void dynstring_realloc(DynString *cstr, int new_size);
void dynstring_chcat(DynString *cstr, int ch);
void dynstring_reset(DynString *cstr);
/*******************************dynstring.h begin****************************/

/*******************************dynarray.h begin****************************/
/*动态数组定义*/
typedef struct DynArray 
{
    int count;			// 动态数组元素个数
    int capacity;		// 动态数组缓冲区长度度
    void **data;		// 指向数据指针数组
} DynArray;

void dynarray_add(DynArray *parr, void *data);
void dynarray_init(DynArray *parr,int initsize);
void dynarray_free(DynArray *parr);                 
int dynarray_search(DynArray *parr, int key);
/*******************************dynarray.h begin****************************/

/********************************lex.h begin*****************************/
/* 单词编码 */
enum e_TokenCode
{  
	/* 运算符及分隔符 */
	TK_PLUS,		// + 加号
    TK_MINUS,		// - 减号
    TK_STAR,		// * 星号
    TK_DIVIDE,		// / 除号
    TK_MOD,			// % 求余运算符
    TK_EQ,			// == 等于号
    TK_NEQ,			// != 不等于号
    TK_LT,			// < 小于号
    TK_LEQ,			// <= 小于等于号
    TK_GT,			// > 大于号
    TK_GEQ,			// >= 大于等于号
    TK_ASSIGN,		// = 赋值运算符 
    TK_POINTSTO,	// -> 指向结构体成员运算符
    TK_DOT,			// . 结构体成员运算符
	TK_AND,         // & 地址与运算符
	TK_OPENPA,		// ( 左圆括号
	TK_CLOSEPA,		// ) 右圆括号
	TK_OPENBR,		// [ 左中括号
	TK_CLOSEBR,		// ] 右圆括号
	TK_BEGIN,		// { 左大括号
	TK_END,			// } 右大括号
    TK_SEMICOLON,	// ; 分号    
    TK_COMMA,		// , 逗号
	TK_ELLIPSIS,	// ... 省略号
	TK_EOF,			// 文件结束符

    /* 常量 */
    TK_CINT,		// 整型常量
    TK_CCHAR,		// 字符常量
    TK_CSTR,		// 字符串常量

	/* 关键字 */
	KW_CHAR,		// char关键字
	KW_SHORT,		// short关键字
	KW_INT,			// int关键字
    KW_VOID,		// void关键字 
    KW_STRUCT,		// struct关键字   
	KW_IF,			// if关键字
	KW_ELSE,		// else关键字
	KW_FOR,			// for关键字
	KW_CONTINUE,	// continue关键字
    KW_BREAK,		// break关键字   
    KW_RETURN,		// return关键字
    KW_SIZEOF,		// sizeof关键字
    
    KW_ALIGN,		// __align关键字	
    KW_CDECL,		// __cdecl关键字 standard c call
	KW_STDCALL,     // __stdcall关键字 pascal c call

	/* 标识符 */
	TK_IDENT
};

/* 词法状态 */
enum e_LexState
{
	LEX_NORMAL,
	LEX_SEP
};

/* 单词存储结构定义 */
typedef struct TkWord
{
    int  tkcode;					// 单词编码 
    struct TkWord *next;			// 指向哈希冲突的其它单词
    char *spelling;					// 单词字符串 
    struct Symbol *sym_struct;		// 指向单词所表示的结构定义
    struct Symbol *sym_identifier;	// 指向单词所表示的标识符
} TkWord;

#define MAXKEY	1024				// 哈希表容量

extern TkWord* tk_hashtable[MAXKEY];// 单词哈希表
extern DynArray tktable;			// 单词动态数组
#define CH_EOF   (-1)				// 文件尾标识


TkWord* tkword_direct_insert(TkWord* tp);
TkWord* tkword_insert(char * p);
int is_nodigit(char c);
int is_digit(char c);
void getch();
void parse_num();
void parse_string(char sep);
void parse_comment();
void get_token();
char *get_tkstr(int v);
void init_lex();
void test_lex();
void color_token(int lex_state);
/*****************************lex.h end*************************************/


/*****************************error.h begin***********************************/
/* 错误级别 */
enum e_ErrorLevel
{
	LEVEL_WARNING,
	LEVEL_ERROR,
};

/* 工作阶段 */
enum e_WorkStage
{
	STAGE_COMPILE,
	STAGE_LINK,
};
void warning(char *fmt, ...);
void error(char *fmt, ...);
void expect(char *msg);
void skip(int c);
void link_error(char *fmt, ...);
/*****************************error.h end*************************************/


/*******************************stack.h begin****************************/
/*栈定义*/
typedef struct Stack 
{
	void **base;	// 栈底指针
	void **top;		// 栈顶指针
	int stacksize;	// 栈当前可使用的最大容量,以元素为单位	
} Stack;

void stack_init(Stack *stack,int initsize);
void* stack_push(Stack *stack, void *element, int size);
void stack_pop(Stack *stack);
void* stack_get_top(Stack *stack);
int stack_is_empty(Stack *stack);
void stack_destroy(Stack *stack);
/*******************************stack.h begin****************************/

/*******************************symbol.h begin****************************/
/* 类型存储结构定义 */
typedef struct Type 
{
    int t;
    struct Symbol *ref;
} Type;

/* 符号存储结构定义 */
typedef struct Symbol 
{
    int v;						// 符号的单词编码
    int r;						// 符号关联的寄存器
    int c;						// 符号关联值
    Type type;					// 符号类型
    struct Symbol *next;		// 关联的其它符号，结构体定义关联成员变量符号，函数定义关联参数符号
    struct Symbol *prev_tok;	// 指向前一定义的同名符号
} Symbol;

extern Type char_pointer_type, int_type,default_func_type;
extern Symbol *sym_sec_rdata;
Symbol *struct_search(int v);
Symbol *sym_search(int v);
Symbol *sym_direct_push(Stack *ss, int v, Type *type, int c);
Symbol *sym_push(int v, Type *type, int r, int c);
void sym_pop(Stack *ptop, Symbol *b);
void mk_pointer(Type *type);
extern Stack global_sym_stack, local_sym_stack;
Symbol *func_sym_push(int v, Type *type);
Symbol *sec_sym_put(char *sec,int c);
Symbol *var_sym_put(Type *type, int r, int v, int addr);
void check_lvalue();
int type_size(Type *type, int *a);
/*******************************symbol.h end****************************/


/*******************************grammar.h begin****************************/
/* 语法状态 */
enum e_SynTaxState
{
	SNTX_NUL,       // 空状态，没有语法缩进动作
	SNTX_SP,		// 空格
	SNTX_LF_HT,		// 换行并缩进，每一个声明、函数定义、语句结束都要置为此状态
	SNTX_DELAY      // 延迟输出
};

/* 存储类型 */
enum e_StorageClass
{
	SC_GLOBAL =   0x00f0,		// 包括：包括整型常量，字符常量、字符串常量,全局变量,函数定义          
	SC_LOCAL  =   0x00f1,		// 栈中变量
	SC_LLOCAL =   0x00f2,       // 寄存器溢出存放栈中
	SC_CMP    =   0x00f3,       // 使用标志寄存器
	SC_VALMASK=   0x00ff,       // 存储类型掩             
	SC_LVAL   =   0x0100,       // 左值       
	SC_SYM    =   0x0200,       // 符号	

	SC_ANOM	  = 0x10000000,     // 匿名符号
	SC_STRUCT = 0x20000000,     // 结构体符号
	SC_MEMBER = 0x40000000,     // 结构成员变量
	SC_PARAMS = 0x80000000,     // 函数参数
};

/* 类型编码 */
enum e_TypeCode
{
	/* types */                                                     
	T_INT    =  0,			// 整型                     
	T_CHAR   =  1,			// 字符型                 
	T_SHORT  =  2,			// 短整型                       
	T_VOID   =  3,			// 空类型                        
	T_PTR    =  4,			// 指针                          
	T_FUNC   =  5,			// 函数                    
	T_STRUCT =  6,			// 结构体 
	
	T_BTYPE  =  0x000f,		// 基本类型掩码          
	T_ARRAY  =  0x0010,		// 数组
};

#define ALIGN_SET 0x100  // 强制对齐标志

extern int syntax_state;
extern int syntax_level;

void translation_unit();
void external_declaration(int l);
int type_specifier(Type *type);
void init_declarator_list(int l,Type *btype);
void initializer(Type *type, int c, struct Section *sec);
void struct_specifier(Type *type);
void struct_declaration_list(Type *type);
void struct_declaration(int *maxalign,int *offset,Symbol ***ps);
void declarator(Type *type, int *v,int *force_align);
void function_calling_convention (int *fc);
void struct_member_alignment(int *force_align);
void direct_declarator(Type *type,int *v,int func_call);
void direct_declarator_postfix(Type *type,int func_call);
void parameter_type_list(Type *type,int func_call);
void funcbody(Symbol *sym);
void statement(int *bsym, int *csym);
void compound_statement(int *bsym, int *csym);
int is_type_specifier(int v);
void statement(int *bsym, int *csym);
void expression_statement();
void for_statement(int *bsym, int *csym);
void break_statement(int *bsym);
void continue_statement(int *csym);
void if_statement(int *bsym, int *csym);
void return_statement();
void assignment_expression();
void expression();
void equality_expression();
void relational_expression();
void additive_expression();
void multiplicative_expression();
void unary_expression();
void argument_expression_list();
void postfix_expression();
void primary_expression();
void sizeof_expression();
void syntax_indent();
/*******************************grammar.h end****************************/


/*******************************scc.h begin****************************/
/* 输出类型 */
enum e_OutType
{	
    OUTPUT_OBJ,		// 目标文件
	OUTPUT_EXE,		// EXE可执行文件
    OUTPUT_MEMORY	// 内存中直接运行，不输出
};
                                 
extern DynString tkstr;
extern DynArray sections;
extern FILE *fin;
extern char ch;
extern char *filename;
extern int token;
extern int tkvalue;
extern int line_num;
int elf_hash(char *name);
void *mallocz(int size);
int calc_align(int n, int align);
/******************************scc.h end*************************/


