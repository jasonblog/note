#include<stdio.h>
#include<malloc.h>
#include<math.h>
#define MaxNumKey 6 /*关键字项数的最大值*/
#define Radix 10    /*关键字基数，此时是十进制整数的基数*/
#define MaxSize 1000
#define N 6
typedef int KeyType; /*定义关键字类型为字符型*/
typedef struct {
    KeyType key[MaxNumKey]; /*关键字*/
    int next;
} SListCell; /*静态链表的结点类型*/

typedef struct {
    SListCell data[MaxSize];        /*存储元素，data[0]为头结点*/
    int keynum;                     /*每个元素的当前关键字个数*/
    int length;                     /*静态链表的当前长度*/
} SList;    /*静态链表类型*/
typedef int addr[Radix]; /*指针数组类型*/

typedef struct {
    KeyType key; /* 关键字项 */
} DataType;

void PrintList(SList L);
void PrintList2(SList L);
void InitList(SList* L, DataType d[], int n);
int trans(char c);
void Distribute(SListCell data[], int i, addr f, addr r);
void Collect(SListCell data[], addr f, addr r);
void RadixSort(SList* L);
void InitList(SList* L, DataType a[], int n)
/* 初始化静态链表L(把数组D中的数据存于L中) */
{
    char ch[MaxNumKey], ch2[MaxNumKey];
    int i, j, max = a[0].key;

    for (i = 1; i < n; i++)             /*将最大的关键字存入max*/
        if (max < a[i].key) {
            max = a[i].key;
        }

    (*L).keynum = (int)(log10(max)) + 1; /*求关键字的个数*/
    (*L).length = n;                    /*待排序个数*/

    for (i = 1; i <= n; i++) {
        itoa(a[i - 1].key, ch, 10);             /*将整型转化为字符,并存入ch*/

        for (j = strlen(ch); j < (*L).keynum;
             j++) { /*如果ch的长度<max的位数,则在ch前补'0'*/
            strcpy(ch2, "0");
            strcat(ch2, ch);
            strcpy(ch, ch2);
        }

        for (j = 0; j < (*L).keynum; j++) {     /*将每个关键字的各个位数存入key*/
            (*L).data[i].key[j] = ch[(*L).keynum - 1 - j];
        }
    }

    for (i = 0; i < (*L).length; ++i) {     /*初始化静态链表*/
        (*L).data[i].next = i + 1;
    }

    (*L).data[(*L).length].next = 0;
}

int trans(char c)
/*将字符c转化为对应的整数*/
{
    return c - '0';
}
void Distribute(SListCell data[], int i, addr f, addr r)
/*为data中的第i个关键字key[i]建立Radix个子表，使同一子表中元素的key[i]相同*/
/*f[0..Radix-1]和r[0..Radix-1]分别指向各个子表中第一个和最后一个元素*/
{
    int j, p;

    for (j = 0; j < Radix; j++) {       /*将各个子表初始化为空表*/
        f[j] = 0;
    }

    for (p = data[0].next; p; p = data[p].next) {
        j = trans(data[p].key[i]);      /*将对应的关键字字符转化为整数类型*/

        if (!f[j]) {                    /*f[j]是空表，则f[j]指示第一个元素*/
            f[j] = p;
        } else {
            data[r[j]].next = p;
        }

        r[j] = p;                       /*将p所指的结点插入第j个子表中*/
    }
}



void Collect(SListCell data[], addr f, addr r)
/*按key[i]将f[0..Radix-1]所指各子表依次链接成一个静态链表*/
{
    int j, t;

    for (j = 0; !f[j]; j++); /*找第一个非空子表，succ为求后继函数*/

    data[0].next = f[j];
    t = r[j];           /*r[0].next指向第一个非空子表中第一个结点*/

    while (j < Radix - 1) {
        for (j = j + 1; j < Radix - 1 && !f[j]; j++); /*找下一个非空子表*/

        if (f[j]) {                         /*将非空链表连接在一起*/
            data[t].next = f[j];
            t = r[j];
        }
    }

    data[t].next = 0;   /*t指向最后一个非空子表中的最后一个结点*/
}

void RadixSort(SList* L)
/*对L进行基数排序，使得L成为按关键字非递减的静态链表，L.r[0]为头结点*/
{
    int i;
    addr f, r;

    for (i = 0; i < (*L).keynum; i++) { /*由低位到高位依次对各关键字进行分配和收集*/
        Distribute((*L).data, i, f, r); /*第i趟分配*/
        Collect((*L).data, f, r);       /*第i趟收集*/
        printf("第%d趟收集后:", i + 1);
        PrintList2(*L);
    }
}



void main()
{
    DataType d[N] = {268, 126, 63, 730, 587, 184};
    SList L;
    int* adr;
    InitList(&L, d, N);
    printf("待排序元素个数是%d个，关键字个数为%d个\n", L.length, L.keynum);
    printf("排序前的元素:\n");
    PrintList2(L);
    printf("排序前的元素的存放位置:\n");
    PrintList(L);
    RadixSort(&L);
    printf("排序后元素的存放位置:\n");
    PrintList(L);
}
void PrintList(SList L)
/*按数组序号形式输出静态链表*/
{
    int i, j;
    printf("序号 关键字 地址\n");

    for (i = 1; i <= L.length; i++) {
        printf("%2d    ", i);

        for (j = L.keynum - 1; j >= 0; j--) {
            printf("%c", L.data[i].key[j]);
        }

        printf("    %d\n", L.data[i].next);
    }
}
void PrintList2(SList L)
/*按链表形式输出静态链表*/
{
    int i = L.data[0].next, j;

    while (i) {
        for (j = L.keynum - 1; j >= 0; j--) {
            printf("%c", L.data[i].key[j]);
        }

        printf(" ");
        i = L.data[i].next;
    }

    printf("\n");
}
