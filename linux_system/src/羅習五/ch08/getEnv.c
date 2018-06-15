/*
目的：拿到環境變數的值
使用方法：
getEnv 環境變數的名稱
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int i;
	char* value;
	for (i=1; argv[i]!=NULL; i++) {
		value = getenv(argv[i]);
		printf("%s=%s\n", argv[i], value);
	}
	
	return 0;
}

