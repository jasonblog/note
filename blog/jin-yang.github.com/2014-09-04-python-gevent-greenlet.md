---
title: Greenlet Gevent
layout: post
comments: true
language: chinese
category: [python,program,linux]
keywords: python,greenlet,併發,協程,gevent
description: 對於服務器端的編程，從多進程，到多線程，再到異步回調，再到現在比較流行的協程的方式。對於 Python 來說，支持多進程；由於存在 GIL，實際對於線程會有性能影響。對於 Python 協程在此介紹一下 greenlet 的實現。
---

對於服務器端的編程，從多進程，到多線程，再到異步回調，再到現在比較流行的協程的方式。對於 Python 來說，支持多進程；由於存在 GIL，實際對於線程會有性能影響。

對於 Python 協程在此介紹一下 greenlet 的實現。

<!-- more -->

## 簡介


### 安裝

在 CentOS 中，可以通過如下方式安裝。

{% highlight text %}
# pip install greenlet
{% endhighlight %}

<!--
{% highlight c %}
gr1.dead         gr1.gettrace     gr1.parent       gr1.switch
gr1.error        gr1.gr_frame     gr1.run          gr1.throw
gr1.getcurrent   gr1.GreenletExit gr1.settrace
{% endhighlight %}
-->

如下是簡單的測試函數。

{% highlight python %}
from greenlet import greenlet

def test1(arg):
    print 12, arg
    gr2.switch()
    print 34

def test2():
    print 56
    gr1.switch()
    print 78

gr1 = greenlet(test1)
gr2 = greenlet(test2)
gr1.switch("foo bar")

# OUTPUT:
# 12 foo bar
# 56
# 34
{% endhighlight %}

### greenlet 入門

首先查看如下的示例。

{% highlight python %}
from greenlet import greenlet

def foo():
    print "coroutine foo"
    gr2 = greenlet(bar)
    value = gr2.switch()
    print value

def bar():
    print "coroutine bar"
    return "bar return value"

gr1 = greenlet(foo)
gr1.switch()

# OUTPUT:
# coroutine foo
# coroutine bar
# bar return value
{% endhighlight %}

首先創建了一個名稱為 gr1 的 greenlet 協程，然後調用 switch() 方法，讓這個協程開始執行，也就是開始執行 foo() 函數。在 foo() 函數中，打印字符串，新建另外一個協程，並切換過去執行 ... ...

上述的例子基本和同步執行沒有任何區別，接著修改下上述的代碼，如下：

{% highlight python %}
from greenlet import greenlet

def foo():
    print "coroutine foo"
    ret = gr2.switch()
    print "coroutine end", ret

def bar():
    print "coroutine bar"
    return "coroutine bar return value"

gr1 = greenlet(foo)
gr2 = greenlet(bar)

out = gr1.switch()
print out

# OUTPUT:
# coroutine foo
# coroutine bar
# coroutine bar return value
{% endhighlight %}

可以看到，當協程 gr2 執行完 bar() 函數之後，將參數返回到了最外層，而沒有返回到 foo() 中，而且可以看到 foo() 最後一行程序沒有執行。So, WHY ???

## greenlent 層次關係

上述的程序，主要是和 greenlet 的層次關係了相關。在 greenlet 模塊的初始話的時候，就會先建立一個最頂層的 greenlet 對象，他屬於 greenlet 層次關係中的 root 節點。

在創建 greenlet 的時候，如果在構造函數中並沒有指明 parent 對象，那麼構建出來的 greenlet 對象將會默認將當前環境所在的 greenlet 作為 parent，當這個 greenlet 對象執行完成之後，將會調度其 parent 繼續執行，而且會將返回參數也傳遞過去。

而在上述代碼中，gr1 和 gr2 他們兩個的 parent 都默認設置為了初始化建立的 greenlet 對象，所以在 gr2 執行完之後，並沒有返回到 gr1 的 foo 函數中，而是直接返回到了最外層。



