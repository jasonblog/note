---
title: Lua 源码解析
layout: post
comments: true
language: chinese
category: [program, linux]
keywords: lua,源码解析
description: Lua 类似于 Java、Python，实际执行的都不是机器码，而是运行在虚拟机上的，而虚拟机则屏蔽了底层不同的硬件，从而使得这些程序可以跨平台执行，包括不同的操作系统以及不同的硬件平台。相比而言，Lua 的代码十分简单，源码总共才 2W 行左右，但是却实现了很多不错的特性。接下来，我们看看 Lua 具体是如何工作的。
---

Lua 类似于 Java、Python，实际执行的都不是机器码，而是运行在虚拟机上的，而虚拟机则屏蔽了底层不同的硬件，从而使得这些程序可以跨平台执行，包括不同的操作系统以及不同的硬件平台。

相比而言，Lua 的代码十分简单，源码总共才 2W 行左右，但是却实现了很多不错的特性。

接下来，我们看看 Lua 具体是如何工作的。

<!-- more -->

## 简介

Lua 的代码同样可以被编译，不过与 C/C++ 不同的是，是将源程序编译成为字节码，然后交由虚拟机解释执行。在 Lua 中，每个函数编译器都将创建一个原型(prototype)，它由一组指令及其使用到的常量组成。

最初的 Lua 虚拟机是基于栈的，到 1993 年，Lua5.0 版本，采用了基于寄存器的虚拟机，使得 Lua 的解释效率得到提升。

