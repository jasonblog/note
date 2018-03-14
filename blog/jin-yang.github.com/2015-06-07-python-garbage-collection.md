---
title: Python 的垃圾回收
layout: post
language: chinese
comments: true
category: [linux, python]
keywords: python,garbage collection,垃圾回收
description: Python 帶有自動垃圾回收機制，但是它真的能處理所有的垃圾回收問題嗎？ 是不是有了自動垃圾回收，就不用再擔心內存洩露了呢？ 為什麼經常會看到文章說，在實現自己的類時，不要使用 \_\_del\_\_() ？ 多當代的語言都實現了垃圾回收機制，包括 Java、Ruby、Lua、Go等，從而將眾碼農從內存管理中釋放出來，那麼 Python 的垃圾回收是怎麼實現的呢？ 篇文章將詳細介紹 Python 垃圾回收的使用方法，包括示例以及相關的源碼分析。
---

Python 帶有自動垃圾回收機制，但是它真的能處理所有的垃圾回收問題嗎？ 是不是有了自動垃圾回收，就不用再擔心內存洩露了呢？ 為什麼經常會看到文章說，在實現自己的類時，不要使用 \_\_del\_\_() ？

很多當代的語言都實現了垃圾回收機制，包括 Java、Ruby、Lua、Go等，從而將眾碼農從內存管理中釋放出來，那麼 Python 的垃圾回收是怎麼實現的呢？

這篇文章將詳細介紹 Python 垃圾回收的使用方法，包括示例以及相關的源碼分析。

<!-- more -->

## 簡介

![garbage-logo]{: .pull-right width='230'}
在介紹 Python 的垃圾收集回收（Garbage Collection）之前，先稍微介紹一些背景。

現在的主流語言基本都支持三種最基本的內存分配方式： A）靜態分配，如靜態變量、全劇變量，通常這一部分內存不需要釋放或者回收； B）自動分配，通常由語言或者編譯器控制，如棧中局部變量、參數傳遞； C）動態分配，這部分通常在棧中分配，由用戶申請。

那麼，垃圾收集機制就是用來處理程序動態分配內存時產生的垃圾。

在沒有垃圾回收機制時，比如在寫 C 語言程序時，我們需要時刻記著什麼時候申請了內存，什麼時候需要釋放內存！！！如果忘了釋放或者在一個條件分支內沒有釋放內存，那麼就會造成內存洩露；如果提前釋放了，後面的程序仍然使用這部分內存，就必然導致內存異常。

垃圾回收機制使得眾碼農不再需要考慮動態內存的管理。

### Python 的垃圾回收

在 Python 中，主要通過引用計數（Reference Counting）進行垃圾回收。引用計數法有其明顯的優點，如高效、實現邏輯簡單、將垃圾回收隨機分配到運行的階段，正常程序的運行比較平穩。

但是，很多帶有自動垃圾回收的語言並未使用引用計數，必然存在者缺點，通常有：

- 邏輯簡單，但實現有些麻煩。每個對象需要分配單獨的空間來統計引用計數，並且需要對引用計數進行維護，在維護的時候很容易會出錯。

- 在一些場景下，可能會比較慢。正常來說垃圾回收會比較平穩運行，但是當需要釋放一個大的對象時，比如字典，需要對引用的所有對象循環嵌套調用，從而可能會花費比較長的時間。

- 循環引用。這將是引用計數的致命傷，引用計數對此是無解的，因此必須要使用其它的垃圾回收算法對其進行補充。

也就是說，Python 的垃圾回收機制，很大一部分是為了處理可能產生的循環引用，是對引用計數的補充。<br><br>


Python 採用了 "標記-清除"（Mark and Sweep）算法，解決容器對象可能產生的循環引用問題。

跟其名稱一樣，該算法在進行垃圾回收時分成了兩步，分別是： A）標記階段，遍歷所有的對象，如果是可達的（reachable），也就是還有對象引用它，那麼就標記該對象為可達； B）清除階段，再次遍歷對象，如果發現某個對象沒有標記為可達，則就將其回收。

注意，在進行垃圾回收的階段，會暫停整個應用程序，等待標記清除結束後才會恢復應用程序的運行。<br><br>


為了減少垃圾回收的時間，Python 通過 "分代回收"（Generation Collection）以空間換時間的方法提高垃圾回收效率。

