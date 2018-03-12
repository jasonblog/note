---
title: Lua CAPI 使用
layout: post
comments: true
language: chinese
category: [program,linux]
keywords: lua, c, api, lua state, stack
description: Lua 是扩展性非常良好的语言，虽然核心非常精简，但是用户可以依靠 Lua 库来实现大部分工作，还可以通过与 C 函数相互调用来扩展程序功能。在 C 中嵌入 Lua 脚本既可以让用户在不重新编译代码的情况下修改 Lua 代码更新程序，也可以给用户提供一个自由定制的接口，这种方法遵循了机制与策略分离的原则；在 Lua 中调用 C 函数可以提高程序的运行效率。Lua 与 C 的相互调用在工程中相当实用，这里就简单讲解 Lua 与 C 相互调用的方法。
---

Lua 是扩展性非常良好的语言，虽然核心非常精简，但是用户可以依靠 Lua 库来实现大部分工作，还可以通过与 C 函数相互调用来扩展程序功能。

在 C 中嵌入 Lua 脚本既可以让用户在不重新编译代码的情况下修改 Lua 代码更新程序，也可以给用户提供一个自由定制的接口，这种方法遵循了机制与策略分离的原则；在 Lua 中调用 C 函数可以提高程序的运行效率。

Lua 与 C 的相互调用在工程中相当实用，这里就简单讲解 Lua 与 C 相互调用的方法。

<!-- more -->

## 栈

Lua 与 C 交互的栈是一个重要的概念，作为一种嵌入式语言，所有的 Lua 程序最后都需要通过 Lua 解释器将其解析成字节码的形式才能执行。

<!--文章首先解释了为什么要引入Lua栈，然后对访问栈常用的API进行了总结，并使用这些API的注意事项，最后从Lua源代码来看栈的实现原理。-->

可在应用程序中嵌入 Lua 解释器，此时使用 Lua 的目的是方便扩展这个应用程序，用 Lua 实现相应的工作；另一方面，在 Lua 程序中也可以使用那些用 C 语言实现的函数，比如 `string.find()` 。

为了方便在两个语言之间进行交换数据，Lua 引入了一个虚拟栈，同时提供了一系列的 API ，通过这些 API C 语言就可以方便从 Lua 中获取相应的值，也可以方便地把值返回给 Lua，当然，这些操作都是通过栈作为桥梁来实现的。

### API

Lua 提供了大量的 API 操作栈，用于向栈中压入元素、查询栈中的元素、修改栈的大小等操作。

#### 1. 向栈中压入元素

通常都是以 `lua_push` 开头来命名，比如 `lua_pushnunber`、`lua_pushstring`、`lua_pushcfunction`、`lua_pushcclousre` 等函数都是向栈顶中压入一个 Lua 值。

<!--
在 Lua 代码中调用 C 实现的函数并且被调用的 C 函数有返回值时，被调用的C函数通常就要用到这些接口，把返回值压入栈中，返回给Lua（当然这些C函数也要求返回一个值，告诉Lua一共返回（压入）了多少个值）。值得注意的是，向栈中压入一个元素时，应该确保栈中具有足够的空间，可以调用lua_checkstack来检测是否有足够的空间。

实质上这些API是把C语言里面的值封装成Lua类型的值压入栈中的，对于那些需要垃圾回收的元素，在压入栈时，都会在Lua（也就是Lua虚拟机中）生成一个副本。比如lua_pushstring(lua_State *L, const char *s)会向中栈压入由s指向的以'\0'结尾的字符串，在C中调用这个函数后，我们可以任意释放或修改由s指向的字符串，也不会出现问题，原因就是在执行lua_pushstring过程中Lua会生成一个内部副本。实质上，Lua不会持有指向外部字符串的指针，也不会持有指向任何其他外部对象的指针（除了C函数，因为C函数总是静态的）。