相比来说，Lua 的代码十分简单，源码可以直接从 [www.lua.org](http://www.lua.org/download.html) 上下载，其中有关虚拟机相关的内容可以参考 lua.c 和 luac.c 的实现。其中 luac 实现了如何编译 lua 程序，而 lua 则包括了运行的虚拟机。

## 虚拟机

Lua 一直把虚拟机执行代码的效率作为一个非常重要的设计目标，而采用什么样的指令系统的对于虚拟机的执行效率来说至关重要，通常根据指令获取操作数的方式不同，可以把虚拟机分为两种：基于栈的虚拟机 (Stack Based VM) 以及基于寄存器的虚拟机 (Register Based VM)。

### Stack based VM

目前，大多数的虚拟机都采用传统的 Stack Based VM，如 JVM、Python，该模型的指令一般都是在当前 stack 中获取和保存操作数。例如，一个简单的加法赋值运算 a=b+c，对于该模型，一般会被转化成如下的指令。

{% highlight text %}
push b;     // 将变量b的值压入stack
push c;     // 将变量c的值压入stack
add;        // 将stack顶部的两个值弹出后相加，然后将结果压入stack顶
mov a;      // 将stack顶部结果放到a中
{% endhighlight %}

由于 Stack Based VM 的指令都是基于当前 stack 来查找操作数的，这就相当于所有操作数的存储位置都是运行期决定的，在编译器的代码生成阶段不需要额外为在哪里存储操作数费心，所以 stack based 的编译器实现起来相对比较简单直接，也正因为这个原因，每条指令占用的存储空间也比较小。

但是，对于一个简单的运算，stack based vm 会使用过多的指令组合来完成，这样就增加了整体指令集合的长度。vm 会使用同样多的迭代次数来执行这些指令，这对于效率来说会有很大的影响。并且，由于操作数都要放到stack上面，使得移动这些操作数的内存复制大大增加，这也会影响到效率。

### Register Based VM

Lua 采用该模型，指令都是在已经分配好的寄存器中存取操作数，对于上面的运算，Register Based VM 一般会使用如下的指令。

{% highlight text %}
add a b c; // 将b与c对应的寄存器的值相加，将结果保存在a对应的寄存器中
{% endhighlight %}

Register Based VM 的指令可以直接对应标准的 3 地址指令，用一条指令完成了上面多条指令的计算工作，并且有效地减少了内存复制操作，这样的指令系统对于效率有很大的帮助。

不过，在编译器设计上，就要在代码生成阶段对寄存器进行分配，采用图着色这样的算法，增加了实现的复杂度，并且每条指令所占用的存储空间也相应的增加了。

接下来查看 lua 的实现细节。




## 类型定义

Lua 是一种动态类型的语言 (dynamically typed language)，也就是说，在定义的变量中不包含类型的信息，只有相应的值才会包含类型信息。

本类型有八种：nil、boolean、number、string、function、userdata、thread 和 table，在 src/lua.h 文件中定义，如 LUA_TSTRING、LUA_TFUNCTIOIN 等。

需要注意的数据类型是 userdata，在内部实现分为了 LUA_TLIGHTUSERDATA 和 LUA_TUSERDATA，两者都对应 void* 指针，区别在于，前者是由 Lua 外部的使用者来完成，后者则是通过 Lua 内部来完成，也就是说前者是通过用户来维护其生命周期。

另外，Lua 支持垃圾回收，内部用一个宏 iscollectable() 标示哪些类型需要进行 gc 操作，包括 LUA_TSTRING 之后的数据类型，都需要进行 gc 操作。

{% highlight lua %}
#define iscollectable(o)  (ttype(o) >= LUA_TSTRING)
{% endhighlight %}

需要 gc 的数据类型，都会有一个 CommonHeader 成员，并且这个成员在结构体定义的最开始部分。在 Lua 中，值通过 TValue 结构体来表示。

{% highlight c %}
typedef union Value {
  GCObject *gc;    /* collectable objects */
  void *p;         /* light userdata */
  int b;           /* booleans */
  lua_CFunction f; /* light C functions */
  lua_Integer i;   /* integer numbers */
  lua_Number n;    /* float numbers */
} Value;

#define TValuefields Value value_; int tt_

typedef struct lua_TValue {
  TValuefields;
} TValue;
{% endhighlight %}

在 Lua 中个被称为 Tagged Values，也就是包括了一个值以及其类型。在 Value 中，gc 表示需要垃圾回收的一些值，如 string、table 等；p 表示 light userdata，不会被 gc 的。

注意，由于 Value 采用了 union 类型，这里可能会导致空间的浪费，对于某些硬件是可以进行优化的。

### Tables

Table 是 Lua 中唯一表示数据结构的类型，其中的函数环境 (env)、元表 (metatable)、模块 (module) 和注册表 (registery) 等都是通过 table 表示。在 Lua-5.0 后，table 是以一种混合型数据结构来实现的，它包含一个哈希表部分和一个数组部分。

对于哈希部分，如果发生冲突则通过链表解决。

{% highlight c %}
typedef union TKey {
  struct {
    TValuefields;
    int next;  /* for chaining (offset for next node) */
  } nk;
  TValue tvk;
} TKey;

typedef struct Node {
  TValue i_val;
  TKey i_key;
} Node;

typedef struct Table {
  CommonHeader;
  lu_byte flags;  /* 1<<p means tagmethod(p) is not present */
  lu_byte lsizenode;  /* log2 of size of 'node' array */
  unsigned int sizearray;  /* size of 'array' array */
  TValue *array;  /* array part */
  Node *node;
  Node *lastfree;  /* any free position is before this position */
  struct Table *metatable;
  GCObject *gclist;
} Table;

{% endhighlight %}

成员 array、sizearray 用于表示数组部分及其大小，node、lsizenode 表示哈希部分，不过需要注意的是，后者保存的是哈希表大小的幂次，而非真实大小，比如哈希表的大小为 2^N，则 lsizenode 的值是 N。

#### 新建

table 也是可以进行垃圾回收的对象。

{% highlight c %}
Table *luaH_new (lua_State *L) {
  GCObject *o = luaC_newobj(L, LUA_TTABLE, sizeof(Table));
  Table *t = gco2t(o);
  t->metatable = NULL;
  t->flags = cast_byte(~0);
  t->array = NULL;
  t->sizearray = 0;
  setnodevector(L, t, 0);
  return t;
}
{% endhighlight %}

首先，所有可 GC 的对象均通过 luaC_newobj() 创建一个新的可回收对象，并把创建的对象放到 GC 链表中；然后，通过 setnodevector() 来初始化 table 哈希表部分，其初始大小为 1 且 node 指向一个静态全局变量 dummynode_ 而非 NULL，从而减少操作表时的判断操作。

#### 查找

实际上也就是对于 t[key] 操作所执行的过程。

{% highlight c %}
const TValue *luaH_get (Table *t, const TValue *key) {
  switch (ttype(key)) {
    case LUA_TSHRSTR: return luaH_getshortstr(t, tsvalue(key));
    case LUA_TNUMINT: return luaH_getint(t, ivalue(key));
    case LUA_TNIL: return luaO_nilobject;
    case LUA_TNUMFLT: {
      lua_Integer k;
      if (luaV_tointeger(key, &k, 0)) /* index is int? */
        return luaH_getint(t, k);  /* use specialized version */
      /* else... */
    }  /* FALLTHROUGH */
    default:
      return getgeneric(t, key);
  }
}
{% endhighlight %}

如上，根据不同的类型的下标，会进入不同的查询。

对于字符串类型，通过 luaH_getstr() 先获得相应字符串在哈希表中的链表，然后遍历这个链表，采用内存地址比较字符串，若找到则返回相应的值，否则 nil 。

如果是整型则调用 luaH_getint() 查找，如果 key 的值小于等于数组大小，则直接返回相应的值，否则去哈希表中去查找。

对应其他类型，统一调用 getgeneric()，也就是计算 hash 值并在链表中查找，通过 luaV_equalobj() 对各种类型进行比较。

#### 赋值

也就是 t["key"]=1，会调用函数 luaH_set() 。

{% highlight c %}
TValue *luaH_set (lua_State *L, Table *t, const TValue *key) {
  const TValue *p = luaH_get(t, key);
  if (p != luaO_nilobject)
    return cast(TValue *, p);
  else return luaH_newkey(L, t, key);
}
{% endhighlight %}

首先查找 key 是否在 table 中，存在则替换原来的值，否则调用 luaH_newkey() 插入新值。



## 虚拟机的体系结构图

如上所述，所谓的 Register Based VM 是指指令的寻址方式，而实际上，Lua 解释器还是一个以栈为中心的结构。

![lua operation classes]({{ site.url }}/images/lua/structure.png "lua operation classes"){: .pull-center}

在 struct lua_State 中，有许多个字段用于描述这个栈结构，其中 stack 成员用于指向绝对栈底，而 base 指向了当前正在执行的函数的第一个参数，而 top 指向栈顶的第一个空元素。

可以看到，这个体系结构中并没有独立出来的寄存器，从以下代码来看：

{% highlight text %}
#define RA(i)    (base+GETARG_A(i))
/* to be used after possible stack reallocation */
#define RB(i)    check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base+GETARG_B(i))
#define RC(i)    check_exp(getCMode(GET_OPCODE(i)) == OpArgR, base+GETARG_C(i))
#define RKB(i)    check_exp(getBMode(GET_OPCODE(i)) == OpArgK, \
    ISK(GETARG_B(i)) ? k+INDEXK(GETARG_B(i)) : base+GETARG_B(i))
