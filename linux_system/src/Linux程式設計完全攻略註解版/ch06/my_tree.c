#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

#define REGU_FILE 1
#define DIR_FILE 2

typedef struct node {
    struct node* next;
    unsigned int level;
    char* name;
    char* fullname;
} filenode;


typedef struct head {
    struct node* head;
    struct node* rear;
} headnode;

int insert_sort(headnode* link_stack, filenode* new)
{


    filenode* next = link_stack->head;
    filenode* prev = NULL;

    while (next) {
        if (strcmp(new->name, next->name) > 0) {
            prev = next;
            next = next->next;
        } else {
            break;
        }
    }

    if (link_stack->head == NULL && link_stack->rear == NULL) { //the first one
        link_stack->head = new;
        link_stack->rear = new;
    } else if (prev == NULL && next != NULL) {  //insert in front of the queue
        new->next = link_stack->head;
        link_stack->head = new;
    } else if (prev != NULL && next == NULL) {   //insert in end of the queue.
        prev->next = new;
        link_stack->rear = new;
    } else {
        prev->next = new;
        new->next = next;
    }
}

headnode* read_dir_to_link_stack(char* dir, int level)
{
    DIR* dirp = NULL;

    if (NULL == (dirp = opendir(dir))) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    headnode* ptr = (headnode*)malloc(sizeof(headnode));

    if (ptr == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    ptr->head = NULL;
    ptr->rear = NULL;

    struct dirent* entp = NULL;

    while (NULL != (entp = readdir(dirp))) {
        if (strcmp(entp->d_name, "..") == 0 ||
            strcmp(entp->d_name, ".") == 0) { //ignore ./ ../
            continue;
        } else {
            filenode* temp = (filenode*)malloc(sizeof(filenode));

            if (temp == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            temp->next = NULL;
            temp->level = level;

            temp->name = (char*)malloc(strlen(entp->d_name) + 1);
            sprintf(temp->name , "%s\0", entp->d_name);

            temp->fullname = (char*)malloc(strlen(dir) + 1 + strlen(entp->d_name) + 1);
            sprintf(temp->fullname, "%s/%s\0", dir, entp->d_name);

            insert_sort(ptr, temp);
        }
    }

    closedir(dirp);
    return ptr;
}
/*
    type:1, regufile. 2 dir.
*/
void out_file_info(filenode* ptr, int type)
{
    int i;
    printf("|");

    for (i = 0; i < ptr->level; i++) {
        printf("    ");
    }

    printf("|-- ");
    printf("%s\n", ptr->name);
}

void pop_file_tree(headnode* link_stack)
{
    while (link_stack->head != NULL) {
        struct stat stat_src;

        if (lstat(link_stack->head->fullname, &stat_src) != 0) {
            fprintf(stderr, "%s(%d): stat error(%s)!\n", __FILE__, __LINE__,
                    strerror(errno));
        }

        if (S_ISDIR(stat_src.st_mode)) {
            filenode* temp = link_stack->head;
            link_stack->head = link_stack->head->next;

            if (link_stack->head == NULL) {
                link_stack->rear == NULL;
            }

            out_file_info(temp, DIR_FILE);

            dir_tree(temp->fullname, link_stack, temp->level);

            free(temp->name);
            free(temp->fullname);
            free(temp);
        } else {
            filenode* temp = link_stack->head;
            link_stack->head = link_stack->head->next;

            if (link_stack->head == NULL) {
                link_stack->rear == NULL;
            }

            out_file_info(temp, REGU_FILE);

            free(temp->name);
            free(temp->fullname);
            free(temp);
        }
    }
}

dir_tree(char* dirname, headnode* link_stack, int level)
{
    headnode* ret = NULL;
    ret = read_dir_to_link_stack(dirname, level + 1);

    if (link_stack->head != NULL && ret->head != NULL) {
        ret->rear->next = link_stack->head;
        link_stack->head = ret->head;
    }

    if (link_stack->head == NULL && ret->head != NULL) {
        link_stack->head = ret->head;
    }

    if (link_stack->rear == NULL && ret->rear != NULL) {
        link_stack->rear = ret->rear;
    }

    free(ret);
    pop_file_tree(link_stack);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "pls useage %s dir_name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct stat stat_src;

    if (lstat(argv[1], &stat_src) != 0) {
        fprintf(stderr, "%s(%d): stat error(%s)!\n", __FILE__, __LINE__,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (S_ISREG(stat_src.st_mode)) {    //regular file
        fprintf(stderr, "%s [error opening dir]\n", argv[1]);
        exit(EXIT_FAILURE);
    } else if (S_ISDIR(stat_src.st_mode)) {
        headnode* link_stack = (headnode*)malloc(sizeof(headnode));

        if (link_stack == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        link_stack->head = NULL;
        link_stack->rear = NULL;

        printf("%s\n", argv[1]);
        dir_tree(argv[1], link_stack, -1);

        free(link_stack);
    } else {
        return 1;
    }
}