总之，一旦C中值被压入栈中，Lua就会生成相应的结构（实质就是Lua中实现的相应数据类型）并管理（比如自动垃圾回收）这个值，从此不会再依赖于原来的C值。
-->

#### 2. 获取栈中的元素

从栈中获取一个值的函数，通常都是以 `lua_to` 开头来命名，比如 `lua_tonumber`、`lua_tostring`、`lua_touserdata`、`lua_tocfunction` 等函数都是从栈中指定的索引处获取一个值，可在 C 函数中用这些接口获取从 Lua 中传递给 C 函数的参数。

<!--
如果指定的元素不具有正确的类型，调用这些函数也不会出问题的。在这种情况下，lua_toboolean、lua_tonumber、lua_tointeger和lua_objlen会返回0，而其他函数会返回NULL。对于返回NULL的函数，可以直接通过返回值，即可以知道调用是否正确；对于返回0的函数，通常先需要使用lua_is*系列函数，判断调用是否正确。

注意lua_to*和lua_is*系列函数都是试图转换栈中元素为相应中的值。比如lua_isnumber不会检查是否为数字类型，而是检查是否能转换为数字类型；lua_isstring也类似，它对于任意数字，lua_isstring都返回真。要想真正返回栈中元素的类型，可以用函数lua_type。每种类型对应于一个常量（LUA_TNIL，LUA_TBOOLEAN，LUA_TNUMBER等），这些常量定义在头文件lua.h中。

值得一提是lua_tolstring函数，它的函数原型是const char *lua_tolstring (lua_State *L, int index, size_t *len)。它会把栈中索引为index的Lua值装换为一个C字符串。若参数Len不为NULL，则*Len会保存字符串的长度。栈中的Lua值必须为string或number类型，否则函数返回NULL。若栈中Lua值为number类型，则该函数实质会改变栈中的值为string类型，由于这个原因，在利用lua_next遍历栈中的table时，对key使用lua_tolstring尤其需要注意，除非知道key都是string类型。lua_tolstring函数返回的指针，指向的是Lua虚拟机内部的字符串，这个字符串是以'\0'结尾的，但字符串中间也可能包含值为0的字符。由于Lua自身的垃圾回收，因此当栈中的字符串被弹出后，函数返回的指针所有指向的字符串可能就不能再有效了。也说明了，当一个C函数从Lua收到一个字符串参数时，在C函数中，即不能在访问字符串时从栈中弹出它，也不能修改字符串。

#### 3. 其它操作栈的函数

int lua_call(lua_State *L, int nargs, int nresults);

调用栈中的函数，在调用lua_call之前，程序必须首先要保证被调用函数已压入栈，其次要被调用函数需要的参数也已经按顺序压入栈，也就是说，第一个参数最先被压入栈，依次类推。nargs是指需要压入栈中参数的个数，当函数被调用后，之前压入的函数和参数都会从栈中弹出，并将函数执行的结果按顺序压入栈中，因此最后一个结果压入栈顶，同时，压入栈的个数会根据nresults的值做调整。与lua_call相对应的是lua_pcall函数，lua_pcall会以保护模式调用栈中的函数。以保护模式调用意思是，当被调用的函数发生任何错误时，该错误不会传播，不像lua_call会把错误传递到上一层，lua_pcall所调用的栈中函数发送错误时，lua_pcall会捕捉这个错误，并向栈中压入一个错误信息，并返回一个错误码。在应用程序中编写主函数时，应该使用lua_pcall来调用栈中的函数，捕获所有错误。而在为Lua编写扩展的C函数时，应该调用lua_call，把错误返回到脚本层。

void lua_createtable (lua_State *L, int narr, int nrec);
创建一个新的table，并把它压入栈顶，参数narr和nrec分别指新的table将会有多少个数组元素和多少需要hash的元素，Lua会根据这个两个值为新的table预分配内存。对于事先知道table结构，利用这两个参数能提高创建新table的性能。对于事先不知道table结构，则可以使用void lua_newtable (lua_State *L),它等价于lua_createtable(L, 0, 0)。

