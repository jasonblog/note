#include <stdio.h>
void    pr_stdio(const char*, FILE*);
int main(void)
{
    FILE*    fp;
    fputs("enter any character\n", stdout);

    if (getchar() == EOF) {
        printf("getchar error");
    }

    fputs("one line to standard error\n", stderr);
    pr_stdio("stdin",  stdin);          //
    pr_stdio("stdout", stdout);             //
    pr_stdio("stderr", stderr);             //

    if ((fp = fopen("/etc/motd", "r")) == NULL) { //
        printf("fopen error");
    }

    if (fgetc(fp) == EOF) {
        printf("getc error");
    }

    pr_stdio("/etc/motd", fp);
    return (0);
}
void pr_stdio(const char* name, FILE* fp)
{
    printf("stream = %s, ", name);

    if (fp->_flags & _IO_UNBUFFERED) {      //
        printf("unbuffered");
    } else if (fp->_flags & _IO_LINE_BUF) {     //
        printf("line buffered");
    } else {                    //
        printf("fully buffered or modified");
    }

    printf(", buffer size = %d\n", fp->_IO_buf_end - fp->_IO_buf_base);
}


