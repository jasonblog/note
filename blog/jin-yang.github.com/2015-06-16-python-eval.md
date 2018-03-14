---
title: Python 動態執行
layout: post
language: chinese
comments: true
category: [program, python]
keywords: python,eval,動態執行
description: 在 Python 中，允許通過 exec 和 eval 執行以字符串形式表示的代碼片段，這體現了動態語言的特性，可以讓代碼變得更靈活。實際上，在使用 exec 和 eval 時，也是需要進行編譯的，沒錯是 "編譯"。 是與 C 需要編譯成機器碼不同，CPython 需要編譯成字節碼 (ByteCode) 。接下來，我們看看 Python 的動態執行相關內容。
---

在 Python 中，允許通過 exec 和 eval 執行以字符串形式表示的代碼片段，這體現了動態語言的特性，可以讓代碼變得更靈活。實際上，在使用 exec 和 eval 時，也是需要進行編譯的，沒錯是 "編譯"。

只是與 C 需要編譯成機器碼不同，CPython 需要編譯成字節碼 (ByteCode) 。

接下來，我們看看 Python 的動態執行相關內容。

<!-- more -->

## 簡介

在採用了這種 "動態" 的用法後，讓代碼引入了某些不安定因素，例如，這些代碼片段執行後可能對全局造成影響，尤其是當使用全局名稱空間時，它的作用範圍難以控制。

另外，對執行的效率也有影響，如上所述，Python 在執行代碼之前也是要預編譯的，因此這些字符串形式的代碼片段在執行的時候，需要編譯的過程，哪怕是使用 compile 編譯後重復使用，第一次的編譯是難以避免的。

當然，一般來說，一個 Python 源碼都是需要進行編譯的，只是有些通過 import 的模塊已經是編譯之後的了，實際上加載的是 pyc 的格式，此時省去了編譯的過程。

### import 過程

接下來我們看看 import 的導入過程。

1. 首先找到相應的模塊，實際上就是查找 `builtin` 的模塊，然後遍歷 `sys.path` 的目錄。

2. 加載 pyc 或者 py 文件。

    如果存在字節碼文件，且 checksum 與當前解析器匹配，除非部分在 `.py` 文件，否則 `.pyc` 的時間戳要等於或者新於 `.py` 文件，此時會加載 `.pyc` 文件。

    如果不滿足上述條件，則會編譯 `.py` 源碼。

3. 如果從文件加載，在執行代碼前，會設置 `__file__`、`__package__`、`__path__` 等變量。

4. 解析器執行模塊中的代碼。

5. 將該模塊插入到 `sys.modules` 中。

上述是 Python 解析器加載過程，在這一過程中肯定不會使用到類似 exec 的加載方法，如果在 Python 中要進行編譯加載可以通過如下方式實現。

{% highlight python %}
>>> code = compile('a = 1 + 2', '<string>', 'exec')
>>> type(code)
<type 'code'>
>>> exec code
>>> print a
3
{% endhighlight %}

上述是執行編譯後的字節碼，當然編譯後的類型是 code，接下來看看 compile() 使用方式。

{% highlight text %}
compile(source, filename, mode[, flags[, dont_inherit]])
{% endhighlight %}

該函數將 source 代碼編譯成字節碼，然後可以通過 exec 或者 eval() 執行；filename 指定文件標示 (hits)，用來打印運行期間出錯時的文件；mode 為如下的三種類型之一：

* eval: 配合 eval() 使用，不過只能為表達式 (expression)，表達式是由表示符 (identifiers)、常量內建類型 (literals)、數學操作符+布爾操作符 (operators)、函數調用()、下標操作[]；

* exec: 配合多語句的 exec 使用，可以使用聲明 (statements)，可以有 for、if 等聲明語句，可以是單行或多行，當然，表達式也是聲明的一種；

* single: 配合單一語句的 exec 使用，常為 print、raw_input 等交互式操作。

如下是表達式和聲明的示例：

{% highlight python %}
#----- eval，表達式
print 42
if x: do_y()
return
a = 7

#----- exec，聲明
3 + 5
map(lambda x: x*x, range(10))
[a.x for a in some_iterable]
yield 7
{% endhighlight %}

可以參考如下的一些與 compile() 相關的常見示例：

