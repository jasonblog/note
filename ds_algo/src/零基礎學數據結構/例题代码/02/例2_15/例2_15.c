/*----------------包含头文件部分------------------------*/
#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
/*----------------结构体定义部分-------------------------*/
struct Node {
    char name[10];
    int score;
    struct Node* next;
};
typedef struct Node ListNode;
/*--------------------函数声明部分--------------------------*/
ListNode* CreateList(int n);
void InsertList(ListNode* h, int i, char name[], int score, int n);
void DeleteList(ListNode* h, int i, int n);
void PrintList(ListNode* h);
/*----------------函数实现部分------------------------*/
/*----------------创建链表------------------------*/
ListNode* CreateList(int n)
/*在链表的末端插入新的结点，建立链表*/
{
    ListNode* head;
    ListNode* p, *pre;
    int i;
    head = (ListNode*)malloc(sizeof(ListNode));
    head->next = NULL;
    pre = head;

    for (i = 1; i <= n; i++) {
        printf("input name of the %d student:", i);
        p = (ListNode*)malloc(sizeof(ListNode));    /*为要插入的结点分配内存空间*/
        scanf("%s", &p->name);
        printf("input score of the %d student:", i);
        scanf("%d", &p->score);
        pre->next = p;                              /*将p指向的新结点插入链表*/
        pre = p;
    }

    p->next = NULL;
    return head;
}
/*--------------------输出链表元素------------------*/
void PrintList(ListNode* h)
{
    ListNode* p;
    p = h->next;

    while (p) {
        printf("%s,%d", p->name, p->score);
        p = p->next;
        printf("\n");
    }
}
/*--------------------主函数------------------*/
void main()
{
    ListNode* h;
    int i = 1, n, score;
    char name[10];

    while (i) {
        /*输入提示信息*/
        printf("1--建立新的链表\n");
        printf("2--添加元素\n");
        printf("3--删除元素\n");
        printf("4--输出当前表中的元素\n");
        printf("0--退出\n");

        scanf("%d", &i);

        switch (i) {
        case 1:
            printf("intput the number of elements.");                   /*输入创建链表结点的个数*/
            scanf("%d", &n);
            h = CreateList(n);              /*创建链表*/
            printf("list elements is:\n");
            PrintList(h);                   /*输出链表元素*/
            break;

        case 2:
            printf("input the position. of insert element:");
            scanf("%d", &i);                /*在链表的第i个位置插入*/
            printf("input name of the student:");
            scanf("%s", name);
            printf("input score of the student:");
            scanf("%d", &score);
            InsertList(h, i, name, score, n); /*插入结点*/
            printf("list elements is:\n");
            PrintList(h);
            break;

        case 3:
            printf("input the position. of delete element:");
            scanf("%d", &i);
            DeleteList(h, i, n);            /*删除链表的第i个结点*/
            printf("list elements is:\n");
            PrintList(h);
            break;

        case 4:
            printf("list elements is:\n");
            PrintList(h);
            break;

        case 0:
            return;
            break;

        default:
            printf("ERROR!Try again!\n\n");
        }
    }

}
/*-----------------插入链表结点--------------------------*/
void InsertList(ListNode* h, int i, char name[], int e, int n)
{
    ListNode* q, *p;
    int j;

    if (i < 1 || i > n + 1) {
        printf("Error!Please input again.\n");
    } else {
        j = 0;
        p = h;

        while (j < i - 1) {
            p = p->next;
            j++;
        }

        q = (ListNode*)malloc(sizeof(ListNode)); /*为要插入的结点分配内存空间*/
        strcpy(q->name, name);
        q->score = e;
        q->next = p->next; /*把新结点插入到链表中*/
        p->next = q;
    }
}
/*----------------删除链表结点------------------*/
void DeleteList(ListNode* h, int i, int n)
{
    ListNode* p, *q;
    int j;
    char name[10];
    int score;

    if (i < 1 || i > n) {
        printf("Error!Please input again.\n");
    } else {
        j = 0;
        p = h;

        while (j < i - 1) {
            p = p->next;
            j++;
        }

        q = p->next;                            /*q指向的结点为要删除的结点*/
        p->next = q->next;
        strcpy(name, q->name);
        score = q->score;
        free(q);                                /*释放q指向的结点*/
        printf("name=%s,score=%d\n", name, score);
    }
}