# 源碼解析

Greenlet 是通過 C 語言實現的，也就是說，實際上是 C 語言實現的一個模塊，首先看看一個簡單的示例。

{% highlight c %}
#include <Python.h>

int add(int arg1, int arg2)
{
    return arg1 + arg2;
}

static PyObject* wrap_add(PyObject *self, PyObject *args)
{
    //Convert the python input objects into C objects
    int arg1, arg2;

    if(!PyArg_ParseTuple(args, "ii", &arg1, &arg2))
            return NULL;
    //Call c function
    int result = add(arg1,arg2);

    //wrap the result from c function to python object.
    return (PyObject*)Py_BuildValue("i", result);
}

// define the PyMethodDef methods
static PyMethodDef wrap_methods[] ={
    {"add", wrap_add, METH_VARARGS},
    {NULL, NULL}
};

// initilization function
PyMODINIT_FUNC initfoobar(void)
{
    Py_InitModule("foobar", wrap_methods);
}
{% endhighlight %}

注意 C 文件名 foobar.c 和 PyMODINIT_FUNC initfoobar(void) 以及 Py_InitModule("foobar") 中的部分必須一致。

{% highlight text %}
$ gcc -fpic -c -I /usr/include/python2.7 foobar.c
$ gcc -shared -o foobar.so foobar.o
$ python
>>> import foobar
>>> foobar.add(1,2)
{% endhighlight %}

每個 greenlet 其實就是一個函數以及保存這個函數執行時的上下文，對於函數來說上下文也就是其 stack；同一個進程的所有的 greenlets 共用操作系統分配的用戶棧。

## 模塊初始化

首先，在導入模塊的時候，會調用初始化函數。

{% highlight c %}
PyMODINIT_FUNC initgreenlet(void)
{
    PyObject* m = NULL;
    char** p = NULL;
    PyObject *c_api_object;
    static void *_PyGreenlet_API[PyGreenlet_API_pointers];

    GREENLET_NOINLINE_INIT();
    m = Py_InitModule("greenlet", GreenMethods); // 初始化greenlet模塊
    if (m == NULL) {
        INITERROR;
    }

    if (PyModule_AddStringConstant(m, "__version__", GREENLET_VERSION) < 0) {
        INITERROR;
    }

    ts_curkey = PyString_InternFromString("__greenlet_ts_curkey");
    ts_delkey = PyString_InternFromString("__greenlet_ts_delkey");
    if (ts_curkey == NULL || ts_delkey == NULL) {
        INITERROR;
    }
    if (PyType_Ready(&PyGreenlet_Type) < 0) {
        INITERROR;
    }
    PyExc_GreenletError = PyErr_NewException("greenlet.error", NULL, NULL);
    if (PyExc_GreenletError == NULL) {
        INITERROR;
    }
    PyExc_GreenletExit = PyErr_NewException("greenlet.GreenletExit", NULL, NULL);
    if (PyExc_GreenletExit == NULL) {
        INITERROR;
    }
    ts_empty_tuple = PyTuple_New(0);
    if (ts_empty_tuple == NULL) {
        INITERROR;
    }
    ts_empty_dict = PyDict_New();
    if (ts_empty_dict == NULL) {
        INITERROR;
    }

    // 1. 創建root greenlet對象，首先會將ts_current創建為main greenlet
    ts_current = green_create_main();
    if (ts_current == NULL) {
        INITERROR;
    }
    Py_INCREF(&PyGreenlet_Type);

    // 2. 為當前的模塊添加屬性，如下的這個是比較重要的，也就是greenlet的構造結構
    PyModule_AddObject(m, "greenlet", (PyObject*) &PyGreenlet_Type);
    Py_INCREF(PyExc_GreenletError);
    PyModule_AddObject(m, "error", PyExc_GreenletError);
    Py_INCREF(PyExc_GreenletExit);
    PyModule_AddObject(m, "GreenletExit", PyExc_GreenletExit);
    PyModule_AddObject(m, "GREENLET_USE_GC", PyBool_FromLong(GREENLET_USE_GC));
    PyModule_AddObject(m, "GREENLET_USE_TRACING", PyBool_FromLong(GREENLET_USE_TRACING));

    /* also publish module-level data as attributes of the greentype. */
    for (p=copy_on_greentype; *p; p++) {
        PyObject* o = PyObject_GetAttrString(m, *p);
        if (!o) continue;
        PyDict_SetItemString(PyGreenlet_Type.tp_dict, *p, o);
        Py_DECREF(o);
    }

    /* Expose C API ... ... */
}
{% endhighlight %}

