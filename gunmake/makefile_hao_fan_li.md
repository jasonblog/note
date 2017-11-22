# Makefile 好範例


自動化變量

```
$@	表示規則中的目標。
$<	表示規則中的第一個條件。
$?	表示規則中所有比目標新的條件，組成一個列表，以空格分隔。
$^	表示規則中的所有條件，組成一個列表，以空格分隔。
```

###示例1


```sh
main: main.o stack.o maze.o
    gcc main.o stack.o maze.o -o main
```

可以改寫成：


```sh
main: main.o stack.o maze.o
    gcc $^ -o $@
```

###示例2

有時候希望只對更新過的條件進行操作，例如有一個庫文件libsome.a依賴於幾個目標文件：


```sh
libsome.a: foo.o bar.o lose.o win.o 
    ar r libsome.a $?
    ranlib libsome.a
```

##常用變量

```sh
變量名          說明
AR              靜態庫打包命令的名字，缺省值是ar。
ARFLAGS         靜態庫打包命令的選項，缺省值是rv。
AS              彙編器的名字，缺省值是as。
ASFLAGS         彙編器的選項，沒有定義。
CC              C編譯器的名字，缺省值是cc。
CFLAGS          C編譯器的選項，沒有定義。
CXX             C++編譯器的名字，缺省值是g++。
CXXFLAGS        C++編譯器的選項，沒有定義。
CPP             C預處理器的名字，缺省值是$(CC) -E。
CPPFLAGS        C預處理器的選項，沒有定義。
LD              鏈接器的名字，缺省值是ld。
LDFLAGS         鏈接器的選項，沒有定義。
TARGET          _ARCH 和目標平臺相關的命令行選項，沒有定義。
OUTPUT          _OPTION 輸出的命令行選項，缺省值是-o $@。
LINK.o          把.o文件鏈接在一起的命令行，缺省值是$(CC) $(LDFLAGS) $(TARGET_ARCH)。
LINK.c          把.c文件鏈接在一起的命令行，缺省值是$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)。
LINK.cc         把.cc文件（C++源文件）鏈接在一起的命令行，缺省值是$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH)。
COMPILE.c       編譯.c文件的命令行，缺省值是$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c。
COMPILE.cc      編譯.cc文件的命令行，缺省值是$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c。
RM              刪除命令的名字，缺省值是rm -f。
```


##函數
###函數的調用語法
函數調用，很像變量的使用，也是以“$”來標識的，其語法如下:


```sh
$(<function> <arguments>)
```

或是

```sh
${<function> <arguments>}
```

這裡，`<function>`就是函數名，make 支持的函數不多。`<arguments>`是函數的參數，參數間以逗號“，”分隔，而函數名和參數之間以“空格”分隔。函數調用以 “$” 開頭，以 圓括號或花括號 把函數名和參數括起。感覺很像一個變量，是不是?函數中的參數可以使用變量，為了風格的統一，函數和變量的括號最好一樣，如使用 `$(subst a,b,$(x))` 這樣的形式，而不是 `$(subst a,b,${x})`的形式。因為統一會更清楚，也會減少一些不必要的麻煩。


還是來看一個示例:

```sh
comma:= ,
empty:=
space:= $(empty) $(empty)
foo:= a b c
bar:= $(subst $(space),$(comma),$(foo))
```

在這個示例中，$(comma)的值是一個逗號。$(space)使用了$(empty)定義了一個空格，$(foo)的值是“a b c”，$(bar)的定義用，調用了函數“subst”，這是一個替換函數，這個函數有三個參數，第一個參數是被替換字串，第二個參數是替換字串，第三個參數是替換操作作用的字串。這個函數也就是把$(foo)中的空格替換成逗號，所以$(bar)的值是“a,b,c”。


###字符串處理函數
####subst

```sh
$(subst <from>,<to>,<text>)
```

- 名稱:字符串替換函數——subst。
- 功能:把字串<text>中的<from>字符串替換成<to>。
- 返回:函數返回被替換過後的字符串。
- 示例: $(subst ee,EE,feet on the street) 把“feet on the street”中的“ee”替換成“EE”，返回結果是“fEEt on the strEEt”。
- 

#### patsubst

```sh
$(patsubst <pattern>,<replacement>,<text>)
```

- 名稱:模式字符串替換函數——patsubst。

- 功能:查找`<text>`中的單詞(單詞以“空格”、“Tab”或“回車”“換行”分隔)是否符合模式`<pattern>`，如果匹配的話，則以`<replacement>`替換。這裡，`<pattern>`可以包括通配符%，表示任意長度的字串。如果`<replacement>`中也包含%，那麼，`<replacement>`中的這個%`<pattern>`中的那個%所代表的字串。(可以用“\”來轉義，以\%來表示真實含義的%字符)

- 返回:函數返回被替換過後的字符串。

- 示例:` $(patsubst %.c,%.o,x.c.c bar.c) ` 把字串“x.c.c bar.c”符合模式[%.c]的單詞替換成[%.o]，返回結果是“x.c.o bar.o”

- 備註: 這和我們前面“變量章節”說過的相關知識有點相似。

如:

`$(var:<pattern>=<replacement>)` 相當於 `(patsubst <pattern>,<replacement>,$(var))`

而 `$(var: <suffix>=<replacement>)` 則相當於 `$(patsubst %<suffix>,%<replacement>,$(var))` 。

例如有: `objects = foo.o bar.o baz.`

那麼， `$(objects:.o.c)=` 和 `$(patsubst %.o,%.c,$(objects))` 是一樣的。


#### strip

```sh
$(strip <string>)
```

- 名稱:去空格函數——strip。
- 功能:去掉`<string>`字串中開頭和結尾的空字符。
- 返回:返回被去掉空格的字符串值。
- 例： `$(strip a b c )` 把字串“a b c ”去到開頭和結尾的空格，


#### findstring
```sh
$(findstring <find>,<in>)
```

- 名稱:查找字符串函數——findstring。
- 功能:在字串<in>中查找<find>字串。
- 返回:如果找到，那麼返回<find>，否則返回空字符串。
- 示例:

```sh
$(findstring a,a b c)
$(findstring a,b c)
```

```sh
第一個函數返回“a”字符串，第二個返回“”字符串(空字符串)
```

#### filter

```sh
$(filter <pattern...>,<text>)
```

- 名稱:過濾函數——filter。
- 功能:以`<pattern>`模式過濾`<text>`字符串中的單詞， 保留  符合模式`<pattern>`的單詞。可以有多個模式。
- 返回:返回符合模式`<pattern>`的字串。
- 示例:



