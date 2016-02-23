/* screen_capture.c源文件 */
struct tagWNDCLASSA {
    int        style;
    void*     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    void*   hInstance;
    void*       hIcon;
    void*     hCursor;
    void*      hbrBackground;
    char*      lpszMenuName;
    char*      lpszClassName;
};

struct HBITMAP__		{ int unused; };
struct HDC__ 				{ int unused; };
struct HWND__ 			{ int unused; };
struct HINSTANCE__ 	{ int unused; };
struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };

struct BITMAPINFOHEADER{
        int      biSize;
        int      biWidth;
        int      biHeight;
        short    biPlanes;
        short    biBitCount;
        int      biCompression;
        int      biSizeImage;
        int      biXPelsPerMeter;
        int      biYPelsPerMeter;
        int      biClrUsed;
        int      biClrImportant;
};

struct RGBQUAD {
        char    rgbBlue;
        char    rgbGreen;
        char    rgbRed;
        char    rgbReserved;
};

struct BITMAPINFO {
    struct BITMAPINFOHEADER    bmiHeader;
    struct RGBQUAD             bmiColors[1];
};

struct BITMAPFILEHEADER {
        short   bfType;
        int    __align(2)	bfSize;
        short   bfReserved1;
        short   bfReserved2;
        int    __align(2)	bfOffBits;
};

struct OPENFILENAMEA {
   int        						lStructSize;
   struct HWND__*         hwndOwner;
   struct HINSTANCE__*    hInstance;
   char*       						lpstrFilter;
   char*        					lpstrCustomFilter;
   int        						nMaxCustFilter;
   int        						nFilterIndex;
   char*        					lpstrFile;
   int        						nMaxFile;
   char*       						lpstrFileTitle;
   int        						nMaxFileTitle;
   char*       						lpstrInitialDir;
   char*      				 		lpstrTitle;
   int        						Flags;
   short         					nFileOffset;
   short         					nFileExtension;
   char*       						lpstrDefExt;
   int       							lCustData;
   void* 									lpfnHook;
   char*       						lpTemplateName;
};

struct tagWNDCLASSA *test_return_struct_pointer()
{
  struct tagWNDCLASSA   wndclass ; 
  wndclass.style         = 654321 ;
  wndclass.lpfnWndProc   = 0 ;
  wndclass.cbClsExtra    = 0 ;
	return &wndclass;
}

int test_structassign()
{
  struct tagWNDCLASSA   wndclass ; 
  wndclass.style         = 3 ;
  wndclass.lpfnWndProc   = 0 ;
  wndclass.cbClsExtra    = 0 ;
	return 1;
}

int MB_OK = 0;          /* #define MB_OK 0x00000000L*/
int MB_ICONERROR = 16;  
int BI_RGB = 0;
int DIB_RGB_COLORS = 0;
int SM_CXSCREEN = 0;
int SM_CYSCREEN = 1;
int SRCCOPY = 13369376;  /*#define SRCCOPY    (DWORD)0x00CC0020 */
int NULL = 0;
int OFN_HIDEREADONLY = 4;
int OFN_PATHMUSTEXIST = 2048; /*#define OFN_PATHMUSTEXIST            0x00000800*/

struct HDC__* 		__stdcall GetDC();
struct HWND__* 		__stdcall GetDesktopWindow();
struct HBITMAP__* __stdcall CreateCompatibleBitmap();

int   __stdcall GetDIBits();
int   __stdcall MessageBoxA();
int		__cdecl 	fwrite();
int   __stdcall ReleaseDC();
int   __stdcall GetSaveFileNameA();
int 	__stdcall GetSystemMetrics();
void* 	__stdcall SelectObject();
int 	__stdcall BitBlt();
int 	__stdcall DeleteDC();
int 	__stdcall DeleteObject();

void* __cdecl 	memset();
void  __cdecl 	free();
int 	__cdecl 	fclose();
char* __cdecl 	strcpy();
int   __cdecl 	abs();

void	SaveBitmap(char *szFilename,struct HBITMAP__* hBitmap)
{
	struct HDC__*						hdc=NULL;
	struct FILE*						fp=NULL;
	void*										pBuf=NULL;
	struct BITMAPINFO				bmpInfo;
	struct BITMAPFILEHEADER	bmpFileHeader;
	for(;;)
	{		
		hdc=GetDC(NULL);
	
		memset(&bmpInfo,0,sizeof(struct BITMAPINFO));
		bmpInfo.bmiHeader.biSize=sizeof(struct BITMAPINFOHEADER);
		GetDIBits(hdc,hBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS);
		
	if(bmpInfo.bmiHeader.biSizeImage<=0)
			bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;
		
	if((pBuf=malloc(bmpInfo.bmiHeader.biSizeImage))==NULL)
		{
			MessageBoxA(NULL,"Unable to Allocate Bitmap Memory","Error",MB_OK+MB_ICONERROR);
			break;
		}
		
		bmpInfo.bmiHeader.biCompression=BI_RGB;
		GetDIBits(hdc,hBitmap,0,bmpInfo.bmiHeader.biHeight,pBuf,&bmpInfo,DIB_RGB_COLORS);	
		
		fp=fopen(szFilename,"wb");
		if(fp == NULL)
		{
			MessageBoxA(NULL,"Unable to Create Bitmap File","Error",MB_OK+MB_ICONERROR);
			break;
		}
		
		bmpFileHeader.bfReserved1=0;
		bmpFileHeader.bfReserved2=0;
		bmpFileHeader.bfSize=sizeof(struct BITMAPFILEHEADER)+sizeof(struct BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
		bmpFileHeader.bfType= 19778; /*0x424d "BM" */
		bmpFileHeader.bfOffBits=sizeof(struct BITMAPFILEHEADER)+sizeof(struct BITMAPINFOHEADER);
		
		fwrite(&bmpFileHeader,sizeof(struct BITMAPFILEHEADER),1,fp);
		fwrite(&bmpInfo.bmiHeader,sizeof(struct BITMAPINFOHEADER),1,fp);
		fwrite(pBuf,bmpInfo.bmiHeader.biSizeImage,1,fp);
		break;
	}
	
	if(hdc)
		ReleaseDC(NULL,hdc);
	
	if(pBuf)
		free(pBuf);
	
	if(fp)
		fclose(fp);
}

/*********************************************************** 
 * 功能:	main函数
 **********************************************************/
int main()
{
	struct 						OPENFILENAMEA	ofn;
	char							strFileName[512]; 
	int								nWidth;
	int								nHeight;
	struct HDC__*			hBmpFileDC;
	struct HWND__*		hDesktopWnd;
	struct HDC__*		  hDesktopDC;
	struct HBITMAP__*	hBmpFileBitmap;
	struct HBITMAP__* hOldBitmap;
  struct tagWNDCLASSA *pwndclass;	
	
  strcpy(strFileName,"ScreenShot.bmp");
  memset(&ofn,0,sizeof(struct OPENFILENAMEA));
	ofn.lStructSize=sizeof(struct OPENFILENAMEA);
	ofn.Flags=OFN_HIDEREADONLY+OFN_PATHMUSTEXIST;
	ofn.lpstrFilter="Bitmap Files (*.bmp)|*.bmp";
	ofn.lpstrDefExt="bmp";
	ofn.lpstrFile=strFileName;
	ofn.nMaxFile=512;
	ofn.hwndOwner = NULL;
	if(GetSaveFileNameA(&ofn)==0)	return 1;
	
	hDesktopWnd=GetDesktopWindow();
	hDesktopDC=GetDC(hDesktopWnd);
	nWidth=GetSystemMetrics(SM_CXSCREEN);
	nHeight=GetSystemMetrics(SM_CYSCREEN);
	hBmpFileDC=CreateCompatibleDC(hDesktopDC);
	hBmpFileBitmap=CreateCompatibleBitmap(hDesktopDC,nWidth,nHeight);
	hOldBitmap = SelectObject(hBmpFileDC,hBmpFileBitmap);
	BitBlt(hBmpFileDC,0,0,nWidth,nHeight,hDesktopDC,0,0,SRCCOPY);
	SelectObject(hBmpFileDC,hOldBitmap);
	
	SaveBitmap(ofn.lpstrFile,hBmpFileBitmap);
	
	DeleteDC(hBmpFileDC);
	DeleteObject(hBmpFileBitmap);
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