代碼挺長，不過其實就是 1) 初始化；2) 創建默認的最頂層的 greenlet 對象，而且將 ts_current 這個全局變量指向它；3) 將 greenlet 模塊添加到當前的 namespace 。

接下來看看這個初始化創建的頂層的 greenlet 對象是怎麼創建的，其中創建頂層 greenlet 對象，棧底部為 -1，頂部為 1 。
{% highlight c %}
static PyGreenlet* green_create_main(void)
{
    PyGreenlet* gmain;  //待會創建的頂層的greenlet的引用
    //創建一個字典對象
    PyObject* dict = PyThreadState_GetDict();
    if (dict == NULL) {
        if (!PyErr_Occurred())
            PyErr_NoMemory();
        return NULL;
    }

    // 為當前線程創建main greenlet對象
    gmain = (PyGreenlet*) PyType_GenericAlloc(&PyGreenlet_Type, 0);
    if (gmain == NULL)
        return NULL;
    gmain->stack_start = (char*) 1; // 棧頂設置為1
    gmain->stack_stop = (char*) -1; // 棧底設置為-1，char*是無符號，可以保證這個是最大值
    gmain->run_info = dict;
    Py_INCREF(dict);
    return gmain;
}
{% endhighlight %}

上述參數中，將 stack_stop 設置為 -1，因為是無符號，所以 -1 其實是最大值。在該函數中，其實最重要的就是設置了兩個值：stack_stop、stack_start，其中後者為棧頂，保存了當前最新的棧內容。


## 結構體定義

接下來看看 greenlet 的真正結構體在頭文件中的定義，如下所示。

{% highlight c %}
/**
 * States:
 * stack_stop == NULL && stack_start == NULL:  did not start yet
 * stack_stop != NULL && stack_start == NULL:  already finished
 * stack_stop != NULL && stack_start != NULL:  active
 */
typedef struct _greenlet { // 協程對應的C結構體
    PyObject_HEAD
    char* stack_start;             // 棧頂，NULL標示已經結束了或未開始
    char* stack_stop;              // 棧底
    char* stack_copy;              // 棧保存到的內存地址
    intptr_t stack_saved;          // 棧保存在外面的大小
    struct _greenlet* stack_prev;  // 棧之間的上下層關係
    struct _greenlet* parent;      // 父對象，構成greenlet對象的層次關係
    PyObject* run_info;            // 對應的run對象（函數）
    struct _frame* top_frame;      // 這裡可以理解為主要是控制python程序計數器
    int recursion_depth;           // 棧深度
    PyObject* weakreflist;
    PyObject* exc_type;
    PyObject* exc_value;
    PyObject* exc_traceback;
    PyObject* dict;
} PyGreenlet;
{% endhighlight %}

其中 stack_start、stack_stop 分別指向當前協程棧的頂部和底部，當前棧的內容從棧頂出、入棧；另外，如上所示，可以通過這幾個標誌位的值來判斷當前 greenlet 對象的狀態。

另外，通過如下的結構體定義 C 語言與 Python 的對應關係，其中包括了相應的方法，例如 \_\_init\_\_()、\_\_new\_\_() 等。

