#include <cstdlib>
using namespace std;
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};
static inline int abs(int a, int b)
{
    return a > b ? a - b : b - a;
}
class Solution
{
public:
    bool isBalanced(TreeNode* root)
    {
        bool unbalenced = false;
        getHeight(unbalenced, root);
        return !unbalenced;
    }
    int getHeight(bool& unbalenced, TreeNode* root)
    {
        if (root == NULL || unbalenced) {
            return -1;
        }

        int left = getHeight(unbalenced, root->left);
        int right = getHeight(unbalenced, root->right);

        if (abs(left, right) > 1) {
            unbalenced = true;
        }

        return 1 + (left > right ? left : right);
    }

};
int main(int argc, char** argv)
{
    return 0;
}
