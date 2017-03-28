# （三）：變量的使用


仔細研究我們的之前Makefile發現，我們還有改進的地方，就是此處：


```sh
target_bin : main.o debug.o ipc.o timer.o tools.o  
>---gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
```

如果增加一個源文件xx.c的話，需要在兩處或多處增加xx.o文件。我們可以使用變量來解決這個問題。之前說過，Makefile的變量就像C語言的宏一樣，使用時在其位置上直接展開。變量在聲明時賦予初值，在引用變量時需要給在變量名前加上“$”符號，但最好用小括號“（）”或是大括號“{}”把變量給包括起來。

默認目標target_bin也在多處出現了，該文件也可以使用變量代替。
修改我們的Makefile如下：

```sh
SRC_OBJ = main.o debug.o ipc.o timer.o tools.o  
SRC_BIN = target_bin  
$(SRC_BIN) : $(SRC_OBJ)  
>---gcc -o $(SRC_BIN) $(SRC_OBJ)  
  
clean:  
>---rm $(SRC_OBJ) $(SRC_BIN)  
```

這樣每次有新增的文件是隻需要在SRC_OBJ變量裡面增加一個文件即可。要修改最終目標的名字是可以只修改變量SRC_BIN。
其實在之前還說過特殊變量：

```sh
$@，表示規則中的目標。
$<，表示規則中的第一個依賴文件。
$?，表示規則中所有比目標新的條件，組成一個列表，以空格分隔。
$^，表示規則中的所有條件，組成一個列表，以空格分隔。
```

上一節我們看到make -p有很多自定義的變量，比如CC。其中很多變量我們可以直接使用或修改其變量值或增加值。我們的Makefile中可以使用CC（默認值為cc）、RM（默認值為rm -f）。
 
由此可見我們的Makefile還可以進一步修改：

```sh
SRC_OBJ = main.o debug.o ipc.o timer.o tools.o  
SRC_BIN = target_bin  
$(SRC_BIN) : $(SRC_OBJ)  
>---$(CC) -o $@ $^

clean:  
>---$(RM) $(SRC_OBJ) $(SRC_BIN)  
```

這樣的Makefile編譯也是可用的。

但是這樣的Makefile還是需要我們手動添加文件，還是不夠自動化，最好增刪文件都要修改Makefile。偉大的人類真是太懶了！！於是乎，他們發明了一個函數wilcard（函數後面會講到），它可以用來獲取指定目錄下的所有的.c文件列表。這樣的話我們可以自動獲取當前目錄下所有.c源文件，然後通過其他方法再得到.o文件列表，這樣的話就不需要在每次增刪文件時去修改Makefile了。所謂其他方法這裡給出兩種：

1.     使用patsubst函數。在$(patsubst %.c,%.o,$(dir) )中，patsubst把$(dir)中的變量符合後綴是.c的全部替換成.o。

2.     變量值的替換。 我們可以替換變量中的共有的部分，其格式是“$(var:a=b)”或“${var:a=b}”，其意思是，把變量“var”中所有以“a”字串“結尾”的“a”替換成“b”字串。


修改後的Makefile如下：

```sh
# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
SRC = $(wildcard *.c)  
SRC_OBJ = $(SRC:.c=.o)  
SRC_BIN = target_bin  
   
$(SRC_BIN) : $(SRC_OBJ)  
>---$(CC) -o $@ $^  
   
clean:  
>---$(RM) $(SRC_OBJ) $(SRC_BIN)  
```

其中# 後面的內容為註釋。
這樣終於滿足了那些懶人的想法了。可見在使用變量時，的確可以是編譯變得更自動化。
 
其實變量的定義有三種運算符=、:=、?=、+=。

`1.`     =運算符可以讀取到後面定義的變量。比如：

```sh
VAR = $(VAR2)  
VAR2 = hello_make  
   
all:  
>---@echo =====$(VAR)=====  
```

運行結果為：

```sh
#  
=====hello_make=====  
#  
```

但是這種定義可能會導致並非我們意願的事發生，並不是很符合C語言的編程習慣。

`2.`     :=運算符在遇到變量定義時立即展開。

