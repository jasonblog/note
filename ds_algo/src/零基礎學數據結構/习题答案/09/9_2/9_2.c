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
void TreePrint(BiTree T, int level);
void main()
{
    BiTree T;
    int level, flag;
    printf("根据括号嵌套(a(b(c,d),e(f,g)))建立二叉树:\n");
    CreateBitTree(&T, "(a(b(c,d),e(f,g)))");
    level = TreeDepth(T);
    printf("按树状打印二叉树：\n");
    TreePrint(T, level);
    flag = CompleteTree(T);

    if (flag == 1) {
        printf("该二叉树是完全二叉树.\n");
    }

    DestroyBitTree(&T);
}
int CompleteTree(BiTree T)
/*判断二叉树是否是完全二叉树*/
{
    BiTree queue[MaxSize], p;
    int front = 0, rear = 0, flag = 1, flag1 = 1;

    if (T == NULL) {            /*如果是空二叉树，返回1*/
        return 1;
    } else {
        queue[++rear] = T;      /*将根结点入队*/

        while (front != rear) {
            p = queue[++front];

            if (p->lchild ==
                NULL) { /*如果*p的左子树为空，右子树不为空，则说明不是完全二叉树*/
                flag1 = 0;

                if (p->rchild != NULL) {
                    flag = 0;
                }
            } else {            /*如果*p的左子树不为空，将左子树入队。*/
                flag = flag1;
                queue[++rear] = p->lchild;

                if (p->rchild == NULL) { /*如果*p的右子树为空，则说明不是完全二叉树*/
                    flag1 = 0;
                } else {            /*如果*p的右子树不为空，则将右子树入队*/
                    queue[++rear] = p->rchild;
                }
            }
        }

        return flag;
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
