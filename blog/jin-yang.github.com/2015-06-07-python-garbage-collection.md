---
title: Python 的垃圾回收
layout: post
language: chinese
comments: true
category: [linux, python]
keywords: python,garbage collection,垃圾回收
description: Python 带有自动垃圾回收机制，但是它真的能处理所有的垃圾回收问题吗？ 是不是有了自动垃圾回收，就不用再担心内存泄露了呢？ 为什么经常会看到文章说，在实现自己的类时，不要使用 \_\_del\_\_() ？ 多当代的语言都实现了垃圾回收机制，包括 Java、Ruby、Lua、Go等，从而将众码农从内存管理中释放出来，那么 Python 的垃圾回收是怎么实现的呢？ 篇文章将详细介绍 Python 垃圾回收的使用方法，包括示例以及相关的源码分析。
---

Python 带有自动垃圾回收机制，但是它真的能处理所有的垃圾回收问题吗？ 是不是有了自动垃圾回收，就不用再担心内存泄露了呢？ 为什么经常会看到文章说，在实现自己的类时，不要使用 \_\_del\_\_() ？

很多当代的语言都实现了垃圾回收机制，包括 Java、Ruby、Lua、Go等，从而将众码农从内存管理中释放出来，那么 Python 的垃圾回收是怎么实现的呢？

这篇文章将详细介绍 Python 垃圾回收的使用方法，包括示例以及相关的源码分析。

<!-- more -->

## 简介

![garbage-logo]{: .pull-right width='230'}
在介绍 Python 的垃圾收集回收（Garbage Collection）之前，先稍微介绍一些背景。

现在的主流语言基本都支持三种最基本的内存分配方式： A）静态分配，如静态变量、全剧变量，通常这一部分内存不需要释放或者回收； B）自动分配，通常由语言或者编译器控制，如栈中局部变量、参数传递； C）动态分配，这部分通常在栈中分配，由用户申请。

那么，垃圾收集机制就是用来处理程序动态分配内存时产生的垃圾。

在没有垃圾回收机制时，比如在写 C 语言程序时，我们需要时刻记着什么时候申请了内存，什么时候需要释放内存！！！如果忘了释放或者在一个条件分支内没有释放内存，那么就会造成内存泄露；如果提前释放了，后面的程序仍然使用这部分内存，就必然导致内存异常。

垃圾回收机制使得众码农不再需要考虑动态内存的管理。

### Python 的垃圾回收

在 Python 中，主要通过引用计数（Reference Counting）进行垃圾回收。引用计数法有其明显的优点，如高效、实现逻辑简单、将垃圾回收随机分配到运行的阶段，正常程序的运行比较平稳。

但是，很多带有自动垃圾回收的语言并未使用引用计数，必然存在者缺点，通常有：

- 逻辑简单，但实现有些麻烦。每个对象需要分配单独的空间来统计引用计数，并且需要对引用计数进行维护，在维护的时候很容易会出错。

- 在一些场景下，可能会比较慢。正常来说垃圾回收会比较平稳运行，但是当需要释放一个大的对象时，比如字典，需要对引用的所有对象循环嵌套调用，从而可能会花费比较长的时间。

- 循环引用。这将是引用计数的致命伤，引用计数对此是无解的，因此必须要使用其它的垃圾回收算法对其进行补充。

也就是说，Python 的垃圾回收机制，很大一部分是为了处理可能产生的循环引用，是对引用计数的补充。<br><br>


Python 采用了 "标记-清除"（Mark and Sweep）算法，解决容器对象可能产生的循环引用问题。

跟其名称一样，该算法在进行垃圾回收时分成了两步，分别是： A）标记阶段，遍历所有的对象，如果是可达的（reachable），也就是还有对象引用它，那么就标记该对象为可达； B）清除阶段，再次遍历对象，如果发现某个对象没有标记为可达，则就将其回收。

注意，在进行垃圾回收的阶段，会暂停整个应用程序，等待标记清除结束后才会恢复应用程序的运行。<br><br>


为了减少垃圾回收的时间，Python 通过 "分代回收"（Generation Collection）以空间换时间的方法提高垃圾回收效率。

