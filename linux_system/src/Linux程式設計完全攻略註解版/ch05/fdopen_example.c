#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
int main(void)
{
    int fd;
    FILE* stream;
    unlink("test.txt");
    fd = open("test.txt", O_CREAT | O_WRONLY, S_IREAD | S_IWRITE); //open
    stream = fdopen(fd, "w");       //

    if (stream == NULL) {
        printf("fdopen failed\n");
    } else {
        fprintf(stream, "Hello world\n");   //
        fclose(stream);
    }

    printf("the content of the test.txt is:\n");
    system("cat test.txt");                 //
    return 0;
}


