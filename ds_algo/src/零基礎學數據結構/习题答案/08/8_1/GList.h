
typedef enum {ATOM, LIST} ElemTag; /*ATOM=0，表示原子，LIST=1，表示子表*/
typedef struct {
    ElemTag tag;                /*标志位tag用于区分元素是原子还是子表*/
    union {
        AtomType atom;      /*AtomType是原子结点的值域，用户自己定义类型*/
        struct {
            struct GLNode* hp, *tp;     /*hp指向表头，tp指向表尾*/
        } ptr;
    };
}* GList, GLNode;


GLNode* GetHead(GList L)
/*求广义表的表头结点操作*/
{
    GLNode* p;

    if (!L) {                       /*如果广义表为空表，则返回1*/
        printf("该广义表是空表！");
        return NULL;
    }

    p = L->ptr.hp;                  /*将广义表的表头指针赋值给p*/

    if (!p) {
        printf("该广义表的表头是空表.");
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
    if (!L) {                       /*如果广义表为空表，则返回1*/
        printf("该广义表是空表！");
        return;
    }

    return L->ptr.hp;               /*如果广义表不是空表，则返回指向表尾结点的指针*/
}
int GListLength(GList L)
/*求广义表的长度操作*/
{
    int length = 0;

    while (L) {                         /*如果广义表非空，则将p指向表尾指针，统计表的长度*/
        L = L->ptr.tp;
        length++;
    }

    return length;
}
int GListDepth(GList L)
/*求广义表的深度操作*/
{
    int max, depth;
    GLNode* p;

    if (!L) {                       /*如果广义表非空，则返回1*/
        return 1;
    }

    if (L->tag == ATOM) {           /*如果广义表是原子，则返回0*/
        return 0;
    }

    for (max = 0, p = L; p; p = p->ptr.tp) { /*逐层处理广义表*/
        depth = GListDepth(p->ptr.hp);

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

        if (L->tag == ATOM) {       /*复制原子*/
            (*T)->atom = L->atom;
        } else {                    /*递归复制子表*/
            CopyList(&((*T)->ptr.hp), L->ptr.hp);
            CopyList(&((*T)->ptr.tp), L->ptr.tp);
        }
    }
}