分代回收是基于这样的一个统计事实，对于程序，存在一定比例的内存块的生存周期比较短；而剩下的内存块，生存周期会比较长，甚至会从程序开始一直持续到程序结束。这一比例通常在 80%～90% 之间，这种思想简单点说就是：对象存在时间越长，越可能不是垃圾，应该越少去收集。

这样在执行标记-清除算法时可以有效减小遍历的对象数，从而提高垃圾回收的速度。<br><br>


总体来说，在 Python 中，主要通过引用计数进行垃圾回收；通过 "标记-清除" 解决容器对象可能产生的循环引用问题；通过 "分代回收" 以空间换时间的方法提高垃圾回收效率。


## 实战

下面通过 Python 程序大致介绍垃圾回收机制，也可以参考 [Generational GC in Python and Ruby][gc-in-python-ruby]，该文章同时介绍了 Python 和 Ruby 的垃圾回收机制。

### 引用计数

如上所述，Python 的垃圾回收采用引用计数，每个对象都会维护一个引用计数器，当引用计数变为 0 时该对象被回收。

当对象被创建并赋值给变量时，该对象的引用计数被设置为 1；当该对象被赋值给其他变量时，或者作为参数传递给函数，该对象的引用计数同样也加 1 。

当变量被赋值给另外一个对象或者显示销毁时，源对象的引用计数也会自动减 1，只有当减到 0 时，该对象才会被真正删除。

对象的引用计数可以通过 sys 提供的 getrefcount() 函数获得，该函数会包含传参的引用。

{% highlight python %}
>>> foo = "XYZ"                            # 此时引用计数是1
>>> sys.getrefcount(foo)                   # 返回2，包括了getrefcount()的参数引用
>>> bar = foo                              # 2
>>> foo = 123                              # 1
{% endhighlight %}

当字符串对象 "XYZ" 被创建并赋值给 foo 时，引用计数是 1，当增加了一个别名 bar 时，引用计数变成了 2，不过当 foo 被重新赋值给整数对像 123 时，"XYZ" 对象的引用计数自减 1 ，又变成 1 了。

### 循环引用 (Cyclic References)

循环引用可以使一组对象的引用计数不是 0 ，但是这些对象实际上并没有被任何外部变量引用，只是他们之间相互引用。这也就是说，实际上对象应该被删除了，但是引用计数机制却告诉我们不能回收这些对象。

{% highlight python %}
import sys, gc

gc.set_debug(gc.DEBUG_STATS|gc.DEBUG_LEAK)
a=[]
b=[]
a.append(b)
b.append(a)
print 'a(%s) refcount: %d' % (hex(id(a)), sys.getrefcount(a))  # 3
print 'b(%s) refcount: %d' % (hex(id(b)), sys.getrefcount(b))  # 3
del a, b
print gc.collect()
print 'gc.garbage is:', gc.garbage
{% endhighlight %}

在调用 del 之前，列表还被 a、b 引用，称之为可达的（reachable），此时不能删除；在执行 del 之后，两者实际只剩下了对象之间的相互引用，因此是需要被删除的。但是引用计数仍然为 1，Python 的引用计数机制不会回收这两个对象。

如上所述，为了解决循环引用，Python 引入了 "标记-清除" 和 "分代收集" 机制，可以通过 gc.collect() 调用，该函数会返回不可达（unreachable）对象数。

实际上，由于调用了 gc.set_debug()，gc 模块会把垃圾回收是的信息 上面的执行结果显示，有两个是不可达的（unreachable）对象，这也就是需要回收的对象了，当调用 gc.collect() 时，首先会监测到 a、b 都将成为了不可达(unreachable)对象，且循环引用将被拆除，此时 a、b 引用数都是 0，两者将被回收，所以 gc.collect() 将返回 2 。

在上述的示例中，如果只删除 a 或者 b ，实际也不会被垃圾回收处理掉。<br><br>


在执行上述的程序时，可以看到 uncollectable 的对象数是 0 ，也就是没有不可回收的对像。当然，这也意味着可能会出现不可回收的对像，下面看一种循环引用产生的垃圾回收问题。

{% highlight python %}
import sys, gc

class A:
    def __init__(self, name): self.name = name
    def __del__(self): pass
