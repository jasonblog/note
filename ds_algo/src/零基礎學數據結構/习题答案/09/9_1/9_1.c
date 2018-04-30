/*包含头文件及宏定义*/
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
typedef char DataType;
#define MaxSize 100                 /*定义栈的最大容量*/

typedef struct Node {           /*二叉链表存储结构类型定义*/
    DataType data;              /*数据域*/
    struct Node* lchild;            /*指向左孩子结点*/
    struct Node* rchild;            /*指向右孩子结点*/
}* BiTree, BitNode;

/*函数的声明*/
void CreateBitTree(BiTree* T, char str[]);
void DestroyBitTree(BiTree* T);
void TreePrint(BiTree T, int nLayer);
void TreePrint(BiTree T, int level);
void main()
{
    BiTree T;
    int num, level;
    printf("根据括号嵌套(a(b(c,d),e(f(,g)))建立二叉树:\n");
    CreateBitTree(&T, "(a(b(c,d),e(f(,g)))");
    level = TreeDepth(T);
    printf("按树状打印二叉树：\n");
    TreePrint(T, level);
    num = LeafCount(T);
    printf("叶子结点个数=%2d\n", num);
    num = TreeCount(T);
    printf("结点总数=%2d\n", num);
    DestroyBitTree(&T);
}
int TreeCount(BiTree T)
/*统计结点的个数*/
{
    if (T == NULL) {
        return 0;
    }

    return 1 + TreeCount(T->lchild) + TreeCount(T->rchild);
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
int TreeDepth(BiTree T)
/*计算二叉树的深度*/
{
    if (T == NULL) {
        return 0;
    }

    return
        TreeDepth(T->lchild) > TreeDepth(T->rchild) ? 1 + TreeDepth(
            T->lchild) : 1 + TreeDepth(T->rchild);
}
void  CreateBitTree(BiTree* T, char str[])
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
void DestroyBitTree(BiTree* T)
/*销毁二叉树操作*/
{
    if (*T) {                       /*如果是非空二叉树*/
        if ((*T)->lchild) {
            DestroyBitTree(&((*T)->lchild));
        }

        if ((*T)->rchild) {
            DestroyBitTree(&((*T)->rchild));
        }

        free(*T);
        *T = NULL;
    }
}
