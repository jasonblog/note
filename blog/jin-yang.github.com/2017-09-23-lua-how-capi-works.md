---
title: Lua CAPI 使用
layout: post
comments: true
language: chinese
category: [program,linux]
keywords: lua, c, api, lua state, stack
description: Lua 是擴展性非常良好的語言，雖然核心非常精簡，但是用戶可以依靠 Lua 庫來實現大部分工作，還可以通過與 C 函數相互調用來擴展程序功能。在 C 中嵌入 Lua 腳本既可以讓用戶在不重新編譯代碼的情況下修改 Lua 代碼更新程序，也可以給用戶提供一個自由定製的接口，這種方法遵循了機制與策略分離的原則；在 Lua 中調用 C 函數可以提高程序的運行效率。Lua 與 C 的相互調用在工程中相當實用，這裡就簡單講解 Lua 與 C 相互調用的方法。
---

Lua 是擴展性非常良好的語言，雖然核心非常精簡，但是用戶可以依靠 Lua 庫來實現大部分工作，還可以通過與 C 函數相互調用來擴展程序功能。

在 C 中嵌入 Lua 腳本既可以讓用戶在不重新編譯代碼的情況下修改 Lua 代碼更新程序，也可以給用戶提供一個自由定製的接口，這種方法遵循了機制與策略分離的原則；在 Lua 中調用 C 函數可以提高程序的運行效率。

Lua 與 C 的相互調用在工程中相當實用，這裡就簡單講解 Lua 與 C 相互調用的方法。

<!-- more -->

## 棧

Lua 與 C 交互的棧是一個重要的概念，作為一種嵌入式語言，所有的 Lua 程序最後都需要通過 Lua 解釋器將其解析成字節碼的形式才能執行。

<!--文章首先解釋了為什麼要引入Lua棧，然後對訪問棧常用的API進行了總結，並使用這些API的注意事項，最後從Lua源代碼來看棧的實現原理。-->

可在應用程序中嵌入 Lua 解釋器，此時使用 Lua 的目的是方便擴展這個應用程序，用 Lua 實現相應的工作；另一方面，在 Lua 程序中也可以使用那些用 C 語言實現的函數，比如 `string.find()` 。

為了方便在兩個語言之間進行交換數據，Lua 引入了一個虛擬棧，同時提供了一系列的 API ，通過這些 API C 語言就可以方便從 Lua 中獲取相應的值，也可以方便地把值返回給 Lua，當然，這些操作都是通過棧作為橋樑來實現的。

### API

Lua 提供了大量的 API 操作棧，用於向棧中壓入元素、查詢棧中的元素、修改棧的大小等操作。

#### 1. 向棧中壓入元素

通常都是以 `lua_push` 開頭來命名，比如 `lua_pushnunber`、`lua_pushstring`、`lua_pushcfunction`、`lua_pushcclousre` 等函數都是向棧頂中壓入一個 Lua 值。

<!--
在 Lua 代碼中調用 C 實現的函數並且被調用的 C 函數有返回值時，被調用的C函數通常就要用到這些接口，把返回值壓入棧中，返回給Lua（當然這些C函數也要求返回一個值，告訴Lua一共返回（壓入）了多少個值）。值得注意的是，向棧中壓入一個元素時，應該確保棧中具有足夠的空間，可以調用lua_checkstack來檢測是否有足夠的空間。

實質上這些API是把C語言裡面的值封裝成Lua類型的值壓入棧中的，對於那些需要垃圾回收的元素，在壓入棧時，都會在Lua（也就是Lua虛擬機中）生成一個副本。比如lua_pushstring(lua_State *L, const char *s)會向中棧壓入由s指向的以'\0'結尾的字符串，在C中調用這個函數後，我們可以任意釋放或修改由s指向的字符串，也不會出現問題，原因就是在執行lua_pushstring過程中Lua會生成一個內部副本。實質上，Lua不會持有指向外部字符串的指針，也不會持有指向任何其他外部對象的指針（除了C函數，因為C函數總是靜態的）。

總之，一旦C中值被壓入棧中，Lua就會生成相應的結構（實質就是Lua中實現的相應數據類型）並管理（比如自動垃圾回收）這個值，從此不會再依賴於原來的C值。
-->

#### 2. 獲取棧中的元素

從棧中獲取一個值的函數，通常都是以 `lua_to` 開頭來命名，比如 `lua_tonumber`、`lua_tostring`、`lua_touserdata`、`lua_tocfunction` 等函數都是從棧中指定的索引處獲取一個值，可在 C 函數中用這些接口獲取從 Lua 中傳遞給 C 函數的參數。