```sh
VAR := $(VAR2)                                                                                                                                                                                          
VAR2 = hello_make  
   
all:  
>---@echo =====$(VAR)=====  
```

運行結果為：

```sh
#  
==========  
# 
```

`3.`     ?=運算符在複製之前先做判斷變量是否已經存在。例如var1 ?= $(var2)的意思是：如果var1沒有定義過，那麼?=相當於=，如果var1先前已經定義了，則什麼也不做，不會給var重新賦值。


`4.`     +=運算符是給變了追加值。如果變量還沒有定義過就直接用+=賦值，那麼+=相當於=
 
如何使用這幾個運算符要看實際情況，有時一個大的工程可能有許多Makefile組成，變量可能在多個Makefile中都在使用，這時可能使用+=比較好。使用:=有時可能比要好。

有時在編譯程序時，我們需要編譯器給出警告，或加入調試信息，或告知編譯器優化可執行文件。編譯時C編譯器的選項CFLAGS使用的較多，默認沒有提供值，我們可以給該變量賦值。有時我們還需要使用鏈接器選項LFLAGS告訴鏈接器鏈接時需要的庫文件。可能我們還需要給出包含頭文件的路徑，因為頭文件很可能和源文件不再同一目錄。所以，我們今天的Makefile加上部分註釋又更新了：


```sh
# A commonMakefile for c programs, version 1.0  
# Copyright (C)2014 shallnew \at 163 \dot com  
   
CFLAGS += -g -Wall-Werror -O2  
CPPFLAGS += -I.-I./inc                                                                                                                                                                    
LDFLAGS +=-lpthread  
   
# SRC_OBJ =$(patsubst %.c, %.o, $(wildcard *.c))  
SRC_FILES =$(wildcard *.c)  
SRC_OBJ =$(SRC_FILES:.c=.o)  
SRC_BIN =target_bin  
   
$(SRC_BIN) :$(SRC_OBJ)  
>---$(CC) -o $@$^ $(LDFLAGS)  
   
clean:  
>---$(RM)$(SRC_OBJ) $(SRC_BIN)  
```

編譯：


```sh
# make  
cc -g -Wall-Werror -O2 -I. -I./inc  -c -o debug.odebug.c  
cc -g -Wall-Werror -O2 -I. -I./inc  -c -o ipc.oipc.c  
cc -g -Wall-Werror -O2 -I. -I./inc  -c -o main.omain.c  
cc -g -Wall-Werror -O2 -I. -I./inc  -c -o timer.otimer.c  
cc -g -Wall-Werror -O2 -I. -I./inc  -c -o tools.otools.c  
cc -o target_bindebug.o ipc.o main.o timer.o tools.o -lpthread  
```

可見我們的預編譯選項，編譯選項都用到了，之前我們說過make的使用隱含規則自動推導：

```sh
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) –c  
```

其中變量CFLAGS 和 CPPFLAGS均是我們給出的，變量$(TARGET_ARCH)未給，所以在編譯輸出可以看到-c前面有2個空，最早未給變量是有四個空。

目前給出的Makefile基本上可以適用於那些源代碼全部在同一目錄下的簡單項目，並且基本上在增刪文件時不需要再去手動修改Makefile代碼。在新的一個項目只需要把該Makefile拷貝到源代碼目錄下，再修改一下你需要編譯的可執行文件名稱以及你需要的編譯連接選項即可。

後面章節將會講到如何寫多目錄源代碼工程下的Makefile。

最後，今天的最終Makefile是這樣的：


```sh
# A commonMakefile for c programs, version 1.0  
# Copyright (C)2014 shallnew \at 163 \dot com  
   
CFLAGS += -g -Wall-Werror -O2  
CPPFLAGS += -I.-I./inc                                                                                                                                                                    
LDFLAGS +=-lpthread  
   
# SRC_OBJ =$(patsubst %.c, %.o, $(wildcard *.c))  
SRC_FILES =$(wildcard *.c)  
SRC_OBJ =$(SRC_FILES:.c=.o)  
SRC_BIN =target_bin  
   
$(SRC_BIN) :$(SRC_OBJ)  
>---$(CC) -o $@$^ $(LDFLAGS)  
   
clean:  
>---$(RM)$(SRC_OBJ) $(SRC_BIN)  
```