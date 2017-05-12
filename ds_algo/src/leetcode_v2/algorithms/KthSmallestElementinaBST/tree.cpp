#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
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
    int kthSmallest(TreeNode* root, int k)
    {
        int result;
        /*
        if (root == nullptr) {
            return 0;
        }
        */
        kthSmallest(root, k, result);
        return result;
    }
private:
    bool kthSmallest(TreeNode* root, int& k, int& result)
    {
        if (root->left) {
            if (kthSmallest(root->left, k, result)) {
                return true;
            }
        }

        k -= 1;

        if (k == 0) {
            result = root->val;
            return true;
        }

        if (root->right) {
            if (kthSmallest(root->right, k, result)) {
                return true;
            }
        }

        return false;
    }
};
TreeNode* mk_node(int val)
{
    return new TreeNode(val);
}
TreeNode* mk_child(TreeNode* root, TreeNode* left, TreeNode* right)
{
    root->left = left;
    root->right = right;
    return root;
}
TreeNode* mk_child(TreeNode* root, int left, int right)
{
    return mk_child(root, new TreeNode(left), new TreeNode(right));
}
TreeNode* mk_child(int root, int left, int right)
{
    return mk_child(new TreeNode(root), new TreeNode(left), new TreeNode(right));
}
int main(int argc, char** argv)
{
    Solution solution;
    TreeNode* root = mk_child(6, 4, 8);
    mk_child(root->left, 3, 5);
    mk_child(root->right, 7, 9);

    for (int i = 1; i <= 7; ++i) {
        printf("%d\n", solution.kthSmallest(root, i));
    }

    return 0;
}