<!--
如果指定的元素不具有正確的類型，調用這些函數也不會出問題的。在這種情況下，lua_toboolean、lua_tonumber、lua_tointeger和lua_objlen會返回0，而其他函數會返回NULL。對於返回NULL的函數，可以直接通過返回值，即可以知道調用是否正確；對於返回0的函數，通常先需要使用lua_is*系列函數，判斷調用是否正確。

注意lua_to*和lua_is*系列函數都是試圖轉換棧中元素為相應中的值。比如lua_isnumber不會檢查是否為數字類型，而是檢查是否能轉換為數字類型；lua_isstring也類似，它對於任意數字，lua_isstring都返回真。要想真正返回棧中元素的類型，可以用函數lua_type。每種類型對應於一個常量（LUA_TNIL，LUA_TBOOLEAN，LUA_TNUMBER等），這些常量定義在頭文件lua.h中。

值得一提是lua_tolstring函數，它的函數原型是const char *lua_tolstring (lua_State *L, int index, size_t *len)。它會把棧中索引為index的Lua值裝換為一個C字符串。若參數Len不為NULL，則*Len會保存字符串的長度。棧中的Lua值必須為string或number類型，否則函數返回NULL。若棧中Lua值為number類型，則該函數實質會改變棧中的值為string類型，由於這個原因，在利用lua_next遍歷棧中的table時，對key使用lua_tolstring尤其需要注意，除非知道key都是string類型。lua_tolstring函數返回的指針，指向的是Lua虛擬機內部的字符串，這個字符串是以'\0'結尾的，但字符串中間也可能包含值為0的字符。由於Lua自身的垃圾回收，因此當棧中的字符串被彈出後，函數返回的指針所有指向的字符串可能就不能再有效了。也說明了，當一個C函數從Lua收到一個字符串參數時，在C函數中，即不能在訪問字符串時從棧中彈出它，也不能修改字符串。

#### 3. 其它操作棧的函數

int lua_call(lua_State *L, int nargs, int nresults);

調用棧中的函數，在調用lua_call之前，程序必須首先要保證被調用函數已壓入棧，其次要被調用函數需要的參數也已經按順序壓入棧，也就是說，第一個參數最先被壓入棧，依次類推。nargs是指需要壓入棧中參數的個數，當函數被調用後，之前壓入的函數和參數都會從棧中彈出，並將函數執行的結果按順序壓入棧中，因此最後一個結果壓入棧頂，同時，壓入棧的個數會根據nresults的值做調整。與lua_call相對應的是lua_pcall函數，lua_pcall會以保護模式調用棧中的函數。以保護模式調用意思是，當被調用的函數發生任何錯誤時，該錯誤不會傳播，不像lua_call會把錯誤傳遞到上一層，lua_pcall所調用的棧中函數發送錯誤時，lua_pcall會捕捉這個錯誤，並向棧中壓入一個錯誤信息，並返回一個錯誤碼。在應用程序中編寫主函數時，應該使用lua_pcall來調用棧中的函數，捕獲所有錯誤。而在為Lua編寫擴展的C函數時，應該調用lua_call，把錯誤返回到腳本層。

void lua_createtable (lua_State *L, int narr, int nrec);
創建一個新的table，並把它壓入棧頂，參數narr和nrec分別指新的table將會有多少個數組元素和多少需要hash的元素，Lua會根據這個兩個值為新的table預分配內存。對於事先知道table結構，利用這兩個參數能提高創建新table的性能。對於事先不知道table結構，則可以使用void lua_newtable (lua_State *L),它等價於lua_createtable(L, 0, 0)。

除了上面提到的C API，還有許多其他有用的C API，比如操作table的接口有：lua_getfield、lua_setfield、lua_gettable、lua_settable等接口，在具體使用時，可以參照Lua手冊。
-->

### 源碼解析

程序中為了加載執行 Lua 腳本，首先需要調用 `luaL_newstate()` 初始化 Lua 虛擬機，該函數會創建 Lua 與 C 交互的棧，返回指向 `lua_State` 類型的指針，後面幾乎所有 API 都需要 `lua_State*` 作為入參，這樣就使得每個 Lua 狀態機是各自獨立的，不共享任何數據。

這裡的 `lua_State` 就保存了一個 Lua 解析器的執行狀態，它代表一個新的線程 (同上，非操作系統中的線程)，每個線程擁有獨立的數據棧、函數調用鏈、調試鉤子和錯誤處理方法。