{% highlight c %}
PyTypeObject PyGreenlet_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "greenlet.greenlet",                /* tp_name */
    sizeof(PyGreenlet),                 /* tp_basicsize */
    0,                                  /* tp_itemsize */
    (destructor)green_dealloc,          /* tp_dealloc */
    &green_as_number,                   /* tp_as _number*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | GREENLET_GC_FLAGS,   /* tp_flags */
    (traverseproc)GREENLET_tp_traverse, /* tp_traverse */
    (inquiry)GREENLET_tp_clear,         /* tp_clear */
    0,                                  /* tp_richcompare */
    offsetof(PyGreenlet, weakreflist),  /* tp_weaklistoffset */
    green_methods,                      /* tp_methods, 對象方法的定義 */
    green_getsets,                      /* tp_getset, 屬性方法 */
    offsetof(PyGreenlet, dict),         /* tp_dictoffset，這個對象的dict的偏移 */
    (initproc)green_init,               /* tp_init，初始化，這裡主要是設置run以及parent */
    GREENLET_tp_alloc,                  /* tp_alloc，內存分配，其實也就是分配sizeof(PyGreenlet)的大小 */
    green_new,                          /* tp_new, 構造函數，這裡會將parent默認的設置為全局的greenlet */
    GREENLET_tp_free,                   /* tp_free，釋放 */
    (inquiry)GREENLET_tp_is_gc,         /* tp_is_gc */
};
{% endhighlight %}

## 初始化

上述的結構體中，包含了構造、初始化函數，Python 函數在執行時，分別調用 \_\_new\_\_()、\_\_init\_\_() 函數，在此也就分別對應了 green_new() 和 green_init() 函數；接下來，我們依次查看這兩個函數。

{% highlight c %}
// 創建一個gr對象，默認將parent指向當前環境所屬的gr對象，如果構造函數中指定，則會在init設置
static PyObject* green_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    // 先用基本對象來構造
    PyObject* o = PyBaseObject_Type.tp_new(type, ts_empty_tuple, ts_empty_dict);
    if (o != NULL) {
        if (!STATE_OK) {
            Py_DECREF(o);
            return NULL;
        }
        Py_INCREF(ts_current);
        // 默認將parent設置為ts_current，意味著新建gr的parent就是創建gr的對象
        ((PyGreenlet*) o)->parent = ts_current;
    }
    return o;
}
{% endhighlight %}

在該函數中，非常清楚，首先構造父類，接著將當前 greenlet 設置未默認的 parent，也就是說假如在 A 中創建另外一個 greenlet B，則會將 B 的 parent 設置為 A；如果初始化時傳入 parent 參數，則會在後面的 init 函數中設置。

接下來看看 \_\_init\_\_() 函數。

{% highlight c %}
// 主要是檢查設置run以及parent
static int green_init(PyGreenlet *self, PyObject *args, PyObject *kwargs)
{
    PyObject *run = NULL;
    PyObject* nparent = NULL;
    static char *kwlist[] = {"run", "parent", 0}; // 查看兩個對象是否存在
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OO:green", kwlist,
                     &run, &nparent))
        return -1;
    if (run != NULL) {  // run對象必須存在，否則需要進行設置
        if (green_setrun(self, run, NULL))
            return -1;
    }
    if (nparent != NULL && nparent != Py_None)
        return green_setparent(self, nparent, NULL); // 判斷parent鏈是否正常
    // 在初始化的時候可以沒有parent
    return 0;
}
{% endhighlight %}

這裡初始化函數主要是設置構造的時候傳遞進來的執行函數，也就是 run 對象，然後就如果有傳遞 parent 的話，將會將這個 greenlet 對象的 parent 設置為傳遞進來的 greenlet 對象。


## 任務切換

gr1.switch("foo") 實際會調用 green_switch() 函數，最終調用 g_switch()，實際的入參是 target=gr1、args="foo"，函數的實現如下。

