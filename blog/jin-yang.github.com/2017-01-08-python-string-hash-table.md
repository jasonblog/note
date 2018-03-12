---
title: Python 字典对象实现
layout: post
comments: true
language: chinese
category: [python]
keywords: python,dictionary,字典,hash table,哈希表,冲突
description: 在 Python 中有一个字典，可以看作是一个 Key Value 对，其代码是通过哈希表实现，也就是说，字典是一个数组，而数组的索引是键经过哈希函数处理后得到的。Python 字典是用哈希表 (hash table) 实现，哈希表是一个数组，它的索引是对键运用哈希函数计算求得的。一个好的哈希函数会将冲突数量降到最小，将各个值均匀分布到数组中，而 Python 中的哈希函数 (主要用于字符串和整数) 很常规：冲突时采用开放寻址，相比链表来说，其 CPU Cache 的命中率更高。这里简单结合 Python 中 Hash 函数的实现。
---

在 Python 中有一个字典，可以看作是一个 Key Value 对，其代码是通过哈希表实现，也就是说，字典是一个数组，而数组的索引是键经过哈希函数处理后得到的。

Python 字典是用哈希表 (hash table) 实现，哈希表是一个数组，它的索引是对键运用哈希函数计算求得的。

这里简单结合 Python 中 Hash 函数的实现。

<!-- more -->

## 字典使用

一个好的哈希函数会将冲突数量降到最小，将各个值均匀分布到数组中，不过，对于 Python 中的哈希函数 (主要用于字符串和整数) 很常规，冲突时采用开放寻址法，相比于链表来说，其 CPU Cache 的命中率更高。

如下是经常的使用方法。

{% highlight text %}
----- 创建初始化字典对象
info = {"name" : "foobar", "gender": "male"}
info = dict(name = 'foobar', gender = 'male')

----- 对于第二种方式，在如下场景时可能会出现隐藏的bug
key = 'name'
info = { key :'foobar'}     # {'name':'foobar'}
info = dict(key = 'foobar') # {'key': 'foobar'}

----- 可以通过fromkeys函数进行初始化，值默认是None，也可以通过第二个参数指定默认值
info = {}.fromkeys(['name', 'gender'])              # {'gender' : None, 'name' : None}
info = dict().fromkeys(['name', 'gender'])          # {'gender' : None, 'name' : None}
info = {}.fromkeys(['name', 'gender'], 'invalid')   # {'gender' : 'invalid', 'name' : 'invalid'}

----- 获取值
print info['name']                 # 不存在时会触发KeyError异常
print info.get('name')             # 不存在返回None而非异常
print info.get('notexist', 'oops') # 不存在时返回指定的默认值

----- 更新、添加、删除
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

Python 地址中有一个 `setdefault` 函数，主要是用于获取信息，如果获取不到就按照它的参数设置该值。

{% highlight python %}
a = { "key": "hello world" }  
a.setdefault("key", "456"))   # 因为之前已经设置了key对应的值，此时不会设置

a.setdefault("key_sth", "123"))   # 之前没有设置，此时会设置
{% endhighlight %}

## 源码解析

实际上，计算的 Hash 值，可以通过如下的函数进行计算。

{% highlight text %}
>>> map(hash, (0, 1, 2, 3))
[0, 1, 2, 3]
>>> map(hash, ("namea", "nameb", "namec", "named"))
[-6456208310023038713, -6456208310023038716, -6456208310023038715, -6456208310023038718]
{% endhighlight %}

准确来说，是 CPython 中的实现，其对应的结构体如下。

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
    Py_ssize_t ma_mask;          // 总共有ma_mask+1个slots，可以通过key_hash&ma_mask得到对应的slot
    PyDictEntry *ma_table;       // 保存的hash表
    PyDictEntry *(*ma_lookup)(PyDictObject *mp, PyObject *key, long hash);
    PyDictEntry ma_smalltable[PyDict_MINSIZE];
};
{% endhighlight %}


<!--
Active, Unused, Dummy。
Unused:me_key == me_value == NULL，即未使用的空闲状态。
Active:me_key != NULL, me_value != NULL，即该entry已被占用
Dummy:me_key == dummy, me_value == NULL。

哈希探测结束的条件是探测到一个Unused的entry。但是dict操作中必定会有删除操作，如果删除时仅把Active标记成Unused，显然该entry之后的所有entry都不可能被探测到，所以引入了dummy结构。遇到dummy就说明当前entry处于空闲状态，但探测不能结束。这样就解决了删除一个entry之后探测链断裂的问题。

PyDict_New() 创建新字典对象
 |-PyString_FromString() 第一次会初始化dummy对象
 |-PyObject_GC_New() 如果没有缓存，则通过该函数创建一个

Dict 对象的插入
字典对象的插入实际是通过 PyDict_SetItem() 函数完成，简单来说就是，如果不存在 Key-Value 则插入，存在则覆盖；基本的处理步骤如下：

1. 通过 ma_lookup 所指向的函数得到 key 所对应的 entry，该函数对于字符串来说是 lookdict_string()，整形是 lookdict()；
2. 返回的值分为如下几种场景：
   * me_key = NULL 空不存在，可以直接使用；
   * me_key = key 对应的值已经存在，可以直接返回；
   * me_hash = hash && 字符串相同，不同的 key 对象，但是值相同，同样认为相同；
   * me_key = dummy 对应的值已经删除；
   * 冲突，通过如下方式探测。

PyDict_SetItem()
 |-PyString_CheckExact()  如果是string对象，那么实际会通过string_hash计算hash值
 |-PyObject_Hash()  否则是int类型，则通过int_hash计算hash值
 |-dict_set_item_by_hash_or_entry() 其中entry为0，也就是通过hash添加，会判断是否调整大小
   |-insertdict()  实际调用这里的接口
   | |-ma_lookup() 通过该指针指向的对象查找，一般默认为lookdict_string()函数
   | |-insertdict_by_entry()
   |-insertdict_by_entry()
   |-dictresize() 只有在插入的时候会调整字典的大小

TODO:
  校验下，循环中可以替换，但是无法新增或者删除。
-->

## 参考

详细可以参考 [深入 Python 字典的内部实现](http://python.jobbole.com/85040/) 以及其原文 [Python dictionary implementation](http://www.laurentluce.com/posts/python-dictionary-implementation/) 。

<!--
摘抄出来的实现
http://www.cnblogs.com/xiangnan/p/3859578.html
-->


{% highlight text %}
{% endhighlight %}
