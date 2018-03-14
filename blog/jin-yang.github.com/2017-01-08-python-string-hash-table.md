---
title: Python 字典對象實現
layout: post
comments: true
language: chinese
category: [python]
keywords: python,dictionary,字典,hash table,哈希表,衝突
description: 在 Python 中有一個字典，可以看作是一個 Key Value 對，其代碼是通過哈希表實現，也就是說，字典是一個數組，而數組的索引是鍵經過哈希函數處理後得到的。Python 字典是用哈希表 (hash table) 實現，哈希表是一個數組，它的索引是對鍵運用哈希函數計算求得的。一個好的哈希函數會將衝突數量降到最小，將各個值均勻分佈到數組中，而 Python 中的哈希函數 (主要用於字符串和整數) 很常規：衝突時採用開放尋址，相比鏈表來說，其 CPU Cache 的命中率更高。這裡簡單結合 Python 中 Hash 函數的實現。
---

在 Python 中有一個字典，可以看作是一個 Key Value 對，其代碼是通過哈希表實現，也就是說，字典是一個數組，而數組的索引是鍵經過哈希函數處理後得到的。

Python 字典是用哈希表 (hash table) 實現，哈希表是一個數組，它的索引是對鍵運用哈希函數計算求得的。

這裡簡單結合 Python 中 Hash 函數的實現。

<!-- more -->

## 字典使用

一個好的哈希函數會將衝突數量降到最小，將各個值均勻分佈到數組中，不過，對於 Python 中的哈希函數 (主要用於字符串和整數) 很常規，衝突時採用開放尋址法，相比於鏈表來說，其 CPU Cache 的命中率更高。

如下是經常的使用方法。

{% highlight text %}
----- 創建初始化字典對象
info = {"name" : "foobar", "gender": "male"}
info = dict(name = 'foobar', gender = 'male')

----- 對於第二種方式，在如下場景時可能會出現隱藏的bug
key = 'name'
info = { key :'foobar'}     # {'name':'foobar'}
info = dict(key = 'foobar') # {'key': 'foobar'}

----- 可以通過fromkeys函數進行初始化，值默認是None，也可以通過第二個參數指定默認值
info = {}.fromkeys(['name', 'gender'])              # {'gender' : None, 'name' : None}
info = dict().fromkeys(['name', 'gender'])          # {'gender' : None, 'name' : None}
info = {}.fromkeys(['name', 'gender'], 'invalid')   # {'gender' : 'invalid', 'name' : 'invalid'}

----- 獲取值
print info['name']                 # 不存在時會觸發KeyError異常
print info.get('name')             # 不存在返回None而非異常
print info.get('notexist', 'oops') # 不存在時返回指定的默認值

----- 更新、添加、刪除
info['name'] = 'kidding'
info.update({'name':'kidding', 'gender':'female'})
info.update(name='kidding', blog='female')
del info['name']                   # 或者info.pop('name')

d.keys()
for key, value in info.items():
    print key, ':',  value
{% endhighlight %}

如上，key 可以是 int 和 string 的混合。

### setdefault

Python 地址中有一個 `setdefault` 函數，主要是用於獲取信息，如果獲取不到就按照它的參數設置該值。

{% highlight python %}
a = { "key": "hello world" }  
a.setdefault("key", "456"))   # 因為之前已經設置了key對應的值，此時不會設置

a.setdefault("key_sth", "123"))   # 之前沒有設置，此時會設置
{% endhighlight %}

## 源碼解析

實際上，計算的 Hash 值，可以通過如下的函數進行計算。

{% highlight text %}
>>> map(hash, (0, 1, 2, 3))
[0, 1, 2, 3]
>>> map(hash, ("namea", "nameb", "namec", "named"))
[-6456208310023038713, -6456208310023038716, -6456208310023038715, -6456208310023038718]
{% endhighlight %}

準確來說，是 CPython 中的實現，其對應的結構體如下。

{% highlight c %}
typedef struct {
    Py_ssize_t me_hash;
    PyObject *me_key;
    PyObject *me_value;
} PyDictEntry;

struct _dictobject {
    PyObject_HEAD
    Py_ssize_t ma_fill;          // Active+Dummy
    Py_ssize_t ma_used;          // Active
    Py_ssize_t ma_mask;          // 總共有ma_mask+1個slots，可以通過key_hash&ma_mask得到對應的slot
    PyDictEntry *ma_table;       // 保存的hash表
    PyDictEntry *(*ma_lookup)(PyDictObject *mp, PyObject *key, long hash);
    PyDictEntry ma_smalltable[PyDict_MINSIZE];
};
{% endhighlight %}


<!--
Active, Unused, Dummy。
Unused:me_key == me_value == NULL，即未使用的空閒狀態。
Active:me_key != NULL, me_value != NULL，即該entry已被佔用
Dummy:me_key == dummy, me_value == NULL。

哈希探測結束的條件是探測到一個Unused的entry。但是dict操作中必定會有刪除操作，如果刪除時僅把Active標記成Unused，顯然該entry之後的所有entry都不可能被探測到，所以引入了dummy結構。遇到dummy就說明當前entry處於空閒狀態，但探測不能結束。這樣就解決了刪除一個entry之後探測鏈斷裂的問題。

PyDict_New() 創建新字典對象
 |-PyString_FromString() 第一次會初始化dummy對象
 |-PyObject_GC_New() 如果沒有緩存，則通過該函數創建一個

Dict 對象的插入
字典對象的插入實際是通過 PyDict_SetItem() 函數完成，簡單來說就是，如果不存在 Key-Value 則插入，存在則覆蓋；基本的處理步驟如下：

1. 通過 ma_lookup 所指向的函數得到 key 所對應的 entry，該函數對於字符串來說是 lookdict_string()，整形是 lookdict()；
2. 返回的值分為如下幾種場景：
   * me_key = NULL 空不存在，可以直接使用；
   * me_key = key 對應的值已經存在，可以直接返回；
   * me_hash = hash && 字符串相同，不同的 key 對象，但是值相同，同樣認為相同；
   * me_key = dummy 對應的值已經刪除；
   * 衝突，通過如下方式探測。

PyDict_SetItem()
 |-PyString_CheckExact()  如果是string對象，那麼實際會通過string_hash計算hash值
 |-PyObject_Hash()  否則是int類型，則通過int_hash計算hash值
 |-dict_set_item_by_hash_or_entry() 其中entry為0，也就是通過hash添加，會判斷是否調整大小
   |-insertdict()  實際調用這裡的接口
   | |-ma_lookup() 通過該指針指向的對象查找，一般默認為lookdict_string()函數
   | |-insertdict_by_entry()
   |-insertdict_by_entry()
   |-dictresize() 只有在插入的時候會調整字典的大小

TODO:
  校驗下，循環中可以替換，但是無法新增或者刪除。
-->

## 參考

詳細可以參考 [深入 Python 字典的內部實現](http://python.jobbole.com/85040/) 以及其原文 [Python dictionary implementation](http://www.laurentluce.com/posts/python-dictionary-implementation/) 。

<!--
摘抄出來的實現
http://www.cnblogs.com/xiangnan/p/3859578.html
-->


{% highlight text %}
{% endhighlight %}
