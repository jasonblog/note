---
title: Lua 源碼解析
layout: post
comments: true
language: chinese
category: [program, linux]
keywords: lua,源碼解析
description: Lua 類似於 Java、Python，實際執行的都不是機器碼，而是運行在虛擬機上的，而虛擬機則屏蔽了底層不同的硬件，從而使得這些程序可以跨平臺執行，包括不同的操作系統以及不同的硬件平臺。相比而言，Lua 的代碼十分簡單，源碼總共才 2W 行左右，但是卻實現了很多不錯的特性。接下來，我們看看 Lua 具體是如何工作的。
---

Lua 類似於 Java、Python，實際執行的都不是機器碼，而是運行在虛擬機上的，而虛擬機則屏蔽了底層不同的硬件，從而使得這些程序可以跨平臺執行，包括不同的操作系統以及不同的硬件平臺。

相比而言，Lua 的代碼十分簡單，源碼總共才 2W 行左右，但是卻實現了很多不錯的特性。

接下來，我們看看 Lua 具體是如何工作的。

<!-- more -->

## 簡介

Lua 的代碼同樣可以被編譯，不過與 C/C++ 不同的是，是將源程序編譯成為字節碼，然後交由虛擬機解釋執行。在 Lua 中，每個函數編譯器都將創建一個原型(prototype)，它由一組指令及其使用到的常量組成。

最初的 Lua 虛擬機是基於棧的，到 1993 年，Lua5.0 版本，採用了基於寄存器的虛擬機，使得 Lua 的解釋效率得到提升。