#define RKC(i)    check_exp(getCMode(GET_OPCODE(i)) == OpArgK, \
    ISK(GETARG_C(i)) ? k+INDEXK(GETARG_C(i)) : base+GETARG_C(i))
#define KBx(i)    check_exp(getBMode(GET_OPCODE(i)) == OpArgK, k+GETARG_Bx(i))
{% endhighlight %}

当指令操作数的类型是寄存器时，它的内容是以 base 为基址在栈上的索引值。如图所示，寄存器实际是 base 之上栈元素的别名；当指令操作数的类型的常数时，它首先判断 B 操作数的最位是否为零；如果是零，则按照和寄存器的处理方法一样做，如果不是零，则在常数表中找相应的值。

Lua 中函数的执行过程是这样的，首先将函数压栈，然后依次将参数压栈，形成图中所示的栈的内容. 因此 R[0] 到 R[n] 分别表示了 Arg[1] 到 Arg[N+1]。

在第一个参数之下，就是当前正在执行的函数，对于 Lua 的函数来说，指向类型为 Prototype 的 TValue，在 Prototype 中字段 code 指向了一个数组用来表示组成这个函数的所有指令，字段 k 指向一个数组来表示这个函数使用到的所有常量。最后，Lua 在解释执行过程中有专门的变量 pc 来指向下一条要执行的指令。

