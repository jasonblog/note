#include <stdio.h>
#include <wchar.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *str = "中文"; 
	printf("%d\n", (int)strlen(str));
	return 0;
}

