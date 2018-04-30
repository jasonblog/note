#include <iostream>
using namespace std;
typedef char type;

//标志域的值,link表示指针，thread表示线索
typedef enum {link, thread} tag;

//二叉树的二叉线索存储表示

typedef struct bin {
    type data;
    struct bin* lchild;  //左孩子指针
    struct bin* rchild; //右孩子指针
    tag ltag;              //左标志位
    tag rtag;             //右标志位
} bin_thr_node, *bin_thr_tree;

//定义全局变量pre来记录刚刚访问过的节点
bin_thr_tree pre;

//递归线索化二叉树

void in_threading(bin_thr_tree tree)
{
    if (tree) {
        in_threading(tree->lchild);    //左子树线索化

        if (!tree->lchild) { //树的左子树为空，则指向前驱
            tree->ltag = thread;
            tree->lchild = pre;
        }

        if (!pre->rchild) { //前驱的右子树为空，则其右子树指向当前结点
            pre->rtag = thread;
            pre->rchild = tree;
        }

        pre = tree;                            //保持pre指向tree的前驱
        in_threading(tree->rchild);   //右子树线索化
    }
}

//线索化二叉树
int inorder_thr(bin_thr_tree& thr, bin_thr_tree t)
{
    thr = new bin_thr_node;
    thr->data = 0;
    thr->ltag = link;
    thr->rtag = thread;
    thr->rchild = thr;  //右孩子回指

    if (!t) {           //假如t为空，则thr的左孩子回指
        thr->lchild = thr;
    } else {
        thr->lchild = t;
        pre = thr;
        in_threading(t);
        pre->rchild = thr;
        pre->rtag = thread;
        thr->rchild = pre;
    }

    return 1;
}

//先序创建二叉树.

//按先序次序输入二叉树中结点的值(一个字符)， 字符'#'表示空树。

void pre_create_tree(bin_thr_tree& t)
{
    char ch;
    scanf("%c", &ch);

    if ('#' == ch) {
        t = NULL;
    } else {
        t = new bin_thr_node;
        t->data = ch;
        t->ltag = link;
        t->rtag = link;
        pre_create_tree(t->lchild);
        pre_create_tree(t->rchild);
    }
}

//  遍历中序线过二叉树

void in_order_traverse(bin_thr_tree t)
{
    bin_thr_tree point;
    point = t->lchild;

    while (point != t) {
        while (link == point->ltag) {
            point = point->lchild;
        }

        //访问最左端的孩子
        printf("%c", point->data);

        //访问后继结点
        while (thread == point->rtag &&
               point->rchild != t) { //记住是point->rchild != t;
            point = point->rchild;
            printf("%c", point->data);
        }

        //访问最后继结点的右孩子(最后继的结点的左孩子必为其前驱，故必访问过，无需再访问)
        point = point->rchild;
    }
}



int main()
{
    bin_thr_tree t, thr;

    //先序创建二叉树
    pre_create_tree(t);

    //线索化二叉树
    inorder_thr(thr, t);

    //遍历线索二叉树
    in_order_traverse(thr);
    printf("\n");
    return 0;
}