分代回收是基於這樣的一個統計事實，對於程序，存在一定比例的內存塊的生存週期比較短；而剩下的內存塊，生存週期會比較長，甚至會從程序開始一直持續到程序結束。這一比例通常在 80%～90% 之間，這種思想簡單點說就是：對象存在時間越長，越可能不是垃圾，應該越少去收集。

這樣在執行標記-清除算法時可以有效減小遍歷的對象數，從而提高垃圾回收的速度。<br><br>


總體來說，在 Python 中，主要通過引用計數進行垃圾回收；通過 "標記-清除" 解決容器對象可能產生的循環引用問題；通過 "分代回收" 以空間換時間的方法提高垃圾回收效率。


## 實戰

下面通過 Python 程序大致介紹垃圾回收機制，也可以參考 [Generational GC in Python and Ruby][gc-in-python-ruby]，該文章同時介紹了 Python 和 Ruby 的垃圾回收機制。

### 引用計數

如上所述，Python 的垃圾回收採用引用計數，每個對象都會維護一個引用計數器，當引用計數變為 0 時該對象被回收。

當對象被創建並賦值給變量時，該對象的引用計數被設置為 1；當該對象被賦值給其他變量時，或者作為參數傳遞給函數，該對象的引用計數同樣也加 1 。

當變量被賦值給另外一個對象或者顯示銷燬時，源對象的引用計數也會自動減 1，只有當減到 0 時，該對象才會被真正刪除。

對象的引用計數可以通過 sys 提供的 getrefcount() 函數獲得，該函數會包含傳參的引用。

{% highlight python %}
>>> foo = "XYZ"                            # 此時引用計數是1
>>> sys.getrefcount(foo)                   # 返回2，包括了getrefcount()的參數引用
>>> bar = foo                              # 2
>>> foo = 123                              # 1
{% endhighlight %}

當字符串對象 "XYZ" 被創建並賦值給 foo 時，引用計數是 1，當增加了一個別名 bar 時，引用計數變成了 2，不過當 foo 被重新賦值給整數對像 123 時，"XYZ" 對象的引用計數自減 1 ，又變成 1 了。

### 循環引用 (Cyclic References)

循環引用可以使一組對象的引用計數不是 0 ，但是這些對象實際上並沒有被任何外部變量引用，只是他們之間相互引用。這也就是說，實際上對象應該被刪除了，但是引用計數機制卻告訴我們不能回收這些對象。

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

在調用 del 之前，列表還被 a、b 引用，稱之為可達的（reachable），此時不能刪除；在執行 del 之後，兩者實際只剩下了對象之間的相互引用，因此是需要被刪除的。但是引用計數仍然為 1，Python 的引用計數機制不會回收這兩個對象。

如上所述，為了解決循環引用，Python 引入了 "標記-清除" 和 "分代收集" 機制，可以通過 gc.collect() 調用，該函數會返回不可達（unreachable）對象數。

實際上，由於調用了 gc.set_debug()，gc 模塊會把垃圾回收是的信息 上面的執行結果顯示，有兩個是不可達的（unreachable）對象，這也就是需要回收的對象了，當調用 gc.collect() 時，首先會監測到 a、b 都將成為了不可達(unreachable)對象，且循環引用將被拆除，此時 a、b 引用數都是 0，兩者將被回收，所以 gc.collect() 將返回 2 。

在上述的示例中，如果只刪除 a 或者 b ，實際也不會被垃圾回收處理掉。<br><br>


在執行上述的程序時，可以看到 uncollectable 的對象數是 0 ，也就是沒有不可回收的對像。當然，這也意味著可能會出現不可回收的對像，下面看一種循環引用產生的垃圾回收問題。

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

從輸出中可以看到 uncollectable 字樣，額，很明顯這次垃圾回收搞不定了，造成了內存洩漏。

當對象含有 `__del__()` 方法（在 Python 中稱為 finalizer）時，對於這樣的對象，跟據 Python 的定義，在釋放該對象佔用的資源前需要調用該函數。由於 Python 的垃圾回收機制不能保證垃圾回收的順序，可能在刪除 b 之後，在 `a.__del__()` 中仍然會調用 b 對象，這樣將會造成異常。

為此，Python 採取了比較保守的策略，也就是說對於當自定的類，如果存在 `__del__()` 時，不會對該對象進行垃圾回收。這樣的對象，Python 會直接放到一個 garbage 列表中，這個列表在運行期間不會釋放，對於 Python 來說不會有內存洩露，但是對於該程序來說，實際上已經發生了內存洩露。<br><br>


