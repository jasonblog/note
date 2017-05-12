#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
struct TreeNode {
    int val;
    struct TreeNode* left;
    struct TreeNode* right;
};
bool isLeftRightSymmetric(struct TreeNode* left, struct TreeNode* right)
{
    if (left == NULL) {
        return right == NULL;
    }

    if (right == NULL) {
        return false;
    }

    return left->val == right->val
           && isLeftRightSymmetric(left->left, right->right)
           && isLeftRightSymmetric(left->right, right->left);
}
bool isSymmetric(struct TreeNode* root)
{
    if (root == NULL) {
        return true;
    }

    return isLeftRightSymmetric(root->left, root->right);
}
struct TreeNode* mk_node(int val)
{
    struct TreeNode* p = malloc(sizeof(*p));
    p->val = val;
    p->left = p->right = NULL;
    return p;
}
struct TreeNode* mk_children(struct TreeNode* root, int left, int right)
{
    assert(root != NULL);
    root -> left = mk_node(left);
    root -> right = mk_node(right);
    return root;
}
int main(int argc, char** argv)
{
    struct TreeNode* root = mk_node(1);
    mk_children(root, 2, 2);
    mk_children(root->left, 3, 4);
    mk_children(root->right, 4, 3);
    printf("%d\n", isSymmetric(root));
    return 0;
}
