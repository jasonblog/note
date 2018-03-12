---
title: Greenlet Gevent
layout: post
comments: true
language: chinese
category: [python,program,linux]
keywords: python,greenlet,并发,协程,gevent
description: 对于服务器端的编程，从多进程，到多线程，再到异步回调，再到现在比较流行的协程的方式。对于 Python 来说，支持多进程；由于存在 GIL，实际对于线程会有性能影响。对于 Python 协程在此介绍一下 greenlet 的实现。
---

对于服务器端的编程，从多进程，到多线程，再到异步回调，再到现在比较流行的协程的方式。对于 Python 来说，支持多进程；由于存在 GIL，实际对于线程会有性能影响。

对于 Python 协程在此介绍一下 greenlet 的实现。

<!-- more -->

## 简介


### 安装

在 CentOS 中，可以通过如下方式安装。

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

如下是简单的测试函数。

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

### greenlet 入门

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

首先创建了一个名称为 gr1 的 greenlet 协程，然后调用 switch() 方法，让这个协程开始执行，也就是开始执行 foo() 函数。在 foo() 函数中，打印字符串，新建另外一个协程，并切换过去执行 ... ...

上述的例子基本和同步执行没有任何区别，接着修改下上述的代码，如下：

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

可以看到，当协程 gr2 执行完 bar() 函数之后，将参数返回到了最外层，而没有返回到 foo() 中，而且可以看到 foo() 最后一行程序没有执行。So, WHY ???

## greenlent 层次关系

上述的程序，主要是和 greenlet 的层次关系了相关。在 greenlet 模块的初始话的时候，就会先建立一个最顶层的 greenlet 对象，他属于 greenlet 层次关系中的 root 节点。

在创建 greenlet 的时候，如果在构造函数中并没有指明 parent 对象，那么构建出来的 greenlet 对象将会默认将当前环境所在的 greenlet 作为 parent，当这个 greenlet 对象执行完成之后，将会调度其 parent 继续执行，而且会将返回参数也传递过去。

而在上述代码中，gr1 和 gr2 他们两个的 parent 都默认设置为了初始化建立的 greenlet 对象，所以在 gr2 执行完之后，并没有返回到 gr1 的 foo 函数中，而是直接返回到了最外层。



# 源码解析

Greenlet 是通过 C 语言实现的，也就是说，实际上是 C 语言实现的一个模块，首先看看一个简单的示例。

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

注意 C 文件名 foobar.c 和 PyMODINIT_FUNC initfoobar(void) 以及 Py_InitModule("foobar") 中的部分必须一致。

{% highlight text %}
$ gcc -fpic -c -I /usr/include/python2.7 foobar.c
$ gcc -shared -o foobar.so foobar.o
$ python
>>> import foobar
>>> foobar.add(1,2)
{% endhighlight %}

每个 greenlet 其实就是一个函数以及保存这个函数执行时的上下文，对于函数来说上下文也就是其 stack；同一个进程的所有的 greenlets 共用操作系统分配的用户栈。

## 模块初始化

首先，在导入模块的时候，会调用初始化函数。

{% highlight c %}
PyMODINIT_FUNC initgreenlet(void)
{
    PyObject* m = NULL;
    char** p = NULL;
    PyObject *c_api_object;
    static void *_PyGreenlet_API[PyGreenlet_API_pointers];

    GREENLET_NOINLINE_INIT();
    m = Py_InitModule("greenlet", GreenMethods); // 初始化greenlet模块
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

    // 1. 创建root greenlet对象，首先会将ts_current创建为main greenlet
    ts_current = green_create_main();
    if (ts_current == NULL) {
        INITERROR;
    }
    Py_INCREF(&PyGreenlet_Type);

    // 2. 为当前的模块添加属性，如下的这个是比较重要的，也就是greenlet的构造结构
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

代码挺长，不过其实就是 1) 初始化；2) 创建默认的最顶层的 greenlet 对象，而且将 ts_current 这个全局变量指向它；3) 将 greenlet 模块添加到当前的 namespace 。