實際上幾乎所有的 API 操作，都是圍繞這個 `lua_State` 結構來進行的，包括把數據壓入棧、從棧中取出數據、執行棧中的函數等操作。

`struct lua_State` 在 lstate.h 頭文件中定義，其代碼如下:

{% highlight c %}
struct lua_State {
	CommonHeader;
	lu_byte status;
	StkId top;                     /* 指向數據棧中，第一個可使用的空間*/
	global_State *l_G;
	CallInfo *ci;                  /* 保存著當前正在調用的函數的運行狀態 */
	const Instruction *oldpc;
	StkId stack_last;              /* 指向數據棧中，最後一個可使用的空間 */
	StkId stack;                   /* 指向數據棧開始的位置 */
	int stacksize;                 /* 棧當前的大小，注意並不是可使用的大小*/
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
	CallInfo base_ci;              /* 保存調用鏈表的第一個節點*/
};
{% endhighlight %}

<!--
      lua_State中主要包含兩個重要的數據結構，一個是數據棧，另外一個是調用棧（實質上是一個雙向鏈表）。數據棧實質就是一個動態數組，數組中每個元素類型為TValue。Lua中任何數據類型（nil，number，stirng，userdata，function等）都是用該結構體TValue來實現的。其定義如下（源碼裡面使用了大量的宏和typedef來定義TValue，為了方便閱讀，把它展開了）：


      Lua中所有的數據類型都是由結構體TValue來實現的，它把值和類型綁在一起，每個數據都攜帶了它自身的類型信息。用成員tt_保存數據的類型，成員value_用來保存數據值，它使用的一個聯合體來實現的：

[cpp] view plain copy

     union  Value {
           GCObject *gc;   /*gc指向一個對象，這些對象是需要垃圾回收的數據類型，比如table、string等*/
           void *p;        /* lua中的light userdata類型，實質上保存的就是一個指針 */
           int b;          /*boolean類型*/
           lua_CFunction f;/*lua中light C functions（沒有upvalue），即只是函數指針 */
           double n;       /*lua中的number類型*/
    };

      上面提到到數據棧是在函數stack_init中創建的（初始化虛擬機時調用的luaL_newstate，就是通過調用lua_newstate函數，lua_newstate調用f_luaopen函數，最後f_luaopen函數調用stack_init來初始化棧的），函數stack_init在lstate.c中實現，代碼如下：

[cpp] view plain copy

    static void stack_init (lua_State *L1, lua_State *L) {
          int i; CallInfo *ci;

          /* 為數據棧分配空間，並且初始化lua_State與數據棧相關的成員*/
          L1->stack = luaM_newvector(L, BASIC_STACK_SIZE, TValue);
          L1->stacksize = BASIC_STACK_SIZE;
          for (i = 0; i < BASIC_STACK_SIZE; i++)
            setnilvalue(L1->stack + i);  /* erase new stack */
          L1->top = L1->stack;
          L1->stack_last = L1->stack + L1->stacksize - EXTRA_STACK;

          /*初始化lua_State與調用鏈表相關的成員*/
          ci = &L1->base_ci;
          ci->next = ci->previous = NULL;
          ci->callstatus = 0;
          ci->func = L1->top;
          setnilvalue(L1->top++);  /* 'function' entry for this 'ci' */
          ci->top = L1->top + LUA_MINSTACK;
          L1->ci = ci;
    }

 調用棧實質上用一個雙向鏈表來實現的，鏈表中的每個節點是用一個CallInfo的結構體來實現，保存著正在調用的函數的運行狀態。結構體CallInfo在lstate.h定義的，代碼如下：

[cpp] view plain copy

    typedef struct CallInfo {
          StkId  func; /* 指向被調用的函數在棧中的位置*/
          StkId  top; /*指向被調用的函數可以使用棧空間最大的位置，即限定了調用一個函數可以棧空間的大小*/
          struct CallInfo *previous, *next; /* 指向調用鏈表的前一個節點和下一個節點 */
          short nresults;                  /* 當前被調用的函數期待返回結果的數量*/
          lu_byte callstatus;              /*用來標識當前調用的是C函數還是Lua函數*/
          union {
            struct {                       /* 當調用Lua調用函數時保存信息的結構體*/
              StkId base;
              const Instruction *savedpc;
            } l;
            struct {                       /*當調用C調用函數時保存信息的結構體*/
              int ctx;
              lua_CFunction k;
              ptrdiff_t old_errfunc;
              ptrdiff_t extra;
              lu_byte old_allowhook;
              lu_byte status;
            } c;
        } u;
    } CallInfo;


     從CallInfo的定義，可以知道它的成員變量func和top同樣指向數據棧的位置，但不同的是，它所關注的是函數調用時相關的位置。並且在用gdb調試有嵌入Lua的C代碼時，我們可以除了查看C中的調用棧信息外，還可以用上面的調用鏈表獲取完整的Lua調用鏈表，在鏈表中的每一個節點中，我們可以使用CallInfo中的成員變量func來獲取每一個lua函數所在的文件名和行號等調試信息。
