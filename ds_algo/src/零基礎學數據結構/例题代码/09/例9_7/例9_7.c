/*包含头文件*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<malloc.h>
/*树的孩子兄弟链表定义*/
typedef int DataType;
typedef struct CSNode {
    struct CSNode* firstchild, *nextsibling;
    DataType data;
} CSNode, *CSTree;
void InitCSTree(CSTree* T);
void DestroyCSTree(CSTree* T);
void CreateCSTree(CSTree* T, DataType* e, int* index);
int DepCSTree(CSTree T);
int PreTraverseCSTree(CSTree T, void(*visit)(DataType* e));
int PostTraverseCSTree(CSTree T, void(*visit)(DataType* e));
void DisplayCSTree(DataType* e);
void InitCSTree(CSTree* T)
/*树的初始化*/
{
    *T = 0;
}
void DestroyCSTree(CSTree* T)
/*树的销毁操作*/
{
    CSTree p = *T;

    if (p) {
        DestroyCSTree(&(p->firstchild));/*销毁第一个孩子结点*/
        DestroyCSTree(&(p->nextsibling));/*销毁兄弟结点*/
        free(p);
        *T = 0;
    }
}
void CreateCSTree(CSTree* T, DataType* e, int* index)
/*创建树操作*/
{
    if (e[*index] == 0) {
        *T = 0;
        (*index)++;
    } else {
        *T = (CSTree)malloc(sizeof(CSNode)); /*生成结点*/
        (*T)->data = e[*index];
        (*index)++;
        CreateCSTree(&((*T)->firstchild), e, index); /*创建第一个孩子结点*/
        CreateCSTree(&((*T)->nextsibling), e, index); /*创建兄弟结点*/
        return;
    }
}

int DepCSTree(CSTree T)
/*求树的深度*/
{
    CSNode* p;
    int k, d = 0;

    if (T == NULL) {        /*如果是空树，返回0*/
        return 0;
    }

    p = T->firstchild;      /*p指向树的第一孩子结点*/

    while (p != NULL) {
        k = DepCSTree(p);   /*求子p树的深度*/

        if (d < k) {
            d = k;    /*d保存树的最大深度*/
        }

        p = p->nextsibling; /*进入p的下一个结点*/
    }

    return d + 1;

}

int PreTraverseCSTree(CSTree T, void(*visit)(DataType* e))
/*树的先根遍历*/
{
    if (T) {
        (*visit)(&T->data);
        PreTraverseCSTree(T->firstchild, visit);
        PreTraverseCSTree(T->nextsibling, visit);
    }
}
int PostTraverseCSTree(CSTree T, void(*visit)(DataType* e))
/*树的后根遍历*/
{
    if (T) {
        PostTraverseCSTree(T->firstchild, visit);
        (*visit)(&T->data);
        PostTraverseCSTree(T->nextsibling, visit);
    }
}

void DisplayCSTree(DataType* e)
/*输出树的结点*/
{
    printf("%2c", *e);
}


main()
{
    int test[] = {'A', 'B', 'E', 0, 'F', 'H', 0, 'I', 0, 'J', 0, 0, 0, 'C', 0, 'D', 'G', 0, 0, 0, 0};
    int h = 0;
    CSTree T;
    InitCSTree(&T);
    CreateCSTree(&T, test, &h);
    printf("树的先根遍历结果是:\n");
    PreTraverseCSTree(T, DisplayCSTree);
    printf("\n");
    printf("树的后根遍历结果是:\n");
    PostTraverseCSTree(T, DisplayCSTree);
    printf("\n");
    printf("树的深度是:%2d", DepCSTree(T));
    printf("\n");
    DestroyCSTree(&T);

}

