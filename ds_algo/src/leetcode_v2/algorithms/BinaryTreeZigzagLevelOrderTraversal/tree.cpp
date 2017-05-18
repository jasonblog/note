#include <queue>
#include <deque>
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
    vector<vector<int>> zigzagLevelOrder(TreeNode* root)
    {
        vector<vector<int>> result;

        if (root == nullptr) {
            return result;
        }

        deque<bucket> q;
        q.push_back(bucket(root, 0));
        int level = 0;
        result.push_back(vector<int>());

        while (!q.empty()) {
            //Bucket b;
            //auto b = q.front();
            //q.pop();
            //auto p = b.node;
            //result[level].push_back(p->val);
            if (level & 0x1) {
                auto b = q.back();
                q.pop_back();
                auto p = b.node;
                result[level].push_back(p->val);

                if (p->left) {
                    q.push_back(bucket(p->left, level + 1));
                }

                if (p->right) {
                    q.push_back(bucket(p->right, level + 1));
                }
            } else {
                auto b = q.front();
                q.pop_front();
                auto p = b.node;
                result[level].push_back(p->val);

                if (p->right) {
                    q.push_back(bucket(p->right, level + 1));
                }

                if (p->left) {
                    q.push_back(bucket(p->left, level + 1));
                }
            }

            if (!q.empty() && q.front().level != level) {
                result.push_back(vector<int>());
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
    TreeNode* root = new TreeNode(3);
    mk_children(root, 9, 20);
    mk_children(root->right, 15, 7);
    //mk_children(root->right, new TreeNode(4), nullptr);
    auto result = solution.zigzagLevelOrder(root);

    for (auto v : result) {
        for_each(v.begin(), v.end(), [](int i) {
            cout << i << ' ';
        });
        cout << endl;
    }

    return 0;
}
