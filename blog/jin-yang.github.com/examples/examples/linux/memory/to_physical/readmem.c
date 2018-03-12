#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

typedef unsigned short  u16;
typedef signed short    s16;
typedef int             s32;
typedef unsigned int    u32;
typedef unsigned int    uint32;
typedef unsigned long long u64;



u64 utl_str2int(char *str)
{
    u64 ret = 0;
    int  i, len;
    char tmp = 0;
    char *ptr = str;

    len = strlen(str);
    if((ptr[1] == 'x') || (ptr[1] == 'X')) { // HEX
        ptr += 2;
        for (i = 2; i < len; i++) {
            tmp = *ptr++;
            if ((tmp <= '9') && (tmp >= '0')) {
                tmp = tmp -'0';
                ret = (ret << 4) + tmp;
            } else if ((tmp <= 'F') && (tmp >= 'A')) {
                tmp = tmp - 'A' + 10;
                ret = (ret << 4) + tmp;
            } else if ((tmp <= 'f') && (tmp >= 'a')) {
                tmp = tmp -'a' + 10;
                ret = (ret << 4) + tmp;
            }
        }
        return ret;
    } else {
        for (ret = 0, i = 0; i < len; i++) {
            if (ptr[i] >= '0' && ptr[i] <= '9') {
                ret = 10 * ret + (ptr[i] - '0');
            }
        }
        return ret;
    }
}

int main(int argc, char *argv[])
{
    int i;

    if(argc != 3){
        printf("   Usage: %s address length(bytes)\n", argv[0]);
        return 0;
    }

    u64 addr = utl_str2int(argv[1]);
    int size = (int)utl_str2int(argv[2]);

    printf("read from 0x%llX, size %d\n", addr, size);

    int fd = open("/dev/dram", O_RDONLY);
    if(fd < 0){
        perror("open");
        exit(EXIT_FAILURE);
    }

    addr = addr & 0xFFFFFFFFFFFFFFF8;
    __off64_t off = lseek64(fd, addr, SEEK_SET);
    if(off < 0){
        perror("lseek64");
        exit(EXIT_FAILURE);
    }

    if(size > 256){
        size = 256;
    } else if(size == 0){
        size = 8;
    }
    size = (size + 7) & 0xFFFFFFFFFFFFFFF8;

    u32 buf[256 / 4];
    int read_size = read(fd, (char*)buf, size);
    if(read_size < 0){
        perror("read");
        exit(EXIT_FAILURE);
    }

    for(i = 0; i< size / 8; i++){
        printf("0x%llX:\t %08x  %08x \n", addr + i * 8, buf[2 * i], buf[2 * i + 1]);
    }

    close(fd);

    return 0;
}
