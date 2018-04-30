
typedef enum {ATOM, LIST} ElemTag; /*ATOM=0，表示原子，LIST=1，表示子表*/
typedef struct {
    ElemTag tag;                    /*标志位tag用于区分元素是原子还是子表*/
    union {
        AtomType atom;              /*AtomType是原子结点的值域，用户自己定义类型*/
        struct GLNode* hp;      /*hp指向表头，tp指向表尾*/
    } ptr;
    struct GLNode* tp;
}* GList, GLNode;


GLNode* GetHead(GList L)
/*求广义表的表头结点操作*/
{
    GLNode* p;
    p = L->ptr.hp;                  /*将广义表的表头指针赋值给p*/

    if (!p) {                       /*如果广义表为空表，则返回1*/
        printf("该广义表是空表！");
        return NULL;
    } else if (p->tag == LIST) {
        printf("该广义表的表头是非空的子表。");
    } else {
        printf("该广义表的表头是原子。");
    }

    return p;
}

GLNode* GeTail(GList L)
/*求广义表的表尾操作*/
{
    GLNode* p, *tail;
    p = L->ptr.hp;

    if (!p) {                       /*如果广义表为空表，则返回1*/
        printf("该广义表是空表！");
        return NULL;
    }

    tail = (GLNode*)malloc(sizeof(GLNode)); /*生成tail结点*/
    tail->tag = LIST;                       /*将标志域置为LIST*/
    tail->ptr.hp = p->tp;                   /*将tail的表头指针域指向广义表的表尾*/
    tail->tp = NULL;                    /*将tail的表尾指针域置为空*/
    return tail;                            /*返回指向广义表表尾结点的指针*/
}

int GListLength(GList L)
/*求广义表的长度操作*/
{
    int length = 0;                     /*初始化化广义表的长度*/
    GLNode* p = L->ptr.hp;

    while (p) {                         /*如果广义表非空，则将p指向表尾指针，统计表的长度*/
        length++;
        p = p->tp;
    }

    return length;
}

int GListDepth(GList L)
/*求广义表的深度操作*/
{
    int max, depth;
    GLNode* p;

    if (L->tag == LIST && L->ptr.hp == NULL) { /*如果广义表非空，则返回1*/
        return 1;
    }

    if (L->tag == ATOM) {               /*如果广义表是原子，则返回0*/
        return 0;
    }

    p = L->ptr.hp;

    for (max = 0; p; p = p->tp) {       /*逐层处理广义表*/
        depth = GListDepth(p);

        if (max < depth) {
            max = depth;
        }
    }

    return max + 1;
}

void CopyList(GList* T, GList L)
/*广义表的复制操作。由广义表L复制得到广义表T*/
{
    if (!L) {                       /*如果广义表为空，则T为空表*/
        *T = NULL;
    } else {
        *T = (GList)malloc(sizeof(GLNode)); /*表L不空，为T建立一个表结点*/

        if (*T == NULL) {
            exit(-1);
        }

        (*T)->tag = L->tag;

        if (L->tag == ATOM) {                   /*复制原子*/
            (*T)->ptr.atom = L->ptr.atom;
        } else {
            CopyList(&((*T)->ptr.hp), L->ptr.hp);    /*递归复制表头*/
        }

        if (L->tp == NULL) {
            (*T)->tp = L->tp;
        } else {
            CopyList(&((*T)->tp), L->tp);    /*递归复制表尾*/
        }
    }
}
