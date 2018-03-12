#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>

typedef struct{
    char name[4];
    int  age;
}people;

int main(int argc, char** argv) // map a normal file as shared mem:
{
    int fd,i;
    people *p_map;
    char temp;

    /* open file */
    if((fd = open("share.txt", O_CREAT|O_RDWR|O_TRUNC, 00644)) < 0) {
        perror("open file");
        exit(EXIT_FAILURE);
    }
    lseek(fd, sizeof(people)*5-1, SEEK_SET);
    write(fd, "", 1);
    /* OR open file according to the file size.
    struct stat sb;
    if ((fd = open(argv[1], O_RDWR)) < 0)
        perror("open file");
    if ((fstat(fd, &sb)) == -1) // get file attributes
        perror("fstat");
    if ((mapped = (char *)mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE,
    */

    /* map the file to memory space */
    if ((p_map = (people *)mmap(NULL, sizeof(people)*10, PROT_READ|PROT_WRITE,
                MAP_SHARED, fd, 0)) == (void *)-1) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    close(fd); // the file has saved in memory

    temp = 'a';
    for(i=0; i<10; i++) {
        temp += 1;
        memcpy((*(p_map+i)).name, &temp, 2 );
        (*(p_map+i)).age = 20+i;
    }
    printf(" initialize over \n ");
    sleep(11);

    for(i=0; i<10; i++) {
        (*(p_map+i)).age = 0;
    }
    printf(" set age to 0\n ");

    munmap( p_map, sizeof(people)*10 );
    printf( "umap ok \n" );

    return 0;
}
