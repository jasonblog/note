#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <climits>
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
    bool isValidBST(TreeNode* root)
    {
        int prev = INT_MIN;
        bool isFirst = true;
        return isValidBST(root, prev, isFirst);
    }
private:
    bool isValidBST(TreeNode* root, int& prev, bool& isFirst)
    {
        if (root == nullptr) {
            return true;
        }

        if (!isValidBST(root->left, prev, isFirst)) {
            return false;
        }

        if (isFirst) {
            isFirst = false;
        } else {
            if (root->val <= prev) {
                return false;
            }
        }

        prev = root->val;
        return isValidBST(root->right, prev, isFirst);
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
    TreeNode* root = mk_child(mk_node(INT_MIN), nullptr, nullptr);
    //TreeNode *root = mk_child(mk_node(1), mk_node(1), nullptr);
    //TreeNode *root = mk_child(mk_node(1), nullptr, mk_node(1));
    //TreeNode *root = mk_child(10, 5, 15);
    //mk_child(root->right, 6, 20);
    //TreeNode *root = mk_child(3,1,4);
    cout << solution.isValidBST(root) << endl;
    return 0;
}
