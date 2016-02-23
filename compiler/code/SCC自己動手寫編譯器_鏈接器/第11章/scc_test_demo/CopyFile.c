/* CopyFile.c源文件 */
/*********************************************************** 
 * 功能:	main主函数
 **********************************************************/
 struct _iobuf 
 {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
};

int main(int argc, char **argv, char **env)
{
	int ch;
	struct _iobuf  *srcfile;
	struct _iobuf  *destfile;
	char srcfname[300];    
	char destfname[300]; 
	int i=0;
	char buf[2000]; 
	int NULL = 0;
  int EOF  = -1; 
  int SEEK_SET = 0;

		printf("源文件名：");
	scanf("%s",srcfname);

	printf("目标文件名：");
	scanf("%s",destfname);

	srcfile=fopen(srcfname,"rb");
	if(srcfile==NULL)
	{
		printf("读入文件未找到！\n");
		printf("按任意键继续...");
		getchar();
		exit(1); 
	}

	destfile = fopen(destfname, "wb");
	if(destfile==NULL) 
	{
		printf("写入文件未找到！\n");
		printf("按任意键继续...");
		getchar();
		fclose(srcfile);
		exit(1);
	}

	for(;(ch=getc(srcfile))!=EOF;)
	{
	  putchar(ch);
		fwrite(&ch,1,1,destfile);
	}

	fclose(srcfile);
	fclose(destfile);
	
	printf("按任意键继续...");
	getchar();
	return 0;
}

/*********************************************************** 
 * 功能:	入口函数
 **********************************************************/
void  _entry()
{
	int argc; char **argv; char **env; int ret;
	int start_info = 0;

	__getmainargs(&argc, &argv, &env, 0, &start_info);
	ret = main(argc, argv, env);
	exit(ret);
}