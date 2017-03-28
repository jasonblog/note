# （二）:隱含規則自動推導


上一節的Makefile勉強可用，但還寫的比較繁瑣，不夠簡潔。對每一個.c源文件，都需要寫一個生成其對應的.o目標文件的規則，如果有幾百個或上千個源文件，都手動來寫，還不是很麻煩，這也不夠自動化啊。
這樣，我們把生成.o目標文件的規則全部刪除掉，就是這樣一個Makefile文件：


```sh

target_bin : main.o debug.o ipc.o timer.o tools.o  
>---gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
   
clean:  
>---rm *.o target_bin  
```

這下簡潔了不少，這樣也能用嗎？試試看吧先，make一下：


```sh
# make  
cc    -c -o main.o main.c  
cc    -c -o debug.o debug.c  
cc    -c -o ipc.o ipc.c  
cc    -c -o timer.o timer.c  
cc    -c -o tools.o tools.c  
gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
```

原來醬紫都可以啊！！target_bin後面那一群依賴文件怎麼生成呢？不是沒有生成*.o目標文件的規則了嗎？再看屏幕編譯輸出內容：

```sh
cc    -c -o main.o main.c  
cc    -c -o debug.o debug.c  
cc    -c -o ipc.o ipc.c  
cc    -c -o timer.o timer.c  
cc    -c -o tools.o tools.c 
```

怎麼長的和之前不太一樣呢，尤其是前面那個cc是何物？

其實make可以自動推導文件以及文件依賴關係後面的命令，於是我們就沒必要去在每一個*.o文件後都寫上類似的命令，因為，我們的 make 會自動推導依賴文件，並根據隱含規則自己推導命令。所以上面.o文件是由於make自動推導出的依賴文件以及命令來生成的。

下面來看看make是如何推導的。

命令make –p可以打印出很多默認變量和隱含規則。Makefile變量可以理解為C語言的宏，直接展開即可（後面會講到）。取出我們關心的部分：


```sh
# default  
OUTPUT_OPTION = -o $@  
# default  
CC = cc  
# default  
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) –c  
# Implicit Rules  
%.o: %.c  
#  commands to execute (built-in):  
>---$(COMPILE.c) $(OUTPUT_OPTION) $<  
```

其中cc是一個符號鏈接，指向gcc，這就可以解釋為什麼我們看到的編譯輸出為cc，其實還是使用gcc在編譯。

```sh
# ll /usr/bin/cc    
lrwxrwxrwx. 1 root root 3 Dec  3  2013 /usr/bin/cc -> gcc  
```

變量$(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH)都為空。所以%.o: %.c規則命令展開為


```sh
cc    -c -o $@ $<  
```

再看屏幕輸出編譯內容，摘取一條：

```sh
cc    -c -o main.o main.c  
```

是不是看出點什麼？`$@和main.o`對應，$<和main.c對應。其實$@和$<是兩個變量。$@為規則中的目標，$<為規則中的第一個依賴文件。%.o:%.c是一種稱為模式規則的特殊規則。因為main.o符合該模模式，再推導出依賴文件main.c，最終推導出整個規則為：

```sh
main.o : main.c：  
>--- cc    -c -o main.o main.c 
```

其餘幾個目標也同樣推導。make自動推導的功能為我們減少了不少的Makefile代碼，尤其是對源文件比較多的大型工程，我們的Makefile可以不用寫得那麼繁瑣了。
最後，今天的Makefile相對於上一節進化成這個樣子了：


```sh
target_bin : main.o debug.o ipc.o timer.o tools.o  
>---gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
   
clean:  
>---rm *.o target_bin  
```

