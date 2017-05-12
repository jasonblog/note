#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};
static inline bool isLeaf(TreeNode* root)
{
    return root->left == NULL && root->right == NULL;
}
class Solution
{
public:
    int sumNumbers(TreeNode* root)
    {
        return sumNumbers(root, 0);
    }
    int sumNumbers1(TreeNode* root)
    {
        if (root == NULL) {
            return 0;
        }

        vector<int> numbers;
        getNumbers(numbers, root, 0);
        int sum = 0;
        for_each(numbers.begin(), numbers.end(), [&sum](int value) {
            sum += value;
        });
        return sum;
    }
private:
    int sumNumbers(TreeNode* root, int sum)
    {
        if (root == NULL) {
            return 0;
        }

        sum = sum * 10 + root->val;

        if (isLeaf(root)) {
            return sum;
        } else {
            return sumNumbers(root->left, sum) + sumNumbers(root->right, sum);
        }
    }
    void getNumbers(vector<int>& result, TreeNode* root, int number)
    {
        if (root == NULL) {
            return;
        }

        number *= 10;
        number += root->val;

        if (isLeaf(root)) {
            result.push_back(number);
        } else {
            getNumbers(result, root->left, number);
            getNumbers(result, root->right, number);
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
int main(int argc, char** argv)
{
    Solution solution;
    TreeNode* root = mk_child(mk_node(1), mk_node(0), nullptr);
    cout << solution.sumNumbers(root) << endl;
    return 0;
}