-->


## 線程

線程作為 Lua 中一種基本的數據類型，代表獨立的執行線程 (independent threads of execution)，這也是實現協程 (coroutines) 的基礎，注意這裡的線程類型不要與操作系統線程混淆，Lua 中的線程類型是 Lua 虛擬機實現一種數據類型。

從 Lua 腳本來看，一個協程就是一個線程類型，準確來說，協程是一個線程外加一套良好的操作接口，比如：

{% highlight lua %}
local co = coroutine.create(function() print("hello world") end)
print(co)  --output: thread: 0038BEE0
{% endhighlight %}

從實現角度來看，一個線程類型數據就是一個 Lua 與 C 交互的棧，每個棧包含函數調用鏈和數據棧，還有獨立的調試鉤子和錯誤信息，線程類型數據與 table 數據類型類似，它也是需要 GC 來管理。

### 新建棧

為了加載執行 Lua 腳本，首先要調用 `lua_newstate()` 函數來初始化虛擬機，該函數在初始化虛擬機狀態的同時，還會創建整個虛擬機的第一個線程，也就是主線程，或者說是第一個交互棧。

在已經初始化的全局狀態中創建一個新的線程可以調用函數 `lua_newthread()`，聲明如下：

{% highlight c %}
lua_State *lua_newthread (lua_State *L);
{% endhighlight %}

創建一個線程就擁有一個獨立的執行棧了，但是它與其線程共用虛擬機的全局狀態；Lua 沒有提供 API 關閉或者銷燬一個線程，類似其它 GC 對象一樣，由虛擬機管理。

也就是說，一個 Lua 虛擬機只有一個全局的狀態，但可以包含多個執行環境 (或者說多個線程、交互棧，從腳本角度來說，也可以說是多個協程)。

### 源碼解析

全局狀態的結構體 `global_State` 的代碼如下：

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

一個 Lua 虛擬機只有一個全局的 `global_State`，在調用 `lua_newstate()` 時，會創建和初始化這個全局結構，這個全局結構管理著 Lua 中全局唯一的信息。

<!---
, 主要包括以下信息：
    lua_Alloc frealloc：虛擬機內存分配策略，可以在調用lua_newstate時指定參數，修改該策略，或者調用luaL_newstate函數使用默認的內存分配策略。也可以通過函數               lua_setallocf：來設置內存分配策略。
    stringtable strt：全局的字符串哈希表，即保存那些短字符串，使得整個虛擬機中短字符串只有一份實例。
    TValue l_registry：保存全局的註冊表，註冊表就是一個全局的table（即整個虛擬機中只有一個註冊表），它只能被C代碼訪問，通常，它用來保存那些需要在幾個模塊中共享的數據。比如通過luaL_newmetatable創建的元表就是放在全局的註冊表中。
    lua_CFunction panic：但出現無包含錯誤（unprotected errors）時，會調用這個函數。這個函數可以通過lua_atpanic來修改。
    UpVal uvhead:指向保存所有open upvalues雙向鏈表的頭部。
    struct Table *mt[LUA_NUMTAGS]：保存基本類型的元表，注意table和userdata都有自己的元表。
    struct lua_State *mainthread：指向主lua_State，或者說是主線程、主執行棧。Lua虛擬機在調用函數lua_newstate初始化全局狀態global_State時也會創建一個主線程，
當然根據需要也可以調用lua_newthread來創建新的線程，但是整個虛擬機，只有一個全局的狀態global_State。
-->

線程對應的數據結構 `lua_State` 的定義如下：

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

可以看到，`struct lua_State` 跟其它可回收的數據類型一樣，結構體帶用 `CommonHeader` 的頭，也即是說它也是 GC 回收的對象之一。

