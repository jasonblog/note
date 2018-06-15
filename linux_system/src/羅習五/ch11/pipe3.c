#include <assert.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
	int pipefd[2];
	char buf[200];
	
	int ret;
	pipe(pipefd);
	ret = fork();
	assert(ret>=0);
	if (ret==0) {	/*child*/
		close(1);			/*關閉stdout*/
		dup(pipefd[1]);		/*將pipefd複製到stdout*/
		close(pipefd[1]);
		close(pipefd[0]);
		printf("hello");	/*印出 “hello” 到stdout*/
	} else {
		close(0);			/*關閉stdin*/
		dup(pipefd[0]);		/*將pipefd複製到stdin*/
		close(pipefd[0]);
		close(pipefd[1]);
		scanf("%s", buf);	/*從stdin讀入資料*/
		printf("parent: %s\n", buf);
	}
	return 0;
}

