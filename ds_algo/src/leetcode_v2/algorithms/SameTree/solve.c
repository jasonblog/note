#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
struct TreeNode {
    int val;
    struct TreeNode* left;
    struct TreeNode* right;
};
bool isSameTree(struct TreeNode* p, struct TreeNode* q)
{
    if (p == NULL) {
        return q == NULL;
    }

    if (q == NULL) {
        return false;
    }

    if (p->val != q->val) {
        return false;
    }

    return isSameTree(p->left, q->left) && isSameTree(p->right, q->right);
}
int main(int argc, char** argv)
{
    return 0;
}
