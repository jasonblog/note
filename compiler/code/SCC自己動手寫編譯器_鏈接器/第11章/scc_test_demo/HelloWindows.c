/* HelloWindows.c源文件 */
/* 程序用到的结构定义 */
struct tagPOINT
{
    int  x;
    int  y;
} ;

struct tagMSG 
{
    void*    					hwnd;
    int      					message;
    int      					wParam;
    int      					lParam;
    int      					time;
    struct tagPOINT   pt;
};

struct tagWNDCLASSA 
{
    int       style;
    void*     lpfnWndProc;
    int       cbClsExtra;
    int       cbWndExtra;
    void*   	hInstance;
    void*     hIcon;
    void*     hCursor;
    void*     hbrBackground;
    char*     lpszMenuName;
    char*     lpszClassName;
};

struct tagRECT
{
    int    left;
    int    top;
    int    right;
    int    bottom;
};

struct tagPAINTSTRUCT 
{
    void*         	hdc;
    int        			fErase;
    struct tagRECT  rcPaint;
    int       	 		fRestore;
    int        			fIncUpdate;
    char        		rgbReserved[32];
};

struct _STARTUPINFOA 
{
    int   	cb;
    char* 	lpReserved;
    char* 	lpDesktop;
    char* 	lpTitle;
    int   	dwX;
    int   	dwY;
    int   	dwXSize;
    int   	dwYSize;
    int   	dwXCountChars;
    int   	dwYCountChars;
    int   	dwFillAttribute;
    int   	dwFlags;
    short 	wShowWindow;
    short 	cbReserved2;
    char* 	lpReserved2;
    void* 	hStdInput;
    void* 	hStdOutput;
    void* 	hStdError;
};

/* 程序用到的函数声明 */
void* __stdcall LoadIconA(void* hInstance,  char* lpIconName);
void* __stdcall LoadCursorA(void* hInstance,  char* lpCursorName);
void* __stdcall GetStockObject(int c);
short __stdcall	RegisterClassA(struct tagWNDCLASSA *lpWndClass);
int	  __stdcall	MessageBoxA(void* hWnd, char* lpText,  
														char* lpCaption,  int uType);														
void* __stdcall CreateWindowExA(int dwExStyle,char* lpClassName,
	char* lpWindowName,int dwStyle,int X,int Y,int nWidth,int nHeight, 
	void* hWndParent, void* hMenu, void* hInstance, void* lpParam);	
int	  __stdcall	ShowWindow(void* hWnd,int nCmdShow);
int	  __stdcall	UpdateWindow(void* hWnd);
int	  __stdcall	GetMessageA(struct tagMSG* lpMsg, void* hWnd, 
	int wMsgFilterMin,  int wMsgFilterMax);	
int	  __stdcall TranslateMessage(struct tagMSG* lpMsg);
int	  __stdcall DispatchMessageA(struct tagMSG* lpMsg);
void* __stdcall BeginPaint(void* hWnd,struct tagPAINTSTRUCT* lpPaint);
int 	__stdcall  GetClientRect(void* hWnd,  struct tagRECT* lpRect);
int 	__stdcall DrawTextA(void* hDC,   char* lpString,   int nCount,  
	struct tagRECT* lpRect, int uFormat);	
int 	__stdcall EndPaint(void* hWnd,   struct tagPAINTSTRUCT*lpPaint);
int 	__stdcall GetModuleHandleA(char* lpModuleName);
char* __stdcall GetCommandLineA();
void GetStartupInfoA(struct _STARTUPINFOA* lpStartupInfo);
void 	__stdcall PostQuitMessage(int nExitCode);
int __stdcall DefWindowProcA(void* hWnd,int Msg,int wParam,int lParam);
int __stdcall WndProc (void *a, int b, int c , int d) ;

/* 程序用到的常量定义 */
char* szAppName 					= "HelloWin";
int   NULL 								= 0;
int 	CS_VREDRAW 					= 1;					
int 	CS_HREDRAW 					= 2;					
int 	IDI_APPLICATION 		= 32512;
int 	IDC_ARROW 					= 32512;
int 	WHITE_BRUSH 				= 0;
int 	MB_ICONERROR 				= 16;
int 	WS_OVERLAPPEDWINDOW = 13565952; 	/*0xcf0000*/
int 	CW_USEDEFAULT 			= 2147483648; /*0x80000000*/
int 	WM_PAINT 						= 15;
int 	WM_DESTROY 					= 2;
int 	DT_SINGLELINE 			= 32;
int 	DT_CENTER 					= 1;
int 	DT_VCENTER 					= 4;
int 	SW_SHOWDEFAULT 			= 10;

/*********************************************************** 
 * 功能:	WinMain主函数函数
 **********************************************************/
int __stdcall WinMain (void* hInstance, void* hPrevInstance,
                       char* szCmdLine, int iCmdShow)
{
     void*         					hwnd ;
     struct tagMSG          msg ;
     struct tagWNDCLASSA    wndclass ;
 
     wndclass.style         = CS_VREDRAW + CS_HREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIconA (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursorA (NULL, IDC_ARROW) ; 
     wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = 0 ;
     wndclass.lpszClassName = szAppName ;
     
     if (RegisterClassA (&wndclass) == 0)
     {
          MessageBoxA (NULL, "Cann’t RegisterClass!", szAppName, MB_ICONERROR);
          return 0 ;
     }

     hwnd = CreateWindowExA (0,szAppName,
                          szCmdLine, 
                          WS_OVERLAPPEDWINDOW, 
                          CW_USEDEFAULT, 
                          CW_USEDEFAULT,  
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,  
                          NULL, 
                          NULL,
                          hInstance,
                          NULL) ; 
     if(hwnd == NULL)
     		MessageBoxA(NULL,"error",szCmdLine,0);

     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;

     for(;GetMessageA (&msg, NULL, 0, 0);)
     {
          TranslateMessage (&msg);
          DispatchMessageA (&msg);
     }

     return msg.wParam ;
}
 
/*********************************************************** 
 * 功能:	窗口消息处理函数
 **********************************************************/
int __stdcall WndProc (void* hwnd, int message, int wParam, int lParam)
{
     void*         					hdc ;
     struct tagPAINTSTRUCT 	ps ;
     struct tagRECT        	rect ;     

		if(message == WM_PAINT)
     {
          hdc = BeginPaint (hwnd, &ps) ;
          GetClientRect (hwnd, &rect) ;
          DrawTextA (hdc, "Hello,world!", -1, &rect,
          					 DT_SINGLELINE + DT_CENTER + DT_VCENTER);         
          EndPaint (hwnd, &ps) ;
          return 0 ;
      }
     else if(message == WM_DESTROY)
     {
          PostQuitMessage (0) ;
          return 0 ;
     }

     return DefWindowProcA (hwnd, message, wParam, lParam);
}

/*********************************************************** 
 * 功能:	入口函数
 **********************************************************/
int _entry()
{
	char *szcmd; 

	szcmd = GetCommandLineA();	
	exit(WinMain(GetModuleHandleA(0), NULL, szcmd,SW_SHOWDEFAULT));
}
 