如上图所示，每个函数的 Proto 都有一个属于本函数的常量表，用于存放编译过程中函数所用到的常量。

另外，在 Proto 中还包含了一个 upvalue 描述表，用于存放在编译过程中确定的本函数所使用的 upvalue。在运行时，通过 OP_CLOSURE 指令创建一个 closure 时，会根据 Proto 中的描述，为这个 closure 初始化 upvalue 表，该表的访问不需要使用名称，而是通过 id 进行。



### 字节码

Lua 的指令使用一个 32bit 的 unsigned integer 表示，可以通过 luac 编译成字节码，或者直接查看。

{% highlight text %}
----- 查看foobar.lua的具体内容
$ cat foobar.lua
#!/usr/bin/lua
print('Hello World')

----- 然后通过luac命令编译
$ luac foobar.lua

----- 两个以上-l会打印详细信息
$ luac -l -l foobar.lua
{% endhighlight %}

然后通过 vi 的 :%!xxd 或者 hexedit 查看编译后的文件，其中 lua5.1 的字节码文件的头部长 12 字节。

{% highlight text %}
1b4c 7561 5100 0104 0804 0800
{% endhighlight %}

其中的字符为 1-4) "\033Lua"；5) 标识 lua 的版本号，lua5.1 为 0x51；6) 保留，默认是 0x0；7) 标识字节序，little-endian 为 0x01，big-endian 为 0x00；8) sizeof(int) 大小；9) sizeof(size_t)；10) sizeof(Instruction)，内部指令类型的大小；11) sizeof(lua_Number)，lua_Number 即为 double；12) 判断 lua_Number 类型起否有效，一般为 0x00。

其中 lua5.2 字节码文件头的长度为 18 字节。

{% highlight text %}
1b4c 7561 5200 0104 0804 0800 1993 0d0a 1a0a
{% endhighlight %}
其中第 1-12 字节与 lua5.1 意义相同，第 5 字节在 lua5.2 中为 0x52；第 13-18 字节是为了捕获字节码的转换错误而设置的，其值为 "\x19\x93\r\n\x1a\n"。



### 体系结构与指令系统

和虚拟机以及指令相关的文件主要有两个: lopcodes.c 和 lvm.c，这两个文件分别用于描述操作码(指令)和虚拟机。

Lua5.2 共有 40 条指令，通过 enum OpCode 定义，而 luaP_opnames[]、luaP_opmodes[] 分别描述了这些指令的名称和模式，根据指令参数的不同，可以将所有指令分为 4 类。

![lua operation classes]({{ site.url }}/images/lua/operation.png "lua operation classes"){: .pull-center}

除 sBx 之外，所有的指令参数都是 unsigned integer 类型，而 sBx 可以表示负数，但表示方法比较特殊。sBx 的 18bit 可表示的最大整数为 262143，这个数的一半 131071 用来表示 0，所以 -1 可以表示为 -1 + 131071，也就是 131070，而 +1 可以表示为 +1 + 131071，也就是 131072。

ABC 一般用来存放指令操作数据的地址，而地址可以分成 3 种：寄存器id、常量表id、upvalue id。Lua 将当前函数的 stack 作为寄存器使用，编号从 0 开始，当前函数的 stack 与寄存器数组是相同的概念，stack(n) 其实就是 register(n)。

