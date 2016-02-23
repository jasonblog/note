// 《自己动手写编译器、链接器》配套源代码

#include "scc.h"
DynArray sections;			// 节数组

Section *sec_text,			// 代码节
		*sec_data,			// 数据节
		*sec_bss,			// 未初始化数据节
		*sec_idata,			// 导入表节
		*sec_rdata,			// 只读数据节
		*sec_rel,			// 重定位信息节
		*sec_symtab,		// 符号表节	
		*sec_dynsymtab;		// 链接库符号节

int nsec_image;				// 映像文件节个数

/*********************************************************** 
 * 功能:		给节数据重新分配内存,并将内容初始化为0
 * sec:			重新分配内存的节
 * new_size:	节数据新长度
 **********************************************************/
void section_realloc(Section *sec, int new_size)
{
    int size;
    char *data;
    
    size = sec->data_allocated;
    while (size < new_size)
        size = size * 2;
    data = realloc(sec->data, size);
    if (!data)
        error("内存分配失败");
    memset(data + sec->data_allocated, 0, size - sec->data_allocated);/* 新分配的内存数据初始化为0 */ 
    sec->data = data;
    sec->data_allocated = size;
}

/***********************************************************
 * 功能:		给节数据预留至少increment大小的内存空间
 * sec:			预留内存空间的节
 * increment:	预留的空间大小
 * 返回值:		预留内存空间的首地址
 **********************************************************/
void *section_ptr_add(Section *sec, int increment)
{
    int offset, offset1;
    offset = sec->data_offset;
    offset1 = offset + increment;
    if (offset1 > sec->data_allocated)
        section_realloc(sec, offset1);
    sec->data_offset = offset1;
    return sec->data + offset;
}

/*********************************************************** 
 * 功能:			新建节
 * name:			节名称
 * Characteristics:	节属性
 * 返回值:			新增加节
 **********************************************************/
Section * section_new(char *name, int Characteristics)
{
	Section *sec;
	int initsize = 8;
	sec = mallocz(sizeof(Section));
	strcpy(sec->sh.Name,name);
	sec->sh.Characteristics = Characteristics;
	sec->index = sections.count + 1; //one-based index
    sec->data = mallocz(sizeof(char)*initsize);
	sec->data_allocated = initsize;
	if(!(Characteristics & IMAGE_SCN_LNK_REMOVE))
		nsec_image++;
    dynarray_add(&sections, sec);
	return sec;
}

/*********************************************************** 
 * 功能:	查找COFF符号
 * symtab:	保存COFF符号表的节
 * name:	符号名称
 * 返回值:	符号COFF符号表中序号
 **********************************************************/
int coffsym_search(Section *symtab,char *name)
{
	CoffSym *cfsym;
 	int cs,keyno;
	char *csname;
	Section *strtab;
    
	keyno = elf_hash(name);
	strtab = symtab->link;
	cs = symtab->hashtab[keyno];	
	while(cs)
	{	
		cfsym = (CoffSym*)symtab->data+cs;
		csname = strtab->data + cfsym->Name;
		/*此处如果用stricmp外部调用的函数源代码中可不区分大小写，但生成的pe中引用的函数名称大小写还是正确的
		  例如:printf误写为Printf编译运行结果依然是正确的,此处这么改了可能有些问题，由于elf_hash哈希函数对字母
		  区分大小写，如果printf及PrintF的哈希值不同，这样会有问题，如果此处要改，必须将哈希函数改为不区分大小写，
		  即在哈希函数的开头用_strupr处理一下
		*/
		if(!strcmp(name,csname)) 	
    		return cs;
		cs = cfsym->Next;
	}
	return cs;
}


/*********************************************************** 
 * 功能:	增加COFF符号名字符串
 * strtab:	保存COFF字符串表的节
 * name:	符号名称字符串
 * 返回值:	新增COFF字符串
 **********************************************************/
char *coffstr_add(Section *strtab,char* name)
{	
	int len;
	char *pstr;
	len = strlen(name);
	pstr = section_ptr_add(strtab, len+1);
	memcpy(pstr,name,len);
	return pstr;
}

