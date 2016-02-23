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
int  is_nodigit(char c);
int  is_digit(char c);
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


/*******************************grammar.h begin****************************/
/* 语法状态 */
enum e_SynTaxState
{
	SNTX_NUL,       // 空状态，没有语法缩进动作
	SNTX_SP,		// 空格 int a; int __stdcall MessageBoxA(); return 1;
	SNTX_LF_HT,		// 换行并缩进，每一个声明、函数定义、语句结束都要置为此状态
	SNTX_DELAY      // 延迟取出下一单词后确定输出格式，取出下一个单词后，根据单词类型单独调用syntax_indent确定格式进行输出 
};

/* 存储类型 */
enum e_StorageClass
{
	SC_GLOBAL =   0x00f0,		// 包括：包括整型常量，字符常量、字符串常量,全局变量,函数定义          
	SC_LOCAL  =   0x00f1,		// 栈中变量
	SC_LLOCAL =   0x00f2,       // 寄存器溢出存放栈中
	SC_CMP    =   0x00f3,       // 使用标志寄存器
	SC_VALMASK=   0x00ff,       // 存储类型掩码             
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
int type_specifier();
void init_declarator_list(int l);
void initializer();
void struct_specifier();
void struct_declaration_list();
void struct_declaration();
void declarator();
void function_calling_convention ();
void struct_member_alignment();
void direct_declarator();
void direct_declarator_postfix();
void parameter_type_list();
void funcbody();
void statement();
void compound_statement();
int is_type_specifier(int v);
void statement();
void expression_statement();
void for_statement();
void break_statement();
void continue_statement();
void if_statement();
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
extern DynString tkstr;
extern FILE *fin;
extern char ch;
extern char *filename;
extern int token;
extern int tkvalue;
extern int line_num;
int elf_hash(char *name);
void *mallocz(int size);
/******************************scc.h end*************************/