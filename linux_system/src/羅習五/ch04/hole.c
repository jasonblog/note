#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	int fd;
	fd = open("./myHole", O_RDWR| O_CREAT, S_IRUSR| S_IWUSR);
	if (fd <0)
		perror("open");
	write(fd, "1", sizeof("1"));
	lseek(fd, 100000, SEEK_SET);
	write(fd, "2", sizeof("2"));
	lseek(fd, 100000, SEEK_CUR);
	write(fd, "3", sizeof("3"));
	close(fd);
	return 0;
}