{% highlight c %}
// 第一個參數是執行switch的greenlet對象，第二參數是參數數組，第三個是關鍵字參數
static PyObject* green_switch(PyGreenlet* self, PyObject* args, PyObject* kwargs)
{
    Py_INCREF(args);
    Py_XINCREF(kwargs);
    // 實際是調用g_switch方法來進行棧的切換
    return single_result(g_switch(self, args, kwargs));
}
{% endhighlight %}

接著看看 g_switch() 函數的實現。

{% highlight c %}
static PyObject* g_switch(PyGreenlet* target, PyObject* args, PyObject* kwargs)
{
    /* _consumes_ a reference to the args tuple and kwargs dict,
       and return a new tuple reference */
    int err = 0;
    PyObject* run_info;

    /* check ts_current */
    if (!STATE_OK) {     // 查看當前gr狀態，如果有異常則減少參數的引用計數，並返回
        Py_XDECREF(args);
        Py_XDECREF(kwargs);
        return NULL;
    }
    // 1. 獲取運行信息
    run_info = green_statedict(target);
    if (run_info == NULL || run_info != ts_current->run_info) {
        Py_XDECREF(args);
        Py_XDECREF(kwargs);
        PyErr_SetString(PyExc_GreenletError, run_info
                ? "cannot switch to a different thread"
                : "cannot switch to a garbage collected greenlet");
        return NULL;
    }

    ts_passaround_args = args;      // 保存switch傳進來的參數
    ts_passaround_kwargs = kwargs;

    /* find the real target by ignoring dead greenlets,
       and if necessary starting a greenlet. */
    // 2. 找到真正需要指定的greenlet
    while (target) {  // 通過parent引用向上遍歷，只要找到一個可用的就停止
        if (PyGreenlet_ACTIVE(target)) {   // 找到一個可用的greenlet，start!=NULL
            ts_target = target;            // 指向運行的greenlet
            err = g_switchstack();         // 切換到這個棧，裡面會調用slp_switch方法，用匯編實現
            break;
        }
        if (!PyGreenlet_STARTED(target)) { // 如果都沒有啟動，那麼需要初始化，stop!=NULL
            void* dummymarker;             // 通過該對象地址來分割新創建棧，用來做新棧的底部
            ts_target = target;
            err = g_initialstub(&dummymarker); // 對於沒有啟動的協程，這裡需要進行初始化
            if (err == 1) {
                continue; /* retry the switch */
            }
            break;
        }
        target = target->parent;
    }

    /* For a very short time, immediately after the 'atomic'
       g_switchstack() call, global variables are in a known state.
       We need to save everything we need, before it is destroyed
       by calls into arbitrary Python code. */

    //這裡處理switch返回參數，這裡
    //這裡可以看出來，其實switch的返回值也是通過ts_passaround_args，ts_passaround_kwargs這兩個全局變量來傳遞的
    args = ts_passaround_args;
    ts_passaround_args = NULL;
    kwargs = ts_passaround_kwargs;
    ts_passaround_kwargs = NULL;
    if (err < 0) {   //如果在棧的切換的時候出現了錯誤
        /* Turn switch errors into switch throws */
        assert(ts_origin == NULL);
        Py_CLEAR(kwargs);
        Py_CLEAR(args);
    } else {
        PyGreenlet *origin;
        origin = ts_origin;
        ts_origin = NULL;
        Py_DECREF(origin);
    }

    /* We need to figure out what values to pass to the target greenlet
       based on the arguments that have been passed to greenlet.switch(). If
       switch() was just passed an arg tuple, then we'll just return that.
       If only keyword arguments were passed, then we'll pass the keyword
       argument dict. Otherwise, we'll create a tuple of (args, kwargs) and
       return both. */
    if (kwargs == NULL)  //沒有關鍵字參數，那麼直接返回參數列表
    {
        return args;
    }
    else if (PyDict_Size(kwargs) == 0)
    {
        Py_DECREF(kwargs);
        return args;
    }
    else if (PySequence_Length(args) == 0)
    {
        Py_DECREF(args);
        return kwargs;
    }
    else  //兩種參數都有
    {
        PyObject *tuple = PyTuple_New(2);
        if (tuple == NULL) {
            Py_DECREF(args);
            Py_DECREF(kwargs);
            return NULL;
        }
        PyTuple_SET_ITEM(tuple, 0, args);
        PyTuple_SET_ITEM(tuple, 1, kwargs);
        return tuple;
    }
}
{% endhighlight %}