{% highlight python %}
#!/usr/bin/env python
exec_code = compile( """for i in range(5): print "iter time: %d" % i""", '', 'exec' )
exec(exec_code)
eval_code = compile( '1+2', '<string>', 'eval')  # '<string>' or ''
print eval(eval_code)
single_code = compile( 'print "pythoner.com"; a = raw_input("input:"); print a', '', 'single' )
exec(single_code)
print a
{% endhighlight %}

當然，在通過 exec 執行的時候，也可以使用命名空間直接訪問，示例如下，詳細的內容後面介紹。

{% highlight python %}
#!/usr/bin/env python
code = compile('a = 1 + 2', '<string>', 'exec')
ns = {}
exec code in ns
print ns['a']
{% endhighlight %}

## 使用

包括上述的 `compile()` 函數，可以通過 `exec`、`execfile()`、`eval()`、`compile()` 動態執行一段 Python 代碼。

{% highlight python %}
execfile(filename[, globals[, locals]])
eval(expression[, globals[, locals]])
compile(source, filename, mode[, flags[, dont_inherit]])
{% endhighlight %}

exec 在 Python2 中是一個語法聲明 (statement)，類似於 if where；在 Python3 中是一個內建函數。在 Python2 中，可以跟代碼字符串、文件名、代碼對象(通過compile()編譯後)、元組，因為可以跟元組，導致使其看起來很像一個函數，不過該方式與 Python3 兼容。

execfile() 與 exec 相似，不過執行的是文件，與 import 的區別是：不會調用模塊管理部分的代碼，不會創建新的模塊(module)。

eval() 是內建函數，它會解析 (parsed) 並對錶達式求值 (evaluated)。與 exec 的主要區別是：eval 會返回計算結果，而 exec 只是執行；exec 可以執行很多語句，而 eval 只能進行估值。

{% highlight python %}
a = 10
exec("a + 10")               # works
# ret = exec("a + 10")       # error
ret = eval("a + 10")         # works
print ret
{% endhighlight %}

上述兩個函數均可以指定作用域。

### 作用域

也就是命名空間 (namespace)，包括了 local + global 。注意：對於非執行腳本的 local 空間，是不能直接通過字典修改的，所謂的 locals() 返回的字典值，實際上是從本地棧幀中提取出來的字典格式。

可以參考如下的示例：

{% highlight python %}
#!/usr/bin/env python
a = 42
locals()['a'] = 23             # try to modify a local var.
print a                        # oops, this is 23 instead of 42.
print (globals() is locals())  # this is true.
{% endhighlight %}

對於一個可執行的 python 文件，由於 locals 等價於 globals，也就導致 locals 特性與 globals 相同，可以通過字典進行修改。

{% highlight python %}
#!/usr/bin/env python
def foobar():
    a = 2
    locals()['a'] = 8               # try to modify a local var.
    print a                         # OK, still 2
    print (globals() is locals())   # false.
    globals()['a'] = 9              # this works.
foobar()
print a                             # 9
{% endhighlight %}

上述的示例是符合之前的原理的，也就是不能通過 locals() 來修改局部變量，當然也有一些方法去修改局部變量，在此就不再討論了，可以查看 [Be careful with exec and eval in Python]()。另外需要注意的是，局部變量可以在編譯時直接通過 index 定位，所以訪問速度也比全局變量要快的多。

那麼，在動態執行的時候，該如何指定命名空間呢？上述的 execfile()、eval() 都可以在參數中指定，當然，在 Python3 中，exec 也是個函數，所以也可以通過參數指定。

而對於 Python2 中的 exec，如果沒有通過 in 指定，則使用當前域；後跟 in 選項指定一個字典時，該字典將作為 local 和 global 變量作用域；如果跟兩個則分別指定 global 和 local 。


### 執行速度

編譯+運行遠大於直接運行的速度，這是因為解析源碼然後轉換為字節碼的過程非常的耗時，可以通過如下的代碼進行測試。

{% highlight text %}
$ python -s -mtimeit 'code = "a = 2; b = 3; c = a * b"' 'exec code'               # 耗時，編譯+運行
$ python -s -mtimeit 'code = compile("a = 2; b = 3; c = a * b", "&lt;string&gt;", "exec")' 'exec code'
{% endhighlight %}

