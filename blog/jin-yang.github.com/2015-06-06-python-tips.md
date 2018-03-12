---
title: Python 杂项
layout: post
comments: true
category: [program, python]
language: chinese
keywords: python,杂项
description: 在此记录与 Python 相关的乱七八糟的东西。
---

在此记录与 Python 相关的乱七八糟的东西。

<!-- more -->

## 神奇的 property

在 Python 中绑定属性时，如果直接把属性暴露出去，虽然写起来很简单，但是没办法检查参数；通过 property 即可以将一个函数作为属性，并在函数中对参数的值进行检查。

例如，一个学生的成绩必须在 [0, 100] 这个区间才可以，那么我们就可以使用 property 。

{% highlight python %}
#!/usr/bin/env python
#-*- coding:utf-8 -*-
class Student(object):
    @property
    def score(self):
        return self._score
    @score.setter
    def score(self, value):
        if not isinstance(value, int):
            raise ValueError('score must be an integer!')
        if value < 0 or value > 100:
            raise ValueError('score must between 0 ~ 100!')
        self._score = value

s = Student()
s.score = 60       # OK
print s.score      # OK
s.score = 9999     # Fail
{% endhighlight %}

如上，只需要函数添加 @property 修饰符即可；此时，又会创建另外一个修饰符 @score.setter，用于把一个 setter 方法变成属性赋值；如果没有使用 setter，那么该属性为只读。

当然，也可以通过如下方式设置，两者的作用相同。

{% highlight python %}
class Student(object):
    def get_score(self):
        return self._score
    def set_score(self, value):
        if not isinstance(value, int):
            raise ValueError('score must be an integer!')
        if value < 0 or value > 100:
            raise ValueError('score must between 0 ~ 100!')
        self._score = value
    score = property(get_score, set_score)
{% endhighlight %}



## with 语句

通常会有一些任务，开始需要进行设置，处理任务，事后做清理工作。对于这种场景，Python 的 with 语句提供了一种非常方便的处理方式。一个很好的例子是文件处理，你需要获取一个文件句柄，从文件中读取数据，然后关闭文件句柄。

正常的处理过程以及使用 with 时的处理过程如下：

{% highlight python %}
file = open("/tmp/foo.txt")
try:
    data = file.read()
finally:
    file.close()

with open("/tmp/foo.txt") as file:
    data = file.read()
{% endhighlight %}

基本思想是 with 之后的函数返回的对象必须有  \_\_enter\_\_() 和 \_\_exit\_\_() 方法。

紧跟 with 后面的语句被求值后，返回对象的 \_\_enter\_\_() 被调用，这个方法的返回值将被赋值给 as 后面的变量；当 with 后面的代码块全部被执行完之后，将调用前面返回对象的 \_\_exit\_\_() 。

{% highlight python %}
#!/usr/bin/env python
class Sample:
    def __enter__(self):
        print "In __enter__()"
        return "Foo"

    def __exit__(self, type, value, trace):
        print "In __exit__()"

def get_sample():
    print "In get_sample()"
    return Sample()

with get_sample() as sample:
    print "sample:", sample
{% endhighlight %}

执行的顺序为 A) get_sample(); 返回 Sample 对象； B) Sample:\_\_enter\_\_(); 将返回的值赋值给 sample； C) 执行代码块； D) 执行 Sample:\_\_exit\_\_() 。

with 真正强大之处是它可以处理异常。\_\_exit\_\_() 有三个参数，val, type 和 trace，这些参数在异常处理中相当有用。

{% highlight python %}
#!/usr/bin/env python
class Sample:
    def __enter__(self):
        return self

    def __exit__(self, type, value, trace):
        print "type:", type
        print "value:", value
        print "trace:", trace

    def do_something(self):
        bar = 1/0
        return bar + 10

with Sample() as sample:
    sample.do_something()
{% endhighlight %}

在 with 后面的代码块抛出任何异常时，\_\_exit\_\_() 方法被执行。正如例子所示，异常抛出时，与之关联的 type，value 和 stack trace 传给 \_\_exit\_\_() 方法，因此抛出的 ZeroDivisionError 异常被打印出来了。



## 循环缓冲区

Python 中可以很简单的实现一个固定长度的缓冲区，也就是固定长度的 FIFO 队列。

{% highlight python %}
class RingBuffer:
    def __init__(self, size):
        self.data = [None for i in xrange(size)]

    def append(self, x):
        self.data.pop(0)
        self.data.append(x)

    def get(self):
        return self.data
{% endhighlight %}

可以通过如下方式使用。


{% highlight python %}
buf = RingBuffer(4)
for i in xrange(10):
    buf.append(i)
    print buf.get()

## OUTPUT:
#  [None, None, None, 0]
#  [None, None, 0, 1]
#  [None, 0, 1, 2]
#  [0, 1, 2, 3]
#  [1, 2, 3, 4]
#  [2, 3, 4, 5]
#  [3, 4, 5, 6]
#  [4, 5, 6, 7]
#  [5, 6, 7, 8]
#  [6, 7, 8, 9]
{% endhighlight %}

