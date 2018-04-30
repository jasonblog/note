/*包含头文件*/
#include"stdio.h"
#include"stdlib.h"
#include"string.h"
#include<conio.h>
#define MaxSize 100
/*二叉树类型定义*/
typedef struct Node {
    char data;
    struct Node* lchild, *rchild;
} BitNode, *BiTree;

/*函数声明*/
void CreateBiTree1(BiTree* T, char* pre, char* in, int len);
void CreateBiTree2(BiTree* T, char* in, char* post, int len);
void Visit(BiTree T, BiTree pre, char e, int i);
void PrintLevel(BiTree T);
void PrintTLR(BiTree T);
void PrintLRT(BiTree T);
void PrintLevel(BiTree T)
/*按层次输出二叉树的结点*/
{
    BiTree Queue[MaxSize];
    int front, rear;

    if (T == NULL) {
        return;
    }

    front = -1;                         /*初始化化队列*/
    rear = 0;
    Queue[rear] = T;

    while (front != rear) {             /*如果队列不空*/
        front++;                        /*将队头元素出队*/
        printf("%4c", Queue[front]->data); /*输出队头元素*/

        if (Queue[front]->lchild !=
            NULL) { /*如果队头元素的左孩子结点不为空，则将左孩子入队*/
            rear++;
            Queue[rear] = Queue[front]->lchild;
        }

        if (Queue[front]->rchild !=
            NULL) { /*如果队头元素的右孩子结点不为空，则将右孩子入队*/
            rear++;
            Queue[rear] = Queue[front]->rchild;
        }
    }
}
void PrintTLR(BiTree T)
/*先序输出二叉树的结点*/
{
    if (T != NULL) {
        printf("%4c ", T->data); /*输出根结点*/
        PrintTLR(T->lchild);    /*先序遍历左子树*/
        PrintTLR(T->rchild);    /*先序遍历右子树*/
    }
}
void PrintLRT(BiTree T)
/*后序输出二叉树的结点*/
{
    if (T != NULL) {
        PrintLRT(T->lchild);    /*先序遍历左子树*/
        PrintLRT(T->rchild);    /*先序遍历右子树*/
        printf("%4c", T->data); /*输出根结点*/
    }
}

void Visit(BiTree T, BiTree pre, char e, int i)
/*访问结点e*/
{
    if (T == NULL && pre == NULL) {
        printf("\n对不起！你还没有建立二叉树，先建立再进行访问！\n");
        return;
    }

    if (T == NULL) {
        return;
    } else if (T->data == e) {      /*如果找到结点e，则输出结点的双亲结点*/
        if (pre != NULL) {
            printf("%2c的双亲结点是是:%2c\n", e, pre->data);
            printf("%2c结点在%2d层上\n", e, i);
        } else {
            printf("%2c位于第1层，无双亲结点！\n", e);
        }
    } else {
        Visit(T->lchild, T, e, i + 1); /*遍历左子树*/
        Visit(T->rchild, T, e, i + 1); /*遍历右子树*/
    }
}

void CreateBiTree1(BiTree* T, char* pre, char* in, int len)
/*由先序序列和中序序列构造二叉树*/
{
    int k;
    char* temp;

    if (len <= 0) {
        *T = NULL;
        return;
    }

    *T = (BitNode*)malloc(sizeof(BitNode)); /*生成根结点*/
    (*T)->data = *pre;

    for (temp = in; temp < in + len; temp++) /*在中序序列in中找到根结点所在的位置*/
        if (*pre == *temp) {
            break;
        }

    k = temp - in;                      /*左子树的长度*/
    CreateBiTree1(&((*T)->lchild), pre + 1, in, k); /*建立左子树*/
    CreateBiTree1(&((*T)->rchild), pre + 1 + k, temp + 1,
                  len - 1 - k); /*建立右子树*/
}
void CreateBiTree2(BiTree* T, char* in, char* post, int len)
/*由中序序列和后序序列构造二叉树*/
{

    int k;
    char* temp;

    if (len <= 0) {
        *T = NULL;
        return;
    }

    for (temp = in; temp < in + len; temp++) /*在中序序列in中找到根结点所在的位置*/
        if (*(post + len - 1) == *temp) {
            k = temp - in;          /*左子树的长度*/
            (*T) = (BitNode*)malloc(sizeof(BitNode));
            (*T)->data = *temp;
            break;
        }

    CreateBiTree2(&((*T)->lchild), in, post, k);            /*建立左子树*/
    CreateBiTree2(&((*T)->rchild), in + k + 1, post + k,
                  len - k - 1); /*建立右子树*/
}


void main()
{
    BiTree T, ptr = NULL;
    char ch;
    int len;
    char pre[MaxSize], in[MaxSize], post[MaxSize];
    T = NULL;
    /*由中序序列和后序序列构造二叉树*/
    printf("由先序序列和中序序列构造二叉树：\n");
    printf("请你输入先序的字符串序列：");
    gets(pre);
    printf("请你输入中序的字符串序列：");
    gets(in);
    len = strlen(pre);
    CreateBiTree1(&T, pre, in, len);
    /*后序和层次输出二叉树的结点*/
    printf("你建立的二叉树后序遍历结果是：\n");
    PrintLRT(T);
    printf("\n你建立的二叉树层次遍历结果是：\n");
    PrintLevel(T);
    printf("\n");
    printf("请你输入你要访问的结点：");
    ch = getchar();
    getchar();
    Visit(T, ptr, ch, 1);
    /*由中序序列和后序序列构造二叉树*/
    printf("由先序序列和中序序列构造二叉树：\n");
    printf("请你输入中序的字符串序列：");
    gets(in);
    printf("请你输入后序的字符串序列：");
    gets(post);
    len = strlen(post);
    CreateBiTree2(&T, in, post, len);
    /*先序和层次输出二叉树的结点*/
    printf("\n你建立的二叉树先序遍历结果是：\n");
    PrintTLR(T);
    printf("\n你建立的二叉树层次遍历结果是：\n");
    PrintLevel(T);
    printf("\n");
    printf("请你输入你要访问的结点：");
    ch = getchar();
    getchar();
    Visit(T, ptr, ch, 1);
}