A 被大多数指令用来指定计算结果的目标寄存器地址，B、C 用来存放寄存器地址和常量地址，并通过最左面的一个 bit 来区分。在指令生成阶段，如果 B 或 C 所引用的常量地址超出了表示范围，则首先会生成指令将常量装载到临时寄存器，然后再将 B 或 C 改为使用该寄存器地址。

区别在于，后面部是分割成为两个长度为 9 位的操作符(B, C)，一个无符号的 18 位操作符 Bx 还是有符号的 18 位操作符 sBx，这些定义的代码如下。

{% highlight c %}
/*
** size and position of opcode arguments.
*/
#define SIZE_C      9
#define SIZE_B      9
#define SIZE_Bx     (SIZE_C + SIZE_B)
#define SIZE_A      8
#define SIZE_Ax     (SIZE_C + SIZE_B + SIZE_A)

#define SIZE_OP     6

#define POS_OP      0
#define POS_A       (POS_OP + SIZE_OP)
#define POS_C       (POS_A + SIZE_A)
#define POS_B       (POS_C + SIZE_C)
#define POS_Bx      POS_C
#define POS_Ax      POS_A
{% endhighlight %}

### 指令操作模式

也就是 luaP_opmodes[]，使用一个字节来表示指令的操作模式，其具体含义如下：

* 最高位来表示是否是一条测试指令，之所以将这一类型的指令特别地标识出来，是因为 Lua 的指令长度是 32 位，对于分支指令来说，要想在这 32 位中既表示两个操作数来做比较，同时还要表示一个跳转的地址，是很困难的。因此将这种指令分成两条，第一条是测试指令，紧接着一条无条件跳转。如果判断条件成立则将 PC(Program Counter，指示下一条要执行的指令) 加一，跳过下一条无条件跳转指令，继续执行；否则跳转。

* 第二位用于表示 A 操作数是否被设置。

* 接下来的二位用于表示操作数 B 的格式，OpArgN 表示操作数未被使用，OpArgU 表示操作数被使用(立即数)，OpArgR 表示表示操作数是寄存器或者跳转的偏移量，OpArgK 表示操作数是寄存器或者常量。



## 虚拟机的执行

首先看下 Lua 虚拟机是如何执行的，对应函数的源码是 lua.c，编译之后是一个 stand-alone 的解析器，默认会进入一个交互式的命令行解析器，当然也可以执行一个 lua 脚本文件。

该程序最终会调用 luaV_execute() 函数执行，开始会初始化 global_State、lua_State 两个结构体，用来保存上下文的相关信息。

{% highlight text %}
main()
 |-luaL_newstate()                  # 创建global_State+lua_State，并初始化
 |-lua_pcall()                      # 实际会调用pmain()函数
   |                                # 根据不同的参数调用不同的函数
   |-runargs()                      # 执行命令行通过-e指定的命令
   |-doREPL()                       # 执行交互模式，也即read-eval-print loop
   |-handle_script()                # 执行lua脚本
     |-luaL_loadfile()              # 加载lua文件，后面详细介绍
     | |-lua_load()
     |
     |-docall()                     # 调用执行
       |-lua_pcall()
         |-luaD_pcall()             # 实际会调用f_call()函数
{% endhighlight %}

在调用函数执行过程中，最终会调用 luaV_execute() 函数。
<!--
其中，pc 被初始化成为了 L->savedpc，base 被初始化成为了 L->base，即程序从 L->savedpc 开始执行，其中 L->savedpc 在函数调用的预处理过程中指向了当前函数的 code，而 L->base 指向栈中当前函数的下一个位置；cl 表示当前正在执行闭包，k 指向当前闭包的常量表。
-->

其中，主要处理字节码的是 for(;;){} 循环，也即进入到解释器的主循环，处理很简单，取得当前指令，pc 递增，初始化 ra，然后根据指令的操作码进行选择；然后接下来是一大串的 switch ... case ... 处理。

