# （八）：模式規則


上一節講到目錄創建成功，目標文件沒有生產到對應目錄下，這裡我們先給目標文件加上對應目錄，這樣的話產生對應的目標文件會直接生成到對應目錄。我們先給庫文件目標和可執行文件目標加上路徑，如下：

```sh
lib : $(OBJDIR) $(LIBDIR)/$(SRC_LIB)  
  
bin : $(OBJDIR) $(BINDIR)/$(SRC_BIN)  
  
$(OBJDIR) :  
>---@echo "   MKDIR $(notdir $@)..."  
>---@mkdir -p $@  
  
ifneq ($(SRC_BIN),)  
$(BINDIR)/$(SRC_BIN) : $(SRC_OBJ)  
>---$(CC) -o $@ $^ $(LDFLAGS)  
endif  
  
ifneq ($(SRC_LIB),)  
$(LIBDIR)/$(SRC_LIB) : $(SRC_OBJ)  
>---$(AR) rcs $@ $^  
>---cp $@ $(SRC_BASE)/libs  
endif  
```

此時再執行make，完成後查看build目錄樹：


```sh
build/  
└── unix_dbg  
    ├── bin  
    │   └── target_bin  
    ├── lib  
    │   ├── libipc.a  
    │   └── libtools.a  
    └── obj  
        ├── ipc  
        ├── main  
        └── tools  
```

可以看到，生成的目標是在對應目錄下。我們乘勝追擊，把.o文件也將其修改了。我們之前的每個模塊Makefile大致是這樣寫的：


```sh
SRC_BASE = ../..
CFLAGS +=
CPPFLAGS += -I. -I./inc -I$(SRC_BASE)/include
# SRC_OBJ = $(patsubst %.c, %.o, $(wildcard *.c))
SRC_FILES = $(wildcard src/*.c)
SRC_OBJ = $(SRC_FILES:.c=.o)
SRC_LIB = xx.a
include $(SRC_BASE)/Makefile.rule  
```

其中SRC_OBJ在此處給出，然後再在Makefile.rule中使用，此處的.o文件會在.c文件相同目錄下生成，所以我們現在需要將.o文件加上路徑，由於取得路徑是在Makefile.rule裡面，所以我們可以統一在Makefile.rule裡面給變量SRC_OBJ賦值，大致如下：

SRC_OBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(SRC_FILES)))                                                                                                                          
這裡用到函數patsubst、notdir，關於函數會在後面講到。這樣.o文件作為目標生成之後就會生成到相應目錄裡面了。
此時再編譯：


```sh
# make  
make[1]: Entering directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make[1]: *** No rule to make target `../../build/unix_dbg/obj/ipc/ipc.o', needed by `../../build/unix_dbg/lib/libipc.a'.  Stop.  
make[1]: Leaving directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make: *** [ipc] Error 2  
```

發現出錯了，並且是在生成目標文件ipc.o時沒有成功，查看build目錄樹也沒有生成.o文件。為什麼會生成失敗呢？

我們沒有給出生成.o目標的規則，之前可以生成是因為make有通過隱含規則來自動推導的能力（這個之前有講到，鏈接過去）。在我們沒有修改之前，生成.o通過隱含規則來完成：


```sh
%.o: %.c  
#  commands to execute (built-in):  
>---$(COMPILE.c) $(OUTPUT_OPTION) $<  
```

因為所有的.o目標符合該規則，所以會自動推導生成.o文件。我們現在在..o前面加上路徑後沒有符合生成.o的隱含模式規則了，所以就沒有生成該文件，導致編譯出錯。那怎麼辦呢？沒有隱含模式規則，我們可以自己寫符合生成該目標的模式規則。

模式規則類似於普通規則，只是在模式規則中，目標文件是一個帶有模式字符“%”的文件，使用模式來匹配目標文件。在目標文件名中“%”匹配的部分稱為“莖”。使用模式規則時，目標文件匹配之後得到“莖”，依賴根據“莖”產生對應的依賴文件，這個依賴文件必須是存在的或者可被創建的。

所以，我們增加一條模式規則如下：

```sh
$(OBJDIR)/%.o : %.c  
>---$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@  
```

該模式規則中目標文件是$(OBJDIR)/%.o，那麼現在有了符合生成我們需要的.o文件的規則了，編譯一下：

```sh
# make  
make[1]: Entering directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make[1]: *** No rule to make target `../../build/unix_dbg/obj/ipc/ipc.o', needed by `../../build/unix_dbg/lib/libipc.a'.  Stop.  
make[1]: Leaving directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make: *** [ipc] Error 2  
```
發現還是不對，不是已經增加了模式規則了嗎，為何還是沒有生成.o文件。

我們這裡先說說靜態模式規則：

一個規則中可以有多個目標，規則所定義的命令對所有的目標有效。一個具有多目標的規則相當於多個規則。 規則的命令對不同的目標的執行效果不同， 因為在規則的命令中可能使用了自動化變量 “$@” 。 多目標規則意味著所有的目標具有相同的依賴文件。多目標通常用在以下兩種情況：雖然在多目標的規則中， 可以根據不同的目標使用不同的命令 （在命令行中使用自動化變量 “$@” ）。但是， 多目標的規則並不能做到根據目標文件自動改變依賴文件 （像上邊例子中使用自動化變量“$@”改變規則的命令一樣） 。需要實現這個目的是，要用到make的靜態模式。

靜態模式規則是這樣一個規則：規則存在多個目標， 並且不同的目標可以根據目標文件的名字來自動構造出依賴文件。靜態模式規則比多目標規則更通用， 它不需要多個目標具有相同的依賴。但是靜態模式規則中的依賴文件必須是相類似的而不是完全相同
的。靜態模式規則語法如下：


```sh
<targets ...>: <target-pattern>: <prereq-patterns ...>    
<commands>    
....  
```

比如下面是一個靜態模式規則：

```sh
objects = foo.o bar.o  
  
