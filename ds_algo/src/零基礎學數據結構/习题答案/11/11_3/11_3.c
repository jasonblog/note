#include<stdio.h>
#include<malloc.h>
#define MaxSize 20
typedef struct Node {   /*哈希表结点类型定义*/
    int key;
    struct Node* next;
} HashNode;

void CreateHash(int a[], int n, int m, HashNode hash[])
/*使用链地址法创建哈希表*/
{
    int i, j;
    HashNode* p;

    for (i = 0; i < m; i++) { /*初始化时，将表头的next域置为空*/
        hash[i].next = NULL;
    }

    for (i = 0; i < n; i++) { /*计算元素的位置，并插入到对应链表中*/
        p = (HashNode*)malloc(sizeof(HashNode));
        p->key = a[i];
        j = (3 * a[i]) % m;
        p->next = hash[j].next;
        hash[j].next = p;
    }
}
void DispHash(HashNode hash[], int n, int m)
/*输出哈希表*/
{
    int asl = 0, asl1;
    HashNode* p;
    int i, j;
    printf("哈希表为:\n");

    for (i = 0; i < m; i++) {
        printf(" %2d: ", i);
        p = hash[i].next;
        asl1 = j = 0;

        while (p != NULL) { /*输出哈希表中的元素并计算查找成功时的平均查找长度*/
            j++;
            asl1 = asl1 + j;
            printf("%4d", p->key);
            p = p->next;
        }

        asl = asl + asl1;
        printf("\n");
    }

    printf("查找成功时的平均查找长度为:%2.3f\n", asl * 1.0 / n);
}

void main()
{
    int a[] = {34, 5, 45, 67, 21, 38, 92, 69}, n = 8, m = 11;
    HashNode hash[MaxSize];
    CreateHash(a, n, m, hash);
    DispHash(hash, n, m);
}