接下来对其中有主要的几类指令进行说明。

### 传值类的指令，以 MOVE 为代表

{% highlight c %}
void luaV_execute (lua_State *L) {
      ... ...
      vmcase(OP_MOVE) {
        setobjs2s(L, ra, RB(i));
        vmbreak;
      }
      ... ...
}

#define setobjs2s    setobj
#define setobj(L,obj1,obj2) \
    { TValue *io1=(obj1); *io1 = *(obj2); \
      (void)L; checkliveness(G(L),io1); }
{% endhighlight %}

该指令将操作数 A、B 都做为寄存器，然后将 B 的值给 A，而实现也是简单明了，只有一句。宏展开以后，可以看到，R[A]、R[B] 的类型是 TValue，只是将这两域的值传过来即可。对于可回收对象来说，真实值不会保存在栈上，所以只是改了指针，而对于非可回收对象来说，则是直接将值从 R[B] 赋到 R[A]。

### 数值运算类指令，以 ADD 为代表

{% highlight c %}
void luaV_execute (lua_State *L) {
      ... ...
      vmcase(OP_ADD) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        lua_Number nb; lua_Number nc;
        if (ttisinteger(rb) && ttisinteger(rc)) {
          lua_Integer ib = ivalue(rb); lua_Integer ic = ivalue(rc);
          setivalue(ra, intop(+, ib, ic));
        }
        else if (tonumber(rb, &nb) && tonumber(rc, &nc)) {
          setfltvalue(ra, luai_numadd(L, nb, nc));
        }
        else { Protect(luaT_trybinTM(L, rb, rc, ra, TM_ADD)); }
        vmbreak;
      }
      ... ...
}
#define intop(op,v1,v2) l_castU2S(l_castS2U(v1) op l_castS2U(v2))
{% endhighlight %}

当两个值都是数值的话，其中关键的一行是 setivalue(ra, intop(+, ib, ic));，也就是两个操作数相加以后把值赋给 R[A]。值得注意的是，操作数 B,C 都是 RK，即可能是寄存器也可能是常量，这最决于最 B 和 C 的最高位是否为 1，如果是 1 则常量，反之则是寄存器；具体可以参考宏 RKB 的实现。

如果两个操作数不是数值，但可以转换成 Number，则调用 setfltvalue(ra, luai_numadd(L, nb, nc));。如果无法转换，则使用元表机制，该函数的实现如下:

{% highlight c %}
int luaT_callbinTM (lua_State *L, const TValue *p1, const TValue *p2,
                    StkId res, TMS event) {
  const TValue *tm = luaT_gettmbyobj(L, p1, event);  /* try first operand */
  if (ttisnil(tm))
    tm = luaT_gettmbyobj(L, p2, event);  /* try second operand */
  if (ttisnil(tm)) return 0;
  luaT_callTM(L, tm, p1, p2, res, 1);
  return 1;
}

void luaT_trybinTM (lua_State *L, const TValue *p1, const TValue *p2,
                    StkId res, TMS event) {
  if (!luaT_callbinTM(L, p1, p2, res, event)) {
    switch (event) {
      case TM_CONCAT:
        luaG_concaterror(L, p1, p2);
      /* call never returns, but to avoid warnings: *//* FALLTHROUGH */
      case TM_BAND: case TM_BOR: case TM_BXOR:
      case TM_SHL: case TM_SHR: case TM_BNOT: {
        lua_Number dummy;
        if (tonumber(p1, &dummy) && tonumber(p2, &dummy))
          luaG_tointerror(L, p1, p2);
        else
          luaG_opinterror(L, p1, p2, "perform bitwise operation on");
      }
      /* calls never return, but to avoid warnings: *//* FALLTHROUGH */
      default:
        luaG_opinterror(L, p1, p2, "perform arithmetic on");
    }
  }
}
{% endhighlight %}

上面 luaT_trybinTM() 用于调用元表中的元方法，因为在 Lua 老版本中元方法也被叫做 tag method，所以函数最后是以 TM 结尾的。

