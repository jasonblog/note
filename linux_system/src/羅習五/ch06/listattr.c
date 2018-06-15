/*
usage:
listattr filename
*/
#include <sys/types.h>
#include <sys/xattr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void print_value(char* pathname, char* attr) {
    char buf[4096];
    int total;
    total=getxattr(pathname, attr, buf, 4096);
    for (int i=0; i<total; i++)
        printf("%x", buf[i]);
    printf("\n\n");
}

int main (int argc, char** argv) {
    char *buf;
    int total, sum=0;
    char str[4096];
    buf = (char*)malloc(4096);
    total=listxattr(argv[1], NULL, 0);
    total=listxattr(argv[1], buf, total);
    for( ;sum !=total; ){
        sscanf(buf+sum, "%s", str);
        printf("xattr = %s\n", str);
        print_value(argv[1], str);
        sum += strlen(str)+1;
    }
}