<!--
它主要包括以下成員信息：
lu_byte status：線程腳本的狀態，線程可能狀態如下（lua.h）：

    44 /* thread status */
    45 #define LUA_OK      0
    46 #define LUA_YIELD   1
    47 #define LUA_ERRRUN  2
    48 #define LUA_ERRSYNTAX   3
    49 #define LUA_ERRMEM  4
    50 #define LUA_ERRGCMM 5
    51 #define LUA_ERRERR  6

  global_State *l_G：指向全局狀態；

    其他成員主要是數據棧和函數調用棧相關的，這也是lua_State結構中主要信息。還有成員ptrdiff_t errfunc是錯誤處理函數相關，也就是每個調用棧都有獨立的錯誤處理函數，以及調試相關的lua_Hook hook成員等。
-->

## C 庫導入

導入全局性的庫到 Lua 中，這些庫由 C 實現:

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

每一個庫封裝了很多函數, 且每個庫都由庫名和 open 函數導入，以協程庫為例:

{% highlight c %}
{LUA_COLIBNAME, luaopen_coroutine},
{% endhighlight %}

通過看協程的庫的創建過程可以知道如何將 C 函數寫的庫導入 Lua ：

{% highlight c %}
/* 下面是協程庫的lua函數名和對應的C函數 */
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

/* 每個庫必須有的open函數，newlib的實現就是一個table */
LUAMOD_API int luaopen_coroutine (lua_State *L) {
	luaL_newlib(L, co_funcs);
	return 1;
}
{% endhighlight %}

單個 C 函數組成的庫的 open 函數裡, 調用的是 `luaL_newlib(L, co_funcs);` 函數，其實現如下：

{% highlight c %}
/*
 * 根據庫函數數組luaL_Reg的大小創建的table, 這裡的createtable()的實現就是在棧中創建
 * 一個哈希表, 表元素個數為sizeof(l)/sizeof((l)[0]) - 1
 */
#define luaL_newlibtable(L,l)  \
	lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1)

/* 庫的實現就是以l的大小創建了一個table */
#define luaL_newlib(L,l)  \
	(luaL_checkversion(L), luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))
{% endhighlight %}

如下是上面調用的 `luaL_setfuncs()` 函數的實現代碼, 由於當前的棧頂是剛才 newlibtable 出來的 table, 所以現在是將庫函數名 set 到 table 中;

<!--
    下面的實現可以看出, 每個函數創建的閉包前都先複製了一份閉包, 這樣所有的庫函數的閉包是一樣的;
    checkstack函數是檢查當前的棧空間是否足夠放得下nup個閉包元素;
    lua_pushvalue()就是將索引處的值複製到棧頂
-->

{% highlight c %}
/*
** set functions from list 'l' into table at top - 'nup'; each
** function gets the 'nup' elements at the top as upvalues.
** Returns with only the table at the stack.
*/
LUALIB_API void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
	/* nup是閉包元素的個數，如果空間不夠會自動擴展棧空間 */
	luaL_checkstack(L, nup, "too many upvalues");
	for (; l->name != NULL; l++) {  /* fill the table with given functions */
		int i;
		for (i = 0; i < nup; i++)  /* copy upvalues to the top */
			/* 壓入所有的閉包, 當前棧頂(新的table)下的元素是nup個的閉包 */
			lua_pushvalue(L, -nup);
		lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
		lua_setfield(L, -(nup + 2), l->name);
	}
	lua_pop(L, nup);  /* remove upvalues */
}
{% endhighlight %}

下面看下 `luaL_checkstack` 調用的 check 函數，ci 為當前棧中的調用的函數幀，可以看成函數的局部空間，ci->func 為底，ci->top 為空間頂，兩者之間就是當前函數的局部空間:

{% highlight c %}
/*  const int extra = LUA_MINSTACK;  5個額外的空間
 * 調用的是該: lua_checkstack(L, space + extra)) ..
 */
LUA_API int lua_checkstack (lua_State *L, int n) {
	int res;
	CallInfo *ci = L->ci;   /* 當前的函數調用幀, ci->func為函數調用點 */
	lua_lock(L);
	api_check(L, n >= 0, "negative 'n'");
	if (L->stack_last - L->top > n)  /* stack large enough? */
		res = 1;  /* yes; check is OK 空間足夠 */
	else {  /* no; need to grow stack 空間不夠,需要增加棧空間 */
		int inuse = cast_int(L->top - L->stack) + EXTRA_STACK;
		if (inuse > LUAI_MAXSTACK - n)  /* can grow without overflow? */
			res = 0;  /* no */
		else  /* try to grow stack */
			res = (luaD_rawrunprotected(L, &growstack, &n) == LUA_OK);
	}
	if (res && ci->top < L->top + n)
		ci->top = L->top + n;  /* adjust frame top 調用幀頂為棧頂+所需空間 */
	lua_unlock(L);

	return res;
}
{% endhighlight %}