class B:
    def __init__(self, name): self.name = name
    def __del__(self): pass

gc.set_debug(gc.DEBUG_STATS|gc.DEBUG_LEAK)
a = A("ABC")
b = B("XYZ")
print hex(id(a)), hex(id(a.__dict__))
print hex(id(b)), hex(id(b.__dict__))
a.prev = b
b.next = a
del a, b
print gc.collect()
print gc.garbage
{% endhighlight %}

从输出中可以看到 uncollectable 字样，额，很明显这次垃圾回收搞不定了，造成了内存泄漏。

当对象含有 `__del__()` 方法（在 Python 中称为 finalizer）时，对于这样的对象，跟据 Python 的定义，在释放该对象占用的资源前需要调用该函数。由于 Python 的垃圾回收机制不能保证垃圾回收的顺序，可能在删除 b 之后，在 `a.__del__()` 中仍然会调用 b 对象，这样将会造成异常。

为此，Python 采取了比较保守的策略，也就是说对于当自定的类，如果存在 `__del__()` 时，不会对该对象进行垃圾回收。这样的对象，Python 会直接放到一个 garbage 列表中，这个列表在运行期间不会释放，对于 Python 来说不会有内存泄露，但是对于该程序来说，实际上已经发生了内存泄露。<br><br>


这也就是为什么很多 Python 的书籍建议不要使用 `__del__()` 的主要原因。虽然说同时出现定义 `__del__()` 和循环引用的概率比较小，但为了防止内存泄露，在使用 `__del__()` 时一定要慎重，如果要用的话一定要保证没有循环引用。

当用户发现有这种情况时，最根本的时取消 `__del__()` 或者循环引用；如果知道一个安全的次序，那么就可以打破引用环，然后再执行 `del gc.garbage[:]` ，以清空垃圾对象列表，可以参考 [gc – Garbage Collector][gc-module-pymotw] 。<br><br>


对于 gc.garbage ，文档描述为：A list of objects which the collector found to be unreachable but could not be freed (uncollectable objects).

也就是说 gc.garbage 返回的应该是 unreachable 且不能被回收的对象，也就是上述的 a、b（因为有 \_\_del\_\_）。实际上，上面的结果也包含了字典，这个是在 gc.set_debug() 中设置的，gc.DEBUG_LEAK 会包含 gc.DEBUG_SAVEALL，那么所有的 unreachable 对象都将加入 gc.garbage 返回的列表，而不止不能被回收的对象。


### gc 模块

Python 的垃圾回收实际是通过 gc 模块实现的，并将 Python 的垃圾回收管理接口暴露出来，通过该模块可以用来处理垃圾回收相关的工作。

该模块源码保存在 Modules/gcmodule.c 文件中，gc 模块中的函数同时会对应到 gcmodule.c 中的函数，如 gc.get_count() 对应于 gc_get_count()。

常见的函数包括了如下：

- enable()、disable()、isenable()   
  分别用于打开、关闭、查看 Python 的垃圾回收器。注意，当关闭垃圾回收时，只是取消了自动垃圾回收，此时仍然可以通过 gc.collect() 进行回收。

- set_debug()、get_debug()   
  用于设置和过去调试格式，通过设置 (DEBUG_STATS | DEBUG_LEAK) 标志位，实际上就可以打印 gc 模块的所有信息。

- get_count()   
  获取当前的 count 值。

- set_threshold()、get_threshold()   
  设置或者获得每代的上限值。

- collect([generation=2])   
  进行垃圾回收，主要的垃圾回收函数。

- get_referents(obj)   
  返回该对象直接引用的对象，非 container 为 0；其中 class 对象包括了 __dict__ 和类对象。

- get_referrers(obj)   
  返回指向该对象的所有对象，该函数通过遍历分代的所有对象，查找并保存符合的对象。

- get_object()   
  返回分代中的所有对象，该函数会返回一大堆的对象。


### weakref

如上所述，在 Python 中主要通过引用计数来销毁对象，实际上我们可以通过 weakref 模块创建到对象的弱引用，这样 Python 会在对象的引用计数为 0 或只存在对象的弱引用时回收这个对象。

