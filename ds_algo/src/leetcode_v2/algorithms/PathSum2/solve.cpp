#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};
class Solution
{
public:
    vector<vector<int> > pathSum(TreeNode* root, int sum)
    {
        vector<vector<int>> result;
        vector<int> t;
        pathSum(result, t, root, sum);
        return result;
    }
private:
    void pathSum(vector<vector<int>>& result, vector<int> cur, TreeNode* root,
                 int sum)
    {
        if (root == NULL) {
            return;
        }

        cur.push_back(root->val);
        sum -= root->val;

        if (isLeft(root) && sum == 0) {
            result.push_back(cur);
            return;
        }

        pathSum(result, cur, root->left, sum);
        pathSum(result, cur, root->right, sum);
    }
    bool isLeft(TreeNode* p)
    {
        if (p == nullptr) {
            return false;
        }

        return p->left == nullptr && p->right == nullptr;
    }
};
TreeNode* mk_node(int val)
{
    TreeNode* p = new TreeNode(val);
    return p;
}
void mk_child(TreeNode* root, TreeNode* left, TreeNode* right)
{
    root->left = left;
    root->right = right;
}
void mk_child(TreeNode* root, int left, int right)
{
    mk_child(root, mk_node(left), mk_node(right));
}
int main(int argc, char** argv)
{
    Solution solution;
    TreeNode* root = mk_node(5);
    mk_child(root, 4, 8);
    TreeNode* left = root->left;
    TreeNode* right = root->right;
    mk_child(left, mk_node(11), nullptr);
    mk_child(left->left, 7, 2);
    mk_child(right, 13, 4);
    mk_child(right->right, 5, 1);
    auto result = solution.pathSum(root, 22);
    cout << "size: " << result.size() << endl;

    for (auto v : result) {
        for_each(v.begin(), v.end(), [](int i) {
            cout << i << " ";
        });
        cout << endl;
    }
}