在 luaT_callTM() 中，将元方法、第一、第二操作数先后压栈，再调用并取因返回值。


### 逻辑运算类指令，以 EQ 为代表

{% highlight c %}
void luaV_execute (lua_State *L) {
      ... ...
      vmcase(OP_EQ) {
        TValue *rb = RKB(i);
        TValue *rc = RKC(i);
        Protect(
          if (luaV_equalobj(L, rb, rc) != GETARG_A(i))
            ci->u.l.savedpc++;
          else
            donextjump(ci);
        )
        vmbreak;
      }
      ... ...
}
#define dojump(ci,i,e) \
  { int a = GETARG_A(i); \
    if (a != 0) luaF_close(L, ci->u.l.base + a - 1); \
    ci->u.l.savedpc += GETARG_sBx(i) + e; }

#define donextjump(ci)  { i = *ci->u.l.savedpc; dojump(ci, i, 1); }
{% endhighlight %}

其中 luaV_equalobj() 与之前的算术运算类似，如果 RK[B]==RK[C] 并且 A 为 1 的情况下，也就是条件为真，则会使用 pc 取出下一条指令，调用 dojump() 进行跳转.


### 函数调用类指令，以 CALL 为代表

{% highlight c %}
void luaV_execute (lua_State *L) {
      ... ...
      vmcase(P_CALL) {
        int b = GETARG_B(i);
        int nresults = GETARG_C(i) - 1;
        if (b != 0) L->top = ra+b;  /* else previous instruction set top */
        if (luaD_precall(L, ra, nresults)) {  /* C function? */
          if (nresults >= 0)
            L->top = ci->top;  /* adjust results */
          Protect((void)0);  /* update 'base' */
        }
        else {  /* Lua function */
          ci = L->ci;
          goto newframe;  /* restart luaV_execute over new Lua function */
        }
        vmbreak;
      }
      ... ...
}
{% endhighlight %}

其中分为了 C 以及 Lua。




## 闭包实现

在 Lua 中，函数和闭包都是通过闭包实现的。

### 结构体

Lua 中包括了两种闭包：C 闭包和 Lua 闭包，分别通过两种结构体表示。

{% highlight c %}
#define CommonHeader    GCObject *next; lu_byte tt; lu_byte marked
#define ClosureHeader \
    CommonHeader; lu_byte nupvalues; GCObject *gclist

typedef int (*lua_CFunction) (lua_State *L);
typedef struct CClosure {
  ClosureHeader;
  lua_CFunction f;
  TValue upvalue[1];  /* list of upvalues */
} CClosure;

typedef struct LClosure {
  ClosureHeader;
  struct Proto *p;
  UpVal *upvals[1];  /* list of upvalues */
} LClosure;

typedef union Closure {
  CClosure c;
  LClosure l;
} Closure;
{% endhighlight %}

Lua 语言本身是支持闭包的，而所谓的闭包实际上就是把几个值和函数绑定在一起，在 Lua 中，这些值被称为 upvalues；而且，在 Lua 中，每个函数可以和一个 env 绑定。



## Lua 代码编译

关于 lua 代码是如何编译的，可以查看 luac 的代码实现。代码需要执行时，会通过 Proto 格式保存，字节码保存在 code 成员变量中。

{% highlight c %}
typedef struct Proto {
  CommonHeader;
  lu_byte numparams;  /* number of fixed parameters */
  lu_byte is_vararg;
  lu_byte maxstacksize;  /* number of registers needed by this function */
  int sizeupvalues;  /* size of 'upvalues' */
  int sizek;  /* size of 'k' */
  int sizecode;
  int sizelineinfo;
  int sizep;  /* size of 'p' */
  int sizelocvars;
  int linedefined;
  int lastlinedefined;
  TValue *k;  /* constants used by the function */
  Instruction *code;       // opcodes，操作码，以数组格式保存，类型为[int|long]
  struct Proto **p;  /* functions defined inside the function */
  int *lineinfo;  /* map from opcodes to source lines (debug information) */
  LocVar *locvars;  /* information about local variables (debug information) */
  Upvaldesc *upvalues;  /* upvalue information */
  struct LClosure *cache;  /* last-created closure with this prototype */
  TString  *source;  /* used for debug information */
  GCObject *gclist;
} Proto;
{% endhighlight %}