接下来看看这个初始化创建的顶层的 greenlet 对象是怎么创建的，其中创建顶层 greenlet 对象，栈底部为 -1，顶部为 1 。
{% highlight c %}
static PyGreenlet* green_create_main(void)
{
    PyGreenlet* gmain;  //待会创建的顶层的greenlet的引用
    //创建一个字典对象
    PyObject* dict = PyThreadState_GetDict();
    if (dict == NULL) {
        if (!PyErr_Occurred())
            PyErr_NoMemory();
        return NULL;
    }

    // 为当前线程创建main greenlet对象
    gmain = (PyGreenlet*) PyType_GenericAlloc(&PyGreenlet_Type, 0);
    if (gmain == NULL)
        return NULL;
    gmain->stack_start = (char*) 1; // 栈顶设置为1
    gmain->stack_stop = (char*) -1; // 栈底设置为-1，char*是无符号，可以保证这个是最大值
    gmain->run_info = dict;
    Py_INCREF(dict);
    return gmain;
}
{% endhighlight %}

上述参数中，将 stack_stop 设置为 -1，因为是无符号，所以 -1 其实是最大值。在该函数中，其实最重要的就是设置了两个值：stack_stop、stack_start，其中后者为栈顶，保存了当前最新的栈内容。


## 结构体定义

接下来看看 greenlet 的真正结构体在头文件中的定义，如下所示。

{% highlight c %}
/**
 * States:
 * stack_stop == NULL && stack_start == NULL:  did not start yet
 * stack_stop != NULL && stack_start == NULL:  already finished
 * stack_stop != NULL && stack_start != NULL:  active
 */
typedef struct _greenlet { // 协程对应的C结构体
    PyObject_HEAD
    char* stack_start;             // 栈顶，NULL标示已经结束了或未开始
    char* stack_stop;              // 栈底
    char* stack_copy;              // 栈保存到的内存地址
    intptr_t stack_saved;          // 栈保存在外面的大小
    struct _greenlet* stack_prev;  // 栈之间的上下层关系
    struct _greenlet* parent;      // 父对象，构成greenlet对象的层次关系
    PyObject* run_info;            // 对应的run对象（函数）
    struct _frame* top_frame;      // 这里可以理解为主要是控制python程序计数器
    int recursion_depth;           // 栈深度
    PyObject* weakreflist;
    PyObject* exc_type;
    PyObject* exc_value;
    PyObject* exc_traceback;
    PyObject* dict;
} PyGreenlet;
{% endhighlight %}

其中 stack_start、stack_stop 分别指向当前协程栈的顶部和底部，当前栈的内容从栈顶出、入栈；另外，如上所示，可以通过这几个标志位的值来判断当前 greenlet 对象的状态。

另外，通过如下的结构体定义 C 语言与 Python 的对应关系，其中包括了相应的方法，例如 \_\_init\_\_()、\_\_new\_\_() 等。

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
    green_methods,                      /* tp_methods, 对象方法的定义 */
    green_getsets,                      /* tp_getset, 属性方法 */
    offsetof(PyGreenlet, dict),         /* tp_dictoffset，这个对象的dict的偏移 */
    (initproc)green_init,               /* tp_init，初始化，这里主要是设置run以及parent */
    GREENLET_tp_alloc,                  /* tp_alloc，内存分配，其实也就是分配sizeof(PyGreenlet)的大小 */
    green_new,                          /* tp_new, 构造函数，这里会将parent默认的设置为全局的greenlet */
    GREENLET_tp_free,                   /* tp_free，释放 */
    (inquiry)GREENLET_tp_is_gc,         /* tp_is_gc */
};
{% endhighlight %}

## 初始化

上述的结构体中，包含了构造、初始化函数，Python 函数在执行时，分别调用 \_\_new\_\_()、\_\_init\_\_() 函数，在此也就分别对应了 green_new() 和 green_init() 函数；接下来，我们依次查看这两个函数。

