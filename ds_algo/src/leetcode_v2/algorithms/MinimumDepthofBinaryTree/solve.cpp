#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};
class Solution
{
public:
    int minDepth2(struct TreeNode* root)
    {
        if (root == NULL) {
            return 0;
        }

        if (root->left == NULL && root->right == NULL) {
            return 1;
        }

        if (root->left == NULL && root->right != NULL) {
            return minDepth(root->right) + 1;
        }

        if (root->left != NULL && root->right == NULL) {
            return minDepth(root->left) + 1;
        }

        return min(minDepth(root->left), minDepth(root->right));
    }
    int minDepth(struct TreeNode* root)
    {
        if (root == NULL) {
            return 0;
        }

        if (root->left && root->right) {
            return min(minDepth(root->left), minDepth(root->right)) + 1;
        } else if (root->left) {
            return minDepth(root->left) + 1;
        } else {
            return minDepth(root->right) + 1;
        }
    }
};
void mk_children(TreeNode* root, int left, int right)
{
    root->left = new TreeNode(left);
    root->right = new TreeNode(right);
}
void mk_children(TreeNode* root, TreeNode* left, TreeNode* right)
{
    root->left = left;
    root->right = right;
}
int main(int argc, char** argv)
{
    Solution solution;
    TreeNode* root = new TreeNode(1);
    mk_children(root, new TreeNode(2), nullptr);
    cout << solution.minDepth(root) << endl;
    return 0;
}
