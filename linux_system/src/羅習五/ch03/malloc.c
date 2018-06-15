#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    char* p1;
    char* p2;
    /*印出行程的pid，方便我們到/proc目錄裡面找到相對映的檔案*/
    printf("pid = %d\n", getpid());
    printf("malloc(64)\n");
    /*配置64byte記憶體*/
	p1 = (char*)malloc(64);
	printf("p1=%p\n", p1);
	printf("malloc 64*4K\n");
	/*配置256K記憶體*/
	p2 = (char*)malloc(64*4096);
	printf("p2=%p\n", p2);
	/*不要讓程式立即結束，因為我們還要觀察這個程式的記憶體行為*/
	while(1);
}



