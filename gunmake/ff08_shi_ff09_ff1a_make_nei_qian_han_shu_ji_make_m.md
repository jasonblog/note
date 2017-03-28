# （十）：make內嵌函數及make命令顯示


這一節我們講一下make的函數，在之前的章節已經講到了幾個函數：wildcard、patsubst、notdir、shell等。一般函數的調用格式如下：

$(funcname arguments)

或

$(funcname arguments)

其中funcname是需要調用函數的函數名稱，應該是make內嵌函數；arguments是函數參數，參數和函數名之間使用空格分割，如果存在多個參數時，參數之間使用逗號“,”分開。函數調用以“$”開頭，使用成對的圓括號或花括號把函數名和參數括起，一般使用圓括號。

下面來看一下常用的一些函數：

`1.`    獲取匹配模式文件名函數—wildcard 。<br>
用法：$(wildcard PATTERN)
該函數會列出當前目錄下所有符合模式“PATTERN”格式的文件名。返回空格分割的、存在當前目錄下的所有符合模式“PATTERN”的文件名。
例如：


```sh
SRC_FILES = $(wildcard src/*.c)  
```

返回src目錄下所有.c文件列表。

`2.`    字符串替換函數—subst。<br>
用法：$(subst FROM,TO,TEXT)
該函數把字串“TEXT”中的“FROM”字符替換為“TO”，返回替換後的新字符串。

`3.`    模式替換函數—patsubst。<br>
用法：$(patsubst PATTERN,REPLACEMENT,TEXT)

該函數搜索“TEXT”中以空格分開的單詞，將符合模式“TATTERN”替換為“REPLACEMENT” 。參數“PATTERN”中可以使用模式通配符“%”，來代表一個單詞中的若干字符。如果參數“REPLACEMENT”中也包含一個“%” ，那麼“REPLACEMENT”中的“%”將是“TATTERN”中的那個“%”所代表的字符串。

例如：

```sh
SRC_OBJ = $(patsubst %.c, %.o, $(SRC_FILES))
```

將SRC_FILES中所有.c文件替換為.o返回給變量SRC_OBJ。<br>
此函數功能類似之前講過的變量替換，http://blog.csdn.net/shallnet/article/details/37529935
變量替換格式是“$(var:a=b)”或“${var:a=b}”，其意思是，把變量“var”中所有以“a”字串“結尾”的“a”替換成“b”字串。<br>
例如我們存在一個代表所有.c 文件的變量。定義為“src_files = a.c b.c c.c” 。<br>
為了得到這些.c文件所對應的.o源文件。如下兩種使用可以得到同一種結果：

```sh
$(objects:.c=.o)  
$(patsubst %.c,%.o,$( src_files))  
```

`4.`    過濾函數—filter。<br>
用法：$(filter PATTERN…,TEXT)
該函數過濾掉字串“TEXT”中所有不符合模式“PATTERN”的單詞，保留所有符合此模式的單詞。可以使用多個模式。模式中一般需要包含模式字符“%” 。存在多個模式時，模式表達式之間使用空格分割。返回空格分割的“TEXT”字串中所有符合模式“PATTERN”的字串。

`5.`    反過濾函數—filter-out。<br>
用法：$(filter-out PATTERN...,TEXT)
和“filter”函數實現的功能相反。過濾掉字串“TEXT”中所有符合模式“PATTERN” 的單詞， 保留所有不符合此模式的單詞。 可以有多個模式。存在多個模式時，模式表達式之間使用空格分割。

`6.`    取目錄函數—dir。<br>
用法：$(dir NAMES…)
從文件名序列“NAMES…”中取出各個文件名的目錄部分。文件名的目錄部分就是包含在文件名中的最後一個斜線（ “/” ） （包括斜線）之前的部分。返回空格分割的文件名序列“NAMES…”中每一個文件的目錄部分。如果文件名中沒有斜線，認為此文件為當前目錄（ “./” ）下的文件。

`7.`    取文件名函數——notdir。<br>
用法：$(notdir NAMES…)
從文件名序列“NAMES…”中取出非目錄部分。目錄部分是指最後一個斜線（ “/” ） （包括斜線）之前的部分。刪除所有文件名中的目錄部分，只保留非目錄部分。文件名序列“NAMES…”中每一個文件的非目錄部分。

`8.`    取後綴函數—suffix。<br>
用法：$(suffix NAMES…) 
函數從文件名序列“NAMES…”中取出各個文件名的後綴。後綴是文件名中最後一個以點“.”開始的（包含點號）部分，如果文件名中不包含一個點號，則為空。 返回以空格分割的文件名序列“NAMES…”中每一個文件的後綴序列。

