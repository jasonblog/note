---
title: Python 模块简介
layout: post
comments: true
category: [python,program]
language: chinese
keywords: python,模块
description: 简单记录一下 Python 模块相关的内容，包括了内置模块、标准模块以及需要单独安装的三方包。而且还有一些经常使用的三方模块，例如 functools、collections、json 等。
---

简单记录一下 Python 模块相关的内容，包括了内置模块、标准模块以及需要单独安装的三方包。而且还有一些经常使用的三方模块，例如 functools、collections、json 等。

<!-- more -->

![python modules]({{ site.url }}/images/python/python-modules-logo.jpg "python modules"){: .pull-center}

## 模块

Python 中包括了三类的模块：A) 内置的 (builtin)，编译在解析器的内部，可以直接使用；B) 标准模块，通常在安装 python 时已经安装，如 gc；C) 第三方包，需要手动安装，如 MySQLdb 。

常用库可参考 [The Python Standard Library](https://docs.python.org/2/library/)，其中部分模块可以在源码的 Modules 目录下查看，如 ctypes、gcmodule 等；有些包含在 Python 目录下，如 sysmodule.c、bltinmodule.c 等。

其中，后者的模块会直接编译到 python 可执行文件中，可以通过 nm python 查看，不过对于发行版本，通常已经将调试信息清理过，所以 nm 命令无效。

对于 builtin 模块，可以参考源码中的 builtin_methods[] 数组，包括了 abs、compile、format、filter 等一些常见的函数。

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

如上，对于 buildin 模块，在源码中，其有效的函数名通常是 buildin_xxx()；另外，对于内置模块，不会存在 \_\_file\_\_ 属性，而三方的模块可以通过该属性查看模块的安装路径；当然，也可以通过类似 help(abs) 的命令查看。

对于一些三方模块，可以通过通用的工具进行安装。

{% highlight text %}
# python setup.py build                    ← 下载的源码直接安装
# easy_install PACKAGE                     ← 通过easy_install或pip安装
# pip install PACKAGE
{% endhighlight %}

对于 CentOS 来说，三方模块通常安装在 /usr/lib64/python2.7/ 目录下。

### \_\_builtins\_\_ VS. \_\_builtin\_\_

这个是内置模块中比较容易混淆的两个概念。

在 Python 加载后，可以通过 globals().keys() 查看全局变量，会有一个 \_\_builtins\_\_，包括了直接执行的 Python 以及相应的模块，但是在这两种场景下该值是不同的。

如果是 main 执行，则变量对应 \_\_builtin\_\_ 模块；在加载模块中该值对应 \_\_builtin\_\_.\_\_dict\_\_。也就是 \_\_builtins\_\_ 可能对应 \_\_builtin\_\_，也可能对应 \_\_builtin\_\_.\_\_dict\_\_，这个是 CPython 实现的细节，最好在一个通用模块中不要使用 \_\_builtins\_\_ 。

当然，通常很少会使用该变量，如果需要使用，则需多加注意，在此不再赘述，详细内容可以参考 [What's the deal with \_\_builtins\_\_ vs \_\_builtin\_\_](http://mathamy.com/whats-the-deal-with-builtins-vs-builtin.html) ，或者可以查看 [本地版本](/reference/python/What's the deal with __builtins__ vs __builtin__.mht) 。


### 三方模块

三方模块也是经常使用的，是对标准库的扩展。

只有当目录包含一个 \_\_init\_\_.py 文件时，才会被认作是一个包，最简单的是一个空文件；当然，也可以在这个文件中执行一些初始化代码，或者为 \_\_all\_\_ 变量赋值。

其中 \_\_all\_\_ 列表用于指定 from package import * 方式导入的名字，示例如下。

三方模块的目录结构为。

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

各个文件的内容如下。

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
print barstr                          # 打印__init__.py中的变量
test.add(1,2)                         # 执行bar/test.py中的函数
bar("function in bar/test.py")        # 执行bar/test.py中的函数
{% endhighlight %}

接下来列举的就是一些常用的三方模块了，可以在使用的时候以供参考。

## 搜索路径

模块搜索顺序。

1. 导入的模块是否为内置模块；
2. 从 sys.path 变量中搜索模块名。

可以通过 `python -c "import sys; print sys.path"` 查看搜索路径，默认是 A) 当前目录；B) `PYTHONPAHT` 环境变量；C) 安装默认路径。

如果新安装的模块在其它路径下，那么就需要动态添加搜索路径，可以有如下的几种方式。

1. 修改 `PYTHONPATH` 环境变量。
2. 增加 `*.pth` 文件，在遍历已知目录时，如果遇到 `.pth` 文件，会自动将其中的路径添加到 `sys.path` 变量中。
3. 通过 `sys` 模块的 `append()` 方法增加搜索路径。

操作详见如下内容。

{% highlight text %}
----- 修改~/.bashrc文件
export PYTHONPATH=$PYTHONPATH:/home/foobar/workspace

----- 在默认安装路径/usr/lib64/python2.7下增加extra.pth文件，内容如下
/home/foobar/workspace

----- 通过append()函数添加路径
import sys
sys.path.append('/home/foobar/workspace')
{% endhighlight %}

在加载完模块之后，可以通过如下命令查看模块的路径。

{% highlight python %}
import sys
sys.modules['foobar']

foobar.__file__
{% endhighlight %}

另外遇到一个比较奇葩的问题，安装完 Redis 之后，发现 root 用户可以 import 模块，而非 root 用户会报错。首先，十有八九是权限的问题导致的，在 root 导入模块后查看其路径，修改权限；对于 easy_install 安装，会修改 easy-install.pth 文件，也需要保证其它用户对该文件可读。

