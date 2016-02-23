// 《自己动手写编译器、链接器》配套源代码

#include "scc.h"

FILE *fin = NULL;				// 源文件指针
char *filename;					// 源文件名称
DynArray src_files;				// 源文件数组
char outfile[256];				// 输出文件名
int output_type;				// 输出文件类型
float scc_version = 1.00;		// SCC编译器版本号

/*********************************************************** 
 * 功能:	分配堆内存并将数据初始化为'0'
 * size:	分配内存大小
 **********************************************************/
void *mallocz(int size)
{
    void *ptr;
	ptr = malloc(size);
	if (!ptr && size)
        error("内存分配失败");
    memset(ptr, 0, size);
    return ptr;
}

/*********************************************************** 
 * 功能:	计算哈希地址
 * key:		哈希关键字
 * MAXKEY:	哈希表长度
 **********************************************************/
int elf_hash(char *key)
{
    int h = 0, g;
    while (*key) 
	{
        h = (h << 4) + *key++;
        g = h & 0xf0000000;
        if (g)
            h ^= g >> 24;
        h &= ~g;
    }
    return h % MAXKEY;
}

/*********************************************************** 
 * 功能:	计算字节对齐位置
 * n:		未对齐前值
 * align:   对齐粒度
 **********************************************************/
int calc_align(int n, int align)
{                                                     
    return ((n + align - 1) & (~(align - 1)));        
}


/***********************************************************
 * 功能:	初始化
 **********************************************************/
void init ()
{
	dynarray_init(&src_files,1);
    init_lex();

	syntax_state = SNTX_NUL;
	syntax_level = 0;

   	stack_init(&local_sym_stack,8);
	stack_init(&global_sym_stack,8);
	sym_sec_rdata = sec_sym_put(".rdata",0);

	int_type.t = T_INT;
    char_pointer_type.t = T_CHAR;
    mk_pointer(&char_pointer_type);
	default_func_type.t = T_FUNC;
	default_func_type.ref = sym_push(SC_ANOM, &int_type, KW_CDECL, 0);

	optop = opstack - 1;
    
	init_coff();	

}

/*********************************************************** 
 * 功能:	扫尾清理工作
 **********************************************************/
void cleanup()
{	
	int i;
    sym_pop(&global_sym_stack, NULL);
	stack_destroy(&local_sym_stack);
	stack_destroy(&global_sym_stack);
	free_sections();
    
	for(i = TK_IDENT; i < tktable.count; i++)
	{	
        free(tktable.data[i]);	//tktable必须从TK_IDENT形如释放，如果调用dynarray_free会有问题，因为TK_IDENT以下的token没有放在堆中，而是放在静态存储区

	}
    free(tktable.data);
}


/***********************************************************
 * 功能:	得到文件扩展名
 * fname:	文件名称
 **********************************************************/
void get_obj_fname(char *fname)
{
	char *p;
	int i;
	p = strrchr(fname,'.');
	i = p - fname + 1;
    strcpy(outfile,fname);
	strcpy(outfile+i,"obj");
}

/*********************************************************** 
 * 功能:	main主函数
 **********************************************************/
int main(int argc, char ** argv)
{ 
	char *fname;

	init();
    fname = argv[1];
	fin = fopen(fname,"rb");
	if(!fin)
		printf("cannot open SC source file");
	getch();
	line_num = 1;
	get_token();
	translation_unit();
	fclose(fin);

	get_obj_fname(fname);
	write_obj(outfile);

	printf("\n\n%s 代码行数: %d行\n\n",fname,line_num);
	cleanup();
	return 1;
}
