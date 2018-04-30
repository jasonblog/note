/*包含头文件及宏定义*/
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
typedef char DataType;
#define MaxSize 100                 /*定义栈的最大容量*/
#include"LinkBiTree.h"              /*包含二叉树的二叉链表的基本操作*/
/*函数的声明*/
void CreateBitTree2(BiTree* T,
                    char str[]); /*利用括号嵌套的字符串建立二叉树的函数声明*/
void LevelPrint(BiTree T);
void TreePrint(BiTree T, int nLayer);
int LeafCount(BiTree T);
int NotLeafCount(BiTree T);
int BitTreeDepth(BiTree T);
void main()
{
    BiTree T, root;
    int num, depth;
    printf("根据括号嵌套(a(b(c,d),e(f(,g),h(i)))建立二叉树:\n");
    CreateBitTree2(&root, "(a(b(c,d),e(f(,g),h(i)))");
    num = LeafCount(root);
    printf("叶子结点个数=%2d\n", num);
    num = NotLeafCount(root);
    printf("非叶子结点个数=%2d\n", num);
    depth = BitTreeDepth(root);
    printf("二叉树的深度=%2d\n", depth);
    printf("根据括号嵌套(A(B(D(,H(J)),E(,I(K,L))),C(F,G)))建立二叉树:\n");
    CreateBitTree2(&T, "(A(B(D(,H(J)),E(,I(K,L))),C(F,G)))");
    num = LeafCount(T);
    printf("叶子结点个数=%2d\n", num);
    num = NotLeafCount(T);
    printf("非叶子结点个数=%2d\n", num);
    depth = BitTreeDepth(T);
    printf("二叉树的深度=%2d\n", depth);
    DestroyBitTree(&T);
    DestroyBitTree(&root);
}

void LevelPrint(BiTree T)
/*按层次打印二叉树中的结点*/
{
    BiTree queue[MaxSize];                      /*定义一个队列，用于存放结点的指针*/
    BitNode* p;
    int front, rear;                            /*定义队列的队头指针和队尾指针*/
    front = rear = -1;                          /*队列初始化为空*/
    rear++;                                     /*队尾指针加1*/
    queue[rear] = T;                            /*将根结点指针入队*/

    while (front != rear) {                     /*如果队列不为空*/
        front = (front + 1) % MaxSize;
        p = queue[front];                       /*取出队头元素*/
        printf("%c ", p->data);                 /*输出根结点*/

        if (p->lchild !=
            NULL) {                /*如果左孩子不为空，将左孩子结点指针入队*/
            rear = (rear + 1) % MaxSize;
            queue[rear] = p->lchild;
        }

        if (p->rchild !=
            NULL) {                /*如果右孩子不为空，将右孩子结点指针入队*/
            rear = (rear + 1) % MaxSize;
            queue[rear] = p->rchild;
        }
    }
}
void TreePrint(BiTree T, int level)
/*按树状打印的二叉树*/
{
    int i;

    if (T == NULL) {                            /*如果指针为空，返回上一层*/
        return;
    }

    TreePrint(T->rchild, level + 1);            /*打印右子树，并将层次加1*/

    for (i = 0; i < level; i++) {               /*按照递归的层次打印空格*/
        printf("   ");
    }

    printf("%c\n", T->data);                    /*输出根结点*/
    TreePrint(T->lchild, level + 1);            /*打印左子树，并将层次加1*/
}

void  CreateBitTree2(BiTree* T, char str[])
/*利用括号嵌套的字符串建立二叉链表*/
{
    char ch;
    BiTree stack[MaxSize];          /*定义栈，用于存放指向二叉树中结点的指针*/
    int top = -1;                   /*初始化栈顶指针*/
    int flag, k;
    BitNode* p;
    *T = NULL, k = 0;
    ch = str[k];

    while (ch != '\0') {            /*如果字符串没有结束*/
        switch (ch) {
        case '(':
            stack[++top] = p;
            flag = 1;
            break;

        case ')':
            top--;
            break;

        case ',':
            flag = 2;
            break;

        default:
            p = (BiTree)malloc(sizeof(BitNode));
            p->data = ch;
            p->lchild = NULL;
            p->rchild = NULL;

            if (*T == NULL) {   /*如果是第一个结点，表示是根结点*/
                *T = p;
            } else {
                switch (flag) {
                case 1:
                    stack[top]->lchild = p;
                    break;

                case 2:
                    stack[top]->rchild = p;
                    break;
                }
            }
        }

        ch = str[++k];
    }

}

int LeafCount(BiTree T)
/*统计二叉树中叶子结点数目*/
{
    if (!T) {                               /*如果是空二叉树，返回0*/
        return 0;
    } else if (!T->lchild && !T->rchild) {    /*如果左子树和右子树都为空，返回1*/
        return 1;
    } else {
        return LeafCount(T->lchild) + LeafCount(
                   T->rchild);    /*将左子树叶子结点个数与右子树叶子结点个数相加*/
    }
}
int NotLeafCount(BiTree T)
/*统计二叉树中非叶子结点数目*/
{
    if (!T) {                               /*如果是空二叉树，返回0*/
        return 0;
    } else if (!T->lchild && !T->rchild) {  /*如果是叶子结点，返回0*/
        return 0;
    } else {
        return NotLeafCount(T->lchild) + NotLeafCount(T->rchild) +
               1;    /*左右子树的非叶子结点数目加上根结点的个数*/
    }
}

int BitTreeDepth(BiTree T)
/*计算二叉树的深度*/
{
    if (T == NULL) {
        return 0;
    }

    return
        BitTreeDepth(T->lchild) > BitTreeDepth(T->rchild) ? 1 + BitTreeDepth(
            T->lchild) : 1 + BitTreeDepth(T->rchild);
}

int Depth(BiTree T)
{
    if (T == NULL) {
        return 0;
    }

    if (T->lchild == NULL && T->rchild == NULL) {
        return 1;
    }

    return
        Depth(T->lchild) > Depth(T->rchild) ? 1 + Depth(T->lchild) : 1 + Depth(
            T->rchild);
}