### 總結

如果一段動態代碼需要執行多次，那麼最好先通過 compile() 編譯，然後採用空的字典作為 namespace 。


### eval()的安全性

假設通過如下的腳本動態執行一個表達式，然後返回結果，我們希望用戶輸入的是類似 1+2 這樣的表達式，但是用戶可以通過如下的方式直接刪掉一個數據。

{% highlight text %}
$ cat test.py
#!/usr/bin/env python
def foo():
    print 'this can be a dangerous function'
while True:
    s = raw_input("Input expression: ")
    if s == 'quit':
        break
    print "Result is:", eval(s)
    #print "Result is:", eval(s, {}, {})                                  # safer
    #print "Result is:", eval(s, {"__builtins__": None}, {"abs": abs})    # safest

$ ./test.py
Input expression: __import__('os').system('dir')     # 可以執行系統命令，包括rm
Input expression: open('foobar').read()              # 可以讀取文件
Input expression: globals()                          # 查看可以執行的函數，或者執行globals()
Input expression: foo()                              # 可以直接執行剛才查看到的函數
{% endhighlight %}

解決方法是在使用 `eval()` 時設置全局和局部作用域為空 `eval(s, {}, {})` ，但是此時仍然可以執行內建的函數，包括 `abs()`、`__import__`、`open()` 等。

更嚴厲的方法是在 globals 參數中將 `__builtins__` 設置為 None，可以使用的函數在 locals 中設置。需要注意的是，此時自定義的函數仍然可以使用通用的函數，例如上述的 foo() 還可以調用系統函數，所以同樣要對參數進行檢查。

詳細可以參考 [Using eval() safely in python](http://lybniz2.sourceforge.net/safeeval.html)，還可以參考 [Restricted "safe" eval (Python recipe)](http://code.activestate.com/recipes/496746-restricted-safe-/) 。


## 其它

在 [Be careful with exec and eval in Python]() 這篇文章中指出，在使用 eval 等可能會導致一些問題。

### 導致一些模塊不可用

正常來說，函數、類是從模塊中導入的，那麼相應模塊就會保存 `__module__` 變量，而有些模塊是依賴於這一模式的，如：pickle, inspect, pkgutil, pydoc。

{% highlight text %}
>>> from xml.sax.saxutils import quoteattr
>>> quoteattr.__module__
'xml.sax.saxutils'
>>> pickle.loads(pickle.dumps(quoteattr))
<function quoteattr at 0x1005349b0>
>>> quoteattr.__module__ = 'fake'
>>> pickle.loads(pickle.dumps(quoteattr))
Traceback (most recent call last):
  ..
pickle.PicklingError: Can't pickle quoteattr: it's not found as fake.quoteattr
{% endhighlight %}

現在不太確定什麼場景下會觸發這種情況。

### 導致對象不能回收

在一些場景下可能會導致內存洩露。這與 Python 的垃圾回收機制有關，正常採用的是引用計數，為了解決循環引用，引入了 gc 模塊，但是如果對於對象中存在 `__del__()` 方法，那麼將導致對象不能回收。

{% highlight text %}
$ cat test.py
class Foo(object):
    def __del__(self):
        print 'Deleted'
foo = Foo()

$ python test.py                        # 正常執行沒有問題
Deleted

$ python
>>> execfile("test.py", {})
>>> import gc
>>> gc.collect()
>>> gc.garbage                              # 會回收垃圾
{% endhighlight %}

實際上，如果通過 `execfile("test.py")` `execfile("test.py", {}, {})` 執行可以工作，其中前者採用的是當前的命名空間，但是當前的 local 和 global 是相同的；後者則分別是不同的對象。那麼上述的方式是如何產生的循環依賴？估計後面可以拿工具看下。

<!--
class Foo(object):
    def __del__(self):
        print 'Deleted'
foo = Foo()
print "locals:", locals().keys()
print "globals:", globals().keys()

import gc,sys,objgraph
objgraph.show_refs([foo], too_many = 5 , filename = 'too-many.png')
print sys.getrefcount(foo)
gc.collect()
print gc.garbage
-->


{% highlight python %}
{% endhighlight %}
