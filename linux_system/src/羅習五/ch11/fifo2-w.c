#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

char buf[200];

int main(int argc, char **argv) {
	int fd;
	int ret;
	ret=mkfifo("/tmp/shiwulo", 0666);
	printf("mkfifo() = %d\n", ret);
	close(1);
	fd = open("/tmp/shiwulo",O_WRONLY);
	printf("hello\n");
	printf("1234\n");
	printf("5678\n");
	close(fd);
	unlink("/tmp/shiwulo");
	return 0;
}