這也就是為什麼很多 Python 的書籍建議不要使用 `__del__()` 的主要原因。雖然說同時出現定義 `__del__()` 和循環引用的概率比較小，但為了防止內存洩露，在使用 `__del__()` 時一定要慎重，如果要用的話一定要保證沒有循環引用。

當用戶發現有這種情況時，最根本的時取消 `__del__()` 或者循環引用；如果知道一個安全的次序，那麼就可以打破引用環，然後再執行 `del gc.garbage[:]` ，以清空垃圾對象列表，可以參考 [gc – Garbage Collector][gc-module-pymotw] 。<br><br>


對於 gc.garbage ，文檔描述為：A list of objects which the collector found to be unreachable but could not be freed (uncollectable objects).

也就是說 gc.garbage 返回的應該是 unreachable 且不能被回收的對象，也就是上述的 a、b（因為有 \_\_del\_\_）。實際上，上面的結果也包含了字典，這個是在 gc.set_debug() 中設置的，gc.DEBUG_LEAK 會包含 gc.DEBUG_SAVEALL，那麼所有的 unreachable 對象都將加入 gc.garbage 返回的列表，而不止不能被回收的對象。


### gc 模塊

Python 的垃圾回收實際是通過 gc 模塊實現的，並將 Python 的垃圾回收管理接口暴露出來，通過該模塊可以用來處理垃圾回收相關的工作。

該模塊源碼保存在 Modules/gcmodule.c 文件中，gc 模塊中的函數同時會對應到 gcmodule.c 中的函數，如 gc.get_count() 對應於 gc_get_count()。

常見的函數包括瞭如下：

- enable()、disable()、isenable()   
  分別用於打開、關閉、查看 Python 的垃圾回收器。注意，當關閉垃圾回收時，只是取消了自動垃圾回收，此時仍然可以通過 gc.collect() 進行回收。

- set_debug()、get_debug()   
  用於設置和過去調試格式，通過設置 (DEBUG_STATS | DEBUG_LEAK) 標誌位，實際上就可以打印 gc 模塊的所有信息。

- get_count()   
  獲取當前的 count 值。

- set_threshold()、get_threshold()   
  設置或者獲得每代的上限值。

- collect([generation=2])   
  進行垃圾回收，主要的垃圾回收函數。

- get_referents(obj)   
  返回該對象直接引用的對象，非 container 為 0；其中 class 對象包括了 __dict__ 和類對象。

- get_referrers(obj)   
  返回指向該對象的所有對象，該函數通過遍歷分代的所有對象，查找並保存符合的對象。

- get_object()   
  返回分代中的所有對象，該函數會返回一大堆的對象。


### weakref

如上所述，在 Python 中主要通過引用計數來銷燬對象，實際上我們可以通過 weakref 模塊創建到對象的弱引用，這樣 Python 會在對象的引用計數為 0 或只存在對象的弱引用時回收這個對象。

最簡單的方式是通過 weakref.ref(obj[, callback]) 來創建一個弱引用對象，然後通過調用實例可以獲得原來的對象。創建弱引用的時候可以設置回調函數，該函數會在原對象銷燬前調用，並且將 weakref 作為參數傳入。

{% highlight python %}
import weakref

class WeakTest(object):
    def __init__(self, name):
        self.name = name
    def __del__(self):
        print 'Deleting %s' % self

def callback(r):
    print "weakref object:", r, "target object dead!"

o = WeakTest('foobar')                                            # 創建新對象
r = weakref.ref(o, callback)                                      # 創建弱引用對象，回調函數為可選
print "the number of weakref in 'o'", weakref.getweakrefcount(o)  # 原對象的弱引用數目
print "and the objects are:", weakref.getweakrefs(o)              # 原對象弱引用的列表
print r() is o                                                    # 可以通過弱引用可以訪問原對象
p = weakref.proxy(o)                                              # 代理對象，對其使用等價於對原對象使用
print r().name, p.name                                            # 調用原對象的屬性
del o                                                             # 此時r()變為None
{% endhighlight %}

當然，比較方便的方式是通過 proxy() 函數創建代理（proxy），這個代理實際就可以當作原對象使用，對代理對象的操作可以等價於對原對象的操作。

通過弱引用可以很有效的處理循環引用，比如下面的示例。

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

當不使用弱引用時，上述的方式會造成循環引用，通過弱引用可以有效避免循環引用的產生。需要注意的是大部分的對象不能通過弱引用來訪問，如常見的 int、string、list等。

