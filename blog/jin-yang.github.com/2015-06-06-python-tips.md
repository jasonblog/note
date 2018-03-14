---
title: Python 雜項
layout: post
comments: true
category: [program, python]
language: chinese
keywords: python,雜項
description: 在此記錄與 Python 相關的亂七八糟的東西。
---

在此記錄與 Python 相關的亂七八糟的東西。

<!-- more -->

## 神奇的 property

在 Python 中綁定屬性時，如果直接把屬性暴露出去，雖然寫起來很簡單，但是沒辦法檢查參數；通過 property 即可以將一個函數作為屬性，並在函數中對參數的值進行檢查。

例如，一個學生的成績必須在 [0, 100] 這個區間才可以，那麼我們就可以使用 property 。

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

如上，只需要函數添加 @property 修飾符即可；此時，又會創建另外一個修飾符 @score.setter，用於把一個 setter 方法變成屬性賦值；如果沒有使用 setter，那麼該屬性為只讀。

當然，也可以通過如下方式設置，兩者的作用相同。

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



## with 語句

通常會有一些任務，開始需要進行設置，處理任務，事後做清理工作。對於這種場景，Python 的 with 語句提供了一種非常方便的處理方式。一個很好的例子是文件處理，你需要獲取一個文件句柄，從文件中讀取數據，然後關閉文件句柄。

正常的處理過程以及使用 with 時的處理過程如下：

{% highlight python %}
file = open("/tmp/foo.txt")
try:
    data = file.read()
finally:
    file.close()

with open("/tmp/foo.txt") as file:
    data = file.read()
{% endhighlight %}

基本思想是 with 之後的函數返回的對象必須有  \_\_enter\_\_() 和 \_\_exit\_\_() 方法。

緊跟 with 後面的語句被求值後，返回對象的 \_\_enter\_\_() 被調用，這個方法的返回值將被賦值給 as 後面的變量；當 with 後面的代碼塊全部被執行完之後，將調用前面返回對象的 \_\_exit\_\_() 。

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

執行的順序為 A) get_sample(); 返回 Sample 對象； B) Sample:\_\_enter\_\_(); 將返回的值賦值給 sample； C) 執行代碼塊； D) 執行 Sample:\_\_exit\_\_() 。

with 真正強大之處是它可以處理異常。\_\_exit\_\_() 有三個參數，val, type 和 trace，這些參數在異常處理中相當有用。

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

在 with 後面的代碼塊拋出任何異常時，\_\_exit\_\_() 方法被執行。正如例子所示，異常拋出時，與之關聯的 type，value 和 stack trace 傳給 \_\_exit\_\_() 方法，因此拋出的 ZeroDivisionError 異常被打印出來了。



## 循環緩衝區

Python 中可以很簡單的實現一個固定長度的緩衝區，也就是固定長度的 FIFO 隊列。

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

可以通過如下方式使用。


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

另外，實際上可以通過 pop() 和 insert(0, x) 將緩衝區正向輸入，如上述的順序剛好相反。


## 二進制文件操作

對於二進制文件，可以使用 struct 模塊中的 pack、unpack 函數進行讀寫操作；當然對於單個 Byte 可以通過 ord()、hex() 等函數執行。

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
print hex(ord(b))   # 先通過ord()將一個字節轉成一個數，然後轉成16進制的字符串
file.close()
{% endhighlight %}

通過 struct 模塊寫入時，保存的時 struct 轉換後的格式；如果要原生寫入，可以通過如下的方式寫入。

{% highlight python %}
#!/usr/bin/env python
#-*- coding:utf-8 -*-

file = open("test.bin","wb")
file.write("\x5F\x9D\x3E" + ("%c%c" % (0x45, 0xF3)))     # 直接寫入

import binascii
hexs = "5B7F"
file.write(binascii.a2b_hex(hexs))

file.close()
{% endhighlight %}

其中 struct 模塊是基於 C 語言編寫的庫，可以參考官方文檔 [struct — Interpret strings as packed binary data](https://docs.python.org/2/library/struct.html) 。

## docstrings

也就是 Python 的文檔字符串，提供了一種很方便的方式將文檔與 modules、functions、classes 以及 methods ；該字符串是在對象的第一個語句中定義，然後通過 ```__doc__``` 引用。

{% highlight python %}
#!/usr/bin/env python
#-*- coding:utf-8 -*-
"""
This is an example for python docstring, including modules,
    functions, classes, methods, etc.
"""

import math
print math.__doc__            # 標準庫的文檔字符串
print str.__doc__             # 標準函數的文檔字符串

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
print f.__doc__               # 通過實例返回類的文檔字符串
print f.get_doc()             # 或者調用函數返回文檔字符串
print __doc__                 # 本模塊的文檔字符串
print foo.__doc__             # 函數的文檔字符串
print foobar.__doc__          # 自定義類的文檔字符串
print foobar.bar.__doc__      # 類中方法的文檔字符串
{% endhighlight %}

當然，也可以使用 pydoc 模塊，這個模塊是 Python 2.1 引入的，提供了一個交互界面顯示對象的信息，當然也包括了文檔字符串。

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

可以使用 #!/usr/bin/python 或者 #!/usr/bin/env python ，env 可以通過指定的環境變量執行命令。


### 使用 UTF-8

對於非英文的編碼可以使用如下的方法之一。

{% highlight python %}
#-*- coding:utf-8 -*-
#xxx coding:utf-8 xxx
#encoding: utf-8
#coding: utf-8
{% endhighlight %}


### 帶 * 參數

在 python 函數參數中，星號的作用是：*)傳入任意長度的元組；**)傳入任意長度的字典。如果兩者之前有固定的參數列表，則先填充前面的參數。

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


