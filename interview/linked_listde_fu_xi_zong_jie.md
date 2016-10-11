# Linked List的複習總結


##Single Linked List
相比較另一個基本的數據結構array，linked list有幾個優勢：尺寸是可以動態分配，insert或者delete更加方便。缺點就是不可以ramdom access，並且在結構中需要分配額外空間來指示下一個pointer。

##基本操作
作為data strucutre裡面基本的數據結構Linked list有很多特殊的性質值得我們去注意,由於它是線性結構大多數linked list問題還是相對其他數據結構（如tree, graph)較容易的。由於Linked List是由一個個節點(node)相聯組成的,因此首先看一下一個node一般怎麼寫吧（這裡主要先討論的單向的鏈表，雙向鏈表或者其他變種會在之後章節討論）：



文／dol_re_mi（簡書作者）
原文鏈接：http://www.jianshu.com/p/3d4be8cbf94b
著作權歸作者所有，轉載請聯繫作者獲得授權，並標註“簡書作者”。

```c
struct Node {
    int val;
    Node* next;
    Node(int x) : val(x), next(nullptr) {}
}
```

從以上可以看出在c++中Node一般是用struct來生成，並且可以使用constructor來初始化Node，因此初始化node就跟普通的class一樣。瞭解了Node的結構之後，下面就看看怎樣insert, delete one node on the linked list.
首先來看insertion, 我們需要考慮兩種情況：1：如果原來的linked list是空的，insert需要更新第一個node(header),一般linked list由the first node (header)來表示，一旦有了第一個node的地址其他操作都可以從這裡進行; 2：如果原來的linked list是非空，insert需要更新previous node和next node的聯結關係。在這裡我們來介紹一個小技巧，對於第一個node需要update的情形（即linked list可能空的），一般可以使用dummy node來避免使用if來分情況討論，具體方法就是建立一個dummy node使得它next指向linked list的第一個node,但是需要注意的是此時函數返回值必須為Node *即pointer to Node.因此在函數的signature給定的情況下，可以寫一個wrapper函數在wrapper function中call這個函數來完成。以下是具體的insertion函數，注意這裡返回Node pointer指向header,這裡插入的位置是插入節點的值大於前一個節點並且小於後一個節點。

文／dol_re_mi（簡書作者）
原文鏈接：http://www.jianshu.com/p/3d4be8cbf94b
著作權歸作者所有，轉載請聯繫作者獲得授權，並標註“簡書作者”。

```c
Node* insert(Node* node, Node* top)
{
    Node dummy(-1);
    dummy.next = top;

    if (top == nullptr) {
        dummy.next = node;
        return dummy.next;
    }

    Node* prev = &dummy, *cur = prev->next;

    while (cur->val < node->val && cur) {
        prev = cur;
        cur = cur ->next;
    }

    if (cur == nullptr) {
        prev->next = node;
    } else {
        Node* next = cur->next;
        prev->next = node;
        node->next = next;
    }

    return dummy.next;
}
```

其實這裡可以修改一下prev和cur的起始位置（即向前在移一位），這樣可以避免討論head節點是否為空。並且需要注意此時dummy node的初始化值需要改為INT_MIN：

```c
Node* insert(Node* node, Node* top)
{
    Node dummy(INT_MIN);
    dummy.next = top;
    Node* prev = nullptr, cur = &dummy;

    while (cur && cur ->val < node->val) {
        prev = cur;
        cur = cur->next;
    }

    prev->next = node;
    node->next = cur;
    return dummy.next;
}
```

這裡需要注意while裡面首先判斷cur是否存在，再比較cur-val和node->val。

對應insertion的就是delete了，這裡同樣還是用dummy node的方法來解決，這裡刪除的是節點值等於key的所有節點，需要注意的是得一直跟蹤記錄需要刪除的前一個node:
Remove Linked List Elements