最简单的方式是通过 weakref.ref(obj[, callback]) 来创建一个弱引用对象，然后通过调用实例可以获得原来的对象。创建弱引用的时候可以设置回调函数，该函数会在原对象销毁前调用，并且将 weakref 作为参数传入。

{% highlight python %}
import weakref

class WeakTest(object):
    def __init__(self, name):
        self.name = name
    def __del__(self):
        print 'Deleting %s' % self

def callback(r):
    print "weakref object:", r, "target object dead!"

o = WeakTest('foobar')                                            # 创建新对象
r = weakref.ref(o, callback)                                      # 创建弱引用对象，回调函数为可选
print "the number of weakref in 'o'", weakref.getweakrefcount(o)  # 原对象的弱引用数目
print "and the objects are:", weakref.getweakrefs(o)              # 原对象弱引用的列表
print r() is o                                                    # 可以通过弱引用可以访问原对象
p = weakref.proxy(o)                                              # 代理对象，对其使用等价于对原对象使用
print r().name, p.name                                            # 调用原对象的属性
del o                                                             # 此时r()变为None
{% endhighlight %}

当然，比较方便的方式是通过 proxy() 函数创建代理（proxy），这个代理实际就可以当作原对象使用，对代理对象的操作可以等价于对原对象的操作。

通过弱引用可以很有效的处理循环引用，比如下面的示例。

{% highlight python %}
import weakref

class LeakTest(object):
    def __init__(self):
        print "object(0x%x) born" % id(self)
    def __del__(self):
        print "object(0x%x) dead" % id(self)

A = LeakTest()
B = LeakTest()
A.b = weakref.proxy(B)                                # A.b = B
B.a = weakref.proxy(A)                                # B.a = A
del A, B
{% endhighlight %}

当不使用弱引用时，上述的方式会造成循环引用，通过弱引用可以有效避免循环引用的产生。需要注意的是大部分的对象不能通过弱引用来访问，如常见的 int、string、list等。

如果使用多个弱引用对象，可以考虑使用 WeakKeyDictionary、WeakValueDictionary、WeakSet，详细可以参考 [weakref – Garbage-collectable references to objects][weakref-pymotw] 。


weakref 模块的实现在 Modules/_weakref.c 中，是对 Objects/weakrefobject.c 的封装，如果感兴趣可以参考一下源码。





## 源码

Python 的垃圾回收机制主要在 Modules/gcmodule.c 中实现，同时也通过 gc 模块将相应的接口暴露出来，可以通过 gc 模块查看。

### 相关数据结构

Python 中，每个对象中都会包含 PyObject_HEAD 宏，其中包含了 ob_refcnt 用于统计该对象的引用计数。下面的代码是 int 类型的对象，其它类型于此相似，都会包含 PyObject_HEAD 宏。

{% highlight c %}
#define PyObject_HEAD                    \
    Py_ssize_t            ob_refcnt;     \
    struct _typeobject*   ob_type;

typedef struct {
    PyObject_HEAD
    long ob_ival;
} PyIntObject;
{% endhighlight %}

每次新建、赋值、传参等操作时，都会修改 ob_refcnt，当减小到 0 时，就会回收该对象。<br><br>


我们在开头讲过，Python 通过 "标记-删除" 处理可能出现的循环引用问题。

对于 int、string 等类型来说，是不可能产生循环引用的，因为它们内部不可能有对其它对象的引用。能发生循环引用的，只会发生在容器（container）对象中，如 list、dict、class、instance 等。因此 Python 的垃圾回收只针对这些容器对象，并对这些对象，在内部维护了一个双向链表。

每个容器对象都会包含一个 PyGC_Head 结构。

{% highlight c %}
typedef union _gc_head {
    struct {
        union _gc_head *gc_next;
        union _gc_head *gc_prev;
        Py_ssize_t gc_refs;
    } gc;
    long double dummy;  /* force worst-case alignment */
} PyGC_Head;
{% endhighlight %}

通常来说，我们会把这个列表称为 "有效列表"（active list）；由于 Python 同时使用了 "分代回收"，实际上其中的 0 代就对应着上述的有效列表，新建的对象都会先放置到该列表中。<br><br>


