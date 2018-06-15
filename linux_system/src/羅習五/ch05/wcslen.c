#include <stdio.h>
#include <wchar.h>
#include <string.h>

int main(int argc, char **argv)
{
	wchar_t* wstr = L"中文"; 
	printf("%d\n", (int)wcslen(wstr));
	return 0;
}