//(1)先獲取運行信息，參數啥的(2)從切換到的greenlet開始，找到一個可以運行的greenlet對象，通過parent向上遍歷
//(3)如果這個greenlet已經是active了，那麼直接調用g_switchstack方法，如果這個greenlet還沒有啟動，那麼需要g_initialstub進行初始化









gr1(new_greenlet)，默認stack_start = NULL(沒有運行),stack_stop = NULL(沒有啟動)，因而執行g_initialstub()
dummymarker設置為棧底
為什麼要將dummymarker棧底設置於此處？
g_initialstub的棧中包含函數需要的參數等數據，然而&dummymarker的位置恰為g_initialstub棧的ebp。


{% highlight c %}
               |     ^^^       |
               |  older data   |
               |               |
  stack_stop . |_______________|
        .      |               |
        .      | greenlet data |
        .      |   in stack    |
        .    * |_______________| . .  _____________  stack_copy + stack_saved
        .      |               |     |             |
        .      |     data      |     |greenlet data|
        .      |   unrelated   |     |    saved    |
        .      |      to       |     |   in heap   |
 stack_start . |     this      | . . |_____________| stack_copy
               |   greenlet    |
               |               |
               |  newer data   |
               |     vvv       |
{% endhighlight %}


{% highlight c %}
static PyObject * g_switch(PyGreenlet* target, PyObject* args, PyObject* kwargs)
{
    /* _consumes_ a reference to the args tuple and kwargs dict,
       and return a new tuple reference */
    int err = 0;
    PyObject* run_info;

    /* check ts_current */
    if (!STATE_OK) {
        Py_XDECREF(args);
        Py_XDECREF(kwargs);
        return NULL;
    }
        run_info = green_statedict(target);
        if (run_info == NULL || run_info != ts_current->run_info) {
                Py_XDECREF(args);
                Py_XDECREF(kwargs);
                PyErr_SetString(PyExc_GreenletError, run_info
                                ? "cannot switch to a different thread"
                                : "cannot switch to a garbage collected greenlet");
                return NULL;
        }

        ts_passaround_args = args;
        ts_passaround_kwargs = kwargs;

        /* find the real target by ignoring dead greenlets,
           and if necessary starting a greenlet. */
        while (target) {
                if (PyGreenlet_ACTIVE(target)) {
                        ts_target = target;
                        err = g_switchstack();
                        break;
                }
                if (!PyGreenlet_STARTED(target)) {
                        void* dummymarker;
                        ts_target = target;
                        err = g_initialstub(&dummymarker);
                        if (err == 1) {
                                continue; /* retry the switch */
                        }
                        break;
                }
                target = target->parent;
        }

        /* For a very short time, immediately after the 'atomic'
           g_switchstack() call, global variables are in a known state.
           We need to save everything we need, before it is destroyed
           by calls into arbitrary Python code. */
        args = ts_passaround_args;
        ts_passaround_args = NULL;
        kwargs = ts_passaround_kwargs;
        ts_passaround_kwargs = NULL;
        if (err < 0) {
                /* Turn switch errors into switch throws */
                assert(ts_origin == NULL);
                Py_CLEAR(kwargs);
                Py_CLEAR(args);
        } else {
                PyGreenlet *origin;
#if GREENLET_USE_TRACING
                PyGreenlet *current;
                PyObject *tracefunc;
#endif
                origin = ts_origin;
                ts_origin = NULL;
#if GREENLET_USE_TRACING
                current = ts_current;
                if ((tracefunc = PyDict_GetItem(current->run_info, ts_tracekey)) != NULL) {
                        Py_INCREF(tracefunc);
                        if (g_calltrace(tracefunc, args ? ts_event_switch : ts_event_throw, origin, current) < 0) {
                                /* Turn trace errors into switch throws */
                                Py_CLEAR(kwargs);
                                Py_CLEAR(args);
                        }
                        Py_DECREF(tracefunc);
                }
#endif
                Py_DECREF(origin);
        }

        /* We need to figure out what values to pass to the target greenlet
           based on the arguments that have been passed to greenlet.switch(). If
           switch() was just passed an arg tuple, then we'll just return that.
           If only keyword arguments were passed, then we'll pass the keyword
           argument dict. Otherwise, we'll create a tuple of (args, kwargs) and
           return both. */
        if (kwargs == NULL)
        {
                return args;
        }
        else if (PyDict_Size(kwargs) == 0)
        {
                Py_DECREF(kwargs);
                return args;
        }
        else if (PySequence_Length(args) == 0)
        {
                Py_DECREF(args);
                return kwargs;
        }
        else
        {
                PyObject *tuple = PyTuple_New(2);
                if (tuple == NULL) {
                        Py_DECREF(args);
                        Py_DECREF(kwargs);
                        return NULL;
                }
                PyTuple_SET_ITEM(tuple, 0, args);
                PyTuple_SET_ITEM(tuple, 1, kwargs);
                return tuple;
        }
}
{% endhighlight %}