如果使用多個弱引用對象，可以考慮使用 WeakKeyDictionary、WeakValueDictionary、WeakSet，詳細可以參考 [weakref – Garbage-collectable references to objects][weakref-pymotw] 。


weakref 模塊的實現在 Modules/_weakref.c 中，是對 Objects/weakrefobject.c 的封裝，如果感興趣可以參考一下源碼。





## 源碼

Python 的垃圾回收機制主要在 Modules/gcmodule.c 中實現，同時也通過 gc 模塊將相應的接口暴露出來，可以通過 gc 模塊查看。

### 相關數據結構

Python 中，每個對象中都會包含 PyObject_HEAD 宏，其中包含了 ob_refcnt 用於統計該對象的引用計數。下面的代碼是 int 類型的對象，其它類型於此相似，都會包含 PyObject_HEAD 宏。

{% highlight c %}
#define PyObject_HEAD                    \
    Py_ssize_t            ob_refcnt;     \
    struct _typeobject*   ob_type;

typedef struct {
    PyObject_HEAD
    long ob_ival;
} PyIntObject;
{% endhighlight %}

每次新建、賦值、傳參等操作時，都會修改 ob_refcnt，當減小到 0 時，就會回收該對象。<br><br>


我們在開頭講過，Python 通過 "標記-刪除" 處理可能出現的循環引用問題。

對於 int、string 等類型來說，是不可能產生循環引用的，因為它們內部不可能有對其它對象的引用。能發生循環引用的，只會發生在容器（container）對象中，如 list、dict、class、instance 等。因此 Python 的垃圾回收只針對這些容器對象，並對這些對象，在內部維護了一個雙向鏈表。

每個容器對象都會包含一個 PyGC_Head 結構。

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

通常來說，我們會把這個列表稱為 "有效列表"（active list）；由於 Python 同時使用了 "分代回收"，實際上其中的 0 代就對應著上述的有效列表，新建的對象都會先放置到該列表中。<br><br>


在 Python 中的分代回收採用三代，實際就是通過三個鏈表實現，每代通過 threshold 表示該代最多容納對象的個數，當超過該值時會觸發垃圾回收，可以通過 gc.get_threshold() 獲得該參數。

需要注意的是，上述的閾值並非該代中實際含有的對象數，當前每代的對象數可以通過 gc.get_count() 獲得，該函數實際會從該代的頭部遍歷這個鏈表。

以 threshold 的默認值為例，也就是 0~2 代分別為 700、10、10，這實際意味著當 0 代超過 700 後會觸發回收；1 代超過 700x10 後觸發回收；2 代超過 700x10x10 後會觸發回收操作。

也就是說如果觸發了一次當前代的回收，會把上代的 count 加 1 。
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

上述實際是創建了三代的鏈表，並對 threshold 和 count 初始化。




### 對象初始化

下面通過跟蹤一個 list 對象的創建過程來查看 Python 的垃圾回收機制。list 對象通過 PyList_New() 函數創建，其創建過程大致如下。

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

首先通過 PyObject_GC_New() 初始化一個對象，而這個函數實際上是對 _PyObject_GC_New() 的宏定義，最終調用的是 _PyObject_GC_New()。

該函數主要作了兩件事情：A）分配內存結構，同時會將第 0 代的 count 加 1，如果超過了閾值，則會通過 collect_generations() 觸發一次垃圾回收； B）初始化數據結構。注意，在這一步中還沒有將該對象插入到鏈表中。

最終插入鏈表實際是通過 _PyObject_GC_TRACK() 實現的，也就是插入到 _PyGC_generation0 中（第 0 代）。

到此，已經完成了對象的新建，並可以 "監控" 該對象。


### 垃圾回收機制

Python 中的垃圾回收機制實際通過 collect(gen) 實現，其中 gen 是所定義的代數。

當我們一直新建 container 對象時，如果超過了第 0 代的上限，就會通過 collect_generations() 調用 Python 的垃圾回收機制。該函數會從最高代（數字最大）開始檢測，當超過閾值之後就會觸發該代的垃圾回收操作，所以有可能是從第 1、2 代觸發垃圾回收。

當第一次超過 700 後，則會調用 collect_generations()，該函數會從 2 依次向下檢測，當超過閾值則會調用 collect()，此時第 1、2 代的 count 仍為 0 ，所以最終會調用 collect(0) 。

