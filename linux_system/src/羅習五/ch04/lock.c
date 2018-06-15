#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>

int main(int argc, char* argv[]) {
	int fd;
	int ret;
	char opt;
	fd = open (argv [1], O_WRONLY);
	printf("fd = %d is opened\n", fd);
	
	sscanf(argv[2], "%c", &opt);
	switch (opt) {
		case 's':
			ret = flock(fd, LOCK_SH);
			break;
		case 'e':
			ret = flock(fd, LOCK_EX);
			break;
		case 'u':
			ret = flock(fd, LOCK_UN);
			break;
		default:
			printf("input error\n");
	}
	if (ret != 0)
		perror("flock");
	printf("end\n");
	getchar();
	return 0;
}