greenlet 是通過 stack_stop、stack_start 來保存其 stack 的棧底和棧頂的，如果出現將要執行的greenlet的stack_stop和目前棧中的greenlet重疊的情況,就要把這些重疊的greenlet的棧中數據臨時保存到heap中.保存的位置通過stack_copy和stack_saved來記錄,以便恢復的時候從heap中拷貝回棧中stack_stop和stack_start的位置.不然就會出現其棧數據會被破壞的情況.所以應用程序創建的這些greenlet就是通過不斷的拷貝數據到heap中或者從heap中拷貝到棧中來實現併發的.對於io型的應用程序使用coroutine真的非常舒服.

下面是greenlet的一個簡單的棧空間模型(from greenlet.c)









greenlet是python眾多協程實現技術中的一種，eventlet是基於greenlet實現的。而eventlet和libev又是gevent的核心。greenlet的上下文切換清晰易懂，可以結合IO事件循環構建出一些高效的事件處理邏輯。不同於 yield 類型的上下文切換，greenlet的上下文切換從表現形式上看更純粹，可以直接 switch 到另一個greenlet，不用管目標greenlet是否已經在運行，不同greenlet之間處於完全對等的狀態，可以相互 switch 。基於 yield 實現的協程往往只能切換回自己的直接或間接調用者，要想在嵌套的調用中切換出去是比較麻煩的。本質上是因為 yield 只能保留棧頂的幀，Python3對此有改進，可以通過 yield from 嵌套的掛起內層過程調用，但依然不能任意的切換到其他上下文。而greenlet就可以，只要一個過程被封裝進一個greenlet，可以認為這個greenlet就成了一個可以隨時掛起和恢復的實體。當然這種靈活性的代價是代碼的邏輯會變得混亂，debug會更難，不過如果適當的使用greenlet，卻可以收到很好的效果，比如之前介紹過的Motor。