另外，实际上可以通过 pop() 和 insert(0, x) 将缓冲区正向输入，如上述的顺序刚好相反。


## 二进制文件操作

对于二进制文件，可以使用 struct 模块中的 pack、unpack 函数进行读写操作；当然对于单个 Byte 可以通过 ord()、hex() 等函数执行。

{% highlight python %}
#!/usr/bin/env python
#-*- coding:utf-8 -*-
import struct

file = open(r"/tmp/test.bin", "wb")
file.write(struct.pack("idh", 12345, 67.89, 15))
file.close()

# reopen it.
file = open(r"/tmp/test.bin", "rb")
(a, b, c) = struct.unpack("idh", file.read(8+8+2))
print a, b, c

file.seek(0)
b = file.read(1)
print hex(ord(b))   # 先通过ord()将一个字节转成一个数，然后转成16进制的字符串
file.close()
{% endhighlight %}

通过 struct 模块写入时，保存的时 struct 转换后的格式；如果要原生写入，可以通过如下的方式写入。

{% highlight python %}
#!/usr/bin/env python
#-*- coding:utf-8 -*-

file = open("test.bin","wb")
file.write("\x5F\x9D\x3E" + ("%c%c" % (0x45, 0xF3)))     # 直接写入

import binascii
hexs = "5B7F"
file.write(binascii.a2b_hex(hexs))

file.close()
{% endhighlight %}

其中 struct 模块是基于 C 语言编写的库，可以参考官方文档 [struct — Interpret strings as packed binary data](https://docs.python.org/2/library/struct.html) 。

## docstrings

也就是 Python 的文档字符串，提供了一种很方便的方式将文档与 modules、functions、classes 以及 methods ；该字符串是在对象的第一个语句中定义，然后通过 ```__doc__``` 引用。

{% highlight python %}
#!/usr/bin/env python
#-*- coding:utf-8 -*-
"""
This is an example for python docstring, including modules,
    functions, classes, methods, etc.
"""

import math
print math.__doc__            # 标准库的文档字符串
print str.__doc__             # 标准函数的文档字符串

def foo():
    """It's just a foo() function."""
    pass
class foobar(object):
    """It's just a foobar class."""
    def bar():
        """It's just a foobar.bar() function."""
        pass
    def get_doc(self):
        return self.__doc__
f = foobar()
print f.__doc__               # 通过实例返回类的文档字符串
print f.get_doc()             # 或者调用函数返回文档字符串
print __doc__                 # 本模块的文档字符串
print foo.__doc__             # 函数的文档字符串
print foobar.__doc__          # 自定义类的文档字符串
print foobar.bar.__doc__      # 类中方法的文档字符串
{% endhighlight %}

当然，也可以使用 pydoc 模块，这个模块是 Python 2.1 引入的，提供了一个交互界面显示对象的信息，当然也包括了文档字符串。

{% highlight python %}
from pydoc import help
def foobar():
    """
    It's just a foobar function.
    """
    return
help(foobar)
{% endhighlight %}




## 其它


### 首行

可以使用 #!/usr/bin/python 或者 #!/usr/bin/env python ，env 可以通过指定的环境变量执行命令。


### 使用 UTF-8

对于非英文的编码可以使用如下的方法之一。

{% highlight python %}
#-*- coding:utf-8 -*-
#xxx coding:utf-8 xxx
#encoding: utf-8
#coding: utf-8
{% endhighlight %}


### 带 * 参数

在 python 函数参数中，星号的作用是：*)传入任意长度的元组；**)传入任意长度的字典。如果两者之前有固定的参数列表，则先填充前面的参数。

{% highlight python %}
#!/usr/bin/env python
def one_stars(x, *args):
    print 'one_stars, %s:%s' % (type(args), args)
def two_stars(x, **args):
    print 'two_stars, %s:%s' % (type(args), args)
def mix_stars(x, *cargs, **kargs):
    print 'mix_stars, %s:%s, %s:%s' % (type(cargs), cargs, type(kargs), kargs)

one_stars(1, 'abc', 123)
two_stars(1, a=1, b=2)
mix_stars(1, 1, 2, a=1, b=2) # (1, a=1, b=2) OK, (1, a=1, b=2, 1, 2) ERROR
mix_stars(x=1, b=2)

# OUTPUT:
# one_stars, <type 'tuple'>:('abc', 123)
# two_stars, <type 'dict'>:{'a': 1, 'b': 2}
# mix_stars, <type 'tuple'>:(1, 2), <type 'dict'>:{'a': 1, 'b': 2}
# mix_stars, <type 'tuple'>:(), <type 'dict'>:{'b': 2}
{% endhighlight %}


### 列表推导

列表推导的格式为 [ expression for variables in list [if condition]]，推导的变量可以有一个或者多个，如果是多个通常为笛卡尔乘积。

{% highlight python %}
print [x*y for x in [1,2,3] for y in [1,2,3]]
print [(x, y) for x in range(10) if x % 2 if x > 3 for y in range(10) if y > 7 if y != 8]
print [line.rstrip() for line in open('test.txt') if line[0]=='n']
print dict([(x,x*10) for x in range(3)])

