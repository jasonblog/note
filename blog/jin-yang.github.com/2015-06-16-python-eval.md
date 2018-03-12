---
title: Python 动态执行
layout: post
language: chinese
comments: true
category: [program, python]
keywords: python,eval,动态执行
description: 在 Python 中，允许通过 exec 和 eval 执行以字符串形式表示的代码片段，这体现了动态语言的特性，可以让代码变得更灵活。实际上，在使用 exec 和 eval 时，也是需要进行编译的，没错是 "编译"。 是与 C 需要编译成机器码不同，CPython 需要编译成字节码 (ByteCode) 。接下来，我们看看 Python 的动态执行相关内容。
---

在 Python 中，允许通过 exec 和 eval 执行以字符串形式表示的代码片段，这体现了动态语言的特性，可以让代码变得更灵活。实际上，在使用 exec 和 eval 时，也是需要进行编译的，没错是 "编译"。

只是与 C 需要编译成机器码不同，CPython 需要编译成字节码 (ByteCode) 。

接下来，我们看看 Python 的动态执行相关内容。

<!-- more -->

## 简介

在采用了这种 "动态" 的用法后，让代码引入了某些不安定因素，例如，这些代码片段执行后可能对全局造成影响，尤其是当使用全局名称空间时，它的作用范围难以控制。

另外，对执行的效率也有影响，如上所述，Python 在执行代码之前也是要预编译的，因此这些字符串形式的代码片段在执行的时候，需要编译的过程，哪怕是使用 compile 编译后重复使用，第一次的编译是难以避免的。

当然，一般来说，一个 Python 源码都是需要进行编译的，只是有些通过 import 的模块已经是编译之后的了，实际上加载的是 pyc 的格式，此时省去了编译的过程。

### import 过程

接下来我们看看 import 的导入过程。

1. 首先找到相应的模块，实际上就是查找 `builtin` 的模块，然后遍历 `sys.path` 的目录。

2. 加载 pyc 或者 py 文件。

    如果存在字节码文件，且 checksum 与当前解析器匹配，除非部分在 `.py` 文件，否则 `.pyc` 的时间戳要等于或者新于 `.py` 文件，此时会加载 `.pyc` 文件。

    如果不满足上述条件，则会编译 `.py` 源码。

3. 如果从文件加载，在执行代码前，会设置 `__file__`、`__package__`、`__path__` 等变量。

4. 解析器执行模块中的代码。

5. 将该模块插入到 `sys.modules` 中。

上述是 Python 解析器加载过程，在这一过程中肯定不会使用到类似 exec 的加载方法，如果在 Python 中要进行编译加载可以通过如下方式实现。

{% highlight python %}
>>> code = compile('a = 1 + 2', '<string>', 'exec')
>>> type(code)
<type 'code'>
>>> exec code
>>> print a
3
{% endhighlight %}

上述是执行编译后的字节码，当然编译后的类型是 code，接下来看看 compile() 使用方式。

{% highlight text %}
compile(source, filename, mode[, flags[, dont_inherit]])
{% endhighlight %}

该函数将 source 代码编译成字节码，然后可以通过 exec 或者 eval() 执行；filename 指定文件标示 (hits)，用来打印运行期间出错时的文件；mode 为如下的三种类型之一：

* eval: 配合 eval() 使用，不过只能为表达式 (expression)，表达式是由表示符 (identifiers)、常量内建类型 (literals)、数学操作符+布尔操作符 (operators)、函数调用()、下标操作[]；

* exec: 配合多语句的 exec 使用，可以使用声明 (statements)，可以有 for、if 等声明语句，可以是单行或多行，当然，表达式也是声明的一种；

* single: 配合单一语句的 exec 使用，常为 print、raw_input 等交互式操作。

如下是表达式和声明的示例：

{% highlight python %}
#----- eval，表达式
print 42
if x: do_y()
return
a = 7

#----- exec，声明
3 + 5
map(lambda x: x*x, range(10))
[a.x for a in some_iterable]
yield 7
{% endhighlight %}

可以参考如下的一些与 compile() 相关的常见示例：

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

当然，在通过 exec 执行的时候，也可以使用命名空间直接访问，示例如下，详细的内容后面介绍。

{% highlight python %}
#!/usr/bin/env python
code = compile('a = 1 + 2', '<string>', 'exec')
ns = {}
exec code in ns
print ns['a']
{% endhighlight %}

## 使用

包括上述的 `compile()` 函数，可以通过 `exec`、`execfile()`、`eval()`、`compile()` 动态执行一段 Python 代码。

{% highlight python %}
execfile(filename[, globals[, locals]])
eval(expression[, globals[, locals]])
compile(source, filename, mode[, flags[, dont_inherit]])
{% endhighlight %}

exec 在 Python2 中是一个语法声明 (statement)，类似于 if where；在 Python3 中是一个内建函数。在 Python2 中，可以跟代码字符串、文件名、代码对象(通过compile()编译后)、元组，因为可以跟元组，导致使其看起来很像一个函数，不过该方式与 Python3 兼容。

execfile() 与 exec 相似，不过执行的是文件，与 import 的区别是：不会调用模块管理部分的代码，不会创建新的模块(module)。