相比來說，Lua 的代碼十分簡單，源碼可以直接從 [www.lua.org](http://www.lua.org/download.html) 上下載，其中有關虛擬機相關的內容可以參考 lua.c 和 luac.c 的實現。其中 luac 實現瞭如何編譯 lua 程序，而 lua 則包括了運行的虛擬機。

## 虛擬機

Lua 一直把虛擬機執行代碼的效率作為一個非常重要的設計目標，而採用什麼樣的指令系統的對於虛擬機的執行效率來說至關重要，通常根據指令獲取操作數的方式不同，可以把虛擬機分為兩種：基於棧的虛擬機 (Stack Based VM) 以及基於寄存器的虛擬機 (Register Based VM)。

### Stack based VM

目前，大多數的虛擬機都採用傳統的 Stack Based VM，如 JVM、Python，該模型的指令一般都是在當前 stack 中獲取和保存操作數。例如，一個簡單的加法賦值運算 a=b+c，對於該模型，一般會被轉化成如下的指令。

{% highlight text %}
push b;     // 將變量b的值壓入stack
push c;     // 將變量c的值壓入stack
add;        // 將stack頂部的兩個值彈出後相加，然後將結果壓入stack頂
mov a;      // 將stack頂部結果放到a中
{% endhighlight %}

由於 Stack Based VM 的指令都是基於當前 stack 來查找操作數的，這就相當於所有操作數的存儲位置都是運行期決定的，在編譯器的代碼生成階段不需要額外為在哪裡存儲操作數費心，所以 stack based 的編譯器實現起來相對比較簡單直接，也正因為這個原因，每條指令佔用的存儲空間也比較小。

但是，對於一個簡單的運算，stack based vm 會使用過多的指令組合來完成，這樣就增加了整體指令集合的長度。vm 會使用同樣多的迭代次數來執行這些指令，這對於效率來說會有很大的影響。並且，由於操作數都要放到stack上面，使得移動這些操作數的內存複製大大增加，這也會影響到效率。

### Register Based VM

Lua 採用該模型，指令都是在已經分配好的寄存器中存取操作數，對於上面的運算，Register Based VM 一般會使用如下的指令。

{% highlight text %}
add a b c; // 將b與c對應的寄存器的值相加，將結果保存在a對應的寄存器中
{% endhighlight %}

Register Based VM 的指令可以直接對應標準的 3 地址指令，用一條指令完成了上面多條指令的計算工作，並且有效地減少了內存複製操作，這樣的指令系統對於效率有很大的幫助。

不過，在編譯器設計上，就要在代碼生成階段對寄存器進行分配，採用圖著色這樣的算法，增加了實現的複雜度，並且每條指令所佔用的存儲空間也相應的增加了。

接下來查看 lua 的實現細節。




## 類型定義

Lua 是一種動態類型的語言 (dynamically typed language)，也就是說，在定義的變量中不包含類型的信息，只有相應的值才會包含類型信息。

本類型有八種：nil、boolean、number、string、function、userdata、thread 和 table，在 src/lua.h 文件中定義，如 LUA_TSTRING、LUA_TFUNCTIOIN 等。

需要注意的數據類型是 userdata，在內部實現分為了 LUA_TLIGHTUSERDATA 和 LUA_TUSERDATA，兩者都對應 void* 指針，區別在於，前者是由 Lua 外部的使用者來完成，後者則是通過 Lua 內部來完成，也就是說前者是通過用戶來維護其生命週期。

另外，Lua 支持垃圾回收，內部用一個宏 iscollectable() 標示哪些類型需要進行 gc 操作，包括 LUA_TSTRING 之後的數據類型，都需要進行 gc 操作。

{% highlight lua %}
#define iscollectable(o)  (ttype(o) >= LUA_TSTRING)
{% endhighlight %}

需要 gc 的數據類型，都會有一個 CommonHeader 成員，並且這個成員在結構體定義的最開始部分。在 Lua 中，值通過 TValue 結構體來表示。

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

在 Lua 中個被稱為 Tagged Values，也就是包括了一個值以及其類型。在 Value 中，gc 表示需要垃圾回收的一些值，如 string、table 等；p 表示 light userdata，不會被 gc 的。

注意，由於 Value 採用了 union 類型，這裡可能會導致空間的浪費，對於某些硬件是可以進行優化的。

### Tables

Table 是 Lua 中唯一表示數據結構的類型，其中的函數環境 (env)、元表 (metatable)、模塊 (module) 和註冊表 (registery) 等都是通過 table 表示。在 Lua-5.0 後，table 是以一種混合型數據結構來實現的，它包含一個哈希表部分和一個數組部分。

對於哈希部分，如果發生衝突則通過鏈表解決。

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

成員 array、sizearray 用於表示數組部分及其大小，node、lsizenode 表示哈希部分，不過需要注意的是，後者保存的是哈希表大小的冪次，而非真實大小，比如哈希表的大小為 2^N，則 lsizenode 的值是 N。

#### 新建

table 也是可以進行垃圾回收的對象。

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

首先，所有可 GC 的對象均通過 luaC_newobj() 創建一個新的可回收對象，並把創建的對象放到 GC 鏈表中；然後，通過 setnodevector() 來初始化 table 哈希表部分，其初始大小為 1 且 node 指向一個靜態全局變量 dummynode_ 而非 NULL，從而減少操作表時的判斷操作。

#### 查找

實際上也就是對於 t[key] 操作所執行的過程。

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

如上，根據不同的類型的下標，會進入不同的查詢。

對於字符串類型，通過 luaH_getstr() 先獲得相應字符串在哈希表中的鏈表，然後遍歷這個鏈表，採用內存地址比較字符串，若找到則返回相應的值，否則 nil 。

如果是整型則調用 luaH_getint() 查找，如果 key 的值小於等於數組大小，則直接返回相應的值，否則去哈希表中去查找。

對應其他類型，統一調用 getgeneric()，也就是計算 hash 值並在鏈表中查找，通過 luaV_equalobj() 對各種類型進行比較。

#### 賦值

也就是 t["key"]=1，會調用函數 luaH_set() 。

{% highlight c %}
TValue *luaH_set (lua_State *L, Table *t, const TValue *key) {
  const TValue *p = luaH_get(t, key);
  if (p != luaO_nilobject)
    return cast(TValue *, p);
  else return luaH_newkey(L, t, key);
}
{% endhighlight %}

首先查找 key 是否在 table 中，存在則替換原來的值，否則調用 luaH_newkey() 插入新值。



## 虛擬機的體系結構圖

如上所述，所謂的 Register Based VM 是指指令的尋址方式，而實際上，Lua 解釋器還是一個以棧為中心的結構。

![lua operation classes]({{ site.url }}/images/lua/structure.png "lua operation classes"){: .pull-center}

在 struct lua_State 中，有許多個字段用於描述這個棧結構，其中 stack 成員用於指向絕對棧底，而 base 指向了當前正在執行的函數的第一個參數，而 top 指向棧頂的第一個空元素。

可以看到，這個體系結構中並沒有獨立出來的寄存器，從以下代碼來看：

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

當指令操作數的類型是寄存器時，它的內容是以 base 為基址在棧上的索引值。如圖所示，寄存器實際是 base 之上棧元素的別名；當指令操作數的類型的常數時，它首先判斷 B 操作數的最位是否為零；如果是零，則按照和寄存器的處理方法一樣做，如果不是零，則在常數表中找相應的值。

Lua 中函數的執行過程是這樣的，首先將函數壓棧，然後依次將參數壓棧，形成圖中所示的棧的內容. 因此 R[0] 到 R[n] 分別表示了 Arg[1] 到 Arg[N+1]。

在第一個參數之下，就是當前正在執行的函數，對於 Lua 的函數來說，指向類型為 Prototype 的 TValue，在 Prototype 中字段 code 指向了一個數組用來表示組成這個函數的所有指令，字段 k 指向一個數組來表示這個函數使用到的所有常量。最後，Lua 在解釋執行過程中有專門的變量 pc 來指向下一條要執行的指令。

如上圖所示，每個函數的 Proto 都有一個屬於本函數的常量表，用於存放編譯過程中函數所用到的常量。

另外，在 Proto 中還包含了一個 upvalue 描述表，用於存放在編譯過程中確定的本函數所使用的 upvalue。在運行時，通過 OP_CLOSURE 指令創建一個 closure 時，會根據 Proto 中的描述，為這個 closure 初始化 upvalue 表，該表的訪問不需要使用名稱，而是通過 id 進行。



### 字節碼

Lua 的指令使用一個 32bit 的 unsigned integer 表示，可以通過 luac 編譯成字節碼，或者直接查看。

{% highlight text %}
----- 查看foobar.lua的具體內容
$ cat foobar.lua
#!/usr/bin/lua
print('Hello World')

----- 然後通過luac命令編譯
$ luac foobar.lua

----- 兩個以上-l會打印詳細信息
$ luac -l -l foobar.lua
{% endhighlight %}

然後通過 vi 的 :%!xxd 或者 hexedit 查看編譯後的文件，其中 lua5.1 的字節碼文件的頭部長 12 字節。

{% highlight text %}
1b4c 7561 5100 0104 0804 0800
{% endhighlight %}

其中的字符為 1-4) "\033Lua"；5) 標識 lua 的版本號，lua5.1 為 0x51；6) 保留，默認是 0x0；7) 標識字節序，little-endian 為 0x01，big-endian 為 0x00；8) sizeof(int) 大小；9) sizeof(size_t)；10) sizeof(Instruction)，內部指令類型的大小；11) sizeof(lua_Number)，lua_Number 即為 double；12) 判斷 lua_Number 類型起否有效，一般為 0x00。

