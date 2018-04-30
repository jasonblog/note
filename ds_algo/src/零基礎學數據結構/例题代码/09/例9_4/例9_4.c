/*包含头文件*/
#include <stdio.h>
#include <malloc.h>
#include<stdlib.h>
#define MaxSize 100
/*线索二叉树类型定义*/
typedef char DataType;
typedef enum {Link, Thread} PointerTag;
typedef struct Node { /*结点类型*/
    DataType data;
    struct Node* lchild, *rchild;   /*左右孩子子树*/
    PointerTag ltag, rtag;          /*线索标志域*/
} BiThrNode;
typedef BiThrNode* BiThrTree;       /*二叉树类型*/
/*函数声明*/
void CreateBitTree2(BiThrTree* T, char str[]);      /*创建线索二叉树*/
void InThreading(BiThrTree p);                      /*中序线索化二叉树*/
int InOrderThreading(BiThrTree* Thrt,
                     BiThrTree T); /*通过中序遍历二叉树T，使T中序线索化。Thrt是指向头结点的指针*/
int InOrderTraverse(BiThrTree T,
                    int (* visit)(BiThrTree e));   /*中序遍历线索二叉树*/
int Print(BiThrTree
          T);                             /*打印二叉树中的结点及线索标志*/
BiThrNode* FindPoint(BiThrTree T,
                     DataType e);      /*在线索二叉树中查找结点为e的指针*/
BiThrNode* InOrderPre(BiThrNode*
                      p);                /*查找中序线索二叉树的中序前驱*/
BiThrNode* InOrderPost(BiThrNode*
                       p);               /*查找中序线索二叉树的中序后继*/
void DestroyBitTree(BiThrTree* T);
BiThrTree pre;                                      /*pre始终指向已经线索化的结点*/

void main()
/*测试程序*/
{
    BiThrTree T, Thrt;
    BiThrNode* p, *pre, *post;
    CreateBitTree2(&T, "(A(B(,D(G)),C(E(,H),F))");
    printf("线索二叉树的输出序列：\n");
    InOrderThreading(&Thrt, T);
    printf("序列   前驱标志   结点  后继标志\n");
    InOrderTraverse(Thrt, Print);
    p = FindPoint(Thrt, 'D');
    pre = InOrderPre(p);
    printf("元素D的中序直接前驱元素是:%c\n", pre->data);
    post = InOrderPost(p);
    printf("元素D的中序直接后继元素是:%c\n", post->data);
    p = FindPoint(Thrt, 'E');
    pre = InOrderPre(p);
    printf("元素E的中序直接前驱元素是:%c\n", pre->data);
    post = InOrderPost(p);
    printf("元素E的中序直接后继元素是:%c\n", post->data);
    DestroyBitTree(&Thrt);

}
void InThreading(BiThrTree p)
/*二叉树中序线索化*/
{
    if (p) {
        InThreading(p->lchild); /*左子树线索化*/

        if (!p->lchild) {           /*前驱线索化*/
            p->ltag = Thread;
            p->lchild = pre;
        }

        if (!pre->rchild) {         /*后继线索化*/
            pre->rtag = Thread;
            pre->rchild = p;
        }

        pre = p;                    /*pre指向的结点线索化完毕，使p指向的结点成为前驱*/
        InThreading(p->rchild); /*右子树线索化*/
    }
}
int InOrderThreading(BiThrTree* Thrt, BiThrTree T)
/*通过中序遍历二叉树T，使T中序线索化。Thrt是指向头结点的指针*/
{

    if (!(*Thrt = (BiThrTree)malloc(sizeof(BiThrNode)))) { /*为头结点分配内存单元*/
        exit(-1);
    }

    /*将头结点线索化*/
    (*Thrt)->ltag = Link;           /*修改前驱线索标志*/
    (*Thrt)->rtag = Thread;         /*修改后继线索标志*/
    (*Thrt)->rchild = *Thrt;        /*将头结点的rchild指针指向自己*/

    if (!T) {                       /*如果二叉树为空，则将lchild指针指向自己*/
        (*Thrt)->lchild = *Thrt;
    } else {
        (*Thrt)->lchild = T;        /*将头结点的左指针指向根结点*/
        pre = *Thrt;                /*将pre指向已经线索化的结点*/
        InThreading(T);             /*中序遍历进行中序线索化*/
        /*将最后一个结点线索化*/
        pre->rchild = *Thrt;        /*将最后一个结点的右指针指向头结点*/
        pre->rtag = Thread;         /*修改最后一个结点的rtag标志域*/
        (*Thrt)->rchild = pre;      /*将头结点的rchild指针指向最后一个结点*/
    }

    return 1;
}

