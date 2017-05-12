#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <stack>
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
    vector<int> preorderTraversal(TreeNode* root)
    {
        vector<int> result;

        if (root == NULL) {
            return result;
        }

        stack<TreeNode*>s;
        s.push(root);

        while (!s.empty()) {
            TreeNode* p = s.top();
            s.pop();
            result.push_back(p->val);

            if (p->right) {
                s.push(p->right);
            }

            if (p->left) {
                s.push(p->left);
            }
        }

        return result;
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
    TreeNode* root = mk_child(10, 5, 15);
    mk_child(root->right, 6, 20);
    mk_child(root->left, 2, 3);
    auto result = solution.preorderTraversal(root);
    for_each(result.begin(), result.end(), [](int i) {
        cout << i << " ";
    });
    cout << endl;
    return 0;
}