`9.`    取前綴函數—basename<br>
用法：$(basename NAMES…)
從文件名序列“NAMES…”中取出各個文件名的前綴部分（點號之後的部分） 。前綴部分指的是文件名中最後一個點號之前的部分。 返回空格分割的文件名序列“NAMES…”中各個文件的前綴序列。如果文件沒有前綴，則返回空字串。

這裡僅僅講到一些常用的函數，還有一些函數沒有講到，用到的時候可以去翻翻makefile手冊。

通常情況下make在編譯時會打印出當前正在執行的命令，當編譯鏈接選項很長時，會輸出很多東西在屏幕上，如果我 不想再屏幕上看到很多東西，我們可以在命令前面加上@，這樣命令就不會輸出到屏幕了。我們這樣嘗試修改下：


```sh
# make  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/ipc'  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/ipc'  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/tools'  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/tools'  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/main'  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/main'  
```

發現只有進入目錄和退出目錄的顯示，這樣很難知道目前編譯過程。其實我們可以在規則命令處加入一行類似打印：

```sh
@echo "do something......"
```

這樣可以輸出目前正在做的事，又不會輸出正在執行命令。現在將規則修改下如下：

```sh
$(OBJDIR) :  
>---@echo "   MKDIR $(notdir $@)..."  
>---@mkdir -p $@  
  
ifneq ($(SRC_BIN),)  
$(BINDIR)/$(SRC_BIN) : $(SRC_OBJ)  
>---@echo "   LINK $(notdir $@)..."  
>---@$(CC) -o $@ $^ $(LDFLAGS)  
endif  
  
ifneq ($(SRC_LIB),)  
$(LIBDIR)/$(SRC_LIB) : $(SRC_OBJ)  
>---@echo "    ARCHIVE $(notdir $@)..."  
>---@$(AR) rcs $@ $^  
>---@echo "    COPY $@ to $(SRC_BASE)/libs"  
>---@cp $@ $(SRC_BASE)/libs  
endif  
  
$(SRC_OBJ) : $(OBJDIR)/%.o : %.c  
>---@echo "    COMPILE $(notdir $<)..."  
>---@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@  
```

編譯輸出如下：

```sh
# make  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/ipc'  
    COMPILE ipc.c...  
    ARCHIVE libipc.a...  
    COPY ../../build/unix_dbg/lib/libipc.a to ../../libs  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/ipc'  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/tools'  
    COMPILE base64.c...  
    COMPILE md5.c...  
    COMPILE tools.c...  
    ARCHIVE libtools.a...  
    COPY ../../build/unix_dbg/lib/libtools.a to ../../libs  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/tools'  
make[1]: Entering directory `/home/Myprojects/example_make/version-3.1/src/main'  
    COMPILE main.c...  
   LINK target_bin...  
make[1]: Leaving directory `/home/Myprojects/example_make/version-3.1/src/main'  
```

其中目錄切換的輸出仍然很多，我們可以將其關閉，這需要使用到make的參數，在make -C是指定--no-print- 
directory參數。我們將頂層目錄下Makefile規則修改如下：

```sh
$(BUILDDIR):  
>---@echo "    Create directory $@ ..."  
>---mkdir -p $(BUILDDIR)/bin $(BUILDDIR)/lib  
  
$(MODULES):  
>---@$(MAKE) -C $(DIR)/$@ MODULE=$@ --no-print-directory  
  
main:tools ipc  
  
clean :  
>---@for subdir in $(MODULES); \  
>---do $(MAKE) -C $(DIR)/$$subdir MODULE=$$subdir $@ --no-print-directory; \  
>---done  
編譯輸出：  
# make  
    COMPILE ipc.c...  
    ARCHIVE libipc.a...  
    COPY ../../build/unix_dbg/lib/libipc.a to ../../libs  
    COMPILE base64.c...  
    COMPILE md5.c...  
    COMPILE tools.c...  
    ARCHIVE libtools.a...  
    COPY ../../build/unix_dbg/lib/libtools.a to ../../libs  
    COMPILE main.c...  
   LINK target_bin...  
# make clean  
rm -f  ../../build/unix_dbg/obj/ipc/ipc.o ../../build/unix_dbg/lib/libipc.a  
rm -f  ../../build/unix_dbg/obj/main/main.o ../../build/unix_dbg/bin/target_bin  
rm -f  ../../build/unix_dbg/obj/tools/base64.o  ../../build/unix_dbg/obj/tools/md5.o     
../../build/unix_dbg/obj/tools/tools.o ../../build/unix_dbg/lib/libtools.a  
```

這樣看上去輸出清爽多了。其實我們也可以使用make -s 來全面禁止命令的顯示。



