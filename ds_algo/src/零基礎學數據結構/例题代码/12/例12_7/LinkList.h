void InitList(LinkList* head)
/*将单链表初始化为空。动态生成一个头结点，并将头结点的指针域置为空。*/
{
    if ((*head = (LinkList)malloc(sizeof(ListNode))) ==
        NULL) { /*为头结点分配一个存储空间*/
        exit(-1);
    }

    (*head)->next = NULL;           /*将单链表的头结点指针域置为空*/
}
int ListEmpty(LinkList head)
/*判断单链表是否为空，就是通过判断头结点的指针域是否为空*/
{
    if (head->next == NULL) {       /*判断单链表头结点的指针域是否为空*/
        return 1;    /*当单链表为空时，返回1；否则返回0*/
    } else {
        return 0;
    }
}
ListNode* Get(LinkList head, int i)
/*查找单链表中第i个结点。查找成功返回该结点的指针表示成功；否则返回NULL表示失败。*/
{
    ListNode* p;
    int j;

    if (ListEmpty(head)) { /*在查找第i个元素之前，判断链表是否为空*/
        return NULL;
    }

    if (i < 1) {        /*在查找第i个元素之前，判断该序号是否合法*/
        return NULL;
    }

    j = 0;
    p = head;

    while (p->next != NULL && j < i) {
        p = p->next;
        j++;
    }

    if (j == i) {
        return p;    /*找到第i个结点，返回指针p*/
    } else {
        return NULL   ;    /*如果没有找到第i个元素，返回NULL*/
    }
}
ListNode* LocateElem(LinkList head, DataType e)
/*查找线性表中元素值为e的元素，查找成功将对应元素的结点指针返回，否则返回NULL表示失败。*/
{
    ListNode* p;
    p = head->next;     /*指针p指向第一个结点*/

    while (p) {
        if (p->data != e) { /*找到与e相等的元素，返回该序号*/
            p = p->next;
        } else {
            break;
        }
    }

    return p;
}
int LocatePos(LinkList head, DataType e)
/*查找线性表中元素值为e的元素，查找成功将对应元素的序号返回，否则返回0表示失败。*/
{
    ListNode* p;
    int i;

    if (ListEmpty(head)) { /*在查找第i个元素之前，判断链表是否为空*/
        return 0;
    }

    p = head->next;     /*指针p指向第一个结点*/
    i = 1;

    while (p) {
        if (p->data == e) { /*找到与e相等的元素，返回该序号*/
            return i;
        } else {
            p = p->next;
            i++;
        }
    }

    if (!p) {           /*如果没有找到与e相等的元素，返回0，表示失败*/
        return 0;
    }
}

int InsertList(LinkList head, int i, DataType e)
/*在单链表中第i个位置插入一个结点，结点的元素值为e。插入成功返回1，失败返回0*/
{
    ListNode* p,
              *pre;  /*定义指向第i个元素的前驱结点指针pre，指针p指向新生成的结点*/
    int j;
    pre = head;         /*指针p指向头结点*/
    j = 0;

    while (pre->next != NULL &&
           j < i - 1) { /*找到第i－1个结点，即第i个结点的前驱结点*/
        pre = pre->next;
        j++;
    }

    if (!pre) {                 /*如果没找到，说明插入位置错误*/
        printf("插入位置错");
        return 0;
    }

    /*新生成一个结点，并将e赋值给该结点的数据域*/
    if ((p = (ListNode*)malloc(sizeof(ListNode))) == NULL) {
        exit(-1);
    }

    p->data = e;
    /*插入结点操作*/
    p->next = pre->next;
    pre->next = p;
    return 1;
}
int DeleteList(LinkList head, int i, DataType* e)
/*删除单链表中的第i个位置的结点。删除成功返回1，失败返回0*/
{
    ListNode* pre, *p;
    int j;
    pre = head;
    j = 0;

    while (pre->next != NULL && pre->next->next != NULL &&
           j < i - 1) { /*判断是否找到前驱结点*/
        pre = pre->next;
        j++;
    }

    if (j != i - 1) {       /*如果没找到要删除的结点位置，说明删除位置错误*/
        printf("删除位置错误");
        return 0;
    }

    /*指针p指向单链表中的第i个结点，并将该结点的数据域值赋值给e*/
    p = pre->next;
    *e = p->data;
    /*将前驱结点的指针域指向要删除结点的下一个结点，也就是将p指向的结点与单链表断开*/
    pre->next = p->next;
    free(p);                /*释放p指向的结点*/
    return 1;
}
int ListLength(LinkList head)
{
    ListNode* p;
    int count = 0;
    p = head;

    while (p->next != NULL) {
        p = p->next;
        count++;
    }

    return count;
}
void DestroyList(LinkList head)
{
    ListNode* p, *q;
    p = head;

    while (p != NULL) {
        q = p;
        p = p->next;
        free(q);
    }
}