文／dol_re_mi（簡書作者）
原文鏈接：http://www.jianshu.com/p/3d4be8cbf94b
著作權歸作者所有，轉載請聯繫作者獲得授權，並標註“簡書作者”。

```c
void delete(Node* head, int key)
{
    Node dummy(-1);
    dummy.next = head;
    Node* prev = &dummy, *cur = dummy.next;

    while (cur) {
        if (cur->val == key) {
            Node* tmp = cur;
            prev->next = cur->next;
            delete tmp;
        } else {
            prev = prev->next;
        }

        cur = prev->next;
    }
}
```

以上的方法是通過改變前後的節點關係，然後刪除節點的方法特別注意刪除節點之後前驅節點不需要更新。另外注意prev和cur兩個指針需要同步更新， 如果給定要刪除的節點，可以通過copy value的方法來刪除節點：

```c
void delete(Node* n)
{
    if (n->next) {
        n->val = n->next->val;
        Node* tmp = n->next;
        n->next = n->next->next;
        delete tmp;
    } else {
        Node* tmp = n;
        n = nullptr;
        delete tmp;
    }
}
```

這裡需要注意討論下一個節點是否是空的，然後分情況進行處理。類似的方法，刪除整個linked lis如下(不需要dummy node)：

```c
void delete(Node* head)
{
    Node* cur = head;

    while (cur) {
        Node* tmp = cur;
        cur = cur->next;
        delete tmp;
    }
}
```

經典的刪除問題: 刪除重複的節點

```c
ListNode* deleteDuplicates(ListNode* head)
{
    if (head == nullptr) {
        return head;
    }

    ListNode dummy(-1);
    dummy.next = head;
    ListNode* cur = head;

    while (cur) {
        while (cur->next && cur->next->val == cur->val) {
            ListNode* tmp = cur->next;
            cur->next = cur->next->next;
            delete tmp;
        }

        cur = cur->next;
    }

    return dummy.next;
}

```
這裡需要注意一個地方在while循環裡面判斷是否有重複節點時用的是while而並非if,如果採用if就會導致重複節點超過兩個沒辦法刪除。雖然這裡採用兩個loop相互嵌套，實際上running time還是線性的，其實這道題不需要dummy node而且也不需要兩個循環嵌套，更改程序如下：




```c
ListNode* deleteDuplicate(ListNode* head)
{
    if (head == nullptr) {
        return head;
    }

    for (ListNode* prev = head, *cur = prev->next; cur; cur = cur->next) {
        if (prev->val == cur->val) {
            prev->next = cur->next;
            delete cur;
        } else {
            prev = cur;
        }
    }

    return head;
}
```

這裡不需要用dummy node, 用一個循環來遍歷整個linked list,這裡注意head其實並沒有改變因此可以直接返回。

下面增加一定難度，仍然是刪除重複節點， 但是只保留沒有重複的節點

```c
ListNode* deleteDuplicate(ListNode* head)
{
    if (head == nullptr) {
        return head;
    }

    ListNode dummy(INT_MIN);
    dummy.next = head;
    ListNode* prev = &dummy, *cur = head;

    while (cur) {
        bool duplicated = false;

        while (cur->next && cur->next->val == cur->val) {
            duplicated = true;
            ListNode* tmp = cur;
            cur = cur->next;
            delete tmp;
        }

        if (duplicated) {
            ListNode* tmp = cur;
            cur = cur->next;
            delete tmp;
            continue;
        }

        prev->next = cur;
        prev = prev->next;
        cur = cur->next;
    }

    prev->next = cur;
    return dummy.next;
}
```


如果只保留沒有重複的節點，複雜度就提高了。首先需要一個boolean值來記錄該節點是否是重複的，然後需要dummy node因為head也有可能被刪除。然後就是雙重循環在內循環中刪除當前節點，然後指向下一個節點。如果是重複的節點還需要把最後一個節點刪除，然後continue。整個循環中prev節點是不動的，他永遠指向下一個刪除過後的節點，然後再更新自己。最後不要忘記鏈接prev和cur兩個節點。

