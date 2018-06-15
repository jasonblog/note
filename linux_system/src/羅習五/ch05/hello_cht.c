#include <stdio.h>
#include <wchar.h>

int main(int argc, char **argv)
{
	wchar_t *wstr = L"中文"; 
	printf("%d\n",fwide(stdout, 0));
	wprintf(L"%lc\n",wstr);
	printf("%d",fwide(stdout, 1));
	printf("strlen(好) = %d\n",(int)wcslen(L"好"));
	printf("%d",fwide(stdout, 0));
	printf("end\n");
	return 0;
}

