#include<stdlib.h>
#include"tree.h"
node* mk(node* l, node* r, char val)
{
    node* f = (node*)malloc(sizeof(*f));
    f->l = l;
    f->r = r;
    f->v = val;
    return f;
}
void nodefr(node* n)
{
    if (n) {
        nodefr(n->l);
        nodefr(n->r);
        free(n);
    }
}