刪除的一個簡單變化，刪除alternative node採用雙指針iterative方法來處理，特別注意cur指針更新時候需要判斷prev是否是空指針：


```c
void deleteAlt(Node* head)
{
    if (head->next == nullptr) {
        return;
    }

    Node* prev = head, *cur = head->next;

    while (prev && cur) {
        prev->next = cur->next;
        delete cur;
        prev = prev->next;

        if (prev) {
            cur = prev->next;
        }
    }
}
```

Recurive方法是典型的tail-recursive的實例：

```c
void deleteAlt(Node* head)
{
    if (head == nullptr) {
        return;
    }

    Node* next = head->next;

    if (next) {
        head->next = next->next;
        delete next;
        deleteAlt(head->next);
    }
}
```

關於刪除， Delete N nodes after M nodes of linked list特別注意遍歷鏈表的時候檢查是否走到鏈表的盡頭了。

```c
void skipMdeleteN(Node* head, int M, int N)
{
    Node* cur = head;

    while (cur) {
        for (int i = 1; i < M && cur; ++i) {
            cur = cur->next;
        }

        if (cur == nullptr) {
            return;
        }

        Node* t = cur->next;

        for (int i = 1; i < N && t; ++i) {
            Node* tmp = t;
            t = t->next;
            delete t;
        }

        cur->next = t;
        cur = cur->next;
    }
}
```

另一個簡單的例子就是找到linked list第N個節點,如果沒有找到就返回一個極小值,此時區別於數組,這裡需要遍歷linked list。

```c
int getNth(Node* head, int N)
{
    Node* cur = head;
    int count = 0;

    while (cur) {
        if (count == n) {
            return cur->val;
        }

        count++;
        cur = cur->next;
    }

    return INT_MIN;
}
```

利用雙指針的方法，可以用來找到linked list的中間節點，這裡不用區分linked list是偶數個還是奇數個node(奇數和偶數的唯一區別在於while循環的終止條件：奇數為fast->next ==nullptr；偶數為fast ==nullptr)：

```c
void printMid(Node* head)
{
    Node* slow = head, *fast = head;

    if (head) {
        while (fast && fast->next) {
            fast = fast->next->next;
            slow = slow->next;
        }

        cout << slow->val << endl;
    }
}
```
類似的方法可以用於尋找距離尾部N個位置的節點：

```c
Node* findNEnd(Node* head, int n)
{
    int count  = 0;
    Node* slow = head, *fast = head;

    while (count < n) {
        if (fast == nullptr) {
            return nullptr;
        }

        fast = fast->next;
        count++;
    }

    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }

    return slow;
}
```

這裡需要注意的是在第一個while循環裡面，需要判斷一下fast是否已經到達鏈表的尾部，即n的值大於linked list的長度時候需要返回nullptr.

##改變結構
類似於array，linked list也可以進行rotate,但是相對於array，鏈表需要進行遍歷找到改變的節點位置。Rotate a linked list

```c
Node* rotate(Node* head, int k)
{
    if (head == nullptr) {
        return head;
    }

    Node* cur = head, *new, *last;

    for (int i = 1; i < k && cur; ++i) {
        cur = cur->next;
    }

    new = cur->next;
    last = new;
    cur->next = nullptr;

    while (last->next) {
        last = last->next;
    }

    last->next = head;

    return new;
}
```

這裡有個地方要注意，需要檢查k是否超過了linked list的總長度，也就是在循環中加一個條件cur != nullptr.
對於linked list有很多問題圍繞在改變原來linked list的node之間的順序，比如下面這道題：將原來的linked list中在偶數位置的節點按照倒序方式接在linked list的末尾。