eval() 是内建函数，它会解析 (parsed) 并对表达式求值 (evaluated)。与 exec 的主要区别是：eval 会返回计算结果，而 exec 只是执行；exec 可以执行很多语句，而 eval 只能进行估值。

{% highlight python %}
a = 10
exec("a + 10")               # works
# ret = exec("a + 10")       # error
ret = eval("a + 10")         # works
print ret
{% endhighlight %}

上述两个函数均可以指定作用域。

### 作用域

也就是命名空间 (namespace)，包括了 local + global 。注意：对于非执行脚本的 local 空间，是不能直接通过字典修改的，所谓的 locals() 返回的字典值，实际上是从本地栈帧中提取出来的字典格式。

可以参考如下的示例：

{% highlight python %}
#!/usr/bin/env python
a = 42
locals()['a'] = 23             # try to modify a local var.
print a                        # oops, this is 23 instead of 42.
print (globals() is locals())  # this is true.
{% endhighlight %}

对于一个可执行的 python 文件，由于 locals 等价于 globals，也就导致 locals 特性与 globals 相同，可以通过字典进行修改。

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

上述的示例是符合之前的原理的，也就是不能通过 locals() 来修改局部变量，当然也有一些方法去修改局部变量，在此就不再讨论了，可以查看 [Be careful with exec and eval in Python]()。另外需要注意的是，局部变量可以在编译时直接通过 index 定位，所以访问速度也比全局变量要快的多。

那么，在动态执行的时候，该如何指定命名空间呢？上述的 execfile()、eval() 都可以在参数中指定，当然，在 Python3 中，exec 也是个函数，所以也可以通过参数指定。

而对于 Python2 中的 exec，如果没有通过 in 指定，则使用当前域；后跟 in 选项指定一个字典时，该字典将作为 local 和 global 变量作用域；如果跟两个则分别指定 global 和 local 。


### 执行速度

编译+运行远大于直接运行的速度，这是因为解析源码然后转换为字节码的过程非常的耗时，可以通过如下的代码进行测试。

{% highlight text %}
$ python -s -mtimeit 'code = "a = 2; b = 3; c = a * b"' 'exec code'               # 耗时，编译+运行
$ python -s -mtimeit 'code = compile("a = 2; b = 3; c = a * b", "&lt;string&gt;", "exec")' 'exec code'
{% endhighlight %}

### 总结

如果一段动态代码需要执行多次，那么最好先通过 compile() 编译，然后采用空的字典作为 namespace 。


### eval()的安全性

假设通过如下的脚本动态执行一个表达式，然后返回结果，我们希望用户输入的是类似 1+2 这样的表达式，但是用户可以通过如下的方式直接删掉一个数据。

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
Input expression: __import__('os').system('dir')     # 可以执行系统命令，包括rm
Input expression: open('foobar').read()              # 可以读取文件
Input expression: globals()                          # 查看可以执行的函数，或者执行globals()
Input expression: foo()                              # 可以直接执行刚才查看到的函数
{% endhighlight %}

解决方法是在使用 `eval()` 时设置全局和局部作用域为空 `eval(s, {}, {})` ，但是此时仍然可以执行内建的函数，包括 `abs()`、`__import__`、`open()` 等。

更严厉的方法是在 globals 参数中将 `__builtins__` 设置为 None，可以使用的函数在 locals 中设置。需要注意的是，此时自定义的函数仍然可以使用通用的函数，例如上述的 foo() 还可以调用系统函数，所以同样要对参数进行检查。

详细可以参考 [Using eval() safely in python](http://lybniz2.sourceforge.net/safeeval.html)，还可以参考 [Restricted "safe" eval (Python recipe)](http://code.activestate.com/recipes/496746-restricted-safe-/) 。


## 其它

在 [Be careful with exec and eval in Python]() 这篇文章中指出，在使用 eval 等可能会导致一些问题。

### 导致一些模块不可用

正常来说，函数、类是从模块中导入的，那么相应模块就会保存 `__module__` 变量，而有些模块是依赖于这一模式的，如：pickle, inspect, pkgutil, pydoc。

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

现在不太确定什么场景下会触发这种情况。

### 导致对象不能回收

在一些场景下可能会导致内存泄露。这与 Python 的垃圾回收机制有关，正常采用的是引用计数，为了解决循环引用，引入了 gc 模块，但是如果对于对象中存在 `__del__()` 方法，那么将导致对象不能回收。

{% highlight text %}
$ cat test.py
class Foo(object):
    def __del__(self):
        print 'Deleted'
foo = Foo()

$ python test.py                        # 正常执行没有问题
Deleted

$ python
>>> execfile("test.py", {})
>>> import gc
>>> gc.collect()
>>> gc.garbage                              # 会回收垃圾
{% endhighlight %}

实际上，如果通过 `execfile("test.py")` `execfile("test.py", {}, {})` 执行可以工作，其中前者采用的是当前的命名空间，但是当前的 local 和 global 是相同的；后者则分别是不同的对象。那么上述的方式是如何产生的循环依赖？估计后面可以拿工具看下。

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