{% highlight c %}
// 创建一个gr对象，默认将parent指向当前环境所属的gr对象，如果构造函数中指定，则会在init设置
static PyObject* green_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    // 先用基本对象来构造
    PyObject* o = PyBaseObject_Type.tp_new(type, ts_empty_tuple, ts_empty_dict);
    if (o != NULL) {
        if (!STATE_OK) {
            Py_DECREF(o);
            return NULL;
        }
        Py_INCREF(ts_current);
        // 默认将parent设置为ts_current，意味着新建gr的parent就是创建gr的对象
        ((PyGreenlet*) o)->parent = ts_current;
    }
    return o;
}
{% endhighlight %}

在该函数中，非常清楚，首先构造父类，接着将当前 greenlet 设置未默认的 parent，也就是说假如在 A 中创建另外一个 greenlet B，则会将 B 的 parent 设置为 A；如果初始化时传入 parent 参数，则会在后面的 init 函数中设置。

接下来看看 \_\_init\_\_() 函数。

{% highlight c %}
// 主要是检查设置run以及parent
static int green_init(PyGreenlet *self, PyObject *args, PyObject *kwargs)
{
    PyObject *run = NULL;
    PyObject* nparent = NULL;
    static char *kwlist[] = {"run", "parent", 0}; // 查看两个对象是否存在
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OO:green", kwlist,
                     &run, &nparent))
        return -1;
    if (run != NULL) {  // run对象必须存在，否则需要进行设置
        if (green_setrun(self, run, NULL))
            return -1;
    }
    if (nparent != NULL && nparent != Py_None)
        return green_setparent(self, nparent, NULL); // 判断parent链是否正常
    // 在初始化的时候可以没有parent
    return 0;
}
{% endhighlight %}

这里初始化函数主要是设置构造的时候传递进来的执行函数，也就是 run 对象，然后就如果有传递 parent 的话，将会将这个 greenlet 对象的 parent 设置为传递进来的 greenlet 对象。


## 任务切换

gr1.switch("foo") 实际会调用 green_switch() 函数，最终调用 g_switch()，实际的入参是 target=gr1、args="foo"，函数的实现如下。

{% highlight c %}
// 第一个参数是执行switch的greenlet对象，第二参数是参数数组，第三个是关键字参数
static PyObject* green_switch(PyGreenlet* self, PyObject* args, PyObject* kwargs)
{
    Py_INCREF(args);
    Py_XINCREF(kwargs);
    // 实际是调用g_switch方法来进行栈的切换
    return single_result(g_switch(self, args, kwargs));
}
{% endhighlight %}

接着看看 g_switch() 函数的实现。

