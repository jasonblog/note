
void InitList(SeqList* L)
/*将线性表初始化为空的线性表只需要把线性表的长度length置为0*/
{
    L->length = 0;  /*把线性表的长度置为0*/
}
int ListEmpty(SeqList L)
/*判断线性表是否为空，线性表为空返回1，否则返回0*/
{
    if (L.length == 0) {    /*判断线性表的长度是否为9*/
        return 1;    /*当线性表为空时，返回1；否则返回0*/
    } else {
        return 0;
    }
}
int GetElem(SeqList L, int i, DataType* e)
/*查找线性表中第i个元素。查找成功将该值返回给e，并返回1表示成功；否则返回-1表示失败。*/
{
    if (i < 1 || i > L.length) { /*在查找第i个元素之前，判断该序号是否合法*/
        return -1;
    }

    *e = L.list[i - 1];     /*将第i个元素的值赋值给e*/
    return 1;
}
int LocateElem(SeqList L, DataType e)
/*查找线性表中元素值为e的元素，查找成功将对应元素的序号返回，否则返回0表示失败。*/
{
    int i;

    for (i = 0; i < L.length; i++) /*从第一个元素开始比较*/
        if (L.list[i] == e) {
            return i + 1;
        }

    return 0;
}
int InsertList(SeqList* L, int i, DataType e)
/*在顺序表的第i个位置插入元素e，插入成功返回1，如果插入位置不合法返回-1，顺序表满返回0*/
{
    int j;

    if (i < 1 || i > L->length + 1) {   /*在插入元素前，判断插入位置是否合法*/
        printf("插入位置i不合法！\n");
        return -1;
    } else if (L->length >=
               ListSize) { /*在插入元素前，判断顺序表是否已经满，不能插入元素*/
        printf("顺序表已满，不能插入元素。\n");
        return 0;
    } else {
        for (j = L->length; j >= i; j--) {  /*将第i个位置以后的元素依次后移*/
            L->list[j] = L->list[j - 1];
        }

        L->list[i - 1] = e;     /*插入元素到第i个位置*/
        L->length = L->length + 1;  /*将顺序表长增1*/
        return 1;
    }
}
int DeleteList(SeqList* L, int i, DataType* e)
{
    int j;

    if (L->length <= 0) {
        printf("顺序表已空不能进行删除!\n");
        return 0;
    } else if (i < 1 || i > L->length) {
        printf("删除位置不合适!\n");
        return -1;
    } else {
        *e = L->list[i - 1];

        for (j = i; j <= L->length - 1; j++) {
            L->list[j - 1] = L->list[j];
        }

        L->length = L->length - 1;
        return 1;
    }
}
int ListLength(SeqList L)
{
    return L.length;
}
void ClearList(SeqList* L)
{
    L->length = 0;
}