## functools 模块

functools 是 python2.5 引人的，可以参考官方文档 [functools — Higher-order functions and operations on callable objects](https://docs.python.org/2/library/functools.html)

### reduce

这个与 Python 内置的 reduce 相同，每次迭代，将上一次的迭代结果与下一个元素一同执行一个二元的 func 函数。

{% highlight python %}
import functools
a = range(1, 6)
functools.reduce((lambda x,y:x+y), a)  # 1+2+3+4+5=15
reduce((lambda x,y:x+y), a)            # 1+2+3+4+5=15
{% endhighlight %}

### partial

对于一个带 n 个参数函数，partial 会将第一个参数设置为固定参数，并返回一个带 n-1 个参数函数对象。

{% highlight python %}
from operator import add
from functools import partial
add3 = partial(add, 3)
print add3(7)            # 3+7=10
{% endhighlight %}


## collections 模块

该三方模块提供了对内置类型的扩展，是 Python 内建的一个集合模块，提供了许多有用的集合类，包括了多个对象，详细可以查看官方文档 [collections High-performance container datatypes](https://docs.python.org/2/library/collections.html) 。

### defaultdict

在使用 Python 内置的 dict 时，如果引用的 Key 不存在，就会抛出 KeyError，如果希望 key 不存在时，返回一个默认值，就可以用 defaultdict。

{% highlight text %}
>>> from collections import defaultdict
>>> dd = defaultdict(lambda: 'N/A')
>>> dd['key1'] = 'abc'
>>> dd['key1']                       ← key1存在
'abc'
>>> dd['key2']                       ← key2不存在，返回默认值
'N/A'
{% endhighlight %}

注意默认值是调用函数返回的，而函数在创建 defaultdict 对象时传入，除了在 Key 不存在时返回默认值外，该对象的其他行为跟 dict 是完全一样的。

### OrderedDict

使用 Python 内置的字典对象时，Key 是无序的，这样当对 dict 做迭代时，无法确定 Key 的顺序，如果要保持 Key 的顺序，可以用 OrderedDict 。

{% highlight text %}
>>> from collections import OrderedDict
>>> d = dict([('a', 1), ('b', 2), ('c', 3)])
>>> d                                ← dict的Key是无序的
{'a': 1, 'c': 3, 'b': 2}

>>> od = OrderedDict([('a', 1), ('b', 2), ('c', 3)])
>>> od                               ← OrderedDict的Key是有序的
OrderedDict([('a', 1), ('b', 2), ('c', 3)])
{% endhighlight %}

OrderedDict 的 Key 会 **按照插入的顺序排列** ，不是 Key 本身排序：

{% highlight text %}
>>> od = OrderedDict()
>>> od['z'] = 1
>>> od['y'] = 2
>>> od['x'] = 3
>>> od.keys()                        ← 按照插入的Key的顺序返回
['z', 'y', 'x']
{% endhighlight %}

另外，通过 OrderedDict 可以实现一个 FIFO (先进先出) 的 dict，当容量超出限制时，先删除最早添加的 Key：

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

在 Python 中，tuple 可以 **表示不变集合** ，例如，一个点的二维坐标就可以表示成 (1, 2)。但是，看到 (1, 2) 后，很难看出这个 tuple 是用来表示一个坐标的，不过定义一个类又小题大做了，这时就可以使用该对象了。

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
>>> p.x = 3                          ← 只读变量，设置会报错
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
AttributeError: can't set attribute
{% endhighlight %}

<!-- ' -->

namedtuple 是一个函数，它用来创建一个自定义的 tuple 对象，并且规定了 tuple 元素的个数，并可以用属性而不是索引来引用 tuple 的某个元素。这样一来，我们用 namedtuple 可以很方便地定义一种数据类型，它具备 tuple 的不变性，又可以根据属性来引用，使用十分方便。

对于如上的示例，可以通过如下方式验证创建的 Point 对象是 tuple 的一种子类：

{% highlight text %}
>>> isinstance(p, Point)
True
>>> isinstance(p, tuple)
True
{% endhighlight %}

### deque

使用 list 存储数据时，按索引访问元素很快，但是插入和删除元素就很慢了，因为 list 是线性存储，数据量大的时候，插入和删除效率很低。deque 是为了高效实现插入和删除操作的双向列表，适合用于队列和栈：

{% highlight text %}
>>> from collections import deque
>>> q = deque(['a', 'b', 'c'])
>>> q.append('x')
>>> q.appendleft('y')
>>> q
deque(['y', 'a', 'b', 'c', 'x'])
{% endhighlight %}

deque 除了实现 list 的 append() 和 pop() 外，还支持 appendleft() 和 popleft()，这样就可以非常高效地往头部添加或删除元素。

<!--
## JSON 模块

JSON (JavaScript Object Notation) 是一种轻量级的数据交换格式，采用完全独立于语言的文本格式，是 "名称/值" 的集合，详细可以参考 [官方文档](http://json.org/) 。Python2.6 开始加入了 JSON 标准模块，序列化与反序列化的过程分别是 encoding 和 decoding 。

对于简单数据类型，如 string、unicode、int、float、list、tuple、dict，可以直接处理，详细的也可以参考 [Python 操作 json 的标准 api 库](http://docs.python.org/library/json.html) 。

urllib 和 urllib2 的区别
http://www.hacksparrow.com/python-difference-between-urllib-and-urllib2.html
-->

{% highlight python %}
{% endhighlight %}
