#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

typedef struct{
    char name[4];
    int  age;
} people;

int main(int argc, char** argv)  // map a normal file as shared mem:
{
    int fd, i;
    people *p_map;

    fd = open("share.txt", O_CREAT|O_RDWR, 00644);
    p_map = (people*)mmap(NULL, sizeof(people)*10, PROT_READ|PROT_WRITE,
       MAP_SHARED, fd, 0);
    for(i=0; i<10; i++)
        printf( "name: %s age %d;\n",(*(p_map+i)).name, (*(p_map+i)).age );
    munmap(p_map, sizeof(people)*10 );
    close(fd);

    return 0;
}