```c
void modify(Node* head)
{
    if (head == nullptr || head->next == nullptr || head->next->next == nullptr) {
        return;
    }

    Node* first = head, *second = first->next;
    first->next = first->next->next;
    first = first->next;
    second->next = nullptr;

    while (first && first->next) {
        Node* tmp = first->next;
        first->next = first->next->next;
        tmp->next = second;
        second = tmp;
        first = first->next;
    }

    first->next = second;
}
```

這裡需要注意幾個問題，首先如果linked list不超過三個節點那麼沒有任何節點的順序需要改變，然後就是需要雙指針一個用來記錄當前奇數位的節點另一個用來記錄偶數位的節點，注意偶數位的節點需要用倒序的方式添加新的節點，整個過程像構建了兩個linked list最後在頭尾相連完成整個函數的構建。如果是對於原來的linked list倆倆互換的情況呢？
Swap Nodes in Pairs
第一種偷懶的方法就是直接互換節點的數值：

```c
Node* pairswap(Node* head)
{
    Node* p = head;

    while (p && p->next) {
        swap(p - val, p->next->val);
        p = p->next->next;
    }
}
```

下面看如何倆倆互換節點的指針來解決問題,首先討論特殊情形，即一共有少於兩個節點，那麼只要返回原linked list就可以了。接著創建一個dummy node,接著需要三個node, prev, cur, next，每次需要互換cur和next,之後在update三個節點。在更新next節點時候需要判斷cur是否存在，如果不存在則設為nullptr,因此整個循環的判斷也是next是否為nullptr。

```c
Node* pairswap(Node* head)
{
    if (head == nullptr || head->next == nullptr) {
        return head;
    }

    Node dummy(-1);
    dummy.next = head;

    for (Node* prev = &dummy, *cur = prev->next, *next = cur->next; next;
         prev = cur,
         cur = cur->next, next = cur ? cur->next : nullptr) {
        prev->next = next;
        cur->next = next->next;
        next->next = cur;
    }

    return dummy.next;
}
```

在上面的問題基礎上，我們看一下如何對一個linked list反轉，對於array反轉是很容易的，因為array可以直接對每一對頭尾的元素倆倆交換從兩頭向中間靠攏，array查找是常數時間複雜度。但是對於singly linked list只有單向的遍歷因此如果需要向前遍歷則需要從頭開始遍歷。鑑於以上情形，linked list的反轉需要三個節點, prev指向的最前面的那個節點， cur指向當前節點或者中間的節點，next指向的是下一個節點。首先我們先看一下迭代的方法，總體來說就是在循環裡面讓cur->next指向prev, 這樣prev用cur來代替，然後cur用next來替換。整個循環的作用就是把prev node和current node之間的前後關係顛倒過來，並且current node和next node之間的連接關係斷開。注意這裡不可以使用dummy node的技術，因為reverse之後dummy node會變為最後一個node這樣比原來的linked list要多出一個節點。

```c
Node* reverse(Node* head)
{
    Node* prev = nullptr, *cur = head, *next;

    while (cur) {
        next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }

    return prev;
}
```

在看一下遞歸方法：

```c
void reverse(Node** head)
{
    Node* first, *rest;

    if (*head == nullptr || (*head)->next == nullptr) {
        return;
    }

    first = *head;
    rest = first->next;

    recursive(&rest);
    first->next->next = first;
    first->next = nullptr;

    *head = first;
}
```

遞歸方法中把linked list分為兩段，head和其他部分，其他部分作為新的head傳入到recursive function裡面，之後在顛倒head及其後面節點的順序，最後再update head node。 在以上的基礎上，還可以進一步的簡化遞歸過程如下：

```c
Node* reverse(Node* cur, Node* prev)
{
    if (cur == nullptr) {
        return prev;
    }

    Node* last = reverse(cur->next, cur);
    cur->next = prev;
    return last;
}
```

接著通過擴展前面倆倆交換的例子現在變為以k個node為一個單位reverse linked list.

