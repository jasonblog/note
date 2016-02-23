// 《自己动手写编译器、链接器》配套源代码

#include "scc.h"
char *entry_symbol = "_entry";
DynArray array_dll;
DynArray array_lib;
char *lib_path;
short subsystem;

/* -----------------------------------------------------------
 * 声明PE头变量，并进行初始化
 ----------------------------------------------------------- */
IMAGE_DOS_HEADER dos_header = {
    /* IMAGE_DOS_HEADER doshdr */
    0x5A4D, /*WORD e_magic;         DOS可执行文件标记,为'MZ'  */
    0x0090, /*WORD e_cblp;          Bytes on last page of file */
    0x0003, /*WORD e_cp;            Pages in file */
    0x0000, /*WORD e_crlc;          Relocations */

    0x0004, /*WORD e_cparhdr;       Size of header in paragraphs */
    0x0000, /*WORD e_minalloc;      Minimum extra paragraphs needed */
    0xFFFF, /*WORD e_maxalloc;      Maximum extra paragraphs needed */
    0x0000, /*WORD e_ss;            DOS代码的初始化堆栈段 */

    0x00B8, /*WORD e_sp;            DOS代码的初始化堆栈指针 */
    0x0000, /*WORD e_csum;          Checksum */
    0x0000, /*WORD e_ip;            DOS代码的入口IP */
    0x0000, /*WORD e_cs;            DOS代码的入口CS */
    0x0040, /*WORD e_lfarlc;        File address of relocation table */
    0x0000, /*WORD e_ovno;          Overlay number */
    {0,0,0,0}, /*WORD e_res[4];     Reserved words */
    0x0000, /*WORD e_oemid;         OEM identifier (for e_oeminfo) */
    0x0000, /*WORD e_oeminfo;       OEM information; e_oemid specific */
    {0,0,0,0,0,0,0,0,0,0}, /*WORD e_res2[10];      Reserved words */
    0x00000080  /*DWORD   e_lfanew;        指向PE文件头 */
};

BYTE dos_stub[0x40] = {
    /* BYTE dosstub[0x40] */
    /* 14 code bytes + "This program cannot be run in DOS mode.\r\r\n$" + 6 * 0x00 */
    0x0e,0x1f,0xba,0x0e,0x00,0xb4,0x09,0xcd,0x21,0xb8,0x01,0x4c,0xcd,0x21,0x54,0x68,
    0x69,0x73,0x20,0x70,0x72,0x6f,0x67,0x72,0x61,0x6d,0x20,0x63,0x61,0x6e,0x6e,0x6f,
    0x74,0x20,0x62,0x65,0x20,0x72,0x75,0x6e,0x20,0x69,0x6e,0x20,0x44,0x4f,0x53,0x20,
    0x6d,0x6f,0x64,0x65,0x2e,0x0d,0x0d,0x0a,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	};