int Print(BiThrTree T)
/*打印线索二叉树中的结点及线索*/
{
    static int k = 1;
    printf("%2d\t%s\t  %2c\t  %s\t\n", k++, T->ltag == 0 ? "Link" : "Thread",
           T->data,
           T->rtag == 1 ? "Thread" : "Link");
    return 1;
}

int InOrderTraverse(BiThrTree T, int (* visit)(BiThrTree e))
/*中序遍历线索二叉树。其中visit是函数指针，指向访问结点的函数实现*/
{
    BiThrTree p;
    p = T->lchild;                          /*p指向根结点*/

    while (p != T) {                        /*空树或遍历结束时，p==T*/
        while (p->ltag == Link) {
            p = p->lchild;
        }

        if (!visit(p)) {                        /*打印*/
            return 0;
        }

        while (p->rtag == Thread && p->rchild != T) { /*访问后继结点*/
            p = p->rchild;
            visit(p);
        }

        p = p->rchild;
    }

    return 1;
}
BiThrNode* InOrderPre(BiThrNode* p)
/*在中序线索树中找结点*p的中序直接前趋*/
{
    BiThrNode* pre;

    if (p->ltag ==
        Thread) {    /*如果p的标志域ltag为线索，则p的左子树结点即为前驱*/
        return p->lchild;
    } else {
        pre = p->lchild;        /*查找p的左孩子的最右下端结点*/

        while (pre->rtag == Link) { /*右子树非空时，沿右链往下查找*/
            pre = pre->rchild;
        }

        return pre;             /*pre就是最右下端结点*/
    }
}
BiThrNode* InOrderPost(BiThrNode* p)
/*在中序线索树中查找结点*p的中序直接后继*/
{
    BiThrNode* pre;

    if (p->rtag ==
        Thread) {    /*如果p的标志域ltag为线索，则p的右子树结点即为后继*/
        return p->rchild;
    } else {
        pre = p->rchild;        /*查找p的右孩子的最左下端结点*/

        while (pre->ltag == Link) { /*左子树非空时，沿左链往下查找*/
            pre = pre->lchild;
        }

        return pre;             /*pre就是最左下端结点*/
    }
}

void  CreateBitTree2(BiThrTree* T, char str[])
/*利用括号嵌套的字符串建立二叉链表*/
{
    char ch;
    BiThrTree stack[MaxSize];           /*定义栈，用于存放指向二叉树中结点的指针*/
    int top = -1;                   /*初始化栈顶指针*/
    int flag, k;
    BiThrNode* p;
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
            p = (BiThrTree)malloc(sizeof(BiThrNode));
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

                if (stack[top]->lchild) {
                    stack[top]->ltag = Link;
                }

                if (stack[top]->rchild) {
                    stack[top]->rtag = Link;
                }
            }

        }

        ch = str[++k];
    }

}
BiThrNode* FindPoint(BiThrTree T, DataType e)
/*中序遍历线索二叉树，返回元素值为e的结点的指针。*/
{
    BiThrTree p;
    p = T->lchild;                          /*p指向根结点*/

    while (p != T) {                        /*如果不是空二叉树*/

        while (p->ltag == Link) {
            p = p->lchild;
        }

        if (p->data == e) {                     /*找到结点，返回指针*/
            return p;
        }

        while (p->rtag == Thread && p->rchild != T) { /*访问后继结点*/
            p = p->rchild;

            if (p->data == e) {                 /*找到结点，返回指针*/
                return p;
            }
        }

        p = p->rchild;
    }

    return NULL;
}

void DestroyBitTree(BiThrTree* T)
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