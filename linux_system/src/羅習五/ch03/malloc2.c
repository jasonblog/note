#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
	int i;
	char* p;
	printf("pid = %d\n", getpid());
	printf("malloc 64*4K\n");
	for (i=0; i<64*4096; i++)
		p=(char*)malloc(1);
	while(1);
}
