/*
目的：atexit函數沒辦法帶參數給「離開函數」，atexit可以傳參數
使用方法：
on_exit 不用接參數
*/

#include <stdio.h>
#include <stdlib.h>


void myName(int ret, void *arg) {
	printf("%s shiwulo\n", (char*)arg);
}

int main(int argc, char **argv) {
	char* p = "professor";
	on_exit(myName, p);
	return 0;
}

