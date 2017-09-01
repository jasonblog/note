#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    struct stat buf;
    time_t t;
    char* str_time;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &buf) < 0) {
        fprintf(stderr, "Err.(%s) stat() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (time(&t) < 0) {
        fprintf(stderr, "Err.(%s) time() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((str_time = ctime(&t)) == NULL) {
        fprintf(stderr, "Err.(%s) ctime() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    str_time[strlen(str_time) - 1] = '\0';

    if (S_ISREG(buf.st_mode)) {
        printf("-");
    } else if (S_ISDIR(buf.st_mode)) {
        printf("d");
    } else if (S_ISCHR(buf.st_mode)) {
        printf("c");
    } else if (S_ISBLK(buf.st_mode)) {
        printf("b");
    } else if (S_ISFIFO(buf.st_mode)) {
        printf("f");
    } else if (S_ISLNK(buf.st_mode)) {
        printf("l");
    } else if (S_ISSOCK(buf.st_mode)) {
        printf("s");
    } else {
        printf("u");
    }


    if (S_IRUSR & buf.st_mode) {
        printf("r");
    } else {
        printf("-");
    }

    if (S_IWUSR & buf.st_mode) {
        printf("w");
    } else {
        printf("-");
    }

    if (S_IXUSR & buf.st_mode) {
        printf("x");
    } else {
        printf("-");
    }

    if (S_IRGRP & buf.st_mode) {
        printf("r");
    } else {
        printf("-");
    }

    if (S_IWGRP & buf.st_mode) {
        printf("w");
    } else {
        printf("-");
    }

    if (S_IXGRP & buf.st_mode) {
        printf("x");
    } else {
        printf("-");
    }

    if (S_IROTH & buf.st_mode) {
        printf("r");
    } else {
        printf("-");
    }

    if (S_IWOTH & buf.st_mode) {
        printf("w");
    } else {
        printf("-");
    }

    if (S_IXOTH & buf.st_mode) {
        printf("x");
    } else {
        printf("-");
    }

    printf(" %d", buf.st_nlink);
    printf(" %d", buf.st_uid);
    printf(" %d", buf.st_gid);
    printf(" %d ", (int)buf.st_size);
    printf(" %s %s", str_time, argv[1]);

    return (EXIT_SUCCESS);
}
