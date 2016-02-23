/* win_entry.c源文件 */
/* 程序用到的外部函数声明 */
int	  __stdcall	MessageBoxA(void* hWnd, char* lpText,  
														char* lpCaption,  int uType);
int __stdcall GetModuleHandleA(char* lpModuleName);
char* __stdcall GetCommandLineA();
/*********************************************************** 
 * 功能:	WinMain主函数
 **********************************************************/
int __stdcall WinMain (void* hInstance, void* hPrevInstance,
                    char* szCmdLine, int iCmdShow)
{
	  int MB_OK = 0;  /* 0x00000000L */
	  int MB_ICONINFORMATION = 64; /* 0x00000040L */
    MessageBoxA(0,szCmdLine,"支持传入命令行参数的Win32应用程序:", MB_OK + MB_ICONINFORMATION); 
    return 1 ;
}

/*********************************************************** 
 * 功能:	入口函数
 **********************************************************/
int _entry()
{
	char *szCmd; 

	szCmd = GetCommandLineA();	
	exit(WinMain(GetModuleHandleA(0), 0, szCmd,10));
}
