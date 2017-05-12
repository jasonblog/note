#include <iostream>
#include <cstdio>
#include <cstdlib>
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
    void flatten(TreeNode* root)
    {
        slowFlatten(root);
    }
private:
    void slowFlatten(TreeNode* root)
    {
        if (root == nullptr) {
            return ;
        }

        flatten(root->left);
        flatten(root->right);

        if (root->right == nullptr) {
            root->right = root->left;
            root->left = nullptr; // 不能少这个，否则RE
            return;
        }

        TreeNode* p = root->left;

        if (p) {
            while (p->right) {
                p = p->right;
            }

            p->right = root->right;
            root->right = root->left;
        }

        root->left = nullptr;
    }
    void fastFlatten(TreeNode* root)
    {
        while (root) {
            if (root->left) {
                TreeNode* left = root->left;

                while (left->right) {
                    left = left->right;
                }

                left->right = root->right;
                root->right = root->left;
                root->left = nullptr;
            }

            root = root->right;
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
void print_list(TreeNode* root)
{
    TreeNode* p = root;

    while (p) {
        cout << p->val << " ";
        p = p->right;
    }

    cout << endl;
}
int main(int argc, char** argv)
{
    Solution solution;
    TreeNode* root = mk_child(mk_node(1), nullptr, mk_node(2));
    //mk_child(root->left, 3, 4);
    //mk_child(root->right, nullptr, mk_node(6));
    solution.flatten(root);
    print_list(root);
    return 0;
}
