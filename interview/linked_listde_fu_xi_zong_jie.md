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


