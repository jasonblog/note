#include<dirent.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
    if (argc < 3)  {               //check no of args
        fprintf(stderr, "usage %s src_dir dst_src\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat stat_src;

    if (stat(argv[1], &stat_src) != 0)     {   //
        fprintf(stderr, "%s(%d): stat error(%s)!\n",  __   FILE__, __LINE__,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    umask(0000);                   //

    if (S_ISREG(stat_src.st_mode)) {       //
        struct stat stat_dst;

        if (stat(argv[2], &stat_dst) == -1) { //
            if (errno != ENOENT) {       //
                fprintf(stderr, "%s(%d): stat error(%s)!\n", __FILE__, __LINE__,
                        strerror(errno));
                exit(EXIT_FAILURE);
            } else {             //
                cp_file(argv[1], argv[2], stat_src.st_mode); //
            }
        } else {                  //
            if (S_ISDIR(stat_dst.st_mode)) { //
                char* ptr = (char*)malloc(strlen(argv[2]) + 1 + strlen(argv[1]) + 1);
                sprintf(ptr, "%s/%s\0", argv[2], argv[1]);
                cp_file(argv[1], ptr, stat_src.st_mode); //
            } else {                 //
                printf("file %s exist, do you want overwrite it[y/n]:", argv[2]);
                char ch;

                while (!scanf("%c", &ch)) {
                    getchar();
                }

                if (ch == 'Y' || ch == 'y') { //
                    unlink(argv[2]);       //
                    cp_file(argv[1], argv[2], stat_src.st_mode); //
                } else {
                    return 1;
                }
            }
        }
    }

    else if (S_ISDIR(stat_src.st_mode)) {      //
        struct stat stat_dst;

        if (stat(argv[2], &stat_dst) == -1) { //
            if (errno != ENOENT) {       //if errno not cause by file/dir not exist
                fprintf(stderr, "%s(%d): stat error(%s)!\n", __FILE__, __LINE__,
                        strerror(errno));
                exit(EXIT_FAILURE);
            } else {             //
                errno = 0;

                if (-1 == mkdir(argv[2], stat_src.st_mode)) { //
                    perror("mkdir");
                    exit(EXIT_FAILURE);
                }

                cp_dir(argv[1], argv[2]);   //
            }
        } else if (S_ISREG(stat_dst.st_mode)) { //
            fprintf(stderr, "can't copy a dir to a file\n");
            exit(EXIT_FAILURE);
        } else {                      //
            char* ptr = (char*)malloc(strlen(argv[1]) + 1 + strlen(argv[2]) + 1);
            sprintf(ptr, "%s/%s\0", argv[2], argv[1]);

            if (-1 == mkdir(ptr, stat_src.st_mode)) {
                perror("mkdir");
                exit(EXIT_FAILURE);
            }

            cp_dir(argv[1], ptr);
            free(ptr);
        }
    }
}

int cp_dir(const char* src, const char* dst)
{
    DIR* dirp = NULL;

    if (NULL == (dirp = opendir(src))) {       //
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent* entp = NULL;

    while (NULL != (entp = readdir(dirp))) { //
        if (strcmp(entp->d_name, "..") == 0 || strcmp(entp->d_name, ".") == 0) {
            continue;                        //
        }

        char* name_src = (char*) malloc(strlen(src) + 1 + strlen(entp->d_name) + 1);
        sprintf(name_src, "%s/%s\0", src, entp->d_name);
        char* name_dst = (char*) malloc(strlen(dst) + 1 + strlen(entp->d_name) + 1);
        sprintf(name_dst, "%s/%s\0", dst, entp->d_name);

        struct stat stat_src;

        if (stat(name_src, &stat_src) == -1) { //
            fprintf(stderr, "%s(%d): stat error(%s)!\n",
                    __FILE__, __LINE__, strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (S_ISREG(stat_src.st_mode)) {          //
            cp_file(name_src, name_dst, stat_src.st_mode);
            free(name_src);
            free(name_dst);
        } else if (S_ISDIR(stat_src.st_mode)) { //
            if (-1 == mkdir(name_dst, stat_src.st_mode)) {
                perror("mkdir");
                exit(EXIT_FAILURE);
            }

            cp_dir(name_src, name_dst);       //
            free(name_src);
            free(name_dst);
        }
    }
}


