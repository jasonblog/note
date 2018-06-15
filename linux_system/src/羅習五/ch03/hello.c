#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	printf("pid = %d\n", getpid());
	printf("hello\n");
	while(1);
}
