#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <list>
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

        list<TreeNode*> l1, l2;
        getPath(root, p, l1);
        getPath(root, q, l2);
        /*
        for_each(begin(l1), end(l1), [](TreeNode *t){cout << t->val << ' ';});
        cout << endl;
        for_each(begin(l2), end(l2), [](TreeNode *t){cout << t->val << ' ';});
        cout << endl;
        */
        return findCommonNode(l1, l2);
    }
    /*
    TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *p, TreeNode *q)
    {
        if (root == nullptr)
            return nullptr;
        if (root == p || root == q)
            return root;
        TreeNode *left = lowestCommonAncestor(root->left, p, q);
        TreeNode *right = lowestCommonAncestor(root->right, p, q);
        if (left && right)
            return root;
        return left ? left:right;
    }
    */
private:
    bool getPath(TreeNode* root, TreeNode* p, list<TreeNode*>& path)
    {
        if (root == nullptr) {
            return p == nullptr;
        }

        path.push_back(root);

        if (root == p) {
            return true;
        }

        if (getPath(root->left, p, path) || getPath(root->right, p, path)) {
            return true;
        } else {
            path.pop_back();
            return false;
        }
    }
    TreeNode* findCommonNode(const list<TreeNode*>& l1, const list<TreeNode*>& l2)
    {
        int n = l1.size();
        int m = l2.size();
        auto i = begin(l1), j = begin(l2);
        TreeNode* last = nullptr;

        while (i != end(l1) && j != end(l2)) {
            if (*i == *j) {
                last = *i;
            }

            i++, j++;
        }

        return last;
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
    TreeNode* root = mk_child(3, 5, 1);
    mk_child(root->left, 6, 2);
    mk_child(root->left->right, 7, 4);
    mk_child(root->right, 0, 8);
    cout << solution.lowestCommonAncestor(root, root->left,
                                          root->right)->val << endl;
    cout << solution.lowestCommonAncestor(root, root->left,
                                          root->left->right->right)->val << endl;
    return 0;
}
