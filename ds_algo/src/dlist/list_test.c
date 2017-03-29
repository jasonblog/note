/* A doubly linked list test program.
 *  Copyright (C) 2015  Yang Zhang <yzfedora@gmail.com>
 */
#define EMPLOYE_ADD_TAIL
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strcmp() */
#include "list.h"

struct employee {
    char*    name;
    char*    city;
    char*    country;
    unsigned age;

    struct list list;
};


static int employee_add(char* name, char* city, char* country, unsigned age,
                        struct list* head)
{
    struct employee* employee;

    if (!(employee = calloc(1, sizeof(*employee)))) {
        return -1;
    }

    employee->name = name;
    employee->city = city;
    employee->country = country;
    employee->age = age;

    list_add(&employee->list, head);
    return 0;
}

static int employee_add_tail(char* name, char* city, char* country,
                             unsigned age,
                             struct list* head)
{
    struct employee* employee;

    if (!(employee = calloc(1, sizeof(*employee)))) {
        return -1;
    }

    employee->name = name;
    employee->city = city;
    employee->country = country;
    employee->age = age;

    list_add_tail(&employee->list, head);
    return 0;
}

static void employee_travel(struct list* head)
{
    struct employee* pos;

    list_for_each(pos, head, list) {
        printf("Name: %s\n"
               "Age : %d\n"
               "From: %s, %s\n\n",
               pos->name, pos->age, pos->city, pos->country);
    }
}

static void employee_travel_reverse(struct list* head)
{
    struct employee* pos;

    list_for_each_reverse(pos, head, list) {
        printf("Name: %s\n"
               "Age : %d\n"
               "From: %s, %s\n\n",
               pos->name, pos->age, pos->city, pos->country);
    }
}

static void employee_move_by_name(char* name, struct list* head)
{
    struct employee* pos;

    list_for_each(pos, head, list) {
        if (!strcmp(pos->name, name)) {
            list_move(&pos->list, head);
        }
    }
}

static void employee_move_tail_by_name(char* name, struct list* head)
{
    struct employee* pos;

    list_for_each(pos, head, list) {
        if (!strcmp(pos->name, name)) {
            list_move_tail(&pos->list, head);
        }
    }
}

static void employee_del_all(struct list* head)
{
    struct employee* pos;

    list_for_each(pos, head, list) {
        list_del(&pos->list);
        free(pos);
    }
}

int main(int argc, char* argv[])
{
    LIST_HEAD(employee_head);   /* define a doubly linked list */

#ifndef EMPLOYE_ADD_TAIL
    /*
     * add some test samples. the last added entry will in the head of
     * list, like a stack
     */
    employee_add("Linus Torvalds", "Helsinki", "Finland", 46, &employee_head);
    employee_add("Hiroyuki Sawano", "Tokyo", "Japan", 35, &employee_head);
    employee_add("Yang Zhang", "Sichuan", "China", 22, &employee_head);
#else

    /* add the test sample in a normally order, the first added entry will
     * in the head of list. (FIFO) */
    employee_add_tail("Linus Torvalds", "Helsinki", "Finland", 46,
                      &employee_head);
    employee_add_tail("Hiroyuki Sawano", "Tokyo", "Japan", 35,
                      &employee_head);
    employee_add_tail("Yang Zhang", "Sichuan", "China", 22, &employee_head);
#endif


    /* travel the list, and print it to stdout. */
    printf("--------- travel the list ---------\n");
    employee_travel(&employee_head);

    printf("--------- travel the list in reverse order ---------\n");
    employee_travel_reverse(&employee_head);


    /* move last entry which name is "Yang Zhang" to the head of list */
    printf("--------- move Yang Zhang to the head ---------\n");

    employee_move_by_name("Yang Zhang", &employee_head);
    employee_travel(&employee_head);


    printf("--------- move Hiroyuki Sawano to the tail ---------\n");
    employee_move_tail_by_name("Hiroyuki Sawano", &employee_head);
    employee_travel(&employee_head);


    printf("--------- empty the list -------------\n");
    employee_del_all(&employee_head);   /* remove all entrys in the list. */
    printf("list is empty: %s\n",
           list_empty(&employee_head) ? "true" : "false");
    return 0;
}