前面的庫導入過程中 `luaL_requiref()` 是真正的導入函數。

<!--
,實現如下, 說明符中將該函數表示為簡化版的require, 完成了三件事:
    1. 在全局表的_LOADED中查找有沒有modulename對應的值;
    2. 沒有則壓入庫open函數和庫名, 並調用open函數在棧中創建table, 將該table以key為modulename存入_LOADED全局表中;
    3. 如果glb為真,即要求放入全局表中, 則放如全局, _G[modulename] = module

該函數的功能和require的功能類似, 都會檢查_loaded表, 沒有則導入到表中取, 一共使用;
-->

{% highlight c %}
/*
** Stripped-down 'require': After checking "loaded" table, calls 'openf'
** to open a module, registers the result in 'package.loaded' table and,
** if 'glb' is true, also registers the result in the global table.
** Leaves resulting module on the top.
*/
LUALIB_API void luaL_requiref (lua_State *L, const char *modname, lua_CFunction openf, int glb) {
	/* 全局註冊表找到_loaded表 */
	luaL_getsubtable(L, LUA_REGISTRYINDEX, "_LOADED");
	lua_getfield(L, -1, modname);  /* _LOADED[modname] */
	if (!lua_toboolean(L, -1)) {  /* package not already loaded? */
		lua_pop(L, 1);  /* remove field */
		lua_pushcfunction(L, openf);
		lua_pushstring(L, modname);  /* argument to open function */
		/* 調用庫的open函數,在棧中創建了一個table */
		lua_call(L, 1, 1);  /* call 'openf' to open module */
		/* 複製一份以保存到_loaded裡面取 */
		lua_pushvalue(L, -1);  /* make copy of module (call result) */
		lua_setfield(L, -3, modname);  /* _LOADED[modname] = module */
	}
	lua_remove(L, -2);  /* remove _LOADED table */
	if (glb) {
		/* 複製一份保存到_G裡面去 */
		lua_pushvalue(L, -1);  /* copy of module */
		lua_setglobal(L, modname);  /* _G[modname] = module */
	}
}
{% endhighlight %}

## API 簡介

Lua 和 C 之間的數據交互通過堆棧進行，棧中的數據通過索引值進行定位，從棧底向上是從 1 開始遞增的正整數，從棧頂向下是從 -1 開始遞減的負整數，棧的元素按照 FIFO 的規則進出。也就是說，棧頂是 `-1`，棧底是 `1`，其中第 1 個入棧的在棧底。

{% highlight text %}
lua_State* luaL_newstate();
    腳本編譯執行相互獨立，該函數申請一個虛擬機，後續的API都以此指針作為第一個參數。
void lua_close(lua_State *L);
    清理狀態機中所有對象。

lua_State* lua_newthread(lua_State *L)

int lua_gettop(lua_State *L);
    取得棧的高度。
void lua_settop(lua_State *L, int idx);
    設置棧的高度，如果之前的棧頂比新設置的更高，那麼高出來的元素會被丟棄，反之壓入nil來補足大小。

void lua_pushvalue(lua_State *L, int idx);
    將指定索引上值的副本壓入棧。
        for (int i = 1; i <= 3; ++i)
            lua_pushnumber(i);
        // bottom->top 1 2 3
        lua_pushvalue(L, 2)
        // bottom->top 1 2 3 2
void lua_remove(lua_State *L, int idx);
    刪除指定索引上的元素，並將該位置之上的所有元素下移以補空缺。
        for (int i = 1; i <= 3; ++i)
            lua_pushnumber(i);
        // bottom->top 1 2 3
        lua_remove(L, 2)
        // bottom->top 1 3
void lua_insert(lua_State *L, int idx);
    移指定位置上的所有元素以開闢一個空間槽的空間，然後將棧頂元素移到該位置。
        for (int i = 1; i <= 5; ++i)
            lua_pushnumber(i);
        // bottom->top 1 2 3 4 5
        lua_insert(L, 3)
        // bottom->top 1 2 5 4 3
void lua_replace(lua_State *L, int idx);
    彈出棧頂的值，並將該值設置到指定索引上，但它不會移動任何東西。
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