```c
Node* reverseKGroup(Node* head, int k)
{
    if (head == nullptr || head->next == nullptr || k < 2) {
        return head;
    }

    Node dummy(-1);
    dummy.next = head;

    for (Node* prev = &dummy, end = head; end; end = prev->next) {
        for (int i = 1; i < k && end; ++i) {
            end = end->next;
        }

        if (end == nullptr) {
            break;
        }

        prev = reverse(prev, prev->next, end);
    }

    return dummy.next;
}

Node* reverse(Node* prev, Node* begin, Node* end)
{
    Node* end_next = end->next;

    for (Node* p = prev, *cur = prev->next, *next = cur->next; cur != end_next;
         p = cur, cur = next, next = next ? next->next : nullptr) {
        cur->next = p;
    }

    begin->next = end->next;
    prev->next = end;
    return begin;
}
```

這裡有幾個需要注意的地方： 如果k<2或者linked list的節點數不超過兩個， 原linked list保持不變；在循環中始終需要查看end節點是否存在，如果end為空表明指針走到了linked list的尾端整個循環應該停止；這裡的reverse函數與之前的reverse linked list本質是一樣的，只是在循環中加了關於結尾指針的判斷cur != end_next（之前只需要判斷cur != nullptr，因為cur在整個linked list的最後一個節點必然是nullptr). 下面把問題稍微增加一點複雜度: Reverse alternative K nodes in a Singly linked list.

```c
Node* kAltReverse(Node* head, int k)
{
    if (head == nullptr || head->next == nullptr || k < 2) {
        return head;
    }

    Node dummy(-1);
    dummy.next = head;

    for (Node* prev = &dummy, *end = head, int i = 0; end; end = prev->next) {
        for (int j = 1; j < k && end ; ++j) {
            end = end->next;
            ++i;
        }

        if (end == nullptr) {
            break;
        }

        if (i % (2 * k) > 0) {
            prev = reverse(prev, prev->next, end);
        } else {
            prev = end;
        }
    }

    return dummy.next;
}

Node* reverse(Node* prev, Node* begin, Node* end)
{
    Node* end_next = end->next;

    for (Node* p = prev, *cur = prev->next, *next = cur->next; cur != end_next;
         p = cur, cur = p->next; next = next ? next->next; nullptr) {
        cur->next = p;
    }

    begin->next = end_next;
    prev->next = end;
    return begin;
}
```

這道題增加了一個判斷條件`i%(2*k)`來判斷是否需要reverse,另外需要注意的是當不需要reverse時候prev指針的更新。接著看這道題: 給定一個linked list對於偶數位的node reverse並且接在奇數位的node之後。

```c
void modify(Node* head)
{
    if (head == nullptr || head->next == nullptr || head->next->next == nullptr) {
        return;
    }

    Node* first = head, *second = head->next;
    first->next = first->next->next;
    first  = first->next;
    second ->next = nullptr;

    while (first && first - next) {
        Node* tmp = first->next;
        first->next = first->next->next;
        tmp->next = second;
        second = tmp;
    }

    first->next = second;
}
```

這裡用兩個pointer分別記錄偶數位的node和奇數位的node,對於偶數位的reverse沒有按照之前的方法用三個指針來做，而是把遍歷到新的偶數位node添加在目前的偶數pointer(second)的前面,這樣可以做到reverse on the fly一次遍歷就可以把整個問題解決。接下來看一個綜合以上的幾個題目的題目，判斷linked list是否是palindrome.最直觀的方法就是用一個stack，然後遍歷整個linked list,push every node到stack裡面，然後做第二遍遍歷時候pop stack然後與當前linked list裡面的node進行比較，如果遍歷下來所有node都一致那麼就是palindrome反之就不是。下面再看一個解法：


