#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <queue>
#include <stack>
using namespace std;
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};
struct Bucket {
    TreeNode* node;
    int level;
    Bucket(TreeNode* p, int l) : node(p), level(l) {}
};
class Solution
{
public:
    vector<vector<int>> levelOrder(TreeNode* root)
    {
        vector<vector<int>> result;

        if (root == nullptr) {
            return result;
        }

        queue<Bucket> q;
        result.push_back(vector<int>());
        int curLevel = 0;
        q.push(Bucket(root, 0));

        while (!q.empty()) {
            Bucket bucket = q.front();
            q.pop();
            TreeNode* p = bucket.node;
            int level = bucket.level;

            if (level != curLevel) {
                result.push_back(vector<int>());
                curLevel++;
            }

            result[curLevel].push_back(p->val);

            if (p->left) {
                q.push(Bucket(p->left, level + 1));
            }

            if (p->right) {
                q.push(Bucket(p->right, level + 1));
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
    TreeNode* root = mk_child(1, 2, 3);
    mk_child(root->left, 4, 5);
    auto results = solution.levelOrder(root);

    for (auto result : results) {
        for_each(result.begin(), result.end(), [](int i) {
            cout << i << " ";
        });
        cout << endl;
    }

    return 0;
}
