/*包含头文件及宏定义*/
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
typedef char DataType;
#define MaxSize 100                 /*定义栈的最大容量*/

typedef struct Node {           /*二叉链表存储结构类型定义*/
    DataType data;              /*数据域*/
    struct Node* lchild;        /*指向左孩子结点*/
    struct Node* rchild;        /*指向右孩子结点*/
}* BiTree, BitNode;
typedef struct {                /*栈类型定义*/
    BiTree p;
    int flag;
} Stack[MaxSize];
/*函数的声明*/
void CreateBitTree(BiTree* T, char str[]);
void DestroyBitTree(BiTree* T);
void TreePrint(BiTree T, int level);
BiTree Point(BiTree T, DataType e);
void main()
{
    BiTree T, p;
    int level;
    printf("根据括号嵌套(a(b(c,d),e(f,g)))建立二叉树:\n");
    CreateBitTree(&T, "(a(b(c,d(h,i)),e(f,g)))");
    level = TreeDepth(T);
    printf("按树状打印二叉树：\n");
    TreePrint(T, level);
    p = Point(T, 'h');
    Ancestor(T, p);
    DestroyBitTree(&T);
}
int Ancestor(BiTree T, BitNode* p)
/*输出结点*p的所有祖先结点*/
{
    Stack s;
    BitNode* r = T;
    int top = 0, i;

    while (1) {
        while (r != NULL &&
               r != p) { /*如果不是空二叉树且当前结点不是结点*p，则将当前结点指针和左孩子结点入栈*/
            top++;
            s[top].p = r;
            s[top].flag = 0;
            r = r->lchild;
        }

        if (r != NULL && r == p) { /*输出结点*p的所有祖先结点*/
            printf("%c的祖先是", p->data);

            for (i = 1; i < top; i++) {
                printf("%2c,", s[i].p->data);
            }

            printf("%2c\n", s[i].p->data);
            break;
        } else { /*如果当前结点不是*p，则依次退栈，并将当前访问结点记作已经访问，将结点的右孩子结点作为当前结点*/
            while (top > 0 && s[top].flag == 1) {
                top--;
            }

            if (top > 0) {
                s[top].flag = 1;
            }

            r = s[top].p->rchild;
        }
    }
}
BiTree Point(BiTree T, DataType e)
/*查找元素值为e的结点的指针*/
{
    BiTree Q[MaxSize];          /*定义一个队列，用于存放二叉树中结点的指针*/
    int front = 0, rear = 0;        /*初始化队列*/
    BitNode* p;

    if (T) {                    /*如果二叉树非空*/
        Q[rear] = T;
        rear++;

        while (front != rear) { /*如果队列非空*/
            p = Q[front];           /*取出队头指针*/
            front++;            /*将队头指针出队*/

            if (p->data == e) {
                return p;
            }

            if (p->lchild) {        /*如果左孩子结点存在，将左孩子指针入队*/
                Q[rear] = p->lchild; /*左孩子结点的指针入队*/
                rear++;
            }

            if (p->rchild) {        /*如果右孩子结点存在，将右孩子指针入队*/
                Q[rear] = p->rchild; /*右孩子结点的指针入队*/
                rear++;
            }
        }
    }

    return NULL;
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