```c
bool isPalindrome(Node* head)
{
    Node* slow = head, *fast = head;
    Node* second , *prev_slow = head;
    Node* mid_node = nullptr;

    bool res = true;

    if (head == nullptr || head->next == nullptr) {
        return res;
    }

    while (fast && fast->next) {
        prev_slow = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    if (fast) {
        mid_node = slow;
        slow = slow->next;
    }

    second = slow;
    prev_slow ->next = nullptr;
    reverse(&second);
    res = compare(head, second);

    reverse(&second);

    if (mid_node) {
        prev_slow->next = mid_node;
        mid_node->next = second;
    }

    prev_slow->next = second;
    return res;
}

void reverse(Node** head)
{
    Node* prev = nullptr, *cur = *head, *next;

    while (cur) {
        next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }

    *head = prev;
}

bool compare(Node* first, Node* second)
{
    Node* p1 = first, *p2 = second;

    while (p1 && p2) {
        if (p1->val == p2->val) {
            p1 = p1->next;
            p2 = p2->next;
        } else {
            return false;
        }
    }

    if (p1 == nullptr && p2 == nullptr) {
        return true;
    }

    return false;
}
```

這個解法結合了上面的linked list reverse和雙指針方法尋找mid point，但是由於在reverse linked list過程中破壞了原來linked list的結構，因此需要再一次reverse進行復原，在時間消耗上是比較費事的。另外在compare這個函數中，最後採用判斷p1和p2是否同時為空比較巧妙， compare函數本身可以作為一個經典的兩個linked list比較問題。 為了避免改變linked list的原來結構，可以採用雙指針遞歸的方法：

```c
bool isPalindromUnit(Node*& left, Node* right)
{
    if (right == nullptr) {
        return true;
    }

    bool isp = isPalindromUnit(left, right->next);

    if (!isp) {
        return false;
    }

    bool ispl = (right->val == left->val);
    left = left->next;

    return ispl;
}

bool isPalindrom(Node* head)
{
    isPalindrom(head, head);
}
```

這個解法有幾個注意的地方，第一個就是base case的討論，如果right pointer已經到達linked list的尾部（==nullptr）那麼返回true,另外left pointer需要pass by reference這裡用的是*&也可以用pointer to pointer來表示。最後一點這裡不是tail-recursive call，算上遞歸棧空間是O(n).
Linked List問題有時候可以增加一些複雜性，但是本質上還是與傳統的問題是一致的，比如說這道題Given a linked list of line segments, remove middle points, 具體的要求可以查看鏈接，這道題的核心是需要三個節點來判斷中間節點是否需要刪除。


```c
Node* removeMid(Node* head)
{
    Node* cur = head;

    while (cur && cur->next && cur->next->next) {
        Node* mid = cur->next;
        Node* last = mid->next;

        if (mid->x == last->x && cur->x == mid->x || cur->y == mid->y &&
            mid->y == last->y) {
            cur->next = last;
            delete mid;
        } else if (cur->x == mid->x && mid->x != last->x) || (cur->y == mid->y &&
                    mid->y != last->y) {
            cur->last;
        } else {
            cout << "Invalid" << endl;
            eixt(1);
        }
    }

    return head;
}
```

接下來看一道題對於結構變化的linked list,怎樣判斷一個linked list是否有loop?

```c
bool hasCycle(ListNode* head)
{
    ListNode* slow = head, *fast = head;

    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;

        if (slow == fast) {
            return true;
        }
    }

    return false;
}
```

Follow up, 如果需要返回cycle開始的節點呢?

```c
ListNode* detectCycle(ListNode* head)
{
    ListNode* slow = head, *fast = head;

    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;

        if (slow == fast) {
            slow = head;

            while (slow != fast) {
                slow = slow->next;
                fast = fast->next;
            }

            return slow;
        }

        return nullptr;
    }
}
```
還有一道綜合題，結合了reverse linked list和節點刪除的技巧
delete nodes which have a greater value on right side:當然永遠可以使用暴力的解法採用雙重循環檢查每一個節點看是否有右邊的節點擁有更大的數值，但是複雜度將達到O(n^2),下面的解法可以達到線性計算時間。


