
/*包含头文件和二叉排序树的类型定义*/
#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>

typedef int KeyType;
typedef struct { /*元素的定义*/
    KeyType key;
} DataType;

typedef struct Node { /*二叉排序树的类型定义*/
    DataType data;
    struct Node* lchild, *rchild;
} BiTreeNode, *BiTree;
void DeleteNode(BiTree* s);
int BSTDelete(BiTree* T, DataType x);
void InOrderTraverse(BiTree T);
BiTree BSTSearch(BiTree T, DataType x);
int BSTInsert(BiTree* T, DataType x);

BiTree BSTSearch(BiTree T, DataType x)
/*二叉排序树的查找，如果找到元素x，则返回指向结点的指针，否则返回NULL*/
{
    BiTreeNode* p;

    if (T != NULL) {                /*如果二叉排序树不为空*/
        p = T;

        while (p != NULL) {
            if (p->data.key == x.key) { /*如果找到，则返回指向该结点的指针*/
                return p;
            } else if (x.key <
                       p->data.key) { /*如果关键字小于p指向的结点的值，则在左子树中查找*/
                p = p->lchild;
            } else {
                p = p->rchild;    /*如果关键字大于p指向的结点的值，则在右子树中查找*/
            }
        }
    }

    return NULL;
}

int BSTInsert(BiTree* T, DataType x)
/*二叉排序树的插入操作，如果树中不存在元素x，则将x插入到正确的位置并返回1，否则返回0*/
{
    BiTreeNode* p, *cur, *parent = NULL;
    cur = *T;

    while (cur != NULL) {
        if (cur->data.key == x.key) { /*如果二叉树中存在元素为x的结点，则返回0*/
            return 0;
        }

        parent = cur;               /*parent指向cur的前驱结点*/

        if (x.key <
            cur->data.key) { /*如果关键字小于p指向的结点的值，则在左子树中查找*/
            cur = cur->lchild;
        } else {
            cur = cur->rchild;    /*如果关键字大于p指向的结点的值，则在右子树中查找*/
        }
    }

    p = (BiTreeNode*)malloc(sizeof(BiTreeNode)); /*生成结点*/

    if (!p) {
        exit(-1);
    }

    p->data = x;
    p->lchild = NULL;
    p->rchild = NULL;

    if (!parent) {                  /*如果二叉树为空，则第一结点成为根结点*/
        *T = p;
    } else if (x.key <
               parent->data.key) { /*如果关键字小于parent指向的结点，则将x成为parent的左孩子*/
        parent->lchild = p;
    } else {                        /*如果关键字大于parent指向的结点，则将x成为parent的右孩子*/
        parent->rchild = p;
    }

    return 1;
}

int BSTDelete(BiTree* T, DataType x)
/*在二叉排序树T中存在值为x的数据元素时，删除该数据元素结点，并返回1，否则返回0 */
{
    if (!*T) {                      /*如果不存在值为x的数据元素，则返回0*/
        return 0;
    } else {
        if (x.key == (*T)->data.key) {  /*如果找到值为x的数据元素，则删除该结点*/
            DeleteNode(T);
        } else if ((*T)->data.key >
                   x.key) { /*如果当前元素值大于x的值，则在该结点的左子树中查找并删除之*/
            BSTDelete(&(*T)->lchild, x);
        } else {                        /*如果当前元素值小于x的值，则在该结点的右子树中查找并删除之*/
            BSTDelete(&(*T)->rchild, x);
        }

        return 1;
    }
}
void DeleteNode(BiTree* s)
/*从二叉排序树中删除结点s，并使该二叉排序树性质不变*/
{
    BiTree q, x, y;

    if (!(*s)->rchild) {    /*如果s的右子树为空，则使s的左子树成为被删结点双亲结点的左子树*/
        q = *s;
        *s = (*s)->lchild;
        free(q);
    } else if (!
               (*s)->lchild) { /*如果s的左子树为空，则使s的右子树成为被删结点双亲结点的左子树*/
        q = *s;
        *s = (*s)->rchild;
        free(q);
    } else
        /*如果s的左、右子树都存在，则使s的直接前驱结点代替s，并使其直接前驱结点的左子树成为其双亲结点的右子树结点*/
    {
        x = *s;
        y = (*s)->lchild;

        while (y->rchild) { /*查找s的直接前驱结点，y为s的直接前驱结点，x为y的双亲结点*/
            x = y;
            y = y->rchild;
        }

        (*s)->data = y->data; /*结点s被y取代*/

        if (x != *s) {      /*如果结点s的左孩子结点不存在右子树*/
            x->rchild = y->lchild;    /*使y的左子树成为x的右子树*/
        } else {            /*如果结点s的左孩子结点存在右子树*/
            x->lchild = y->lchild;    /*使y的左子树成为x的左子树*/
        }

        free(y);
    }
}
void main()
{
    BiTree T = NULL, p;
    DataType table[] = {37, 32, 35, 62, 82, 95, 73, 12, 5};
    int n = sizeof(table) / sizeof(table[0]);
    DataType x = {73}, s = {32};
    int i;

    for (i = 0; i < n; i++) {
        BSTInsert(&T, table[i]);
    }

    printf("中序遍历二叉排序树得到的序列为：\n");
    InOrderTraverse(T);
    p = BSTSearch(T, x);

    if (p != NULL) {
        printf("\n二叉排序树查找，关键字%d存在\n", x.key);
    } else {
        printf("查找失败！\n");
    }

    BSTDelete(&T, s);
    printf("删除元素%d后，中序遍历二叉排序树得到的序列为：\n", s.key);
    InOrderTraverse(T);
    printf("\n");

}
void InOrderTraverse(BiTree T)
/*中序遍历二叉排序的递归实现*/
{
    if (T) {                            /*如果二叉排序树不为空*/
        InOrderTraverse(T->lchild);         /*中序遍历左子树*/
        printf("%4d", T->data);             /*访问根结点*/
        InOrderTraverse(T->rchild);         /*中序遍历右子树*/
    }
}