### 列表推導

列表推導的格式為 [ expression for variables in list [if condition]]，推導的變量可以有一個或者多個，如果是多個通常為笛卡爾乘積。

{% highlight python %}
print [x*y for x in [1,2,3] for y in [1,2,3]]
print [(x, y) for x in range(10) if x % 2 if x > 3 for y in range(10) if y > 7 if y != 8]
print [line.rstrip() for line in open('test.txt') if line[0]=='n']
print dict([(x,x*10) for x in range(3)])

print {x:x % 2 == 0 for x in range(1, 11)}     # Python3
{% endhighlight %}

### 三元運算

在 Python 中 0、''、[]、()、{}、None 都為假，而且對於 and 如果所有值為真則會返回最後一個值，否則返回第一個假值；對於 or 如果有一個值為真則返回，否則返回最後一個值。

{% highlight text %}
'' and 'b'             # 返回 ''
'a' and 'b' and 'c'    # 返回 'c'
'a' or 'b'             # 返回 'a'
'' or [] or {}         # 返回 {}
{% endhighlight %}

對於三元運算，如取最小值 small = x if x < y else y。也可以使用 and-or ，如 (1 and [a] or [b])[0]，使用列表是為了防止 a 為假。


### join()格式輸出

生成一串數字，中間用逗號分割，其中 range 可以用 xrange 替換。

{% highlight python %}
print ','.join([str(i) for i in range(1,6)])
print ','.join(["%s"%i for i in range(1,6)])
print ",".join(map(lambda i:str(i), range(1,6)))
{% endhighlight %}





### None 判斷

通常的判斷方式可能會有多種，不過可能在使用時有很多的問題，列舉如下。

#### if x is None / x==None

(ob1 is ob2) 等價於 (id(ob1) == id(ob2))，也即當比較相同的對象實例，is 總是返回 True，而 == 最終取決於 "eq()"。

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

需要注意此時無法區分 x==[] 和 x==None，在 Python 中，None、False、空字符串""、0、空列表[]、空字典{}、空元組() 都相當於 False。

#### if not x is None

實際為 if not (x is None)，最好使用 if x is not None 。


### os._exit()/sys.exit()

Python 程序有兩中退出方式:

1. os._exit() 會直接將程序終止，之後的所有代碼都不會繼續執行；

2. sys.exit() 會引發一個 SystemExit 異常，如果這個異常沒有被捕獲，那麼 python 解釋器將會退出，如果有捕獲此異常的代碼，那麼這些代碼還是會執行。

{% highlight python %}
try:
    sys.exit()
except SystemExit:   # 不包含信息，不要使用 SystemExit, e
    print "die"
{% endhighlight %}

### 內置函數 map()、filter()、reduce()

這三個函數比較類似，都是應用於序列 (list, tuple, str等) 的內置函數。

#### map(function, sequence[, sequence, ...]) return: list

對 sequence 中的 item 依次執行 function(item)，執行結果輸出為 list。如果長度不同，則以最長為準，最短以 None 補齊。

#### reduce(function, sequence[, initial]) return: value

對 sequence 中的 item 順序迭代調用 function，必須要有 2 個參數。如果有第 3 個參數，則表示初始值。需要注意的是，function 不能為 None 。

#### filter(function or None, sequence) return list, tuple, or string

對 sequence 中的 item 依次執行 function(item)，將執行結果為 True 的 item 組成 list、string、tuple，這取決於 sequence 的類型。

以下為部分示例。
{% highlight text %}
>>> def add(m, n): return m+n
>>> map(add, range(5), range(2, 7))                  序列的長度必須相同
>>> map(lambda x,y:x+y, range(5), range(2, 7))
>>> map(lambda x:x+x, range(5))
>>> map(None, [1, 3, 5], [2, 4, 6])                  此時等同於zip函數

>>> reduce(lambda x,y:x*y, (1,2), 5)                 5*1*2

>>> filter(lambda x:x%2,i range(10))
{% endhighlight %}

### lambda 函數

快速定義單行的最小函數，類似於內聯函數，不允許使用if函數。

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

processFunc 會根據 collapse 輸出 "this is a test" 或者"this   is\na\ttest" 。


### 字符串

在 Python 中，字符串不以 NULL 結尾，內部表示時會指定字符串的長度，而且一個字符串中可以有多個 NULL 字符。

#### 前綴 r R

在 Python 的 string 前面加上 \'r\'， 是為了告訴編譯器這個 string 是個 raw string，不要轉義 \'\\\' 。 例如，\n 在 raw string 中，是兩個字符，\ 和 n， 而不會轉意為換行符。由於正則表達式和 \ 會有衝突，因此，當一個字符串使用了正則表達式後，最好在前面加上 \'r\' 。

#### 前綴 u U

以 u 或 U 開頭的字符串表示 unicode 字符串。





{% highlight python %}
{% endhighlight %}
