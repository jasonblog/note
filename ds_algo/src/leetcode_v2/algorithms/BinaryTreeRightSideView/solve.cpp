#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};
typedef struct bucket {
    TreeNode* node;
    int level;
    bucket(TreeNode* n, int l): node(n), level(l) {}
} bucket, *Bucket;
class Solution
{
public:
    vector<int> rightSideView(TreeNode* root)
    {
        vector<int> result;

        if (root == nullptr) {
            return result;
        }

        queue<bucket> q;
        q.push(bucket(root, 0));
        int level = 0;

        while (!q.empty()) {
            auto b = q.front();
            q.pop();
            auto p = b.node;

            if (p->left) {
                q.push(bucket(p->left, level + 1));
            }

            if (p->right) {
                q.push(bucket(p->right, level + 1));
            }

            if (q.empty() || q.front().level != level) {
                result.push_back(p->val);
                level = q.front().level;
            }
        }

        return result;
    }
};
void mk_children(TreeNode* root, int left, int right)
{
    root->left = new TreeNode(left);
    root->right = new TreeNode(right);
}
void mk_children(TreeNode* root, TreeNode* left, TreeNode* right)
{
    root->left = left;
    root->right = right;
}
int main(int argc, char** argv)
{
    Solution solution;
    TreeNode* root = new TreeNode(1);
    mk_children(root, 2, 3);
    mk_children(root->left, nullptr, new TreeNode(5));
    //mk_children(root->right, new TreeNode(4), nullptr);
    auto result = solution.rightSideView(root);
    for_each(result.begin(), result.end(), [](int i) {
        cout << i << ' ';
    });
    cout << endl;
    return 0;
}
