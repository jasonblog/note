#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "foobar.h"

int main(int argc, char **argv)
{
	int fd, ret, cmd, val;
	char buf[1024] = "hello word";
	char rbuf[1024];

	fd = open("/dev/foobar_dev0", O_RDWR);
	if(fd < 0){
		perror("open");
		return -1;
	}
	printf("open successful, fd = %d.\n", fd);

	ret = write(fd, buf, strlen(buf));
	if(ret < 0){
		perror("write");
		return -1;
	}
	printf("write %d bytes.\n", ret);

    cmd = DEV_FOOBAR_GETVALUE;
    ret = ioctl(fd, cmd, &val);
    if (ret < 0) {
        perror("ioctl (DEV_FOOBAR_GETVALUE):");
        return -1;
    }
    printf("current value %d\n", val);
    cmd = DEV_FOOBAR_SETVALUE;
    ret = ioctl(fd, cmd, 5);
    if (ret < 0) {
        perror("ioctl (DEV_FOOBAR_SETVALUE):");
        return -1;
    }

    ret = lseek(fd, 0, SEEK_SET);
	if(ret < 0){
        perror("lseek");
        return -1;
    }

    ret = read(fd, rbuf, sizeof(rbuf));
	if(ret < 0){
		perror("read");
		return -1;
	}
	printf("read %d bytes, %s !\n", ret, rbuf);

    close(fd);
	return 0;
}