其中 lua5.2 字節碼文件頭的長度為 18 字節。

{% highlight text %}
1b4c 7561 5200 0104 0804 0800 1993 0d0a 1a0a
{% endhighlight %}
其中第 1-12 字節與 lua5.1 意義相同，第 5 字節在 lua5.2 中為 0x52；第 13-18 字節是為了捕獲字節碼的轉換錯誤而設置的，其值為 "\x19\x93\r\n\x1a\n"。



### 體系結構與指令系統

和虛擬機以及指令相關的文件主要有兩個: lopcodes.c 和 lvm.c，這兩個文件分別用於描述操作碼(指令)和虛擬機。

Lua5.2 共有 40 條指令，通過 enum OpCode 定義，而 luaP_opnames[]、luaP_opmodes[] 分別描述了這些指令的名稱和模式，根據指令參數的不同，可以將所有指令分為 4 類。

![lua operation classes]({{ site.url }}/images/lua/operation.png "lua operation classes"){: .pull-center}

除 sBx 之外，所有的指令參數都是 unsigned integer 類型，而 sBx 可以表示負數，但表示方法比較特殊。sBx 的 18bit 可表示的最大整數為 262143，這個數的一半 131071 用來表示 0，所以 -1 可以表示為 -1 + 131071，也就是 131070，而 +1 可以表示為 +1 + 131071，也就是 131072。

ABC 一般用來存放指令操作數據的地址，而地址可以分成 3 種：寄存器id、常量表id、upvalue id。Lua 將當前函數的 stack 作為寄存器使用，編號從 0 開始，當前函數的 stack 與寄存器數組是相同的概念，stack(n) 其實就是 register(n)。

A 被大多數指令用來指定計算結果的目標寄存器地址，B、C 用來存放寄存器地址和常量地址，並通過最左面的一個 bit 來區分。在指令生成階段，如果 B 或 C 所引用的常量地址超出了表示範圍，則首先會生成指令將常量裝載到臨時寄存器，然後再將 B 或 C 改為使用該寄存器地址。

區別在於，後面部是分割成為兩個長度為 9 位的操作符(B, C)，一個無符號的 18 位操作符 Bx 還是有符號的 18 位操作符 sBx，這些定義的代碼如下。

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

也就是 luaP_opmodes[]，使用一個字節來表示指令的操作模式，其具體含義如下：