除了上面提到的C API，还有许多其他有用的C API，比如操作table的接口有：lua_getfield、lua_setfield、lua_gettable、lua_settable等接口，在具体使用时，可以参照Lua手册。
-->

### 源码解析

程序中为了加载执行 Lua 脚本，首先需要调用 `luaL_newstate()` 初始化 Lua 虚拟机，该函数会创建 Lua 与 C 交互的栈，返回指向 `lua_State` 类型的指针，后面几乎所有 API 都需要 `lua_State*` 作为入参，这样就使得每个 Lua 状态机是各自独立的，不共享任何数据。

这里的 `lua_State` 就保存了一个 Lua 解析器的执行状态，它代表一个新的线程 (同上，非操作系统中的线程)，每个线程拥有独立的数据栈、函数调用链、调试钩子和错误处理方法。

实际上几乎所有的 API 操作，都是围绕这个 `lua_State` 结构来进行的，包括把数据压入栈、从栈中取出数据、执行栈中的函数等操作。

`struct lua_State` 在 lstate.h 头文件中定义，其代码如下:

{% highlight c %}
struct lua_State {
	CommonHeader;
	lu_byte status;
	StkId top;                     /* 指向数据栈中，第一个可使用的空间*/
	global_State *l_G;
	CallInfo *ci;                  /* 保存着当前正在调用的函数的运行状态 */
	const Instruction *oldpc;
	StkId stack_last;              /* 指向数据栈中，最后一个可使用的空间 */
	StkId stack;                   /* 指向数据栈开始的位置 */
	int stacksize;                 /* 栈当前的大小，注意并不是可使用的大小*/
	unsigned short nny;
	unsigned short nCcalls;
	lu_byte hookmask;
	lu_byte allowhook;
	int basehookcount;
	int hookcount;
	lua_Hook hook;
	GCObject *openupval;
	GCObject *gclist;
	struct lua_longjmp *errorJmp;
	ptrdiff_t errfunc;
	CallInfo base_ci;              /* 保存调用链表的第一个节点*/
};
{% endhighlight %}

<!--
      lua_State中主要包含两个重要的数据结构，一个是数据栈，另外一个是调用栈（实质上是一个双向链表）。数据栈实质就是一个动态数组，数组中每个元素类型为TValue。Lua中任何数据类型（nil，number，stirng，userdata，function等）都是用该结构体TValue来实现的。其定义如下（源码里面使用了大量的宏和typedef来定义TValue，为了方便阅读，把它展开了）：


      Lua中所有的数据类型都是由结构体TValue来实现的，它把值和类型绑在一起，每个数据都携带了它自身的类型信息。用成员tt_保存数据的类型，成员value_用来保存数据值，它使用的一个联合体来实现的：

[cpp] view plain copy

     union  Value {
           GCObject *gc;   /*gc指向一个对象，这些对象是需要垃圾回收的数据类型，比如table、string等*/
           void *p;        /* lua中的light userdata类型，实质上保存的就是一个指针 */
           int b;          /*boolean类型*/
           lua_CFunction f;/*lua中light C functions（没有upvalue），即只是函数指针 */
           double n;       /*lua中的number类型*/
    };

      上面提到到数据栈是在函数stack_init中创建的（初始化虚拟机时调用的luaL_newstate，就是通过调用lua_newstate函数，lua_newstate调用f_luaopen函数，最后f_luaopen函数调用stack_init来初始化栈的），函数stack_init在lstate.c中实现，代码如下：

