#include <stdio.h>
#include <stdlib.h>
#include "list.h"

struct a_list {
    struct list_head list;
    int   val;
    const char* str;
};

static void append(struct a_list* ptr, const char* str, int val)
{
    struct a_list* tmp;
    tmp = (struct a_list*)malloc(sizeof(struct a_list));

    if (!tmp) {
        perror("malloc");
        exit(1);
    }

    tmp->str = str;
    tmp->val = val;
    list_add_tail(&(tmp->list), &(ptr->list));
}

int main()
{
    struct a_list  blist;
    struct a_list* iter;

    INIT_LIST_HEAD(&blist.list);

    /* add item to list */
    append(&blist, "NM", 87501);
    append(&blist, "CA", 94041);
    append(&blist, "IL", 60561);

    /* iterates list */
    list_for_each_entry(iter, &blist.list, list) {
        printf("%s %d\n", iter->str, iter->val);
    }

    /* remove all items in the list */
    while (!list_empty(&blist.list)) {
        iter = list_entry(blist.list.next, struct a_list, list);
        list_del(&iter->list);
        free(iter);
    }

    return 0;
}