Lua 源码中可以通过 luaL_loadfile()、luaL_loadstring()、luaL_loadbuffer() 载入并编译 lua 代码，而这些函数实际调用的是 lua_load() 函数。

{% highlight text %}
lua_load()@lapi.c
  |-lua_lock()
  |-luaZ_init()
  |-luaD_protectedparser()
  | |-luaZ_initbuffer()
  | |-luaD_pcall()                    # 实际调用的是f_parser()
  | | |-zgetc()                       # 获取第一个字符，判断是否已经编译
  | | |                               #=================
  | | |-checkmode()                   # 1. 如果是二进制
  | | |-luaU_undump()                 #  1.1 加载预编译的二进制代码
  | | |-checkmode()                   # 2. 如果是文本
  | | |-luaY_parser()
  | | | |-mainfunc()
  | | |   |-luaX_next()
  | | |   | |-llex()
  | | |   |-statlist()                # 词法解析主函数
  | | |     |-statement()             # 根据相应的词法进行处理
  | | |       |-ifstat()              #
  | | |       |-whilestat()
  | | |                               #=================
  | | |-luaF_initupvals()
  | |-luaZ_freebuffer()               # 释放一系列缓存
  | |-... ...
  |-lua_unlock()
{% endhighlight %}

Lua 是一个轻量级高效率的语言，不仅体现在它本身虚拟机的运行效率上，而且也体现在他整个的编译系统的实现上，因为绝大多数的 lua 脚本需要运行期动态的加载编译，如果编译过程本身非常耗时，或者占用很多的内存，也同样会影响到整体的运行效率。

编译系统的工作就是将符合语法规则的 chunk 转换成可运行的 closure，closure 对象是 Lua 运行期一个函数的实例对象。除此之外，还有 Proto 对象，这是 lua 内部代表一个 closure 原型的对象，有关函数的大部分信息都保存在这里：

* 指令列表：包含了函数编译后生成的虚拟机指令。
* 常量表：这个函数运行期需要的所有常量，在指令中，常量使用常量表id进行索引。
* 子proto表：所有内嵌于这个函数的proto列表，在OP_CLOSURE指令中的proto id就是索引的这个表。
* 局部变量描述：这个函数使用到的所有局部变量名称，以及生命期。由于所有的局部变量运行期都被转化成了寄存器id，所以这些信息只是debug使用。
* Upvalue描述：设个函数所使用到的Upvalue的描述，用来在创建closure时初始化Upvalue。

每个 closure 都对应着自己的 proto，而运行期一个 proto 可以产生多个 closure 来代表这个函数实例。


{% highlight lua %}
{% endhighlight %}




<!--

可以参考云风编写的文档，如下为本地文档 <a href="reference/source/lua 源码鉴赏 readinglua 云风.pdf">lua 源码鉴赏</a>，关于源码解析可以参考 <a href="http://blog.csdn.net/yuanlin2008/article/category/1307277">探索Lua5.2内部实现</a>，或者 <a href="https://github.com/lichuang/Lua-Source-Internal">lua 详解</a> 以及相关的 <a href="http://www.codedump.info/?tag=lua">codedump 个人博客</a>。<br><br>

http://simohayha.iteye.com/blog/517748                               lua源码剖析(一)
http://blog.csdn.net/column/details/luainternals.html                探索Lua5.2内部实现
http://blog.csdn.net/INmouse/article/details/1540418                 Lua源码分析(1)
http://www.cppblog.com/airtrack/archive/2012/07/19/184211.aspx
-->
