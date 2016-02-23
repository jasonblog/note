/* struct_test.c源文件 */
/*********************************************************** 
 * 功能:	结构体测试
 **********************************************************/
struct point
{
	int x;
	int y;
};

void struct_test()
{
	struct point  pt;
	struct point *ppt;
	
	ppt = &pt;
	pt.x = 1024;
	pt.y = 768;
	printf("pt.x = %d, pt.y = %d\n",pt.x,pt.y);
	printf("ppt->x = %d, ppt->y = %d\n",ppt->x,ppt->y);	
}

/*********************************************************** 
 * 功能:	求类型长度测试
 **********************************************************/
void sizeof_test()
{
	printf("sizeof(char)=%d,  sizeof(short)=%d, sizeof(int)=%d, 
					sizeof(struct point)=%d\n",
					sizeof(char),	    sizeof(short),	  sizeof(int), 		
					sizeof(struct point));
}

/*********************************************************** 
 * 功能:	强制对齐测试
 **********************************************************/
void struct_align_test()
{
	struct s1
	{
		char 	m1;
		short m2;
		int 	m3;
	};

	struct s2
	{
		char 	m1;
		short __align(1) m2;
		int 	__align(1) m3;
	};
	
	struct s3
	{
		char 	m1;
		short __align(4) m2;
		int 	__align(4) m3;
	};
	
	printf("sizeof(struct s1) = %d\n",sizeof(struct s1));
	printf("sizeof(struct s2) = %d\n",sizeof(struct s2));
	printf("sizeof(struct s3) = %d\n",sizeof(struct s3));
}

/*********************************************************** 
 * 功能:	main函数
 **********************************************************/
int  main()
{
	struct_test();
	sizeof_test();
	struct_align_test();
	
	return 0;
}

/*********************************************************** 
 * 功能:	入口函数
 **********************************************************/
void  _entry()
{
	int ret;
	ret = main();
	exit(ret);
}