協程是用戶態下的上下文切換技術，是對線程時間片的再切分。所謂上下文，一般是指一個子過程調用，比如一個函數。另一方面，我們知道，Python虛擬機的原理是通過棧幀對象PyFrameObject抽象出運行時（棧，指令，符號表，常量表等），通過執行 PyEval_EvalFrameEx 這個C級別的函數來逐個解析字節碼指令。也就是說可調用對象都是通過 PyEval_EvalFrameEx 來執行自己的PyFrameObject的，而按照調用的先後順序，當前PyFrameObject的 f_back 指針會指向上一個PyFrameObject，這樣，某一個時刻，當前線程的棧幀對象按調用的先後順序形成了一個鏈表， 線程的top_frame屬性正好是鏈表的表頭（棧頂），這就是當前線程正在運行的幀。從這種狀態可以看出，對於Python而言，切換當前棧頂的幀是容易的，只要保留棧頂的PyFrameObject，回退到棧頂下的一幀就行，這也是 yield 的基本原理。但問題是，Python的棧幀對象本身從一開始並不是為協程設計的，所以棧幀與棧幀之間的這種執行的先後順序（其實可以理解為執行棧）語言本身卻沒有提供恢復和掛起的機制。這就要求假如你想任意切換上下文的話，必須實現一個機制，可以保存一個執行棧。

通過上面的分析，可以看到，要想在Python中在兩個子過程中作任意的掛起和恢復的話，需要做到兩點：
保存當前子過程的執行棧。 恢復目標子過程的執行棧，並恢復棧頂的狀態繼續執行。

greenlet之所以可以在任意兩個greenlet之間作切換，就是因為其實現了上述的兩點。其總共加起來2000多行C代碼，其中內聯了一小部分但確實相當關鍵的彙編代碼，看懂greenlet的代碼至少要把C語言的過程調用原理、彙編、進程的堆棧、Python的虛擬機執行原理等弄清楚，如果有一個不懂，就不用看源碼了，能用起來就好。畢竟有些部分很繞，光看源碼分析也不一定能完全消化吸收，有些東西也很難用文字表達，只可意會，不可言傳。

greenlet的基本原理簡單說起來就是:
將一個子過程封裝進一個greenlet裡，而一個greenlet代表了一段C的棧內存。 在greenlet裡執行Python的子過程(通常是個函數)，當要切換出去的時候，保存當前greenlet的棧內存，方法是memcpy到堆上，也就是說每一個greenlet可能都需要在堆上保存上下文，掛起的時候就把棧內存memcpy到堆上，恢復的時候再把堆上的上下文（運行的時候棧內存的內容）拷貝到棧上，然後釋放堆上的內存。 恢復棧頂只需要將當前線程的top_frame修改為恢復的greenlet的top_frame就行。

greenlet的基本原則：
除了main greenlet之外，任意一個greenlet都有唯一一個父greenlet。 假如當前greenlet執行完畢，回到自己的父greenlet即可。 可以通過給switch方法的參數來在不同greenlet之間傳遞數據。

greenlet實現的關鍵是先切換C函數的棧，而切換和恢復C的棧需要將%ebp（函數棧底）、%esp（函數棧頂）等寄存器的值保存到本地變量，而恢復的時候就可以通過從堆上拷貝的內存，來恢復寄存器的值。從而達到恢復上下文的目的。





<!--
http://blog.csdn.net/fjslovejhl/article/details/38824963
http://blog.csdn.net/RingoShen/article/details/51119232    openstack基礎_eventlet
http://luckybins.blog.51cto.com/786164/1605902
http://www.tuicool.com/articles/BfiQfuU
http://www.jianshu.com/p/cd41c14b19f4
http://www.bubuko.com/infodetail-345777.html
http://www.cnblogs.com/Security-Darren/p/4167961.html
http://rootk.com/post/python-greenlet.html
http://blog.csdn.net/shuaijiasanshao/article/details/51475571
-->

## 參考

源碼可以從 [github-greenlet](https://github.com/python-greenlet/greenlet) 下載，或者從 [Lightweight in-process concurrent programming](https://pypi.python.org/pypi/greenlet)，以及幫助文檔 [greenlet: Lightweight concurrent programming](http://greenlet.readthedocs.io/en/latest/) 。

可以參考官方文檔 [gevent For the Working Python Developer](http://sdiehl.github.io/gevent-tutorial/)，或者 [gevent程序員值南](http://xlambda.com/gevent-tutorial/) 。

{% highlight c %}
{% endhighlight %}
