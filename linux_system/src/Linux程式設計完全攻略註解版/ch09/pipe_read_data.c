#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int p[2];
    char buf[128];

    pipe(p);
    // 填入10 byte to pipe
    write(p[1], "HelloWorld", 10);
    memset(buf, '\0', sizeof(buf) / sizeof(buf[0]));
    // read 128byte 
    read(p[0], buf, sizeof(buf) / sizeof(buf[0]));
    printf("buf=%s\n", buf);
}
