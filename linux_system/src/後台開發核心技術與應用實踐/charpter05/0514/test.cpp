#include<iostream>
#include"tree.h"
int main()
{
    node* tree1, *tree2, *tree3;
    tree1 = mk(mk(mk(0, 0, '3'), 0, '2'), 0, '1');
    tree2 = mk(0, mk(0, mk(0, 0, '6'), '5'), '4');
    tree3 = mk(mk(tree1, tree2, '8'), 0, '7');
    return 0;
}
