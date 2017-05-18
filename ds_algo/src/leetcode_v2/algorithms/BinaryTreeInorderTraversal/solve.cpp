#include <iostream>
#include <vector>
#include <algorithm>
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
    vector<int> inorderTraversal(TreeNode* root)
    {
        vector<int> result;
        loop(result, root);
        return result;
    }
private:
    void recursive(vector<int>& result, TreeNode* root)
    {
        if (root == nullptr) {
            return;
        }

        recursive(result, root->left);
        result.push_back(root->val);
        recursive(result, root->right);
    }
    void loop(vector<int>& result, TreeNode* root)
    {
        if (root == nullptr) {
            return;
        }

        stack<TreeNode*> s;
        TreeNode* cur = root;

        while (cur || !s.empty()) {
            while (cur) {
                s.push(cur);
                cur = cur->left;
            }

            cur = s.top();
            s.pop();
            result.push_back(cur->val);
            cur = cur->right;
        }
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
    TreeNode* root = mk_child(mk_node(1), nullptr, mk_node(2));
    mk_child(root->right, mk_node(3), nullptr);
    auto result = solution.inorderTraversal(root);
    for_each(result.begin(), result.end(), [](int i) {
        cout << i << " ";
    });
    cout << endl;
    return 0;
}