IMAGE_NT_HEADERS32 nt_header = {
    0x00004550, /* DWORD Signature = IMAGE_NT_SIGNATURE PE文件标识*/
	{
		/* IMAGE_FILE_HEADER FileHeader */
		0x014C,		/*WORD    Machine; 运行平台*/
		0x0003,		/*WORD    NumberOfSections; 文件的节数目 */
		0x00000000, /*DWORD   TimeDateStamp; 文件的创建日期和时间*/
		0x00000000, /*DWORD   PointerToSymbolTable; 指向符号表(用于调试)*/
		0x00000000, /*DWORD   NumberOfSymbols; 符号表中的符号数量（用于调试）*/
		0x00E0,		/*WORD    SizeOfOptionalHeader; IMAGE_OPTIONAL_HEADER32结构的长度*/
		0x030F		/*WORD    Characteristics; 文件属性*/
	},
	{
	    /* IMAGE_OPTIONAL_HEADER OptionalHeader */
		/* 标准域. */
		0x010B, /*WORD    Magic; */
		0x06, /*BYTE    MajorLinkerVersion; 链接器主版本号*/
		0x00, /*BYTE    MinorLinkerVersion; 链接器次版本号*/
		0x00000000, /*DWORD   SizeOfCode; 所有含代码段的总大小*/
		0x00000000, /*DWORD   SizeOfInitializedData; 所有已初始化数据段的总大小*/
		0x00000000, /*DWORD   SizeOfUninitializedData; 所有含有未初始化数据段的大小*/
		0x00000000, /*DWORD   AddressOfEntryPoint; 程序执行入口的相对虚拟地址*/
		0x00000000, /*DWORD   BaseOfCode; 代码段的起始RVA*/
		0x00000000, /*DWORD   BaseOfData; 代码段的起始RVA*/
		
		/* NT附加域 */
		0x00400000, /*DWORD   ImageBase; 程序的建议装载地址 讲一下Windows内存结构，如果大于0x80000000会有什么后果*/
		0x00001000, /*DWORD   SectionAlignment; 内存中段的对齐粒度*/
		0x00000200, /*DWORD   FileAlignment; 文件中段的对齐粒度*/
		0x0004, /*WORD    MajorOperatingSystemVersion; 操作系统的主版本号*/
		0x0000, /*WORD    MinorOperatingSystemVersion; 操作系统的次版本号*/
		0x0000, /*WORD    MajorImageVersion; 程序的主版本号*/
		0x0000, /*WORD    MinorImageVersion; 程序的次版本号*/
		0x0004, /*WORD    MajorSubsystemVersion; 子系统的主版本号*/
		0x0000, /*WORD    MinorSubsystemVersion; 子系统的次版本号*/
		0x00000000, /*DWORD   Win32VersionValue; 保留，设为0*/
		0x00000000, /*DWORD   SizeOfImage; 内存中整个PE映像尺寸*/
		0x00000200, /*DWORD   SizeOfHeaders; 所有头+节表的大小*/
		0x00000000, /*DWORD   CheckSum; 校验和*/
		0x0003, /*WORD    Subsystem; 文件的子系统*/
		0x0000, /*WORD    DllCharacteristics; */
		0x00100000, /*DWORD   SizeOfStackReserve; 初始化时堆栈大小*/
		0x00001000, /*DWORD   SizeOfStackCommit; 初始化时实际提交的堆栈大小*/
		0x00100000, /*DWORD   SizeOfHeapReserve; 初始化时保留的堆大小*/
		0x00001000, /*DWORD   SizeOfHeapCommit; 初始化时实际提交的堆大小*/
		0x00000000, /*DWORD   LoaderFlags; 保留，设为0*/
		0x00000010, /*DWORD   NumberOfRvaAndSizes; 下面的数据目录结构的数量*/
				
				/* IMAGE_DATA_DIRECTORY DataDirectory[16]; 数据目录*/
		{{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
		{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}}
	}};


/* ------------------------------------------------------------- */

/***********************************************************
 * 功能:	计算文件对齐位置
 * n:		未对齐前位置
 **********************************************************/	
DWORD pe_file_align(DWORD n)
{
	DWORD FileAlignment = nt_header.OptionalHeader.FileAlignment; //文件中段的对齐粒度
	return calc_align(n,FileAlignment);
}

/***********************************************************
 * 功能:	计算内存对齐位置
 * n:		未对齐前位置
 **********************************************************/	
DWORD pe_virtual_align(DWORD n)
{ 
	DWORD SectionAlignment = nt_header.OptionalHeader.SectionAlignment; //内存中段的对齐粒度
	return calc_align(n,SectionAlignment);
} 

/***********************************************************
 * 功能:	设置数据目录
 * dir:		目录类型
 * addr:	表的RVA
 * size:	表的大小(以字节计)
 **********************************************************/	
void pe_set_datadir(int dir, DWORD addr, DWORD size)
{
    nt_header.OptionalHeader.DataDirectory[dir].VirtualAddress = addr;
    nt_header.OptionalHeader.DataDirectory[dir].Size = size;
}

/***********************************************************
 * 功能:	加载目标文件
 * fname:	目标文件名
 **********************************************************/	
int load_obj_file(char *fname)
{
	IMAGE_FILE_HEADER fh;
	Section **secs;
	int i,sh_size,nsec_obj=0,nsym;
	FILE *fobj;
	CoffSym *cfsyms;
	CoffSym *cfsym; 
	char *strs, *csname;
	void *ptr;
	int cur_text_offset;
	int cur_rel_offset;
	int *old_to_new_syms;
	int cfsym_index;
	CoffReloc *rel, *rel_end;

    sh_size = sizeof(IMAGE_SECTION_HEADER); 
	fobj = fopen(fname,"rb");	
	fread(&fh,1,sizeof(IMAGE_FILE_HEADER),fobj);
	nsec_obj = fh.NumberOfSections;
	secs = (Section**)sections.data;
	for(i = 0; i < nsec_obj; i++)
	{
		fread(secs[i]->sh.Name,1,sh_size,fobj);
	}

    cur_text_offset = sec_text->data_offset;
    cur_rel_offset = sec_rel->data_offset;
	for(i = 0; i < nsec_obj; i++)
	{
		if(!strcmp(secs[i]->sh.Name,".symtab"))
		{
			cfsyms = mallocz(secs[i]->sh.SizeOfRawData);
			fseek(fobj,SEEK_SET,secs[i]->sh.PointerToRawData);
			fread(cfsyms,1,secs[i]->sh.SizeOfRawData,fobj);
			nsym = secs[i]->sh.SizeOfRawData / sizeof(CoffSym);
			continue;
		}
		if(!strcmp(secs[i]->sh.Name,".strtab"))
		{
			strs = mallocz(secs[i]->sh.SizeOfRawData);
			fseek(fobj,SEEK_SET,secs[i]->sh.PointerToRawData);
			fread(strs,1,secs[i]->sh.SizeOfRawData,fobj);
			continue;
		}
		if(!strcmp(secs[i]->sh.Name,".dynsym") || !strcmp(secs[i]->sh.Name,".dynstr"))
			continue;
		fseek(fobj,SEEK_SET,secs[i]->sh.PointerToRawData);
		ptr = section_ptr_add(secs[i], secs[i]->sh.SizeOfRawData);        
		fread(ptr,1,secs[i]->sh.SizeOfRawData,fobj);		
	}
	old_to_new_syms = mallocz(sizeof(int)*nsym);
	for(i = 1; i < nsym; i++)
	{
		cfsym = &cfsyms[i];
		csname = strs + cfsym->Name;
		cfsym_index = coffsym_add(sec_symtab,csname,cfsym->Value,cfsym->Section,cfsym->Type,cfsym->StorageClass);
		old_to_new_syms[i] = cfsym_index;
	}

	rel = (CoffReloc *)(sec_rel->data+cur_rel_offset);
    rel_end = (CoffReloc *)(sec_rel->data + sec_rel->data_offset);
	for(; rel<rel_end; rel++)
	{
		cfsym_index = rel->cfsym;
		rel->cfsym = old_to_new_syms[cfsym_index];
		rel->offset += cur_text_offset;
	}

	free(cfsyms);
	free(strs);
	free(old_to_new_syms);
	fclose(fobj);
	return 1;
}

/***********************************************************
 * 功能:	从静态库文件中读取一行,并处理掉无效字符
 * line:	数据存储位置
 * size:	读取的最大字符数
 * fp:		已打开的静态库文件指针
 **********************************************************/	
char *get_line(char *line, int size, FILE *fp)
{
	char *p,*p1;
    if (NULL == fgets(line, size, fp))
        return NULL;

	//去掉左空格
	p = line;
	while (*p && isspace(*p))
        p++;
    
	//去掉右空格及回车符
	p1 = strchr(p,'\0');
	while(p1 > p && p1[-1] <= ' ' ) 
		p1--;
	*p1 = '\0';

    return p;
}

/***********************************************************
 * 功能:	由libfile得到相应dll名
 * libfile: 静态库文件名
 **********************************************************/
char *get_dllname(char *libfile)
{
	int n1,n2;
	char *libname,*dllname,*p;
	n1 = strlen(libfile);
	libname = libfile;
	for(p = libfile + n1; n1 >0; p--)
	{
		if(*p == '\\')
		{
			libname = p + 1;
			break;
		}
	}

	n2 = strlen(libname);
	dllname = mallocz(sizeof(char) * n2);
	memcpy(dllname,libname,n2-4);
	memcpy(dllname+n2-4,"dll",3);
	return dllname;
}

/***********************************************************
 * 功能:	加载静态库
 * name:    静态库文件名,不包括路径和后缀
 **********************************************************/
int pe_load_lib_file(char *name)
{
	char libfile[MAX_PATH];	
    int ret = -1;
    char line[400], *dllname,*p;
	FILE *fp;
	sprintf(libfile,"%s%s.slib",lib_path,name);
    fp = fopen(libfile, "rb");
	if(fp)
	{
		dllname = get_dllname(libfile);
		dynarray_add(&array_dll, dllname);
		for (;;) 
		{		
			p = get_line(line, sizeof line, fp);
			if (NULL == p)
				break;
			if (0 == *p || ';' == *p)
				continue;  
			coffsym_add(sec_dynsymtab, p, array_dll.count, sec_text->index, CST_FUNC,IMAGE_SYM_CLASS_EXTERNAL); //此处有问题，i>180时程序没有响应了
		}
		ret = 0; 
		if (fp)
			fclose(fp);
	}
	else
	{
		link_error("\"%s\" 文件打开失败",libfile);
	}        
    return ret;
}

/***********************************************************
 * 功能:	计算程序入口点
 * pe:		PE信息存储结构指针
 **********************************************************/
void get_entry_addr(struct PEInfo *pe)
{   
	unsigned long addr = 0;
	int cs;
	CoffSym *cfsym_entry;
	cs = coffsym_search(sec_symtab,entry_symbol);  
	cfsym_entry = (CoffSym*)sec_symtab->data+cs;
	addr = cfsym_entry->Value;
	pe->entry_addr = addr;
}

/*********************************************************** 
 * 功能:	得到放置静态库的目录
 **********************************************************/
char *get_lib_path()
{
    /* 我们假定编译需要链接的静态库放在与编译器同级目录的lib文件夹下
	   此处需要讲一下静态库
	*/
    char path[MAX_PATH];
	char *p;
    GetModuleFileNameA(NULL, path, sizeof(path));
	p = strrchr(path,'\\');
	*p = '\0';
	strcat(path,"\\lib\\");
    return strdup(path);
}

/***********************************************************
 * 功能:	加载需要链接的静态库
 **********************************************************/
void add_runtime_libs()
{ 
	int i;
	
    int pe_type = 0;

	for(i = 0; i < array_lib.count; i++)
	{
		pe_load_lib_file(array_lib.data[i]);
	}
}

/***********************************************************
 * 功能:	查找导入函数
 * symbol:  符号名
 **********************************************************/
int pe_find_import(char *symbol)
{
    int sym_index;
    sym_index = coffsym_search(sec_dynsymtab, symbol);
    return sym_index;
}

/***********************************************************
 * 功能:		增加导入函数
 * pe:			PE信息存储结构指针
 * sym_index:	符号表的索引
 * name:		符号名
 **********************************************************/
struct ImportSym *pe_add_import(struct PEInfo *pe, int sym_index, char *name)
{
    int i;
    int dll_index;
    struct ImportInfo *p;
    struct ImportSym *s;

    dll_index = ((CoffSym *)sec_dynsymtab->data + sym_index)->Value;
    if (0 == dll_index)
        return NULL;

    i = dynarray_search (&pe->imps, dll_index);
    if (-1 != i) {
        p = pe->imps.data[i];
    }
	else
	{
		p = mallocz(sizeof *p);
		dynarray_init(&p->imp_syms,8);
		p->dll_index = dll_index;
		dynarray_add(&pe->imps, p);
	}

    i = dynarray_search(&p->imp_syms,sym_index);
    if (-1 != i)//找到直接返回，找不到则填加
	{
        return (struct ImportSym*)p->imp_syms.data[i];
	}
	else
	{		
		s = mallocz(sizeof(struct ImportSym) + strlen(name));
		dynarray_add(&p->imp_syms, s);
		strcpy((char*)&s->imp_sym.Name,name);
		return s;
	}
}

/***********************************************************
 * 功能:	解析程序中用到的外部符号
 * pe:		PE信息存储结构指针
 **********************************************************/
int resolve_coffsyms(struct PEInfo *pe)
{
    CoffSym *sym;
    int sym_index, sym_end;
    int ret = 0;
	int found = 1;
	
    sym_end = sec_symtab->data_offset / sizeof(CoffSym);
    for (sym_index = 1; sym_index < sym_end; sym_index++) 
	{
        sym = (CoffSym*)sec_symtab->data + sym_index;
		
        if (sym->Section == IMAGE_SYM_UNDEFINED) 
		{
            char *name = sec_symtab->link->data + sym->Name;
		
            unsigned type = sym->Type;
            int imp_sym = pe_find_import(name);
            struct ImportSym *is;
            if (0 == imp_sym)
                found = 0;
            is = pe_add_import(pe, imp_sym,name);
            if (!is)
                found = 0;
			
            if (found && type == CST_FUNC)
			{
                int offset = is->thk_offset;
                
				char buffer[100];
				
				offset = sec_text->data_offset;
				/*FF /4		JMP r/m32	Jump near,absolute indirect,address given in r/m32*/
				*(WORD*)section_ptr_add(sec_text, 6) = 0x25FF;
				

				sprintf(buffer, "IAT.%s", name);
				is->iat_index = coffsym_add(sec_symtab,buffer,0,sec_idata->index,CST_FUNC,IMAGE_SYM_CLASS_EXTERNAL);
				
				coffreloc_direct_add(offset + 2, is->iat_index, sec_text->index, IMAGE_REL_I386_DIR32);
				is->thk_offset = offset;
				
				sym = (CoffSym*)sec_symtab->data + sym_index;
                sym->Value = offset;
                sym->Section = sec_text->index;
            }
			else
			{
				link_error("'%s'未定义", name);
				ret = 1;
			}
        }
    }
    return ret;
}

/***********************************************************
 * 功能:	向导入符号表中写入符号字符串(dll名称或导入符号)
 * sec:		导入符号所在节
 * sym:		符号名称
 **********************************************************/
int put_import_str(Section *sec, char *sym)
{
    int offset, len;
    char *ptr;
    len = strlen(sym) + 1;
    offset = sec->data_offset;
    ptr = section_ptr_add(sec, len);
    memcpy(ptr, sym, len);
    return offset;
}

/***********************************************************
 * 功能:	创建导入信息（导入目录表及导入符号表）
 * pe:		pe:		PE信息存储结构指针
 **********************************************************/
void pe_build_imports(struct PEInfo *pe)
{
    int thk_ptr, ent_ptr, dll_ptr, sym_cnt, i;
    DWORD rva_base = pe->thunk->sh.VirtualAddress - nt_header.OptionalHeader.ImageBase;
    int ndlls = pe->imps.count;

    for (sym_cnt = i = 0; i < ndlls; ++i)
        sym_cnt += ((struct ImportInfo*)pe->imps.data[i])->imp_syms.count;

    if (0 == sym_cnt)
        return;    

    pe->imp_offs = dll_ptr = pe->thunk->data_offset;
    pe->imp_size = (ndlls + 1) * sizeof(IMAGE_IMPORT_DESCRIPTOR);
    pe->iat_offs = dll_ptr + pe->imp_size;
    pe->iat_size = (sym_cnt + ndlls) * sizeof(DWORD);
    section_ptr_add(pe->thunk, pe->imp_size + 2*pe->iat_size);//预留出dll导出表及iat表空间

    thk_ptr = pe->iat_offs;
    ent_ptr = pe->iat_offs + pe->iat_size;

    for (i = 0; i < pe->imps.count; ++i) 
	{
        int k, n, v;
        struct ImportInfo *p = pe->imps.data[i];
		char *name = array_dll.data[p->dll_index-1];   

        /* 写入dll名称 */
        v = put_import_str(pe->thunk, name);
        
		p->imphdr.FirstThunk  = thk_ptr + rva_base;
        p->imphdr.OriginalFirstThunk = ent_ptr + rva_base;
        p->imphdr.Name = v + rva_base;
		memcpy(pe->thunk->data + dll_ptr,&p->imphdr,sizeof(IMAGE_IMPORT_DESCRIPTOR)); //增加导入表条目

        for (k = 0, n = p->imp_syms.count; k <= n; ++k) 
		{
            if (k < n) 
			{
				struct ImportSym* is = (struct ImportSym*)p->imp_syms.data[k];
                DWORD iat_index = is->iat_index;
                CoffSym *org_sym = (CoffSym *)sec_symtab->data + iat_index;

                org_sym->Value = thk_ptr;
                org_sym->Section = pe->thunk->index;
                v = pe->thunk->data_offset + rva_base;
				
                section_ptr_add(pe->thunk, sizeof(is->imp_sym.Hint));
                put_import_str(pe->thunk, is->imp_sym.Name);

            } 
			else 
			{
                v = 0; 
            }
            *(DWORD*)(pe->thunk->data+thk_ptr) =
            *(DWORD*)(pe->thunk->data+ent_ptr) = v;
            thk_ptr += sizeof (DWORD);
            ent_ptr += sizeof (DWORD);
        }
        dll_ptr += sizeof(IMAGE_IMPORT_DESCRIPTOR);
        dynarray_free(&p->imp_syms);
    }
    dynarray_free(&pe->imps);
}

/***********************************************************
 * 功能:	计算节区的RVA地址
 * pe:		PE信息存储结构指针
 **********************************************************/
int pe_assign_addresses(struct PEInfo *pe)
{
    int i;
    DWORD addr;
    Section *sec,**ps;

    pe->thunk = sec_idata;

	pe->secs = (Section**)mallocz(nsec_image * sizeof (Section*));
    addr = nt_header.OptionalHeader.ImageBase + 1;

    for (i = 0; i < nsec_image; ++i)
    {
        sec = (Section*)sections.data[i];
        ps = &pe->secs[pe->sec_size];
		*ps = sec;

		sec->sh.VirtualAddress = addr = pe_virtual_align(addr);

        if (sec == pe->thunk) 
		{
            pe_build_imports(pe);
        }

        if (sec->data_offset)
        {            
            addr += sec->data_offset;
            ++pe->sec_size;
        }
    }
    return 0;
}

/***********************************************************
 * 功能:	重定位符号地址
 **********************************************************/
void relocate_syms()
{
    CoffSym *sym, *sym_end;
	Section *sec;

    sym_end = (CoffSym *)(sec_symtab->data + sec_symtab->data_offset);
    for(sym = (CoffSym *)sec_symtab->data + 1; sym < sym_end; sym++) 
	{
		sec = (Section*)sections.data[sym->Section-1];        
		sym->Value += sec->sh.VirtualAddress; // 加上节区的RVA地址
    }
}

/*直接重定位--全局变量 间接重定位--函数调用*/
/*wndclass.lpfnWndProc   = WndProc ;属于直接重定位*/
/***********************************************************
 * 功能:	修正需要进行重定位的代码或数据的地址
 **********************************************************/
void coffrelocs_fixup()
{
    Section *sec,*sr;
    CoffReloc *rel, *rel_end, *qrel;
    CoffSym *sym;
    int type, sym_index;
    char *ptr;
    unsigned long val, addr;
	char *name;

	
    sr = sec_rel;
    rel_end = (CoffReloc *)(sr->data + sr->data_offset);
    qrel = (CoffReloc *)sr->data;
    for(rel = qrel; rel < rel_end; rel++) 
	{
		sec = (Section*)sections.data[rel->section - 1];
	    
        sym_index = rel->cfsym;
        sym = &((CoffSym *)sec_symtab->data)[sym_index];
		name = sec_symtab->link->data + sym->Name;
        val = sym->Value;
        type = rel->type;
        addr = sec->sh.VirtualAddress + rel->offset;	
		
		ptr = sec->data + rel->offset;				
        switch(type) 
		{
        case IMAGE_REL_I386_DIR32:  //全局变量
            *(int *)ptr += val;
            break;
        case IMAGE_REL_I386_REL32:  //函数调用
            *(int *)ptr += val - addr;
            break;
        }
    }
}

/***********************************************************
 * 功能:	写PE文件
 * pe:		PE信息存储结构指针
 **********************************************************/
int pe_write(struct PEInfo *pe)
{
    int i;
    FILE *op;
    DWORD file_offset, r;
	int sizeofheaders;

    op = fopen(pe->filename, "wb");
    if (NULL == op) 
	{
        link_error("'%s'生成失败", pe->filename);
        return 1;
    }

    sizeofheaders = pe_file_align(
          sizeof(dos_header)
		+ sizeof(dos_stub)
		+ sizeof(nt_header)
        + pe->sec_size * sizeof (IMAGE_SECTION_HEADER)
        );

    file_offset = sizeofheaders;
    fpad(op, file_offset);


    for (i = 0; i < pe->sec_size; ++i)
	{
		Section *sec = pe->secs[i];
        char *sh_name = sec->sh.Name; 
        unsigned long addr = sec->sh.VirtualAddress - nt_header.OptionalHeader.ImageBase; 
        unsigned long size = sec->data_offset; 
        IMAGE_SECTION_HEADER *psh = &sec->sh;


        if(!strcmp(sec->sh.Name,".text"))
		{
           nt_header.OptionalHeader.BaseOfCode = addr;
           nt_header.OptionalHeader.AddressOfEntryPoint = addr + pe->entry_addr;  
		}
		else if(!strcmp(sec->sh.Name,".data"))
           nt_header.OptionalHeader.BaseOfData = addr;
		else if(!strcmp(sec->sh.Name,".idata"))
		{
			if (pe->imp_size)
			{
                pe_set_datadir(IMAGE_DIRECTORY_ENTRY_IMPORT,
                    pe->imp_offs + addr, pe->imp_size);
                pe_set_datadir(IMAGE_DIRECTORY_ENTRY_IAT,
                    pe->iat_offs + addr, pe->iat_size);
            }  
		}

        strcpy((char*)psh->Name, sh_name);

        psh->VirtualAddress = addr;
        psh->Misc.VirtualSize = size;
		nt_header.OptionalHeader.SizeOfImage = pe_virtual_align(
			size + addr
			); 

        if (sec->data_offset) 
		{			
            psh->PointerToRawData = r = file_offset;
			if(!strcmp(sec->sh.Name,".bss")) 
			{
				sec->sh.SizeOfRawData = 0;
				continue;
			}
            fwrite(sec->data, 1, sec->data_offset, op);
            file_offset = pe_file_align(file_offset + sec->data_offset);
            psh->SizeOfRawData = file_offset - r;
            fpad(op, file_offset);
        }
    }

    nt_header.FileHeader.NumberOfSections = pe->sec_size;
    nt_header.OptionalHeader.SizeOfHeaders = sizeofheaders;

    nt_header.OptionalHeader.Subsystem = subsystem;

    fseek(op, SEEK_SET, 0);
    fwrite(&dos_header, 1, sizeof dos_header, op);
	fwrite(&dos_stub,   1, sizeof dos_stub,   op);
	fwrite(&nt_header,  1, sizeof nt_header,  op);
    for (i = 0; i < pe->sec_size; ++i)
        fwrite(&pe->secs[i]->sh, 1, sizeof(IMAGE_SECTION_HEADER), op);
    fclose (op);
    return 0;
}

/***********************************************************
 * 功能:		输出PE文件
 * filename:	EXE文件名
 **********************************************************/
int pe_output_file(char *filename)
{
    int ret;
    struct PEInfo pe;

    memset(&pe, 0, sizeof(pe));
	dynarray_init(&pe.imps,8);
    pe.filename = filename;
	/* 加载需要链接的静态库 */
    add_runtime_libs();
	/* 计算程序入口点 */
	get_entry_addr(&pe);
	/* 解析程序中用到的外部符号 */
    ret = resolve_coffsyms(&pe);
    if (0 == ret) 
	{
	
		/* 计算节区的RVA地址 */
		pe_assign_addresses(&pe);
		/* 重定位符号地址 */
		relocate_syms();
		/* 修正需要进行重定位的代码或数据的地址 */
		coffrelocs_fixup();
        /* 写PE文件 */
        ret = pe_write(&pe);
        free(pe.secs);
    }

    return ret;
}

