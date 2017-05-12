#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>
#include <queue>
#include <stack>
using namespace std;
struct TreeLinkNode {
    int val;
    TreeLinkNode* left;
    TreeLinkNode* right;
    TreeLinkNode* next;
    TreeLinkNode(int x) : val(x), left(nullptr), right(nullptr), next(nullptr) {}
};
struct Bucket {
    TreeLinkNode* node;
    int level;
    Bucket(TreeLinkNode* p, int l) : node(p), level(l) {}
};
class Solution
{
public:
    void connect(TreeLinkNode* root)
    {
        if (root == nullptr) {
            return;
        }

        TreeLinkNode* p = root;

        while (p->left) {
            TreeLinkNode* q = p;

            while (q) {
                q->left->next = q->right;

                if (q->next) {
                    q->right->next = q->next->left;
                }

                q = q->next;
            }

            p = p->left;
        }
    }
    /*
    void connect(TreeLinkNode *root) {
        if (root == nullptr)
            return;
        queue<Bucket> q;
        q.push(Bucket(root, 0));
        while (!q.empty()) {
            Bucket current = q.front();
            TreeLinkNode *p = current.node;
            int level = current.level;
            q.pop();
            if (!q.empty()) { // 注意检查是否还有节点
                Bucket next = q.front();
                if (current.level == next.level) {
                    p->next = next.node;
                }
            }
            if (p->left)
                q.push(Bucket(p->left, level + 1));
            if (p->right)
                q.push(Bucket(p->right, level + 1));
        }
    }
    */
};
TreeLinkNode* mk_node(int val)
{
    return new TreeLinkNode(val);
}
TreeLinkNode* mk_child(TreeLinkNode* root, TreeLinkNode* left,
                       TreeLinkNode* right)
{
    root->left = left;
    root->right = right;
    return root;
}
TreeLinkNode* mk_child(TreeLinkNode* root, int left, int right)
{
    return mk_child(root, new TreeLinkNode(left), new TreeLinkNode(right));
}
TreeLinkNode* mk_child(int root, int left, int right)
{
    return mk_child(new TreeLinkNode(root), new TreeLinkNode(left),
                    new TreeLinkNode(right));
}
int main(int argc, char** argv)
{
    Solution solution;
    TreeLinkNode* root = mk_child(0, 1, 2);
    mk_child(root->left, 3, 4);
    mk_child(root->right, 5, 6);
    solution.connect(root);
    return 0;
}
