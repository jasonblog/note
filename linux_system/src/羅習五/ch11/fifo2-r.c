#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

char buf[200];

int main(int argc, char **argv) {
	int fd; int ret;
	ret = mkfifo("/tmp/shiwulo", 0666);
	printf("mkfifo() = %d\n", ret);
	close(0);
	fd = open("/tmp/shiwulo",O_RDONLY);
	scanf("%s", buf);
	printf("%s\n", buf);
	scanf("%s", buf);
	printf("%s\n", buf);
	scanf("%s", buf);
	printf("%s\n", buf);
	close(fd);
	unlink("/tmp/shiwulo");
	return 0;
}

