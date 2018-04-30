/*包含头文件*/
#include<stdlib.h>
#include<stdio.h>
#include<malloc.h>
typedef int KeyType;
typedef struct {    /*元素类型定义*/
    KeyType key;    /*关键字*/
    int hi;         /*冲突次数*/
} DataType;
typedef struct {    /*哈希表类型定义*/
    DataType* data;
    int tableSize;  /*哈希表的长度*/
    int curSize;    /*表中关键字个数*/
} HashTable;
void CreateHashTable(HashTable* H, int m, int p, int hash[], int n);
int SearchHash(HashTable H, KeyType k);
void DisplayHash(HashTable H, int m);
void HashASL(HashTable H, int m);
void CreateHashTable(HashTable* H, int m, int p, int hash[], int n)
/*构造一个空的哈希表，并处理冲突*/
{
    int i, sum, addr, di, k = 1;
    (*H).data = (DataType*)malloc(m * sizeof(DataType)); /*为哈希表分配存储空间*/

    if (!(*H).data) {
        exit(-1);
    }

    for (i = 0; i < m; i++) { /*初始化哈希表*/
        (*H).data[i].key = -1;
        (*H).data[i].hi = 0;
    }

    for (i = 0; i < n; i++) { /*求哈希函数地址并处理冲突*/
        sum = 0;        /*冲突的次数*/
        addr = hash[i] % p; /*利用除留余数法求哈希函数地址*/
        di = addr;

        if ((*H).data[addr].key == -1) { /*如果不冲突则将元素存储在表中*/
            (*H).data[addr].key = hash[i];
            (*H).data[addr].hi = 1;
        } else {                    /*用线性探测再散列法处理冲突*/
            do {
                di = (di + k) % m;
                sum += 1;
            } while ((*H).data[di].key != -1);

            (*H).data[di].key = hash[i];
            (*H).data[di].hi = sum + 1;
        }
    }

    (*H).curSize = n;       /*哈希表中关键字个数为n*/
    (*H).tableSize = m;     /*哈希表的长度*/
}
int SearchHash(HashTable H, KeyType k)
/*在哈希表H中查找关键字k的元素*/
{
    int d, d1, m;
    m = H.tableSize;
    d = d1 = k % m;         /*求k的哈希地址*/

    while (H.data[d].key != -1) {
        if (H.data[d].key == k) { /*如果是要查找的关键字k，则返回k的位置*/
            return d;
        } else {            /*继续往后查找*/
            d = (d + 1) % m;
        }

        if (d == d1) {      /*如果查找了哈希表中的所有位置，没有找到返回0*/
            return 0;
        }
    }

    return 0;               /*该位置不存在关键字k*/
}
void DisplayHash(HashTable H, int m)
/*输出哈希表*/
{
    int i;
    printf("哈希表地址：");

    for (i = 0; i < m; i++) {
        printf("%-5d", i);
    }

    printf("\n");
    printf("关键字key:  ");

    for (i = 0; i < m; i++) {
        printf("%-5d", H.data[i].key);
    }

    printf("\n");
    printf("冲突次数：  ");

    for (i = 0; i < m; i++) {
        printf("%-5d", H.data[i].hi);
    }

    printf("\n");

}
void HashASL(HashTable H, int m)
/*求哈希表的平均查找长度*/
{
    float average = 0;
    int i;

    for (i = 0; i < m; i++) {
        average = average + H.data[i].hi;
    }

    average = average / H.curSize;
    printf("平均查找长度ASL=%.2f", average);
    printf("\n");
}

void main()
{
    int hash[] = {23, 35, 12, 56, 123, 39, 342, 90};
    int m = 11, p = 11, n = 8, pos;
    KeyType k;
    HashTable H;
    CreateHashTable(&H, m, p, hash, n);
    DisplayHash(H, m);
    k = 123;
    pos = SearchHash(H, k);
    printf("关键字%d在哈希表中的位置为：%d\n", k, pos);
    HashASL(H, m);
}



