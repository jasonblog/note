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
    vector<int> postorderTraversal(TreeNode* root)
    {
        vector<int> result;

        if (root == nullptr) {
            return result;
        }

        stack<TreeNode*>s;
        pushLeftOrRight(root, s);

        while (!s.empty()) {
            TreeNode* p = getAndPop(s);
            result.push_back(p->val);
            TreeNode* next = s.empty() ? nullptr : s.top();

            if (next && next->left == p) {
                pushLeftOrRight(next->right, s);
            }
        }

        return result;
    }
private:
    void pushLeftOrRight(TreeNode* p, stack<TreeNode*>& s)
    {
        while (p) {
            s.push(p);
            p = p->left != nullptr ? p->left : p->right;
        }
    }
    TreeNode* getAndPop(stack<TreeNode*>& s)
    {
        TreeNode* p = s.top();
        s.pop();
        return p;
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
    TreeNode* root = mk_child(1, 2, 3);
    //mk_child(root->right, 6, 20);
    //mk_child(root->left, 2, 3);
    auto result = solution.postorderTraversal(root);
    for_each(result.begin(), result.end(), [](int i) {
        cout << i << " ";
    });
    cout << endl;
    return 0;
}
