#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
	int pipefd[2];
	char *str = "hello\n\0";
	char buf[200];
	pipe(pipefd);
	write(pipefd[1], str, strlen(str)+1);
	read(pipefd[0], buf, 200);
	printf("%s", buf);
	return 0;
}