{% highlight c %}
static PyObject* g_switch(PyGreenlet* target, PyObject* args, PyObject* kwargs)
{
    /* _consumes_ a reference to the args tuple and kwargs dict,
       and return a new tuple reference */
    int err = 0;
    PyObject* run_info;

    /* check ts_current */
    if (!STATE_OK) {     // 查看当前gr状态，如果有异常则减少参数的引用计数，并返回
        Py_XDECREF(args);
        Py_XDECREF(kwargs);
        return NULL;
    }
    // 1. 获取运行信息
    run_info = green_statedict(target);
    if (run_info == NULL || run_info != ts_current->run_info) {
        Py_XDECREF(args);
        Py_XDECREF(kwargs);
        PyErr_SetString(PyExc_GreenletError, run_info
                ? "cannot switch to a different thread"
                : "cannot switch to a garbage collected greenlet");
        return NULL;
    }

    ts_passaround_args = args;      // 保存switch传进来的参数
    ts_passaround_kwargs = kwargs;

    /* find the real target by ignoring dead greenlets,
       and if necessary starting a greenlet. */
    // 2. 找到真正需要指定的greenlet
    while (target) {  // 通过parent引用向上遍历，只要找到一个可用的就停止
        if (PyGreenlet_ACTIVE(target)) {   // 找到一个可用的greenlet，start!=NULL
            ts_target = target;            // 指向运行的greenlet
            err = g_switchstack();         // 切换到这个栈，里面会调用slp_switch方法，用汇编实现
            break;
        }
        if (!PyGreenlet_STARTED(target)) { // 如果都没有启动，那么需要初始化，stop!=NULL
            void* dummymarker;             // 通过该对象地址来分割新创建栈，用来做新栈的底部
            ts_target = target;
            err = g_initialstub(&dummymarker); // 对于没有启动的协程，这里需要进行初始化
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

    //这里处理switch返回参数，这里
    //这里可以看出来，其实switch的返回值也是通过ts_passaround_args，ts_passaround_kwargs这两个全局变量来传递的
    args = ts_passaround_args;
    ts_passaround_args = NULL;
    kwargs = ts_passaround_kwargs;
    ts_passaround_kwargs = NULL;
    if (err < 0) {   //如果在栈的切换的时候出现了错误
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
    if (kwargs == NULL)  //没有关键字参数，那么直接返回参数列表
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
    else  //两种参数都有
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


//(1)先获取运行信息，参数啥的(2)从切换到的greenlet开始，找到一个可以运行的greenlet对象，通过parent向上遍历
//(3)如果这个greenlet已经是active了，那么直接调用g_switchstack方法，如果这个greenlet还没有启动，那么需要g_initialstub进行初始化









gr1(new_greenlet)，默认stack_start = NULL(没有运行),stack_stop = NULL(没有启动)，因而执行g_initialstub()
dummymarker设置为栈底
为什么要将dummymarker栈底设置于此处？
g_initialstub的栈中包含函数需要的参数等数据，然而&dummymarker的位置恰为g_initialstub栈的ebp。


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







greenlet 是通过 stack_stop、stack_start 来保存其 stack 的栈底和栈顶的，如果出现将要执行的greenlet的stack_stop和目前栈中的greenlet重叠的情况,就要把这些重叠的greenlet的栈中数据临时保存到heap中.保存的位置通过stack_copy和stack_saved来记录,以便恢复的时候从heap中拷贝回栈中stack_stop和stack_start的位置.不然就会出现其栈数据会被破坏的情况.所以应用程序创建的这些greenlet就是通过不断的拷贝数据到heap中或者从heap中拷贝到栈中来实现并发的.对于io型的应用程序使用coroutine真的非常舒服.

下面是greenlet的一个简单的栈空间模型(from greenlet.c)









greenlet是python众多协程实现技术中的一种，eventlet是基于greenlet实现的。而eventlet和libev又是gevent的核心。greenlet的上下文切换清晰易懂，可以结合IO事件循环构建出一些高效的事件处理逻辑。不同于 yield 类型的上下文切换，greenlet的上下文切换从表现形式上看更纯粹，可以直接 switch 到另一个greenlet，不用管目标greenlet是否已经在运行，不同greenlet之间处于完全对等的状态，可以相互 switch 。基于 yield 实现的协程往往只能切换回自己的直接或间接调用者，要想在嵌套的调用中切换出去是比较麻烦的。本质上是因为 yield 只能保留栈顶的帧，Python3对此有改进，可以通过 yield from 嵌套的挂起内层过程调用，但依然不能任意的切换到其他上下文。而greenlet就可以，只要一个过程被封装进一个greenlet，可以认为这个greenlet就成了一个可以随时挂起和恢复的实体。当然这种灵活性的代价是代码的逻辑会变得混乱，debug会更难，不过如果适当的使用greenlet，却可以收到很好的效果，比如之前介绍过的Motor。

协程是用户态下的上下文切换技术，是对线程时间片的再切分。所谓上下文，一般是指一个子过程调用，比如一个函数。另一方面，我们知道，Python虚拟机的原理是通过栈帧对象PyFrameObject抽象出运行时（栈，指令，符号表，常量表等），通过执行 PyEval_EvalFrameEx 这个C级别的函数来逐个解析字节码指令。也就是说可调用对象都是通过 PyEval_EvalFrameEx 来执行自己的PyFrameObject的，而按照调用的先后顺序，当前PyFrameObject的 f_back 指针会指向上一个PyFrameObject，这样，某一个时刻，当前线程的栈帧对象按调用的先后顺序形成了一个链表， 线程的top_frame属性正好是链表的表头（栈顶），这就是当前线程正在运行的帧。从这种状态可以看出，对于Python而言，切换当前栈顶的帧是容易的，只要保留栈顶的PyFrameObject，回退到栈顶下的一帧就行，这也是 yield 的基本原理。但问题是，Python的栈帧对象本身从一开始并不是为协程设计的，所以栈帧与栈帧之间的这种执行的先后顺序（其实可以理解为执行栈）语言本身却没有提供恢复和挂起的机制。这就要求假如你想任意切换上下文的话，必须实现一个机制，可以保存一个执行栈。

通过上面的分析，可以看到，要想在Python中在两个子过程中作任意的挂起和恢复的话，需要做到两点：
保存当前子过程的执行栈。 恢复目标子过程的执行栈，并恢复栈顶的状态继续执行。

greenlet之所以可以在任意两个greenlet之间作切换，就是因为其实现了上述的两点。其总共加起来2000多行C代码，其中内联了一小部分但确实相当关键的汇编代码，看懂greenlet的代码至少要把C语言的过程调用原理、汇编、进程的堆栈、Python的虚拟机执行原理等弄清楚，如果有一个不懂，就不用看源码了，能用起来就好。毕竟有些部分很绕，光看源码分析也不一定能完全消化吸收，有些东西也很难用文字表达，只可意会，不可言传。

greenlet的基本原理简单说起来就是:
将一个子过程封装进一个greenlet里，而一个greenlet代表了一段C的栈内存。 在greenlet里执行Python的子过程(通常是个函数)，当要切换出去的时候，保存当前greenlet的栈内存，方法是memcpy到堆上，也就是说每一个greenlet可能都需要在堆上保存上下文，挂起的时候就把栈内存memcpy到堆上，恢复的时候再把堆上的上下文（运行的时候栈内存的内容）拷贝到栈上，然后释放堆上的内存。 恢复栈顶只需要将当前线程的top_frame修改为恢复的greenlet的top_frame就行。

greenlet的基本原则：
除了main greenlet之外，任意一个greenlet都有唯一一个父greenlet。 假如当前greenlet执行完毕，回到自己的父greenlet即可。 可以通过给switch方法的参数来在不同greenlet之间传递数据。

greenlet实现的关键是先切换C函数的栈，而切换和恢复C的栈需要将%ebp（函数栈底）、%esp（函数栈顶）等寄存器的值保存到本地变量，而恢复的时候就可以通过从堆上拷贝的内存，来恢复寄存器的值。从而达到恢复上下文的目的。





<!--
http://blog.csdn.net/fjslovejhl/article/details/38824963
http://blog.csdn.net/RingoShen/article/details/51119232    openstack基础_eventlet
http://luckybins.blog.51cto.com/786164/1605902
http://www.tuicool.com/articles/BfiQfuU
http://www.jianshu.com/p/cd41c14b19f4
http://www.bubuko.com/infodetail-345777.html
http://www.cnblogs.com/Security-Darren/p/4167961.html
http://rootk.com/post/python-greenlet.html
http://blog.csdn.net/shuaijiasanshao/article/details/51475571
-->

## 参考

源码可以从 [github-greenlet](https://github.com/python-greenlet/greenlet) 下载，或者从 [Lightweight in-process concurrent programming](https://pypi.python.org/pypi/greenlet)，以及帮助文档 [greenlet: Lightweight concurrent programming](http://greenlet.readthedocs.io/en/latest/) 。

可以参考官方文档 [gevent For the Working Python Developer](http://sdiehl.github.io/gevent-tutorial/)，或者 [gevent程序员值南](http://xlambda.com/gevent-tutorial/) 。

{% highlight c %}
{% endhighlight %}