print {x:x % 2 == 0 for x in range(1, 11)}     # Python3
{% endhighlight %}

### 三元运算

在 Python 中 0、''、[]、()、{}、None 都为假，而且对于 and 如果所有值为真则会返回最后一个值，否则返回第一个假值；对于 or 如果有一个值为真则返回，否则返回最后一个值。

{% highlight text %}
'' and 'b'             # 返回 ''
'a' and 'b' and 'c'    # 返回 'c'
'a' or 'b'             # 返回 'a'
'' or [] or {}         # 返回 {}
{% endhighlight %}

对于三元运算，如取最小值 small = x if x < y else y。也可以使用 and-or ，如 (1 and [a] or [b])[0]，使用列表是为了防止 a 为假。


### join()格式输出

生成一串数字，中间用逗号分割，其中 range 可以用 xrange 替换。

{% highlight python %}
print ','.join([str(i) for i in range(1,6)])
print ','.join(["%s"%i for i in range(1,6)])
print ",".join(map(lambda i:str(i), range(1,6)))
{% endhighlight %}





### None 判断

通常的判断方式可能会有多种，不过可能在使用时有很多的问题，列举如下。

#### if x is None / x==None

(ob1 is ob2) 等价于 (id(ob1) == id(ob2))，也即当比较相同的对象实例，is 总是返回 True，而 == 最终取决于 "eq()"。

{% highlight python %}
class foo(object):
   def __eq__(self, other):
      return True
f = foo()
f == None             # True
f is None             # False

list1 = [1, 2, 3]
list2 = [1, 2, 3]
list1==list2          # True
list1 is list2        # False
{% endhighlight %}

#### if not x

需要注意此时无法区分 x==[] 和 x==None，在 Python 中，None、False、空字符串""、0、空列表[]、空字典{}、空元组() 都相当于 False。

#### if not x is None

实际为 if not (x is None)，最好使用 if x is not None 。


### os._exit()/sys.exit()

Python 程序有两中退出方式:

1. os._exit() 会直接将程序终止，之后的所有代码都不会继续执行；

2. sys.exit() 会引发一个 SystemExit 异常，如果这个异常没有被捕获，那么 python 解释器将会退出，如果有捕获此异常的代码，那么这些代码还是会执行。

{% highlight python %}
try:
    sys.exit()
except SystemExit:   # 不包含信息，不要使用 SystemExit, e
    print "die"
{% endhighlight %}

### 内置函数 map()、filter()、reduce()

这三个函数比较类似，都是应用于序列 (list, tuple, str等) 的内置函数。

#### map(function, sequence[, sequence, ...]) return: list

对 sequence 中的 item 依次执行 function(item)，执行结果输出为 list。如果长度不同，则以最长为准，最短以 None 补齐。

#### reduce(function, sequence[, initial]) return: value

对 sequence 中的 item 顺序迭代调用 function，必须要有 2 个参数。如果有第 3 个参数，则表示初始值。需要注意的是，function 不能为 None 。

#### filter(function or None, sequence) return list, tuple, or string

对 sequence 中的 item 依次执行 function(item)，将执行结果为 True 的 item 组成 list、string、tuple，这取决于 sequence 的类型。

以下为部分示例。
{% highlight text %}
>>> def add(m, n): return m+n
>>> map(add, range(5), range(2, 7))                  序列的长度必须相同
>>> map(lambda x,y:x+y, range(5), range(2, 7))
>>> map(lambda x:x+x, range(5))
>>> map(None, [1, 3, 5], [2, 4, 6])                  此时等同于zip函数

>>> reduce(lambda x,y:x*y, (1,2), 5)                 5*1*2

>>> filter(lambda x:x%2,i range(10))
{% endhighlight %}

### lambda 函数

快速定义单行的最小函数，类似于内联函数，不允许使用if函数。

{% highlight text %}
>>> g = lambda x: x+2
>>> g(3)
>>> (lambda x: x+2)(3)

>>> collapse = True
>>> processFunc = collapse and (lambda s: " ".join(s.split())) or (lambda s: s)
>>> s = "this   is\na\ttest"
>>> print s
>>> print s.split()
>>> print " ".join(s.split())
{% endhighlight %}

processFunc 会根据 collapse 输出 "this is a test" 或者"this   is\na\ttest" 。


### 字符串

在 Python 中，字符串不以 NULL 结尾，内部表示时会指定字符串的长度，而且一个字符串中可以有多个 NULL 字符。

#### 前缀 r R

在 Python 的 string 前面加上 \'r\'， 是为了告诉编译器这个 string 是个 raw string，不要转义 \'\\\' 。 例如，\n 在 raw string 中，是两个字符，\ 和 n， 而不会转意为换行符。由于正则表达式和 \ 会有冲突，因此，当一个字符串使用了正则表达式后，最好在前面加上 \'r\' 。

#### 前缀 u U

以 u 或 U 开头的字符串表示 unicode 字符串。





{% highlight python %}
{% endhighlight %}