接下來主要查看 collect() 函數。

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

假設，第 0 代有如下的對象，每個對象用黑色方框表示，其中數字表示引用計數。其中各個對象是通過雙向鏈表相互鏈接的，只是沒有表示出來，其中雙向線表示相互引用，藍色表示本代外部的引用。

![garbage-explain1]{: .pull-center width='400'}

在第 ❶ 步中，首先處理每代的 count，也就是將上一代的加 1，並將本代以及一下的清零。這也就是之前說的 700、700x10、700x10x10 的關係了。

在第 ❷ 步中，將本代以下的鏈表合併到本代，以後的操作實際上都是針對於這一鏈表的操作。也就是說每次 Python 執行垃圾回收時，會檢測本代及以下的對象。

接著在第 ❸ 步中，查找含有外部引用的對象，為了不影響對象的原引用計數，先通過 update_refs() 將 ob_refcnt 複製到 gc_refs，後面的操作都是針對 gc_refs；然後通過 subtract_refs() 遍歷整個鏈表，將本代對象的 gc_refs 及其引用減 1。此時，對於 gc_refs > 0 的對象，一定有該代之外的對象引用，是不應該被刪除的。

執行完 subtract_refs() 之後，各個對象的引用如下所示，可以看出，在此時並不是說 gc_refs = 0 的對象就一定是垃圾對象。

如 "JKL"，雖然此時計算的 gc_refs = 0，但是該對象不應該刪除，因為還被 "MNO" 所引用。

![garbage-explain2]{: .pull-center width='400'}

然後是將 unreachable 的對象分開，在第 ❹ 步中調用 move_unreachable() 函數。該函數會遍歷整個鏈表，在遍歷整個鏈表時，會將 gc_refs=0 的對象移動到 unreachable 列表中，並將 gc_refs 臨時設置為 GC_TENTATIVELY_UNREACHABLE。如上所述，因為這些對象仍可能會被 gc_refs > 0 的對象引用，在遍歷後續的對象時會重新將這些對象設置為 reachable，通過 visit_reachable() 實現。

到此為止，處於 unreachable 鏈表中的對象都是不可達的，也就是應該被回收的垃圾。那麼接下來就需要對這些 unreachable 的對象進行處理了。

接下來在第 ❺ 步中，實際要遍歷 unreachable 鏈表，並處理含有 finalizers 的對象，通過 move_finalizers() 將 unreachable 鏈表中含有 \_\_del\_\_ 的對象放到 finalizers 鏈表中。對於在 finalizers 鏈表中指向的對象同樣也需要放到該鏈表中，該功能是通過 move_finalizer_reachable() 函數實現。

到此，unreachable 中包含了可以通過 Python 回收的對象，而 finalizers 中包含了不能通過 Python 回收的對象，會將這些對象分別打印出來。在 finalizers 中的對象，會保存到 garbage 中，可以通過 gc.garbage() 查看。<br><br>

接下來還會通過 handle_weakrefs() 函數處理 unreachable 鏈表中的 weakref 對象；通過 delete_garbage() 處理 unreachable 鏈表中的垃圾對象。

當然除此之外，Python 也對垃圾回收機制進行了優化，詳細可以參考源碼中的註釋。




## 總結

有 \_\_del\_\_() 函數的對象，如果存在循環引用，會導致內存洩露；如果沒有 \_\_del\_\_() 可以自動垃圾回收掉。

需要注意的是，要麼在實現類時不要單獨實現 \_\_del\_\_() 函數，要麼需要確保不要形成循環引用，對於循環引用可以通過 objgraph 進行檢測。



[weakref-pymotw]:          http://pymotw.com/2/weakref/index.html                 "PyMOTW 中關於弱引用的介紹"
[gc-module-pymotw]:        http://pymotw.com/2/gc/index.html                      "PyMOTW 中關於垃圾回收的介紹，包括如何消除uncollectable對象"
[gc-in-python-ruby]:       http://patshaughnessy.net/2013/10/30/generational-gc-in-python-and-ruby  "關於Python和Ruby垃圾回收機制的比較"


<!-- pictures -->
[garbage-logo]:            /images/python/garbages/garbage-logo.jpg               "垃圾回收logo"
[garbage-explain1]:        /images/python/garbages/gc-explain1.png                "garbage collection 1"
[garbage-explain2]:        /images/python/garbages/gc-explain2.png                "garbage collection 2"