在 Python 中的分代回收采用三代，实际就是通过三个链表实现，每代通过 threshold 表示该代最多容纳对象的个数，当超过该值时会触发垃圾回收，可以通过 gc.get_threshold() 获得该参数。

需要注意的是，上述的阈值并非该代中实际含有的对象数，当前每代的对象数可以通过 gc.get_count() 获得，该函数实际会从该代的头部遍历这个链表。

以 threshold 的默认值为例，也就是 0~2 代分别为 700、10、10，这实际意味着当 0 代超过 700 后会触发回收；1 代超过 700x10 后触发回收；2 代超过 700x10x10 后会触发回收操作。

也就是说如果触发了一次当前代的回收，会把上代的 count 加 1 。
{% highlight c %}
struct gc_generation {
    PyGC_Head head;
    int threshold; /* collection threshold */
    int count;     /* count of allocations or collections of younger generations */
};

#define NUM_GENERATIONS 3
#define GEN_HEAD(n) (&generations[n].head)

static struct gc_generation generations[NUM_GENERATIONS] = {
    /* PyGC_Head,                               threshold,      count */
    { { {GEN_HEAD(0), GEN_HEAD(0), 0} },        700,            0},
    { { {GEN_HEAD(1), GEN_HEAD(1), 0} },        10,             0},
    { { {GEN_HEAD(2), GEN_HEAD(2), 0} },        10,             0},
};
{% endhighlight %}

上述实际是创建了三代的链表，并对 threshold 和 count 初始化。




### 对象初始化

下面通过跟踪一个 list 对象的创建过程来查看 Python 的垃圾回收机制。list 对象通过 PyList_New() 函数创建，其创建过程大致如下。

{% highlight c %}
/* FILE: Objects/listobject.c */
PyObject* PyList_New(Py_ssize_t size) {
    PyListObject *op;
    ... ...
    op = PyObject_GC_New(PyListObject, &PyList_Type);
    ... ...
    _PyObject_GC_TRACK(op);
    return (PyObject *) op;
}
{% endhighlight %}

首先通过 PyObject_GC_New() 初始化一个对象，而这个函数实际上是对 _PyObject_GC_New() 的宏定义，最终调用的是 _PyObject_GC_New()。

该函数主要作了两件事情：A）分配内存结构，同时会将第 0 代的 count 加 1，如果超过了阈值，则会通过 collect_generations() 触发一次垃圾回收； B）初始化数据结构。注意，在这一步中还没有将该对象插入到链表中。

最终插入链表实际是通过 _PyObject_GC_TRACK() 实现的，也就是插入到 _PyGC_generation0 中（第 0 代）。

到此，已经完成了对象的新建，并可以 "监控" 该对象。


### 垃圾回收机制

Python 中的垃圾回收机制实际通过 collect(gen) 实现，其中 gen 是所定义的代数。

当我们一直新建 container 对象时，如果超过了第 0 代的上限，就会通过 collect_generations() 调用 Python 的垃圾回收机制。该函数会从最高代（数字最大）开始检测，当超过阈值之后就会触发该代的垃圾回收操作，所以有可能是从第 1、2 代触发垃圾回收。

当第一次超过 700 后，则会调用 collect_generations()，该函数会从 2 依次向下检测，当超过阈值则会调用 collect()，此时第 1、2 代的 count 仍为 0 ，所以最终会调用 collect(0) 。

接下来主要查看 collect() 函数。

{% highlight c %}
/* FILE: Modules/gcmodule.c */
static Py_ssize_t collect(int generation)
{
    ... ...
    /* ❶ */
    if (generation+1 < NUM_GENERATIONS)
        generations[generation+1].count += 1;
    for (i = 0; i <= generation; i++)
        generations[i].count = 0;

    /* ❷ */
    for (i = 0; i < generation; i++) {
        gc_list_merge(GEN_HEAD(i), GEN_HEAD(generation));
    }
    ... ...

    /* ❸ */
    update_refs(young);
    subtract_refs(young);

    /* ❹ */
    gc_list_init(&unreachable);
    move_unreachable(young, &unreachable);

    /* ❺ */
    gc_list_init(&finalizers);
    move_finalizers(&unreachable, &finalizers);
    move_finalizer_reachable(&finalizers);
}
{% endhighlight %}

