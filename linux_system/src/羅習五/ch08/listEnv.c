/*
使用方法：
不用給任何參數
*/
#include <stdlib.h>
#include <stdio.h>
extern char **environ;

int main(int argc, char**argv) {
	int i;
	while(environ[i] != NULL) {
		printf("%s\n", environ[i++]);
	}
}
