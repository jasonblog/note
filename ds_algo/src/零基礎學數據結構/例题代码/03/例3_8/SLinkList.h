void InitSList(SLinkList* L)
/*静态链表初始化。*/
{
    int i;

    for (i = 0; i < ListSize; i++) {
        (*L).list[i].cur = i + 1;
    }

    (*L).list[ListSize - 1].cur = 0;
    (*L).av = 1;
}
int AssignNode(SLinkList L)
/*从备用链表中取下一个结点空间，分配给要插入链表中的元素*/
{
    int i;
    i = L.av;
    L.av = L.list[i].cur;
    return i;
}
void FreeNode(SLinkList L, int pos)
/*使空闲结点成为备用链表中的结点*/
{
    L.list[pos].cur = L.av;
    L.av = pos;
}
void InsertSList(SLinkList* L, int i, DataType e)
/*插入操作*/
{
    int j, k, x;
    k = (*L).av;
    (*L).av = (*L).list[k].cur;
    (*L).list[k].data = e;
    j = (*L).list[0].cur;

    for (x = 1; x < i - 1; x++) {
        j = (*L).list[j].cur;
    }

    (*L).list[k].cur = (*L).list[j].cur;
    (*L).list[j].cur = k;
}
void DeleteSList(SLinkList* L, int i, DataType* e)
/*删除操作*/
{
    int j, k, x;
    j = (*L).list[0].cur;

    for (x = 1; x < i - 1; x++) {
        j = (*L).list[j].cur;
    }

    k = (*L).list[j].cur;
    (*L).list[j].cur = (*L).list[k].cur;
    (*L).list[k].cur = (*L).av;
    *e = (*L).list[k].data;
    (*L).av = k;
}
