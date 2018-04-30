/*包含头文件*/
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
/*类型定义*/
typedef int DataType;
typedef struct Node {
    DataType data;
    struct Node* prior;
    struct Node* next;
} DListNode, *DLinkList;
/*函数声明*/

void PrintDList(DLinkList head);

int InsertDList(DLinkList head, int i, char e);
/*函数实现*/
int InitDList(DLinkList* head)
/*初始化双向循环链表*/
{
    *head = (DLinkList)malloc(sizeof(DListNode));

    if (!head) {
        return -1;
    }

    (*head)->next = *head;          /*使头结点的prior指针和next指针指向自己*/
    (*head)->prior = *head;
    return 1;
}

int InsertDList(DLinkList* L, int i, char e)
/*在双向循环链表的第i个位置插入元素e建立双向循环链表。*/
{
    DListNode* s, *p;
    int j;
    p = (*L)->next;
    j = 1;

    /*查找链表中第i个结点*/
    while (p != *L && j < i) {
        p = p->next;
        j++;
    }

    if (j > i) {        /*如果要位置不正确，返回NULL*/
        return 0;
    }

    s = (DListNode*)malloc(sizeof(DListNode));

    if (!s) {
        return -1;
    }

    s->data = e;
    /*将s结点插入到双向循环链表*/
    s->prior = p->prior;
    p->prior->next = s;
    s->next = p;
    p->prior = s;
    return 1;
}
void InvertDList(DLinkList* L)
/*对双向循环链表L就地逆置*/
{
    DListNode* p, *q, *r;
    p = (*L)->next;                     /*p指向第一个结点*/

    if ((*L) != NULL && (*L) != (*L)->next) { /*如果链表L不为空*/
        /*将链表L的第一个结点逆置*/
        q = p->next;
        (*L)->next = p;
        p->prior = (*L);
        (*L)->next->next = (*L);
        (*L)->prior = p;

        /*将链表L的每个结点依次插入到L的头部*/
        while (q != *L) {
            r = q->next;        /*r指向下一个待逆置的结点*/
            q->next = (*L)->next;
            (*L)->next->prior = q;
            q->prior = (*L);
            (*L)->next = q;
            q = r;              /*p指向下一个待逆置的结点*/
        }
    }
}

void main()
{
    DLinkList L;

    DListNode* p;
    int i, m, n;
    DataType a[] = {7, 3, 13, 11, 10, 23, 45, 6};
    n = 8;
    InitDList(&L);

    for (i = 0; i < n; i++) {
        InsertDList(&L, i + 1, a[i]);
    }

    printf("逆置前：顺序表L中的元素依次为：");
    PrintDList(L);
    InvertDList(&L);
    printf("逆置后：顺序表L中的元素依次为：");
    PrintDList(L);
}
void PrintDList(DLinkList head)
/*输出双向循环链表中的每一个元素*/
{
    DListNode* p;
    p = head->next;

    while (p != head) {
        printf("%4d", p->data);
        p = p->next;
    }

    printf("\n");
}
