#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
struct TreeNode {
    int val;
    struct TreeNode* left;
    struct TreeNode* right;
};
static inline int max(int a, int b)
{
    return a > b ? a : b;
}
int maxDepth(struct TreeNode* root)
{
    if (root == NULL) {
        return 0;
    }

    return max(maxDepth(root->left), maxDepth(root->right)) + 1;
}
int main(int argc, char** argv)
{
    return 0;
}