* 最高位來表示是否是一條測試指令，之所以將這一類型的指令特別地標識出來，是因為 Lua 的指令長度是 32 位，對於分支指令來說，要想在這 32 位中既表示兩個操作數來做比較，同時還要表示一個跳轉的地址，是很困難的。因此將這種指令分成兩條，第一條是測試指令，緊接著一條無條件跳轉。如果判斷條件成立則將 PC(Program Counter，指示下一條要執行的指令) 加一，跳過下一條無條件跳轉指令，繼續執行；否則跳轉。

* 第二位用於表示 A 操作數是否被設置。

* 接下來的二位用於表示操作數 B 的格式，OpArgN 表示操作數未被使用，OpArgU 表示操作數被使用(立即數)，OpArgR 表示表示操作數是寄存器或者跳轉的偏移量，OpArgK 表示操作數是寄存器或者常量。



## 虛擬機的執行

首先看下 Lua 虛擬機是如何執行的，對應函數的源碼是 lua.c，編譯之後是一個 stand-alone 的解析器，默認會進入一個交互式的命令行解析器，當然也可以執行一個 lua 腳本文件。

該程序最終會調用 luaV_execute() 函數執行，開始會初始化 global_State、lua_State 兩個結構體，用來保存上下文的相關信息。

{% highlight text %}
main()
 |-luaL_newstate()                  # 創建global_State+lua_State，並初始化
 |-lua_pcall()                      # 實際會調用pmain()函數
   |                                # 根據不同的參數調用不同的函數
   |-runargs()                      # 執行命令行通過-e指定的命令
   |-doREPL()                       # 執行交互模式，也即read-eval-print loop
   |-handle_script()                # 執行lua腳本
     |-luaL_loadfile()              # 加載lua文件，後面詳細介紹
     | |-lua_load()
     |
     |-docall()                     # 調用執行
       |-lua_pcall()
         |-luaD_pcall()             # 實際會調用f_call()函數
{% endhighlight %}

在調用函數執行過程中，最終會調用 luaV_execute() 函數。
<!--
其中，pc 被初始化成為了 L->savedpc，base 被初始化成為了 L->base，即程序從 L->savedpc 開始執行，其中 L->savedpc 在函數調用的預處理過程中指向了當前函數的 code，而 L->base 指向棧中當前函數的下一個位置；cl 表示當前正在執行閉包，k 指向當前閉包的常量表。
-->

其中，主要處理字節碼的是 for(;;){} 循環，也即進入到解釋器的主循環，處理很簡單，取得當前指令，pc 遞增，初始化 ra，然後根據指令的操作碼進行選擇；然後接下來是一大串的 switch ... case ... 處理。

接下來對其中有主要的幾類指令進行說明。

### 傳值類的指令，以 MOVE 為代表

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

該指令將操作數 A、B 都做為寄存器，然後將 B 的值給 A，而實現也是簡單明瞭，只有一句。宏展開以後，可以看到，R[A]、R[B] 的類型是 TValue，只是將這兩域的值傳過來即可。對於可回收對象來說，真實值不會保存在棧上，所以只是改了指針，而對於非可回收對象來說，則是直接將值從 R[B] 賦到 R[A]。

### 數值運算類指令，以 ADD 為代表

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

當兩個值都是數值的話，其中關鍵的一行是 setivalue(ra, intop(+, ib, ic));，也就是兩個操作數相加以後把值賦給 R[A]。值得注意的是，操作數 B,C 都是 RK，即可能是寄存器也可能是常量，這最決於最 B 和 C 的最高位是否為 1，如果是 1 則常量，反之則是寄存器；具體可以參考宏 RKB 的實現。

如果兩個操作數不是數值，但可以轉換成 Number，則調用 setfltvalue(ra, luai_numadd(L, nb, nc));。如果無法轉換，則使用元表機制，該函數的實現如下:

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

上面 luaT_trybinTM() 用於調用元表中的元方法，因為在 Lua 老版本中元方法也被叫做 tag method，所以函數最後是以 TM 結尾的。

在 luaT_callTM() 中，將元方法、第一、第二操作數先後壓棧，再調用並取因返回值。


### 邏輯運算類指令，以 EQ 為代表

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

其中 luaV_equalobj() 與之前的算術運算類似，如果 RK[B]==RK[C] 並且 A 為 1 的情況下，也就是條件為真，則會使用 pc 取出下一條指令，調用 dojump() 進行跳轉.


### 函數調用類指令，以 CALL 為代表

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

其中分為了 C 以及 Lua。




## 閉包實現

在 Lua 中，函數和閉包都是通過閉包實現的。

### 結構體

Lua 中包括了兩種閉包：C 閉包和 Lua 閉包，分別通過兩種結構體表示。

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

