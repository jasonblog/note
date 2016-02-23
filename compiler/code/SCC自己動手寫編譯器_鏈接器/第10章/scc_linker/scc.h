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

/*******************************outcoff.h begin****************************/
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;

#pragma pack(push, 1)
/* 节结构定义 */
typedef struct Section 
{ 
	int data_offset;			// 当前数据偏移位置
    char *data;					// 节数据
    int data_allocated;			// 分配内存空间
	char  index;				// 节序号
	struct Section *link;       // 关联的其它节
	int *hashtab;				// 哈希表，只用于存储符号表
	IMAGE_SECTION_HEADER sh;    // 节头
} Section;

/* 符号表记录 
原定义
// Symbol format.
//

typedef struct _IMAGE_SYMBOL {
    union {
        BYTE    ShortName[8];
        struct {
            DWORD   Short;     // if 0, use LongName
            DWORD   Long;      // offset into string table
        } Name;
        PBYTE   LongName[2];
    } N;
    DWORD   Value;
    SHORT   SectionNumber;
    WORD    Type;
    BYTE    StorageClass;
    BYTE    NumberOfAuxSymbols;
} IMAGE_SYMBOL;
*/
/* 修正内容：Name 符号名称原来为8个字节长数组*/
/* COFF符号结构定义 */
typedef struct CoffSym 
{

    DWORD Name;					// 符号名称
	DWORD Next;					// 用于保存冲突链表*/
    /* 
    struct {
		DWORD   Short;			// if 0, use LongName
        DWORD   Long;			// offset into string table
    } name;
	*/
    DWORD   Value;				// 与符号相关的值
    short   Section;			// 节表的索引(从1开始),用以标识定义此符号的节*/
    WORD    Type;				// 一个表示类型的数字
    BYTE    StorageClass;		// 这是一个表示存储类别的枚举类型值
    BYTE    NumberOfAuxSymbols;	// 跟在本记录后面的辅助符号表项的个数
} CoffSym;

#define CST_FUNC    0x20  //Coff符号类型，函数
#define CST_NOTFUNC 0     //Coff符号类型，非函数

/* COFF重定位记录 
原定义:
//
// Relocation format.
//

typedef struct _IMAGE_RELOCATION {
    union {
        DWORD   VirtualAddress;
        DWORD   RelocCount;             // Set to the real count when IMAGE_SCN_LNK_NRELOC_OVFL is set
    };
    DWORD   SymbolTableIndex;
    WORD    Type;
} IMAGE_RELOCATION;
*/
/* 重定位结构定义 */
typedef struct CoffReloc 
{
    DWORD offset;	// 需要进行重定位的代码或数据的地址
    DWORD cfsym;				// 符号表的索引(从0开始)
	BYTE  section;  // 此处讲一下为什么对COFF重定位结构进行修改记录Section信息*/
    BYTE  type;    
} CoffReloc;

#pragma pack(pop)

extern Section *sec_text, *sec_data, *sec_bss, *sec_idata,*sec_rdata,*sec_rel,*sec_symtab,*sec_dynsymtab;
extern DynArray sections;
extern int nsec_image;
void init_coff();
void section_realloc(Section *sec, int new_size);
void *section_ptr_add(Section *sec, int size);
void coffsym_add_update(Symbol *s, int val, int sec_index,short type, char StorageClass);
void coffreloc_add(Section *sec, Symbol *sym, int offset, char type);
void fpad(FILE *fp, int new_pos);
void free_sections();
void coffreloc_direct_add(int offset, int cfsym,char section, char type);
int coffsym_add(Section *symtab,char* name, int val, int sec_index,
							short type,  char StorageClass);
int coffsym_search(Section *symtab,char *name);
/*******************************outcoff.h end****************************/

/*******************************oprand.h begin****************************/
/* 操作数存储结构，存放在语义栈中 */
typedef struct Operand
{
    Type type;			    // 数据类型
    unsigned short r;       // 寄存器或存储类型
    int value;              // 常量值，适用于SC_GLOBAL
    struct Symbol *sym;     // 符号，适用于(SC_SYM | SC_GLOBAL)
} Operand;

void operand_push(Type *type, int r, int value);
void operand_pop();
void operand_swap();
void operand_assign(Operand *opd, int t, int r, int v);
void cancel_lvalue();
void indirection();
/*******************************oprand.h end****************************/

/***********************gencodes.h begin*********************/
/* 寄存器编码 */
enum e_Register
{
    REG_EAX = 0,
    REG_ECX,
    REG_EDX,
	REG_EBX,
	REG_ESP,
	REG_EBP,
	REG_ESI,
	REG_EDI,
	REG_ANY
};
#define REG_IRET  REG_EAX	// 存放函数返回值的寄存器

/* 寻址方式 */
enum e_AddrForm
{
	ADDR_OTHER,				// 寄存器间接寻址 [EAX],[EBX]
	ADDR_REG = 3			// 寄存器直接寻址，EAX,EBX等相当于mod=11000000(C0)
};

#define OPSTACK_SIZE 256
extern Operand opstack[OPSTACK_SIZE],*optop; 
#define FUNC_PROLOG_SIZE 9
extern int rsym, ind, loc;
extern int func_begin_ind;
extern int func_ret_sub;

void gen_byte(char c);
void gen_opcode1(char opcode);
void gen_opcode2(char first, char second);
void gen_long(unsigned int c);
void backpatch(int t, int a);
void gen_addr32(int r, Symbol *sym, int c);
void gen_modrm(int mod,int reg_opcode, int r_m, Symbol *sym, int c);

void spill_reg(int r);
void spill_regs();
int gen_jmpforward(int t);
void gen_jmpbackword(int a);

void gen_prefix(char opcode);
int allocate_reg(int rc);
void gen_invoke(int nb_args);
void gen_op(int op);
int load_1(int rc, Operand *opd);
void store0_1();
int gen_jcc(int t);
void gen_prolog(Type *func_type);
void gen_epilog();
Type *pointed_type(Type *t);
void init_variable(Type *type, Section *sec, int c, int v);
Section * allocate_storage(Type *type,int r, int has_init, int v, int *addr);
/***********************gencodes.h end*********************/

/*******************************outpe.h begin****************************/
/* 导入符号内存存储结构 */
struct ImportSym 
{
    int iat_index;
    int thk_offset;
    IMAGE_IMPORT_BY_NAME imp_sym;
};

/* 导入模块内存存储结构 */
struct ImportInfo 
{
	int dll_index;
	DynArray imp_syms;
	IMAGE_IMPORT_DESCRIPTOR imphdr;
};

/* PE信息存储结构 */
struct PEInfo 
{
    Section *thunk;
    const char *filename;
    DWORD entry_addr;
    DWORD imp_offs;
    DWORD imp_size;
    DWORD iat_offs;
    DWORD iat_size;
	Section **secs;
    int   sec_size;
	DynArray imps;
};

extern DynArray array_dll;
extern DynArray array_lib;
extern char *lib_path;
extern short subsystem;
int pe_output_file(char *filename);
int inproc_run(int argc, char **argv);
void write_obj(char *name);
int load_obj_file(char *fname);
void relocate_syms();
extern char *entry_symbol;
int pe_find_import(char *symbol);
void add_runtime_libs();
void coffrelocs_fixup();
char *get_lib_path();
/*******************************outpe.h end****************************/