/*********************************************************** 
 * 功能:			增加COFF符号
 * symtab:			保存COFF符号表的节
 * name:			符号名称
 * val:				与符号相关的值
 * sec_index:		定义此符号的节
 * type:			Coff符号类型
 * StorageClass:	Coff符号存储类别	
 * 返回值:			符号COFF符号表中序号
 **********************************************************/
int coffsym_add(Section *symtab,char* name, int val, int sec_index,
							short type, char StorageClass)
{
    CoffSym *cfsym;
	int cs,keyno;
	char *csname;
	Section *strtab = symtab->link;
	int *hashtab;
    hashtab = symtab->hashtab;
	cs = coffsym_search(symtab,name);
	if(cs == 0)
	{		
        cfsym = section_ptr_add(symtab, sizeof(CoffSym));
		csname = coffstr_add(strtab, name);
		cfsym->Name = csname - strtab->data;
		cfsym->Value =	val;
		cfsym->Section = sec_index;
		cfsym->Type = type;
		cfsym->StorageClass = StorageClass;
		cfsym->Value =	val;
		keyno = elf_hash(name);
		cfsym->Next = hashtab[keyno];

		cs = cfsym - (CoffSym*)symtab->data;	
		hashtab[keyno] = cs;
	}
	return cs;
}

/***********************************************************
 * 功能:			增加或更新COFF符号,更新只适用于函数先声明后定义的情况
 * s:				符号指针
 * val:				符号值
 * sec_index:		定义此符号的节
 * type:			Coff符号类型
 * StorageClass:	Coff符号存储类别
 **********************************************************/
void coffsym_add_update(Symbol *s, int val, int sec_index,
					short type, char StorageClass) 
{
	char *name; 
	CoffSym *cfsym;
	if (!s->c) 
	{   
		name = ((TkWord*)tktable.data[s->v])->spelling; 
		s->c = coffsym_add(sec_symtab,name,val,sec_index,type,StorageClass);
	}
	else //函数先声明后定义
	{
		cfsym = &((CoffSym *)sec_symtab->data)[s->c];
        cfsym->Value = val;
        cfsym->Section = sec_index;
	}
}

/***********************************************************
 * 功能:	释放所有节数据
 **********************************************************/
void free_sections()
{
	int i;
	Section *sec;
	for(i = 0; i < sections.count; i++)
	{
		sec = (Section*)sections.data[i];
		if(sec->hashtab != NULL)
			free(sec->hashtab);  
		free(sec->data);
	}
	dynarray_free(&sections);
}

/***********************************************************
 * 需要讲一下符号表的存储结构，内存存储结构与文件存储结构
 * 功能:			新建存储COFF符号表的节
 * symtab:			COFF符号表名
 * Characteristics: 节属性
 * strtab_name:		与符号表相关的字符串表
 * 返回值:			存储COFF符号表的节
 **********************************************************/
Section *new_coffsym_section(char *symtab_name, int Characteristics, char *strtab_name)
{
	Section *sec;
	sec = section_new(symtab_name, Characteristics);
	sec->link = section_new(strtab_name, Characteristics);
	sec->hashtab = mallocz(sizeof(int)*MAXKEY);
	return sec;
}

/*********************************************************** 
 * 功能:	增加COFF重定位信息
 * offset:	需要进行重定位的代码或数据在其相应节的偏移位置
 * cfsym:	符号表的索引
 * section: 符号所在节，重点讲一下与Coff区别
 * type:	重定位类型
 **********************************************************/
void coffreloc_direct_add(int offset, int cfsym, char section, char type)
{
	CoffReloc *rel;
	rel = section_ptr_add(sec_rel, sizeof(CoffReloc));
    rel->offset = offset;
	rel->cfsym = cfsym;
	rel->section = section;
    rel->type = type;
}

/*********************************************************** 
 * 功能:	增加重定位条目
 * section: 符号所在节
 * sym:		符号指针
 * offset:	需要进行重定位的代码或数据在其相应节的偏移位置
 * type:	重定位类型
 **********************************************************/