[cpp] view plain copy

    static void stack_init (lua_State *L1, lua_State *L) {
          int i; CallInfo *ci;

          /* 为数据栈分配空间，并且初始化lua_State与数据栈相关的成员*/
          L1->stack = luaM_newvector(L, BASIC_STACK_SIZE, TValue);
          L1->stacksize = BASIC_STACK_SIZE;
          for (i = 0; i < BASIC_STACK_SIZE; i++)
            setnilvalue(L1->stack + i);  /* erase new stack */
          L1->top = L1->stack;
          L1->stack_last = L1->stack + L1->stacksize - EXTRA_STACK;

          /*初始化lua_State与调用链表相关的成员*/
          ci = &L1->base_ci;
          ci->next = ci->previous = NULL;
          ci->callstatus = 0;
          ci->func = L1->top;
          setnilvalue(L1->top++);  /* 'function' entry for this 'ci' */
          ci->top = L1->top + LUA_MINSTACK;
          L1->ci = ci;
    }

 调用栈实质上用一个双向链表来实现的，链表中的每个节点是用一个CallInfo的结构体来实现，保存着正在调用的函数的运行状态。结构体CallInfo在lstate.h定义的，代码如下：

[cpp] view plain copy

    typedef struct CallInfo {
          StkId  func; /* 指向被调用的函数在栈中的位置*/
          StkId  top; /*指向被调用的函数可以使用栈空间最大的位置，即限定了调用一个函数可以栈空间的大小*/
          struct CallInfo *previous, *next; /* 指向调用链表的前一个节点和下一个节点 */
          short nresults;                  /* 当前被调用的函数期待返回结果的数量*/
          lu_byte callstatus;              /*用来标识当前调用的是C函数还是Lua函数*/
          union {
            struct {                       /* 当调用Lua调用函数时保存信息的结构体*/
              StkId base;
              const Instruction *savedpc;
            } l;
            struct {                       /*当调用C调用函数时保存信息的结构体*/
              int ctx;
              lua_CFunction k;
              ptrdiff_t old_errfunc;
              ptrdiff_t extra;
              lu_byte old_allowhook;
              lu_byte status;
            } c;
        } u;
    } CallInfo;


     从CallInfo的定义，可以知道它的成员变量func和top同样指向数据栈的位置，但不同的是，它所关注的是函数调用时相关的位置。并且在用gdb调试有嵌入Lua的C代码时，我们可以除了查看C中的调用栈信息外，还可以用上面的调用链表获取完整的Lua调用链表，在链表中的每一个节点中，我们可以使用CallInfo中的成员变量func来获取每一个lua函数所在的文件名和行号等调试信息。
-->


## 线程

线程作为 Lua 中一种基本的数据类型，代表独立的执行线程 (independent threads of execution)，这也是实现协程 (coroutines) 的基础，注意这里的线程类型不要与操作系统线程混淆，Lua 中的线程类型是 Lua 虚拟机实现一种数据类型。

从 Lua 脚本来看，一个协程就是一个线程类型，准确来说，协程是一个线程外加一套良好的操作接口，比如：

{% highlight lua %}
local co = coroutine.create(function() print("hello world") end)
print(co)  --output: thread: 0038BEE0
{% endhighlight %}

从实现角度来看，一个线程类型数据就是一个 Lua 与 C 交互的栈，每个栈包含函数调用链和数据栈，还有独立的调试钩子和错误信息，线程类型数据与 table 数据类型类似，它也是需要 GC 来管理。

### 新建栈

为了加载执行 Lua 脚本，首先要调用 `lua_newstate()` 函数来初始化虚拟机，该函数在初始化虚拟机状态的同时，还会创建整个虚拟机的第一个线程，也就是主线程，或者说是第一个交互栈。

在已经初始化的全局状态中创建一个新的线程可以调用函数 `lua_newthread()`，声明如下：

{% highlight c %}
lua_State *lua_newthread (lua_State *L);
{% endhighlight %}

创建一个线程就拥有一个独立的执行栈了，但是它与其线程共用虚拟机的全局状态；Lua 没有提供 API 关闭或者销毁一个线程，类似其它 GC 对象一样，由虚拟机管理。

也就是说，一个 Lua 虚拟机只有一个全局的状态，但可以包含多个执行环境 (或者说多个线程、交互栈，从脚本角度来说，也可以说是多个协程)。

### 源码解析

全局状态的结构体 `global_State` 的代码如下：

{% highlight c %}
/*
** `global state', shared by all threads of this state
*/
typedef struct global_State {
	lua_Alloc frealloc;  /* function to reallocate memory */
	void *ud;         /* auxiliary data to `frealloc' */
	lu_mem totalbytes;  /* number of bytes currently allocated - GCdebt */
	l_mem GCdebt;  /* bytes allocated not yet compensated by the collector */
	lu_mem GCmemtrav;  /* memory traversed by the GC */
	lu_mem GCestimate;  /* an estimate of the non-garbage memory in use */
	stringtable strt;  /* hash table for strings */
	TValue l_registry;
	unsigned int seed;  /* randomized seed for hashes */
	lu_byte currentwhite;
	lu_byte gcstate;  /* state of garbage collector */
	lu_byte gckind;  /* kind of GC running */
	lu_byte gcrunning;  /* true if GC is running */
	int sweepstrgc;  /* position of sweep in `strt' */
	GCObject *allgc;  /* list of all collectable objects */
	GCObject *finobj;  /* list of collectable objects with finalizers */
	GCObject **sweepgc;  /* current position of sweep in list 'allgc' */
	GCObject **sweepfin;  /* current position of sweep in list 'finobj' */
	GCObject *gray;  /* list of gray objects */
	GCObject *grayagain;  /* list of objects to be traversed atomically */
	GCObject *weak;  /* list of tables with weak values */
	GCObject *ephemeron;  /* list of ephemeron tables (weak keys) */
	GCObject *allweak;  /* list of all-weak tables */
	GCObject *tobefnz;  /* list of userdata to be GC */
	UpVal uvhead;  /* head of double-linked list of all open upvalues */
	Mbuffer buff;  /* temporary buffer for string concatenation */
	int gcpause;  /* size of pause between successive GCs */
	int gcmajorinc;  /* how much to wait for a major GC (only in gen. mode) */
	int gcstepmul;  /* GC `granularity' */
	lua_CFunction panic;  /* to be called in unprotected errors */
	struct lua_State *mainthread;
	const lua_Number *version;  /* pointer to version number */
	TString *memerrmsg;  /* memory-error message */
	TString *tmname[TM_N];  /* array with tag-method names */
	struct Table *mt[LUA_NUMTAGS];  /* metatables for basic types */
} global_State;
{% endhighlight %}

一个 Lua 虚拟机只有一个全局的 `global_State`，在调用 `lua_newstate()` 时，会创建和初始化这个全局结构，这个全局结构管理着 Lua 中全局唯一的信息。

<!---
, 主要包括以下信息：
    lua_Alloc frealloc：虚拟机内存分配策略，可以在调用lua_newstate时指定参数，修改该策略，或者调用luaL_newstate函数使用默认的内存分配策略。也可以通过函数               lua_setallocf：来设置内存分配策略。
    stringtable strt：全局的字符串哈希表，即保存那些短字符串，使得整个虚拟机中短字符串只有一份实例。
    TValue l_registry：保存全局的注册表，注册表就是一个全局的table（即整个虚拟机中只有一个注册表），它只能被C代码访问，通常，它用来保存那些需要在几个模块中共享的数据。比如通过luaL_newmetatable创建的元表就是放在全局的注册表中。
    lua_CFunction panic：但出现无包含错误（unprotected errors）时，会调用这个函数。这个函数可以通过lua_atpanic来修改。
    UpVal uvhead:指向保存所有open upvalues双向链表的头部。
    struct Table *mt[LUA_NUMTAGS]：保存基本类型的元表，注意table和userdata都有自己的元表。
    struct lua_State *mainthread：指向主lua_State，或者说是主线程、主执行栈。Lua虚拟机在调用函数lua_newstate初始化全局状态global_State时也会创建一个主线程，
当然根据需要也可以调用lua_newthread来创建新的线程，但是整个虚拟机，只有一个全局的状态global_State。
-->

线程对应的数据结构 `lua_State` 的定义如下：

{% highlight c %}
/*
** `per thread' state
*/
struct lua_State {
	CommonHeader;
	lu_byte status;
	StkId top;  /* first free slot in the stack */
	global_State *l_G;
	CallInfo *ci;  /* call info for current function */
	const Instruction *oldpc;  /* last pc traced */
	StkId stack_last;  /* last free slot in the stack */
	StkId stack;  /* stack base */
	int stacksize;
	unsigned short nny;  /* number of non-yieldable calls in stack */
	unsigned short nCcalls;  /* number of nested C calls */
	lu_byte hookmask;
	lu_byte allowhook;
	int basehookcount;
	int hookcount;
	lua_Hook hook;
	GCObject *openupval;  /* list of open upvalues in this stack */
	GCObject *gclist;
	struct lua_longjmp *errorJmp;  /* current error recover point */
	ptrdiff_t errfunc;  /* current error handling function (stack index) */
	CallInfo base_ci;  /* CallInfo for first level (C calling Lua) */
};
{% endhighlight %}

可以看到，`struct lua_State` 跟其它可回收的数据类型一样，结构体带用 `CommonHeader` 的头，也即是说它也是 GC 回收的对象之一。

<!--
它主要包括以下成员信息：
lu_byte status：线程脚本的状态，线程可能状态如下（lua.h）：

    44 /* thread status */
    45 #define LUA_OK      0
    46 #define LUA_YIELD   1
    47 #define LUA_ERRRUN  2
    48 #define LUA_ERRSYNTAX   3
    49 #define LUA_ERRMEM  4
    50 #define LUA_ERRGCMM 5
    51 #define LUA_ERRERR  6

  global_State *l_G：指向全局状态；

    其他成员主要是数据栈和函数调用栈相关的，这也是lua_State结构中主要信息。还有成员ptrdiff_t errfunc是错误处理函数相关，也就是每个调用栈都有独立的错误处理函数，以及调试相关的lua_Hook hook成员等。
-->

## C 库导入

导入全局性的库到 Lua 中，这些库由 C 实现:

{% highlight c %}
/*
** these libs are loaded by lua.c and are readily available to any Lua
** program
*/
static const luaL_Reg loadedlibs[] = {
	{"_G", luaopen_base},
	{LUA_LOADLIBNAME, luaopen_package},
	{LUA_COLIBNAME, luaopen_coroutine},
	//....
	{NULL,NULL}
};
LUALIB_API void luaL_openlibs (lua_State *L) {
	const luaL_Reg *lib;
	/* "require" functions from 'loadedlibs' and set results to global table */
	for (lib = loadedlibs; lib->func; lib++) {
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);  /* remove lib */
	}
}
{% endhighlight %}

每一个库封装了很多函数, 且每个库都由库名和 open 函数导入，以协程库为例:

{% highlight c %}
{LUA_COLIBNAME, luaopen_coroutine},
{% endhighlight %}

通过看协程的库的创建过程可以知道如何将 C 函数写的库导入 Lua ：

{% highlight c %}
/* 下面是协程库的lua函数名和对应的C函数 */
static const luaL_Reg co_funcs[] = {
	{"create", luaB_cocreate},
	{"resume", luaB_coresume},
	{"running", luaB_corunning},
	{"status", luaB_costatus},
	{"wrap", luaB_cowrap},
	{"yield", luaB_yield},
	{"isyieldable", luaB_yieldable},
	{NULL, NULL}
};

/* 每个库必须有的open函数，newlib的实现就是一个table */
LUAMOD_API int luaopen_coroutine (lua_State *L) {
	luaL_newlib(L, co_funcs);
	return 1;
}
{% endhighlight %}

