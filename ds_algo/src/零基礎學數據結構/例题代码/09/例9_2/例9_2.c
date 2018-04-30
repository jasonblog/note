

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
void LevelPrint(BiTree T);                  /*按层次输出二叉树的结点*/
void TreePrint(BiTree T, int nLayer);       /*按树状打印二叉树*/
void main()
{
    BiTree T, root;
    printf("根据括号嵌套(a(b(c,d),e(f(,g),h(i)))建立二叉树:\n");
    CreateBitTree2(&T, "(a(b(c,d),e(f(,g),h(i)))");
    printf("按层次输出二叉树的序列：\n");
    LevelPrint(T);
    printf("\n");
    printf("按树状打印二叉树：\n");
    TreePrint(T, 1);
    printf("根据括号嵌套(A(B(D(,H),E(,I)),C(F,G)))建立二叉树:\n");
    CreateBitTree2(&root, "(A(B(D(,H),E(,I)),C(F,G)))");
    printf("按层次输出二叉树的序列：\n");
    LevelPrint(root);
    printf("\n");
    printf("按树状打印二叉树：\n");
    TreePrint(root, 1);
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


void PreOrderTraverse(BiTree T)
/*先序遍历二叉树的递归实现*/
{
    if (T) {                            /*如果二叉树不为空*/
        printf("%2c", T->data);             /*访问根结点*/
        PreOrderTraverse(T->lchild);        /*先序遍历左子树*/
        PreOrderTraverse(T->rchild);        /*先序遍历右子树*/
    }
}
void InOrderTraverse(BiTree T)
/*中序遍历二叉树的递归实现*/
{
    if (T) {                            /*如果二叉树不为空*/
        InOrderTraverse(T->lchild);         /*中序遍历左子树*/
        printf("%2c", T->data);             /*访问根结点*/
        InOrderTraverse(T->rchild);         /*中序遍历右子树*/
    }
}
void PostOrderTraverse(BiTree T)
/*后序遍历二叉树的递归实现*/
{
    if (T) {                            /*如果二叉树不为空*/
        PostOrderTraverse(T->lchild);       /*后序遍历左子树*/
        PostOrderTraverse(T->rchild);       /*后序遍历右子树*/
        printf("%2c", T->data);             /*访问根结点*/
    }
}


