#include <cstdio>
#include <cstdlib>
using namespace std;
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(NULL) {}
};
class Solution
{

public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2)
    {
        ListNode* p = l1, *q = l2, *result = NULL, *last;
        int len1 = 0, len2 = 0;

        while (p && q) {
            len1++, len2++;
            p = p->next;
            q = q->next;
        }

        while (p) {
            len1++;
            p = p->next;
        }

        while (q) {
            len2++;
            q = q->next;
        }

        if (len1 >= len2) {
            p = l1;
            q = l2;
            result = l1;
        } else {
            p = l2;
            q = l1;
            result = l2;
        }

        bool carry = false;

        while (p && q) {
            p -> val = (p -> val + q -> val);

            if (carry) {
                p -> val++;
            }

            if (p->val >= 10) {
                p->val -= 10;
                carry = true;
            } else {
                carry = false;
            }

            last = p;
            p = p->next;
            q = q->next;
        }

        while (p && carry) {
            if (++(p->val) >= 10) {
                p->val -= 10;
            } else {
                carry = false;
            }

            last = p;
            p = p->next;
        }

        if (carry) {
            last->next = new ListNode(1);
        }

        return result;
    }
};
int main(int argc, char** argv)
{
    Solution solution;
    ListNode* l1 = new ListNode(5);
    ListNode* l2 = new ListNode(5);

    auto result = solution.addTwoNumbers(l1, l2);

    while (result) {
        printf("%d ", result->val);
        result = result -> next;
    }

    return 0;
}
