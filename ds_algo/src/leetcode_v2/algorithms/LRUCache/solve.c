#include <stdio.h>
#include <stdlib.h>
typedef struct node {
    int key;
    int value;
    struct node* next;
    struct node* prev;
} node, *Node;
typedef struct {
    int capacity;
    int size;
    Node head;
    Node last;
}* LRU;
static LRU lru = NULL;
void lruCacheInit(int capacity)
{
    lru = malloc(sizeof(*lru));
    lru -> capacity = capacity;
    lru -> size = 0;
    lru -> head = NULL;
    lru -> last = NULL;
}
Node find(int key)
{
    if (lru -> size == 0) {
        return NULL;
    }

    Node p = lru -> head;

    while (p) {
        if (p->key == key) {
            return p;
        }

        p = p -> next;
    }

    return NULL;
}
Node fetch(int key)
{
    Node p = find(key);

    if (p == NULL) {
        return NULL;
    }

    if (lru -> head == p) { // p已经是头指针
        return p;
    }

    if (lru -> last == p) { // p 是尾部指针
        p -> prev -> next = NULL;
        lru -> last = p -> prev; // update last
        lru -> head -> prev = p;
        p -> next = lru->head;
        p -> prev = NULL;
        lru -> head = p;
        return p;
    }

    p -> next -> prev = p -> prev;
    p -> prev -> next = p -> next;
    p -> next = lru -> head;
    lru -> head -> prev = p;
    p -> prev = NULL;
    lru -> head = p;
    return p;
}
int lruCacheGet(int key)
{
    Node p = fetch(key);

    if (p == NULL) {
        return -1;
    } else {
        return p -> value;
    }
}
void lruCacheSet(int key, int value)
{
    Node p = fetch(key);

    if (p) {
        p -> value = value;
        return;
    }

    if (lru -> size < lru->capacity) {
        Node n = malloc(sizeof(*n));
        n -> key = key;
        n -> value = value;
        n->prev = NULL;

        if (lru -> size == 0) {
            lru -> head = n;
            lru -> last = n;
            n -> next = NULL;
            (lru -> size)++;
        } else {
            n -> next = lru->head;
            lru->head->prev = n;
            lru->head = n;
            (lru->size)++;
        }
    } else {
        Node p = lru->last;

        if (p == lru->head) { // Only one node
            p->key = key;
            p->value = value;
            return;
        }

        p->prev->next = NULL; // delete last node
        lru->last = p->prev; // update last

        p->key = key;
        p->value = value;
        // insert into the head
        lru->head->prev = p;
        p->next = lru->head;
        p->prev = NULL;
        lru->head = p;
    }
}
void lruCacheFree()
{
    if (lru->size == 0) {
        free(lru);
        lru = NULL;
        return;
    }

    Node p = lru->head;
    Node q = p -> next;

    while (q) {
        free(p);
        p = q;
        q = q->next;
    }

    free(p); // free the last node
    lru->head = NULL;
    lru->last = NULL;
    free(lru);
    lru = NULL;

}
void set(int key, int value)
{
    lruCacheSet(key, value);
}
int get(int key)
{
    return lruCacheGet(key);
}
void init(int cap)
{
    lruCacheInit(cap);
}
int main(int argc, char** argv)
{
    init(5);
    get(1);
    set(1, 1);
    get(2);
    lruCacheFree();
    return 0;
}