单个 C 函数组成的库的 open 函数里, 调用的是 `luaL_newlib(L, co_funcs);` 函数，其实现如下：

{% highlight c %}
/*
 * 根据库函数数组luaL_Reg的大小创建的table, 这里的createtable()的实现就是在栈中创建
 * 一个哈希表, 表元素个数为sizeof(l)/sizeof((l)[0]) - 1
 */
#define luaL_newlibtable(L,l)  \
	lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1)

/* 库的实现就是以l的大小创建了一个table */
#define luaL_newlib(L,l)  \
	(luaL_checkversion(L), luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))
{% endhighlight %}

如下是上面调用的 `luaL_setfuncs()` 函数的实现代码, 由于当前的栈顶是刚才 newlibtable 出来的 table, 所以现在是将库函数名 set 到 table 中;

<!--
    下面的实现可以看出, 每个函数创建的闭包前都先复制了一份闭包, 这样所有的库函数的闭包是一样的;
    checkstack函数是检查当前的栈空间是否足够放得下nup个闭包元素;
    lua_pushvalue()就是将索引处的值复制到栈顶
-->

{% highlight c %}
/*
** set functions from list 'l' into table at top - 'nup'; each
** function gets the 'nup' elements at the top as upvalues.
** Returns with only the table at the stack.
*/
LUALIB_API void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
	/* nup是闭包元素的个数，如果空间不够会自动扩展栈空间 */
	luaL_checkstack(L, nup, "too many upvalues");
	for (; l->name != NULL; l++) {  /* fill the table with given functions */
		int i;
		for (i = 0; i < nup; i++)  /* copy upvalues to the top */
			/* 压入所有的闭包, 当前栈顶(新的table)下的元素是nup个的闭包 */
			lua_pushvalue(L, -nup);
		lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
		lua_setfield(L, -(nup + 2), l->name);
	}
	lua_pop(L, nup);  /* remove upvalues */
}
{% endhighlight %}

下面看下 `luaL_checkstack` 调用的 check 函数，ci 为当前栈中的调用的函数帧，可以看成函数的局部空间，ci->func 为底，ci->top 为空间顶，两者之间就是当前函数的局部空间:

{% highlight c %}
/*  const int extra = LUA_MINSTACK;  5个额外的空间
 * 调用的是该: lua_checkstack(L, space + extra)) ..
 */
LUA_API int lua_checkstack (lua_State *L, int n) {
	int res;
	CallInfo *ci = L->ci;   /* 当前的函数调用帧, ci->func为函数调用点 */
	lua_lock(L);
	api_check(L, n >= 0, "negative 'n'");
	if (L->stack_last - L->top > n)  /* stack large enough? */
		res = 1;  /* yes; check is OK 空间足够 */
	else {  /* no; need to grow stack 空间不够,需要增加栈空间 */
		int inuse = cast_int(L->top - L->stack) + EXTRA_STACK;
		if (inuse > LUAI_MAXSTACK - n)  /* can grow without overflow? */
			res = 0;  /* no */
		else  /* try to grow stack */
			res = (luaD_rawrunprotected(L, &growstack, &n) == LUA_OK);
	}
	if (res && ci->top < L->top + n)
		ci->top = L->top + n;  /* adjust frame top 调用帧顶为栈顶+所需空间 */
	lua_unlock(L);

	return res;
}
{% endhighlight %}

前面的库导入过程中 `luaL_requiref()` 是真正的导入函数。

<!--
,实现如下, 说明符中将该函数表示为简化版的require, 完成了三件事:
    1. 在全局表的_LOADED中查找有没有modulename对应的值;
    2. 没有则压入库open函数和库名, 并调用open函数在栈中创建table, 将该table以key为modulename存入_LOADED全局表中;
    3. 如果glb为真,即要求放入全局表中, 则放如全局, _G[modulename] = module