```c
void delLesserNodes(Node** head_ref)
{
    reverseList(head_ref);
    _delLesserNode(*head_ref);
    reverseList(head_ref);
}

void reverseList(Node** head)
{
    Node* cur = head, *prev, *next;

    while (cur) {
        next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }

    *head = prev;
}

void _delLesserNode(Node* head)
{
    Node* current = head, maxNode = head;
    Node* tmp;

    while (current && current->next) {
        if (current->next->val < maxNode->val) {
            tmp = current->next;
            current->next = tmp->next;
            delete tmp;
        } else {
            current = current->next;
            maxnode = current;
        }
    }
}
```

首先需要reverse linked list，然後一個個比較最大節點與當前節點的數值，如果當前節點的數值小則刪除當前節點，最後還原linked list的結構。
當linked list中的value是特殊的數值時，排序linked list也可以採用特殊的方法。比如linked list只含有0, 1 ,2數值，排序linked list.

```c
void sortList(Node* head)
{
    vector<int> tmp(3, 0);
    Node* cur = head;

    while (cur) {
        if (cur->val == 0) {
            tmp[0]++;
        } else if (cur->val == 1) {
            tmp[1]++;
        } else {
            tmp[2]++;
        }

        cur = cur->next;
    }

    cur = head;

    while (cur) {
        if (tmp[i] == 0) {
            ++i;
        } else {
            cur->data = i;
            --tmp[i];
            cur = cur->next;
        }
    }
}
```

這裡採用vector相當於一個hash table記錄0,1, 2的個數，code還可以寫的更簡潔一些：


```c
void sortList(Node* head)
{
    vector<int> count(3, 0);
    Node* ptr = head;

    while (ptr) {
        count[ptr->val]++;
        ptr = ptr->next;
    }

    int i = 0;
    ptr = head;

    while (ptr) {
        if (count[i] == 0) {
            ++i;
        } else {
            ptr->val = i;
            --count[i];
            ptr = ptr->next;
        }
    }
}
```
##兩個或者多個linked list的問題

首先看一下如何將一個linked list分成兩個：

```c
void AltSplit(ListNode* source, ListNode** a, ListNode** b)
{
    ListNode dummyA(-1);
    ListNode dummyB(-1);
    ListNode* cur = source, *ta = &dummyA, *tb = &dummyB;

    while (cur && cur->next) {
        ta->next = cur;
        tb->next = cur->next;
        ta = ta->next;
        tb = tb->next;
    }

    if (cur) {
        ta->next = cur;
    }

    *a = dummyA.next;
    *b = dummyB.next;
}
```

Merge 兩個 sorted linked list, 由於linked list本身已經排好序，可以通過比較兩個list node的值大小來merge,若節點為空則設置為無窮大。


```c
ListNode* mergeList(ListNode* a, ListNode* b)
{
    ListNode dummy(-1);

    for (ListNode* cur = &dummy; a || b; cur = cur->next) {
        int valA = (a == nullptr) ? INT_MAX : a->val;
        int valB = (b == nullptr) ? INT_MAX : b->val;

        if (valA < valB) {
            cur->next = a;
            a = a->next;
        } else {
            cur->next = b;
            b = b->next;
        }
    }

    return dummy.next;
}
```

當然也可以通過一個個確認節點是否為空來merge:


```c
ListNode* mergeList(ListNode* a, ListNode* b)
{
    if (a == nullptr) {
        return b;
    }

    if (b == nullptr) {
        return a;
    }

    ListNode dummy(-1);
    ListNode* result = &dummy;

    for (; a && b; result = result->next) {
        if (a->val < b->val) {
            result->next = a;
            a = a->next;
        } else {
            result->next = b;
            b = b->next;
        }
    }

    result->next = (a ? a : b);
    return dummy.next;
}
```

對於兩個list，經典題型有尋找兩個linked list的intersection


```c

```
