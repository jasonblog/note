/*
目的：測試執行結束時，自動執行函數的功能
用法：atexit 不用接參數
練習：除了印出shiwulo以外，再註冊一個函數，列印出你的名字
*/

#include <stdio.h>
#include <stdlib.h>


void myName() {
	printf("shiwulo\n");
}

int main(int argc, char **argv) {
	atexit(myName);
	return 0;
}

