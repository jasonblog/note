
typedef struct Node {           /*二叉链表存储结构类型定义*/
    DataType data;              /*数据域*/
    struct Node* lchild;            /*指向左孩子结点*/
    struct Node* rchild;            /*指向右孩子结点*/
}* BiTree, BitNode;


void InitBitTree(BiTree* T)
/*二叉树的初始化操作*/
{
    *T = NULL;
}


//BiTree CreateBitTree()
//{
//     char ch;
//     BiTree b;
//     ch = getchar();
//
//     if(ch == '@')     //表示该结点为空结点
//     {
//          b = NULL;
//     }
//     else
//     {
//          b = (BiTree )malloc(sizeof(BitNode));
//          b->data = ch;
//          b->lchild = CreateBitTree();
//          b->rchild = CreateBitTree();
//     }
//
//     return b;
//}







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
void CreateBitTree(BiTree* T)
/*递归创建二叉树*/
{
    DataType ch;
    scanf("%c", &ch);

    if (ch == '#') {
        *T = NULL;
    } else {
        *T = (BiTree)malloc(sizeof(BitNode));       /*生成根结点*/

        if (!(*T)) {
            exit(-1);
        }

        (*T)->data = ch;
        CreateBitTree(&((*T)->lchild));             /*构造左子树*/
        CreateBitTree(&((*T)->rchild));             /*构造右子树*/
    }
}
int InsertLeftChild(BiTree p, BiTree c)
/*二叉树的左插入操作*/
{
    if (p) {                                /*如果指针p不空*/
        c->rchild = p->lchild;                  /*p的原来的左子树成为c的右子树*/
        p->lchild = c;                      /*子树c作为p的左子树*/
        return 1;
    }

    return 0;
}
int InsertRightChild(BiTree p, BiTree c)
/*二叉树的右插入操作*/
{
    if (p) {                                /*如果指针p不空*/
        c->rchild = p->rchild;                  /*p的原来的右子树作为c的右子树*/
        p->rchild = c;                      /*子树c作为p的右子树*/
        return 1;
    }

    return 0;
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
DataType LeftChild(BiTree T, DataType e)
/*返回二叉树的左孩子结点元素值操作*/
{
    BiTree p;

    if (T) {                                    /*如果二叉树不空*/
        p = Point(T, e);                        /*p是元素值e的结点的指针*/

        if (p && p->lchild) {                       /*如果p不为空且p的左孩子结点存在*/
            return p->lchild->data;    /*返回p的左孩子结点的元素值*/
        }
    }

    return;
}
DataType RightChild(BiTree T, DataType e)
/*返回二叉树的右孩子结点元素值操作*/
{
    BiTree p;

    if (T) {                                    /*如果二叉树不空*/
        p = Point(T, e);                        /*p是元素值e的结点的指针*/

        if (p && p->rchild) {                       /*如果p不为空且p的右孩子结点存在*/
            return p->rchild->data;    /*返回p的右孩子结点的元素值*/
        }
    }

    return;
}
int DeleteLeftChild(BiTree p)
/*二叉树的左删除操作*/
{
    if (p) {                                    /*如果p不空*/
        DestroyBitTree(&(p->lchild));               /*删除左子树*/
        return 1;
    }

    return 0;
}
int DeleteRightChild(BiTree p)
/*二叉树的左删除操作*/
{
    if (p) {                                    /*如果p不空*/
        DestroyBitTree(&(p->rchild));               /*删除右子树*/
        return 1;
    }

    return 0;
}
