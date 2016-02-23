/* console_entry.c源文件 */
/*********************************************************** 
 * 功能:	main主函数
 **********************************************************/
int main(int argc, char **argv, char **env)
{
	printf(argv[1]);
	printf("\n");
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