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
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q)
    {
        if (root == nullptr) {
            return nullptr;
        }

        return lowestCommonAncestor(root, p->val, q->val);
    }
private:
    TreeNode* lowestCommonAncestor(TreeNode* root, int v1, int v2)
    {
        if (root->val > v1 && root->val > v2) {
            return lowestCommonAncestor(root->left, v1, v2);
        }

        if (root->val < v1 && root->val < v2) {
            return lowestCommonAncestor(root->right, v1, v2);
        }

        return root;
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
    TreeNode* root = mk_child(6, 2, 8);
    mk_child(root->left, 0, 4);
    mk_child(root->left->right, 3, 5);
    mk_child(root->right, 7, 9);
    cout << solution.lowestCommonAncestor(root, root->left,
                                          root->right)->val << endl;
    cout << solution.lowestCommonAncestor(root, root->left,
                                          root->left->right)->val << endl;
    return 0;
}