Lua 語言本身是支持閉包的，而所謂的閉包實際上就是把幾個值和函數綁定在一起，在 Lua 中，這些值被稱為 upvalues；而且，在 Lua 中，每個函數可以和一個 env 綁定。



## Lua 代碼編譯

關於 lua 代碼是如何編譯的，可以查看 luac 的代碼實現。代碼需要執行時，會通過 Proto 格式保存，字節碼保存在 code 成員變量中。

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
  Instruction *code;       // opcodes，操作碼，以數組格式保存，類型為[int|long]
  struct Proto **p;  /* functions defined inside the function */
  int *lineinfo;  /* map from opcodes to source lines (debug information) */
  LocVar *locvars;  /* information about local variables (debug information) */
  Upvaldesc *upvalues;  /* upvalue information */
  struct LClosure *cache;  /* last-created closure with this prototype */
  TString  *source;  /* used for debug information */
  GCObject *gclist;
} Proto;
{% endhighlight %}

Lua 源碼中可以通過 luaL_loadfile()、luaL_loadstring()、luaL_loadbuffer() 載入並編譯 lua 代碼，而這些函數實際調用的是 lua_load() 函數。

{% highlight text %}
lua_load()@lapi.c
  |-lua_lock()
  |-luaZ_init()
  |-luaD_protectedparser()
  | |-luaZ_initbuffer()
  | |-luaD_pcall()                    # 實際調用的是f_parser()
  | | |-zgetc()                       # 獲取第一個字符，判斷是否已經編譯
  | | |                               #=================
  | | |-checkmode()                   # 1. 如果是二進制
  | | |-luaU_undump()                 #  1.1 加載預編譯的二進制代碼
  | | |-checkmode()                   # 2. 如果是文本
  | | |-luaY_parser()
  | | | |-mainfunc()
  | | |   |-luaX_next()
  | | |   | |-llex()
  | | |   |-statlist()                # 詞法解析主函數
  | | |     |-statement()             # 根據相應的詞法進行處理
  | | |       |-ifstat()              #
  | | |       |-whilestat()
  | | |                               #=================
  | | |-luaF_initupvals()
  | |-luaZ_freebuffer()               # 釋放一系列緩存
  | |-... ...
  |-lua_unlock()
{% endhighlight %}

Lua 是一個輕量級高效率的語言，不僅體現在它本身虛擬機的運行效率上，而且也體現在他整個的編譯系統的實現上，因為絕大多數的 lua 腳本需要運行期動態的加載編譯，如果編譯過程本身非常耗時，或者佔用很多的內存，也同樣會影響到整體的運行效率。

編譯系統的工作就是將符合語法規則的 chunk 轉換成可運行的 closure，closure 對象是 Lua 運行期一個函數的實例對象。除此之外，還有 Proto 對象，這是 lua 內部代表一個 closure 原型的對象，有關函數的大部分信息都保存在這裡：

* 指令列表：包含了函數編譯後生成的虛擬機指令。
* 常量表：這個函數運行期需要的所有常量，在指令中，常量使用常量表id進行索引。
* 子proto表：所有內嵌於這個函數的proto列表，在OP_CLOSURE指令中的proto id就是索引的這個表。
* 局部變量描述：這個函數使用到的所有局部變量名稱，以及生命期。由於所有的局部變量運行期都被轉化成了寄存器id，所以這些信息只是debug使用。
* Upvalue描述：設個函數所使用到的Upvalue的描述，用來在創建closure時初始化Upvalue。

每個 closure 都對應著自己的 proto，而運行期一個 proto 可以產生多個 closure 來代表這個函數實例。


{% highlight lua %}
{% endhighlight %}




<!--

可以參考雲風編寫的文檔，如下為本地文檔 <a href="reference/source/lua 源碼鑑賞 readinglua 雲風.pdf">lua 源碼鑑賞</a>，關於源碼解析可以參考 <a href="http://blog.csdn.net/yuanlin2008/article/category/1307277">探索Lua5.2內部實現</a>，或者 <a href="https://github.com/lichuang/Lua-Source-Internal">lua 詳解</a> 以及相關的 <a href="http://www.codedump.info/?tag=lua">codedump 個人博客</a>。<br><br>

http://simohayha.iteye.com/blog/517748                               lua源碼剖析(一)
http://blog.csdn.net/column/details/luainternals.html                探索Lua5.2內部實現
http://blog.csdn.net/INmouse/article/details/1540418                 Lua源碼分析(1)
http://www.cppblog.com/airtrack/archive/2012/07/19/184211.aspx
-->