all: $(objects)  
  
$(objects): %.o: %.c  
$(CC) -c $(CFLAGS) $< -o $@  
```

該規則描述了所有的.o文件的依賴文件為對應的.c文件，對於目標“foo.o” ，取其莖“foo”替代對應的依賴模式“%.c”中的模式字符“%”之後可得到目標的依賴文件“foo.c”。這就是目標“foo.o”的依賴關係“foo.o: foo.c”，規則的命令行描述瞭如何完成由“foo.c”編譯生成目標“foo.o” 。命令行中“$<”和“$@”是自動化變量，“$<” 表示規則中的第一個依賴文件， “$@” 表示規則中的目標文件。上邊的這個規則描述了以下兩個具體的規則：


```sh
foo.o : foo.c  
>---$(CC) -c $(CFLAGS) foo.c -o foo.o  
bar.o : bar.c  
>---$(CC) -c $(CFLAGS) bar.c -o bar.o  
```

那靜態模式規則和普通的模式規則（非靜態模式規則）有什麼去區別呢？兩者都是用目標模式和依賴模式來構建目標的規則中的文件依賴關係，兩者不同的地方是 make 在執行時使用它們的時機。

靜態模式規則只能用在規則中明確指出的那些文件的重建過程中。不能用在除此之外的任何文件的重建過程中，並且它對指定的每一個目標來說是唯一的。如果一個目標存在於兩個規則，並且這兩個規則都定義了命令， make 執行時就會提示錯誤。

非靜態模式規則可被用在任何和它相匹配的目標上，當一個目標文件同時符合多個目標模式時，make將會把第一個目標匹配的模式規則作為重建它的規則。

那有沒有想過如果我們指定了模式規則後，那還有隱含規則呢，那怎麼選擇執行哪一個模式規則呢？Makefile中明確指定的模式規則會覆蓋隱含模式規則。就是說如果在Makefile中出現了一個對目標文件合適可用的模式規則，那麼make就不會再為這個目標文件尋找其它隱含規則，而直接使用在Makefile中出現的這個規則。在使用時，明確規則永遠優先於隱含規則。


我們繼續說之前的那個問題，我們定義了模式規則後還是沒有生成.o文件，我們現在將其改為靜態規則再試試就看，如下：

```sh
$(SRC_OBJ) : $(OBJDIR)/%.o : %.c
>---$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@  
```
執行後：

```sh
# make  
make[1]: Entering directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make[1]: *** No rule to make target `ipc.c', needed by `../../build/unix_dbg/obj/ipc/ipc.o'.  Stop.  
make[1]: Leaving directory `/home/Myprojects/example_make/version-2.9/src/ipc'  
make: *** [ipc] Error 2  
```

發現提示沒有文件ipc.c，這說明沒有生成.o的原因是沒有.c文件，我很好奇的是為何使用非靜態模式為何不提示呢？（還沒搞懂，再研究研究，知道的可以給個提示哈~~）

缺少依賴文件，為何沒有*.c文件，仔細想想我們的.o文件沒有和.c文件在同一目錄。在我們工程中，將源代碼和二進制文件（.o 文件和可執行文件）安排在不同的目錄來進行區分管理。這種情況下，我們可以使用 make 提供的目錄搜索依賴文件功能。該功能在下一節講述，這一節說的夠多了，有點累了。可惜最終還是沒有給出一個可用的Makefile，在下一節將會給出。



