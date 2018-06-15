#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	int fd;
	int num;
	fd = open("./hello3",O_WRONLY | O_CREAT, 0644);
	for(num=0; num <=100000; num++) {
		write(fd, "1234", sizeof("1234"));
		fdatasync(fd);
		if (num%10000==1) {
			write(1, "*", sizeof("*"));
			fsync(1);
		}
	}
	return 0;
}