假设，第 0 代有如下的对象，每个对象用黑色方框表示，其中数字表示引用计数。其中各个对象是通过双向链表相互链接的，只是没有表示出来，其中双向线表示相互引用，蓝色表示本代外部的引用。

![garbage-explain1]{: .pull-center width='400'}

在第 ❶ 步中，首先处理每代的 count，也就是将上一代的加 1，并将本代以及一下的清零。这也就是之前说的 700、700x10、700x10x10 的关系了。

在第 ❷ 步中，将本代以下的链表合并到本代，以后的操作实际上都是针对于这一链表的操作。也就是说每次 Python 执行垃圾回收时，会检测本代及以下的对象。

接着在第 ❸ 步中，查找含有外部引用的对象，为了不影响对象的原引用计数，先通过 update_refs() 将 ob_refcnt 复制到 gc_refs，后面的操作都是针对 gc_refs；然后通过 subtract_refs() 遍历整个链表，将本代对象的 gc_refs 及其引用减 1。此时，对于 gc_refs > 0 的对象，一定有该代之外的对象引用，是不应该被删除的。

执行完 subtract_refs() 之后，各个对象的引用如下所示，可以看出，在此时并不是说 gc_refs = 0 的对象就一定是垃圾对象。

如 "JKL"，虽然此时计算的 gc_refs = 0，但是该对象不应该删除，因为还被 "MNO" 所引用。

![garbage-explain2]{: .pull-center width='400'}

然后是将 unreachable 的对象分开，在第 ❹ 步中调用 move_unreachable() 函数。该函数会遍历整个链表，在遍历整个链表时，会将 gc_refs=0 的对象移动到 unreachable 列表中，并将 gc_refs 临时设置为 GC_TENTATIVELY_UNREACHABLE。如上所述，因为这些对象仍可能会被 gc_refs > 0 的对象引用，在遍历后续的对象时会重新将这些对象设置为 reachable，通过 visit_reachable() 实现。

到此为止，处于 unreachable 链表中的对象都是不可达的，也就是应该被回收的垃圾。那么接下来就需要对这些 unreachable 的对象进行处理了。

接下来在第 ❺ 步中，实际要遍历 unreachable 链表，并处理含有 finalizers 的对象，通过 move_finalizers() 将 unreachable 链表中含有 \_\_del\_\_ 的对象放到 finalizers 链表中。对于在 finalizers 链表中指向的对象同样也需要放到该链表中，该功能是通过 move_finalizer_reachable() 函数实现。

到此，unreachable 中包含了可以通过 Python 回收的对象，而 finalizers 中包含了不能通过 Python 回收的对象，会将这些对象分别打印出来。在 finalizers 中的对象，会保存到 garbage 中，可以通过 gc.garbage() 查看。<br><br>

接下来还会通过 handle_weakrefs() 函数处理 unreachable 链表中的 weakref 对象；通过 delete_garbage() 处理 unreachable 链表中的垃圾对象。

当然除此之外，Python 也对垃圾回收机制进行了优化，详细可以参考源码中的注释。




## 总结

有 \_\_del\_\_() 函数的对象，如果存在循环引用，会导致内存泄露；如果没有 \_\_del\_\_() 可以自动垃圾回收掉。

需要注意的是，要么在实现类时不要单独实现 \_\_del\_\_() 函数，要么需要确保不要形成循环引用，对于循环引用可以通过 objgraph 进行检测。



[weakref-pymotw]:          http://pymotw.com/2/weakref/index.html                 "PyMOTW 中关于弱引用的介绍"
[gc-module-pymotw]:        http://pymotw.com/2/gc/index.html                      "PyMOTW 中关于垃圾回收的介绍，包括如何消除uncollectable对象"
[gc-in-python-ruby]:       http://patshaughnessy.net/2013/10/30/generational-gc-in-python-and-ruby  "关于Python和Ruby垃圾回收机制的比较"


<!-- pictures -->
[garbage-logo]:            /images/python/garbages/garbage-logo.jpg               "垃圾回收logo"
[garbage-explain1]:        /images/python/garbages/gc-explain1.png                "garbage collection 1"
[garbage-explain2]:        /images/python/garbages/gc-explain2.png                "garbage collection 2"