该函数的功能和require的功能类似, 都会检查_loaded表, 没有则导入到表中取, 一共使用;
-->

{% highlight c %}
/*
** Stripped-down 'require': After checking "loaded" table, calls 'openf'
** to open a module, registers the result in 'package.loaded' table and,
** if 'glb' is true, also registers the result in the global table.
** Leaves resulting module on the top.
*/
LUALIB_API void luaL_requiref (lua_State *L, const char *modname, lua_CFunction openf, int glb) {
	/* 全局注册表找到_loaded表 */
	luaL_getsubtable(L, LUA_REGISTRYINDEX, "_LOADED");
	lua_getfield(L, -1, modname);  /* _LOADED[modname] */
	if (!lua_toboolean(L, -1)) {  /* package not already loaded? */
		lua_pop(L, 1);  /* remove field */
		lua_pushcfunction(L, openf);
		lua_pushstring(L, modname);  /* argument to open function */
		/* 调用库的open函数,在栈中创建了一个table */
		lua_call(L, 1, 1);  /* call 'openf' to open module */
		/* 复制一份以保存到_loaded里面取 */
		lua_pushvalue(L, -1);  /* make copy of module (call result) */
		lua_setfield(L, -3, modname);  /* _LOADED[modname] = module */
	}
	lua_remove(L, -2);  /* remove _LOADED table */
	if (glb) {
		/* 复制一份保存到_G里面去 */
		lua_pushvalue(L, -1);  /* copy of module */
		lua_setglobal(L, modname);  /* _G[modname] = module */
	}
}
{% endhighlight %}

## API 简介

Lua 和 C 之间的数据交互通过堆栈进行，栈中的数据通过索引值进行定位，从栈底向上是从 1 开始递增的正整数，从栈顶向下是从 -1 开始递减的负整数，栈的元素按照 FIFO 的规则进出。也就是说，栈顶是 `-1`，栈底是 `1`，其中第 1 个入栈的在栈底。

{% highlight text %}
lua_State* luaL_newstate();
    脚本编译执行相互独立，该函数申请一个虚拟机，后续的API都以此指针作为第一个参数。
void lua_close(lua_State *L);
    清理状态机中所有对象。

lua_State* lua_newthread(lua_State *L)

int lua_gettop(lua_State *L);
    取得栈的高度。
void lua_settop(lua_State *L, int idx);
    设置栈的高度，如果之前的栈顶比新设置的更高，那么高出来的元素会被丢弃，反之压入nil来补足大小。

void lua_pushvalue(lua_State *L, int idx);
    将指定索引上值的副本压入栈。
        for (int i = 1; i <= 3; ++i)
            lua_pushnumber(i);
        // bottom->top 1 2 3
        lua_pushvalue(L, 2)
        // bottom->top 1 2 3 2
void lua_remove(lua_State *L, int idx);
    删除指定索引上的元素，并将该位置之上的所有元素下移以补空缺。
        for (int i = 1; i <= 3; ++i)
            lua_pushnumber(i);
        // bottom->top 1 2 3
        lua_remove(L, 2)
        // bottom->top 1 3
void lua_insert(lua_State *L, int idx);
    移指定位置上的所有元素以开辟一个空间槽的空间，然后将栈顶元素移到该位置。
        for (int i = 1; i <= 5; ++i)
            lua_pushnumber(i);
        // bottom->top 1 2 3 4 5
        lua_insert(L, 3)
        // bottom->top 1 2 5 4 3
void lua_replace(lua_State *L, int idx);
    弹出栈顶的值，并将该值设置到指定索引上，但它不会移动任何东西。
        for (int i = 1; i <= 5; ++i)
            lua_pushnumber(i);
        // bottom->top 1 2 3 4 5
        lua_replace(L, 3)
        // bottom->top 1 2 5 4


{% endhighlight %}

<!--
http://blog.csdn.net/dingwood/article/details/7247161
-->



{% highlight lua %}
{% endhighlight %}