void coffreloc_add(Section *sec, Symbol *sym, int offset, char type)
{
	int cfsym;
	char *name;
	if (!sym->c) 
		coffsym_add_update(sym, 0,IMAGE_SYM_UNDEFINED, CST_FUNC, IMAGE_SYM_CLASS_EXTERNAL);
	name = ((TkWord*)tktable.data[sym->v])->spelling;
	cfsym = coffsym_search(sec_symtab,name);
    coffreloc_direct_add(offset, cfsym, sec->index,type);
}

/***********************************************************
 * 功能:	COFF初始化
 **********************************************************/
void init_coff()
{
	dynarray_init(&sections,8);
    nsec_image = 0;

	sec_text = section_new(".text",
				IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_CODE);
	sec_data = section_new(".data",
				IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE |
				IMAGE_SCN_CNT_INITIALIZED_DATA);
	sec_rdata = section_new(".rdata",
				IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA);
	sec_idata = section_new(".idata",
				IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | 
				IMAGE_SCN_CNT_INITIALIZED_DATA);
	sec_bss = section_new(".bss",
				IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | 
				IMAGE_SCN_CNT_UNINITIALIZED_DATA);
	sec_rel = section_new(".rel",
				IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ);


	sec_symtab = new_coffsym_section(".symtab",
				IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ,".strtab");

	sec_dynsymtab = new_coffsym_section(".dynsym",
				IMAGE_SCN_LNK_REMOVE | IMAGE_SCN_MEM_READ,".dynstr");

	coffsym_add(sec_symtab,"",0,0,0,IMAGE_SYM_CLASS_NULL);
	coffsym_add(sec_symtab,".data",0,sec_data->index,0,IMAGE_SYM_CLASS_STATIC);
	coffsym_add(sec_symtab,".bss",0,sec_bss->index,0,IMAGE_SYM_CLASS_STATIC);
	coffsym_add(sec_symtab,".rdata",0,sec_rdata->index,0,IMAGE_SYM_CLASS_STATIC);
	coffsym_add(sec_dynsymtab,"",0,0,0,IMAGE_SYM_CLASS_NULL);
}

/***********************************************************
 * 功能:	从当前读写位置到new_pos位置用0填补文件内容
 * fp:		文件指针
 * new_pos: 填补终点位置
 **********************************************************/
void fpad(FILE *fp, int new_pos)
{
    int curpos = ftell(fp);
    while (++curpos <= new_pos)
        fputc(0, fp);
}

/***********************************************************
 * 功能:	输出目标文件
 * name:	目标文件名
 **********************************************************/
void write_obj(char *name)
{
	int file_offset;
	FILE *fout = fopen(name,"wb");
	int i,sh_size,nsec_obj=0;
    IMAGE_FILE_HEADER *fh;

	nsec_obj = sections.count - 2;
    sh_size = sizeof(IMAGE_SECTION_HEADER); 
	file_offset = sizeof(IMAGE_FILE_HEADER)+nsec_obj*sh_size;
	fpad(fout,file_offset);
	fh = mallocz(sizeof(IMAGE_FILE_HEADER));
	for(i = 0; i < nsec_obj; i++)
	{
		Section *sec = (Section*)sections.data[i];
		if(sec->data == NULL) continue;
		fwrite(sec->data,1,sec->data_offset,fout);
		sec->sh.PointerToRawData = file_offset;
		sec->sh.SizeOfRawData = sec->data_offset;
		file_offset += sec->data_offset;
	}
	fseek(fout, SEEK_SET, 0);
	fh->Machine = IMAGE_FILE_MACHINE_I386;
	fh->NumberOfSections = nsec_obj;
	fh->PointerToSymbolTable = sec_symtab->sh.PointerToRawData;
	fh->NumberOfSymbols = sec_symtab->sh.SizeOfRawData/sizeof(CoffSym);
	fwrite(fh,1,sizeof(IMAGE_FILE_HEADER),fout);
	for(i = 0; i < nsec_obj; i++)
	{
		Section *sec = (Section*)sections.data[i];
		fwrite(sec->sh.Name,1,sh_size,fout);
	}

	free(fh);
	fclose(fout);	
}