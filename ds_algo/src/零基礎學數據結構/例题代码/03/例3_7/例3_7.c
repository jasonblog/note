/*包含头文件*/
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
/*类型定义*/
typedef char DataType;
typedef struct Node {
    DataType data;
    struct Node* prior;
    struct Node* next;
} DListNode, *DLinkList;
/*函数声明*/
DListNode* GetElem(DLinkList head, int i);
void PrintDList(DLinkList head);
int CreateDList(DLinkList head, int n);
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
int CreateDList(DLinkList head, int n)
/*创建双向循环链表*/
{
    DListNode* p, *q;
    int i;
    char e;
    q = head;

    for (i = 1; i <= n; i++) {
        printf("输入第%d个元素", i);
        e = getchar();
        p = (DListNode*)malloc(sizeof(DListNode));
        p->data = e;
        /*将新生成的结点插入到双向循环链表*/
        p->next = q->next;
        q->next = p;
        p->prior = q;
        head->prior = p;            /*这里要注意头结点的prior指向新插入的结点*/
        q = p;                      /*q始终指向最后一个结点*/
        getchar();
    }

    return 1;
}
int InsertDList(DLinkList head, int i, char e)
/*在双向循环链表的第i个位置插入元素e。插入成功返回1，否则返回0*/
{
    DListNode* p, *s;
    p = GetElem(head, i);           /*查找链表中第i个结点*/

    if (!p) {
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
DListNode* GetElem(DLinkList head, int i)
/*查找插入的位置，找到返回该结点的指针，否则返回NULL*/
{
    DListNode* p;
    int j;
    p = head->next;
    j = 1;

    while (p != head && j < i) {
        p = p->next;
        j++;
    }

    if (p == head || j > i) {       /*如果要位置不正确，返回NULL*/
        return NULL;
    }

    return p;
}
void main()
{
    DLinkList h;
    int n;
    int pos;
    char e;
    InitDList(&h);
    printf("输入元素个数：");
    scanf("%d", &n);
    getchar();
    CreateDList(h, n);
    printf("链表中的元素：");
    PrintDList(h);
    printf("请输入插入的元素及位置：");
    scanf("%c", &e);
    getchar();
    scanf("%d", &pos);
    InsertDList(h, pos, e);
    printf("插入元素后链表中的元素：");
    PrintDList(h);
}
void PrintDList(DLinkList head)
/*输出双向循环链表中的每一个元素*/
{
    DListNode* p;
    p = head->next;

    while (p != head) {
        printf("%c", p->data);
        p = p->next;
    }

    printf("\n");
}
