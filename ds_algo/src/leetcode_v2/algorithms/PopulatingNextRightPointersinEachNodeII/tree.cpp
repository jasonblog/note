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

        while (p) {
            TreeLinkNode* q = p;
            TreeLinkNode* t1 = getNext(q);
            TreeLinkNode* t2;

            while (t1) {
                if (t1 == q->left) { // 如果t1是左孩子
                    t2 = q->right; // 令t2是它的右孩子，（可能为null）
                } else {
                    t2 = nullptr; // 否则t1已经是右孩子，t2必须从下一个节点寻找
                }

                if (t2 == nullptr) {
                    q = q->next; // 从下一个节点寻找t2
                    t2 = getNext(q);
                }

                if (t2 == nullptr) { // t2为null，说明没有找到最右相邻节点
                    break;
                }

                t1->next = t2; // 更新t1的next指针指向t2
                t1 = t2;
            }

            p = getNext(
                    p); // 处理下一层节点，注意下一层的最初节点就是p的下一层节点的最左节点，即getNext(p)
        }
    }
    void print(TreeLinkNode* root)
    {
        if (root == nullptr) {
            return;
        }

        TreeLinkNode* p = root;

        while (p) {
            printf("%d ", p->val);
            p = p->next;
        }

        printf("\n");
        root = getNext(root);
        print(root);
    }
private:
    TreeLinkNode* getNext(TreeLinkNode*& p)
    {
        if (p == nullptr) {
            return nullptr;
        }

        TreeLinkNode* next = getChild(p);

        if (next) {
            return next;
        }

        p = p->next;
        return getNext(p);
    }
    TreeLinkNode* getChild(TreeLinkNode* p)
    {
        return p->left ? p->left : p->right;
    }
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
    mk_child(root->left, mk_node(3), nullptr);
    mk_child(root->right, nullptr, mk_node(4));
    mk_child(root->left->left, 5, 6);
    mk_child(root->right->right, 7, 8);
    solution.connect(root);
    solution.print(root);
    return 0;
}
