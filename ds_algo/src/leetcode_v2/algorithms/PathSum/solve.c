#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

struct TreeNode {
    int val;
    struct TreeNode* left;
    struct TreeNode* right;
};
bool hasPathSum(struct TreeNode* root, int sum)
{
    if (root == NULL) {
        return false;
    }

    if (root->left == NULL && root->right == NULL) {
        return root->val == sum;
    }

    if (!root->left || !hasPathSum(root->left, sum - root->val)) {
        return hasPathSum(root->right, sum - root->val);
    }

    return true;
}

struct TreeNode* mk_node(int val)
{
    struct TreeNode* p = malloc(sizeof(*p));
    p->val = val;
    p->left = p->right = NULL;
    return p;
}
void mk_child(struct TreeNode* root, struct TreeNode* left,
              struct TreeNode* right)
{
    root->left = left;
    root->right = right;
}
int main(int argc, char** argv)
{
    struct TreeNode* root = mk_node(5);
    mk_child(root, mk_node(4), mk_node(8));
    struct TreeNode* left = root->left;
    struct TreeNode* right = root->right;
    mk_child(left, mk_node(11), NULL);
    mk_child(left->left, mk_node(7), mk_node(2));
    mk_child(right, mk_node(13), mk_node(4));
    mk_child(right->right, NULL, mk_node(1));
    printf("%d\n", hasPathSum(root, 22));
    printf("%d\n", hasPathSum(root, 26));
    printf("%d\n", hasPathSum(root, 18));
    printf("%d\n", hasPathSum(root, 27));
    printf("%d\n", hasPathSum(root, 37));
    printf("%d\n", hasPathSum(root, 100));
    printf("%d\n", hasPathSum(root, 0));
    return 0;
}
