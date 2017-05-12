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
    int rob(TreeNode* root)
    {
        int yes = 0, no = 0;
        dfs(root, &yes, &no);
        return max(yes, no);
    }
private:
    void dfs(TreeNode* root, int* yes, int* no)
    {
        if (root == nullptr) {
            *yes = 0;
            *no = 0;
            return;
        }

        int left_yes, left_no, right_yes, right_no;
        dfs(root->left, &left_yes, &left_no);
        dfs(root->right, &right_yes, &right_no);
        *yes = left_no + right_no + root->val;
        *no = max(left_yes, left_no) + max(right_yes, right_no);
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
    TreeNode* root = mk_child(4, 1, 0);
    return 0;
}
