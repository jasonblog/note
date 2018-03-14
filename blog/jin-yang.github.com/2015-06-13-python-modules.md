---
title: Python 模塊簡介
layout: post
comments: true
category: [python,program]
language: chinese
keywords: python,模塊
description: 簡單記錄一下 Python 模塊相關的內容，包括了內置模塊、標準模塊以及需要單獨安裝的三方包。而且還有一些經常使用的三方模塊，例如 functools、collections、json 等。
---

簡單記錄一下 Python 模塊相關的內容，包括了內置模塊、標準模塊以及需要單獨安裝的三方包。而且還有一些經常使用的三方模塊，例如 functools、collections、json 等。

<!-- more -->

![python modules]({{ site.url }}/images/python/python-modules-logo.jpg "python modules"){: .pull-center}

## 模塊

Python 中包括了三類的模塊：A) 內置的 (builtin)，編譯在解析器的內部，可以直接使用；B) 標準模塊，通常在安裝 python 時已經安裝，如 gc；C) 第三方包，需要手動安裝，如 MySQLdb 。

常用庫可參考 [The Python Standard Library](https://docs.python.org/2/library/)，其中部分模塊可以在源碼的 Modules 目錄下查看，如 ctypes、gcmodule 等；有些包含在 Python 目錄下，如 sysmodule.c、bltinmodule.c 等。

其中，後者的模塊會直接編譯到 python 可執行文件中，可以通過 nm python 查看，不過對於發行版本，通常已經將調試信息清理過，所以 nm 命令無效。

對於 builtin 模塊，可以參考源碼中的 builtin_methods[] 數組，包括了 abs、compile、format、filter 等一些常見的函數。

{% highlight c %}
static PyMethodDef builtin_methods[] = {
    {"__import__",      (PyCFunction)builtin___import__, METH_VARARGS | METH_KEYWORDS, import_doc},
    {"abs",             builtin_abs,        METH_O, abs_doc},
    ... ...
    {"compile",         (PyCFunction)builtin_compile,    METH_VARARGS | METH_KEYWORDS, compile_doc},
    {"delattr",         builtin_delattr,    METH_VARARGS, delattr_doc},
    {"dir",             builtin_dir,        METH_VARARGS, dir_doc},
    {"divmod",          builtin_divmod,     METH_VARARGS, divmod_doc},
    {"eval",            builtin_eval,       METH_VARARGS, eval_doc},
    {"execfile",        builtin_execfile,   METH_VARARGS, execfile_doc},
    {"filter",          builtin_filter,     METH_VARARGS, filter_doc},
    {"format",          builtin_format,     METH_VARARGS, format_doc},
    {"getattr",         builtin_getattr,    METH_VARARGS, getattr_doc},
    ... ...
    {NULL,              NULL},
};
{% endhighlight %}

如上，對於 buildin 模塊，在源碼中，其有效的函數名通常是 buildin_xxx()；另外，對於內置模塊，不會存在 \_\_file\_\_ 屬性，而三方的模塊可以通過該屬性查看模塊的安裝路徑；當然，也可以通過類似 help(abs) 的命令查看。

對於一些三方模塊，可以通過通用的工具進行安裝。

{% highlight text %}
# python setup.py build                    ← 下載的源碼直接安裝
# easy_install PACKAGE                     ← 通過easy_install或pip安裝
# pip install PACKAGE
{% endhighlight %}

對於 CentOS 來說，三方模塊通常安裝在 /usr/lib64/python2.7/ 目錄下。

### \_\_builtins\_\_ VS. \_\_builtin\_\_

這個是內置模塊中比較容易混淆的兩個概念。

在 Python 加載後，可以通過 globals().keys() 查看全局變量，會有一個 \_\_builtins\_\_，包括了直接執行的 Python 以及相應的模塊，但是在這兩種場景下該值是不同的。

如果是 main 執行，則變量對應 \_\_builtin\_\_ 模塊；在加載模塊中該值對應 \_\_builtin\_\_.\_\_dict\_\_。也就是 \_\_builtins\_\_ 可能對應 \_\_builtin\_\_，也可能對應 \_\_builtin\_\_.\_\_dict\_\_，這個是 CPython 實現的細節，最好在一個通用模塊中不要使用 \_\_builtins\_\_ 。

當然，通常很少會使用該變量，如果需要使用，則需多加註意，在此不再贅述，詳細內容可以參考 [What's the deal with \_\_builtins\_\_ vs \_\_builtin\_\_](http://mathamy.com/whats-the-deal-with-builtins-vs-builtin.html) ，或者可以查看 [本地版本](/reference/python/What's the deal with __builtins__ vs __builtin__.mht) 。


### 三方模塊

三方模塊也是經常使用的，是對標準庫的擴展。

只有當目錄包含一個 \_\_init\_\_.py 文件時，才會被認作是一個包，最簡單的是一個空文件；當然，也可以在這個文件中執行一些初始化代碼，或者為 \_\_all\_\_ 變量賦值。

其中 \_\_all\_\_ 列表用於指定 from package import * 方式導入的名字，示例如下。

三方模塊的目錄結構為。

{% highlight text %}
$ tree
.
|-- bar
|   |-- __init__.py
|   |-- __init__.pyc
|   |-- test.py
|   `-- test.pyc
|-- foo
|   `-- __init__.py
`-- test.py
2 directories, 6 files
{% endhighlight %}

各個文件的內容如下。

{% highlight python %}
##### cat bar/__init__.py
barstr = "Hello World, IN BAR"
from test import bar

##### cat bar/test.py
def add(a, b):
    print "Bar", a+b
def bar(var):
    print id(var)

##### cat test.py
# -*- coding:utf-8 -*-
from bar import barstr, test, bar
print barstr                          # 打印__init__.py中的變量
test.add(1,2)                         # 執行bar/test.py中的函數
bar("function in bar/test.py")        # 執行bar/test.py中的函數
{% endhighlight %}

接下來列舉的就是一些常用的三方模塊了，可以在使用的時候以供參考。

## 搜索路徑

模塊搜索順序。

1. 導入的模塊是否為內置模塊；
2. 從 sys.path 變量中搜索模塊名。

可以通過 `python -c "import sys; print sys.path"` 查看搜索路徑，默認是 A) 當前目錄；B) `PYTHONPAHT` 環境變量；C) 安裝默認路徑。

如果新安裝的模塊在其它路徑下，那麼就需要動態添加搜索路徑，可以有如下的幾種方式。

1. 修改 `PYTHONPATH` 環境變量。
2. 增加 `*.pth` 文件，在遍歷已知目錄時，如果遇到 `.pth` 文件，會自動將其中的路徑添加到 `sys.path` 變量中。
3. 通過 `sys` 模塊的 `append()` 方法增加搜索路徑。

操作詳見如下內容。

{% highlight text %}
----- 修改~/.bashrc文件
export PYTHONPATH=$PYTHONPATH:/home/foobar/workspace

----- 在默認安裝路徑/usr/lib64/python2.7下增加extra.pth文件，內容如下
/home/foobar/workspace

----- 通過append()函數添加路徑
import sys
sys.path.append('/home/foobar/workspace')
{% endhighlight %}

在加載完模塊之後，可以通過如下命令查看模塊的路徑。

{% highlight python %}
import sys
sys.modules['foobar']

foobar.__file__
{% endhighlight %}

另外遇到一個比較奇葩的問題，安裝完 Redis 之後，發現 root 用戶可以 import 模塊，而非 root 用戶會報錯。首先，十有八九是權限的問題導致的，在 root 導入模塊後查看其路徑，修改權限；對於 easy_install 安裝，會修改 easy-install.pth 文件，也需要保證其它用戶對該文件可讀。

## functools 模塊

functools 是 python2.5 引人的，可以參考官方文檔 [functools — Higher-order functions and operations on callable objects](https://docs.python.org/2/library/functools.html)

### reduce

這個與 Python 內置的 reduce 相同，每次迭代，將上一次的迭代結果與下一個元素一同執行一個二元的 func 函數。

{% highlight python %}
import functools
a = range(1, 6)
functools.reduce((lambda x,y:x+y), a)  # 1+2+3+4+5=15
reduce((lambda x,y:x+y), a)            # 1+2+3+4+5=15
{% endhighlight %}

### partial

對於一個帶 n 個參數函數，partial 會將第一個參數設置為固定參數，並返回一個帶 n-1 個參數函數對象。

{% highlight python %}
from operator import add
from functools import partial
add3 = partial(add, 3)
print add3(7)            # 3+7=10
{% endhighlight %}


## collections 模塊

該三方模塊提供了對內置類型的擴展，是 Python 內建的一個集合模塊，提供了許多有用的集合類，包括了多個對象，詳細可以查看官方文檔 [collections High-performance container datatypes](https://docs.python.org/2/library/collections.html) 。

### defaultdict

在使用 Python 內置的 dict 時，如果引用的 Key 不存在，就會拋出 KeyError，如果希望 key 不存在時，返回一個默認值，就可以用 defaultdict。

{% highlight text %}
>>> from collections import defaultdict
>>> dd = defaultdict(lambda: 'N/A')
>>> dd['key1'] = 'abc'
>>> dd['key1']                       ← key1存在
'abc'
>>> dd['key2']                       ← key2不存在，返回默認值
'N/A'
{% endhighlight %}

注意默認值是調用函數返回的，而函數在創建 defaultdict 對象時傳入，除了在 Key 不存在時返回默認值外，該對象的其他行為跟 dict 是完全一樣的。

### OrderedDict

使用 Python 內置的字典對象時，Key 是無序的，這樣當對 dict 做迭代時，無法確定 Key 的順序，如果要保持 Key 的順序，可以用 OrderedDict 。

{% highlight text %}
>>> from collections import OrderedDict
>>> d = dict([('a', 1), ('b', 2), ('c', 3)])
>>> d                                ← dict的Key是無序的
{'a': 1, 'c': 3, 'b': 2}

>>> od = OrderedDict([('a', 1), ('b', 2), ('c', 3)])
>>> od                               ← OrderedDict的Key是有序的
OrderedDict([('a', 1), ('b', 2), ('c', 3)])
{% endhighlight %}

OrderedDict 的 Key 會 **按照插入的順序排列** ，不是 Key 本身排序：

{% highlight text %}
>>> od = OrderedDict()
>>> od['z'] = 1
>>> od['y'] = 2
>>> od['x'] = 3
>>> od.keys()                        ← 按照插入的Key的順序返回
['z', 'y', 'x']
{% endhighlight %}

另外，通過 OrderedDict 可以實現一個 FIFO (先進先出) 的 dict，當容量超出限制時，先刪除最早添加的 Key：

{% highlight python %}
from collections import OrderedDict
class LastUpdatedOrderedDict(OrderedDict):
    def __init__(self, capacity):
        super(LastUpdatedOrderedDict, self).__init__()
        self._capacity = capacity
    def __setitem__(self, key, value):
        containsKey = 1 if key in self else 0
        if len(self) - containsKey >= self._capacity:
            last = self.popitem(last=False)
            print 'remove:', last
        if containsKey:
            del self[key]
            print 'set:', (key, value)
        else:
            print 'add:', (key, value)
        OrderedDict.__setitem__(self, key, value)
{% endhighlight %}

### namedtuple

在 Python 中，tuple 可以 **表示不變集合** ，例如，一個點的二維座標就可以表示成 (1, 2)。但是，看到 (1, 2) 後，很難看出這個 tuple 是用來表示一個座標的，不過定義一個類又小題大做了，這時就可以使用該對象了。

{% highlight text %}
>>> from collections import namedtuple
>>> P = namedtuple('Point', ['x', 'y'])
>>> p = P(1, 2)
>>> print p
Point(x=1, y=2)
>>> p.x
1
>>> p.y
2
>>> p.x = 3                          ← 只讀變量，設置會報錯
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
AttributeError: can't set attribute
{% endhighlight %}

<!-- ' -->

namedtuple 是一個函數，它用來創建一個自定義的 tuple 對象，並且規定了 tuple 元素的個數，並可以用屬性而不是索引來引用 tuple 的某個元素。這樣一來，我們用 namedtuple 可以很方便地定義一種數據類型，它具備 tuple 的不變性，又可以根據屬性來引用，使用十分方便。

對於如上的示例，可以通過如下方式驗證創建的 Point 對象是 tuple 的一種子類：

{% highlight text %}
>>> isinstance(p, Point)
True
>>> isinstance(p, tuple)
True
{% endhighlight %}

### deque

使用 list 存儲數據時，按索引訪問元素很快，但是插入和刪除元素就很慢了，因為 list 是線性存儲，數據量大的時候，插入和刪除效率很低。deque 是為了高效實現插入和刪除操作的雙向列表，適合用於隊列和棧：

{% highlight text %}
>>> from collections import deque
>>> q = deque(['a', 'b', 'c'])
>>> q.append('x')
>>> q.appendleft('y')
>>> q
deque(['y', 'a', 'b', 'c', 'x'])
{% endhighlight %}

deque 除了實現 list 的 append() 和 pop() 外，還支持 appendleft() 和 popleft()，這樣就可以非常高效地往頭部添加或刪除元素。

<!--
## JSON 模塊

JSON (JavaScript Object Notation) 是一種輕量級的數據交換格式，採用完全獨立於語言的文本格式，是 "名稱/值" 的集合，詳細可以參考 [官方文檔](http://json.org/) 。Python2.6 開始加入了 JSON 標準模塊，序列化與反序列化的過程分別是 encoding 和 decoding 。

對於簡單數據類型，如 string、unicode、int、float、list、tuple、dict，可以直接處理，詳細的也可以參考 [Python 操作 json 的標準 api 庫](http://docs.python.org/library/json.html) 。

urllib 和 urllib2 的區別
http://www.hacksparrow.com/python-difference-between-urllib-and-urllib2.html
-->

{% highlight python %}
{% endhighlight %}
