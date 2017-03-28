# （一）：基本規則


一般一個稍大的Linux項目會有很多個源文件組成，最終的可執行程序也是由這許多個源文件編譯鏈接而成的。編譯是把一個.c或.cpp文件編譯成中間代碼.o文件，鏈接是就使用這些中間代碼文件生成可執行文件。比如在當前項目目錄下有如下源文件：


```sh
# ls  
common.h  debug.c  debug.h  ipc.c  ipc.h  main.c  tags  timer.c  timer.h  tools.c  tools.h   
```

以上源代碼可以這樣編譯：


```sh
# gcc -o target_bin main.c debug.c ipc.c timer.c tools.c 
```

如果之後修改了其中某一個文件（如tools.c），再執行一下上一行代碼即可，但如果有成千上萬個源文件這樣編譯肯定是不夠合理的。此時我們可以按下面步驟來編譯：


```sh
# gcc -c debug.c  
# gcc -c ipc.c  
# gcc -c main.c  
# gcc -c timer.c  
# gcc -c tools.c  
# gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
```

如果其中tools.c修改了，只需要編譯該文件，再執行最後生成可執行文件的操作，也就是做如下兩步操作即可：


```sh
# gcc -c tools.c  
# gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
```

這樣做看上去應該很合理了。但是如果修改了多個文件，就很可能忘了編譯某一文件，那麼運行時就很有可能出錯。如果是common.h文件修改了，那麼包含該頭文件的所有.c文件都需要重新編譯，這樣一來的話就更復雜更容易出錯了。看來這種方法也不夠好，手動處理很容易出錯。那有沒有一種自動化的處理方式呢？有的，那就是寫一個Makefile來處理編譯過程。
下面給一個簡單的Makefile，在源代碼目錄下建一個名為Makefile的文件：


```sh
target_bin : main.o debug.o ipc.o timer.o tools.o  
>---gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
   
main.o: main.c common.h                                                                                                                                                                     
>---gcc -c main.c  
   
debug.o: debug.c debug.h common.h  
>---gcc -c debug.c  
   
ipc.o: ipc.c ipc.h common.h  
>---gcc -c ipc.c  
   
timer.o: timer.c timer.h common.h  
>---gcc -c timer.c  
   
tools.o: tools.c tools.h common.h  
>---gcc -c tools.c  
```

然後在命令行上執行命令：

```sh
# make   
gcc -c main.c  
gcc -c debug.c  
gcc -c ipc.c  
gcc -c timer.c  
gcc -c tools.c  
gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
#  
# ls  
common.h  common.h~  debug.c  debug.h  debug.o  ipc.c  ipc.h  ipc.o  main.c  main.o  Makefile  Makefile~  tags  target_bin  timer.c  timer.h  timer.o  tools.c  tools.h  tools.o  
#  
```

可見在該目錄下生成了.o文件以及target_bin可執行文件。現在我們只需要執行一個make命令就可以完成所有編譯工作，無需像之前一樣手動執行所有動作，make命令會讀取當前目錄下的Makefile文件然後完成編譯步驟。從編譯過程輸出到屏幕的內容看得到執行make命令之後所做的工作，其實就是我們之前手動執行的那些命令。現在來說一下什麼是Makefile？
所謂Makefile我的理解其實就是由一組組編譯規則組成的文件，每條規則格式大致為：


```sh
target ... : prerequisites ...   
>---command  
        ...  
```

其中target是目標文件，可以為可執行文件、*.o文件或標籤。Prerequisites是產生target所需要的源文件或*.o文件，可以是另一條規則的目標。commond是要產生該目標需要執行的操作系統命令，該命令必須以tab（文中以>---標示tab字符）開頭，不可用空格代替。

說白了就是要產生target，需要依賴後面的prerequisites文件，然後執行commond來產生來得到target。這和我們之前手動執行每條編譯命令是一樣的，其實就是定義好一個依賴關係，我們把產生每個文件的依賴文件寫好，最終自動執行編譯命令。

比如在我們給出的Makefile例子中target_bin main.o等就是target，main.o debug.o ipc.o timer.o tools.o是target_bin的prerequisites，gcc -o target_bin main.o debug.o ipc.o timer.o tools.o就是commond，把所有的目標文件編譯為最終的可執行文件target，而main.c common.h是main.o的prerequisites，其gcc -c main.c命令生成target所需要的main.o文件。

在該例子中，Makefile工作過程如下：

1. 首先查找第一條規則目標，第一條規則的目標稱為缺省目標，只要缺省目標更新了就算完成任務了，其它工作都是為這個目的而做的。 該Makefile中第一條規則的目標target_bin，由於我們是第一次編譯，target_bin文件還沒生成，顯然需要更新，但此時依賴文件main.o debug.o ipc.o timer.o tools.o都沒有生成，所以需要先更新這些文件，然後才能更新target_bin。

2. 所以make會進一步查找以這些依賴文件main.o debug.o ipc.o timer.o tools.o為目標的規則。首先找main.o，該目標也沒有生成，該目標依賴文件為main.c common.h，文件存在，所以執行規則命令gcc -c main.c，生成main.o。其他target_bin所需要的依賴文件也同樣操作。

3. 最後執行gcc -o target_bin main.o debug.o ipc.o timer.o tools.o，更新target_bin。

在沒有更改源代碼的情況下，再次運行make：

```c
# make  
make: `target_bin' is up to date.  
#  
```

得到提示目標target_bin已經是最新的了。
如果修改文件main.c之後，再運行make：

```sh
# vim main.c  
# make  
gcc -c main.c  
gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
```

此時make會自動選擇受影響的目標重新編譯：

首先更新缺省目標，先檢查target_bin是否需要更新，這需要檢查其依賴文件main.o debug.o ipc.o timer.o tools.o是否需要更新。

其次發現main.o需要更新，因為main.o目標的依賴文件main.c最後修改時間比main.o晚，所以需要執行生成目標main.o的命令：gcc -c main.c更新main.o。

最後發現目標target_bin的依賴文件main.o有更新過，所以執行相應命令gcc -o target_bin main.o debug.o ipc.o timer.o tools.o更新target_bin。

總結下，執行一條規則步驟如下：

1. 先檢查它的依賴文件，如果依賴文件需要更新，則執行以該文件為目標的的規則。如果沒有該規則但找到文件，那麼該依賴文件不需要更新。如果沒有該規則也沒有該文件，則報錯退出。

2. 再檢查該文件的目標，如果目標不存在或者目標存在但依賴文件修改時間比他要晚或某依賴文件已更新，那麼執行該規則的命令。

由此可見，Makefile可以自動發現更新過的文件，自動重新生成目標，使用Makefile比自己手動編譯比起來，不僅效率高，還減少了出錯的可能性。
 
Makefile中有很多目標，我們可以編譯其中一個指定目標，只需要在make命令後面帶上目標名稱即可。如果不指定編譯目標的話make會編譯缺省的目標，也就是第一個目標，在本文給出的Makefile第一個目標為target_bin。如果只修改了tools.c文件的話，我們可能只想看看我們的更改的源代碼是否有語法錯誤而又不想重新編譯這個工程的話可以執行如下命令：


```sh
# make tools.o   
gcc -c tools.c 
```

編譯成功，這裡又引出一個問題，如果繼續執行同樣的命令：

```sh
# make tools.o  
make: `tools.o' is up to date.  
```

我們先手動刪掉tools.o文件再執行就可以了，怎麼又是手動呢？我們要自動，要自動！！好吧，我們加一個目標來刪除這些編譯過程中產生的臨時文件，該目標為clean。
我們在上面Makefile最後加上如下內容：

```sh
clean:  
>---rm *.o target_bin  
```

當我們直接make命令時不會執行到該目標，因為沒有被默認目標target_bin目標或以target_bin依賴文件為目標的目標包含在內。我們要執行該目標需要在make時指定目標即可。如下：

```sh
# make clean  
rm *.o target_bin  
```

可見clean目標被執行到了，再執行make時make就會重新生成所有目標對應的文件，因為執行make clean時，那些文件被清除了。

clean目標應該存在與你的Makefile當中，它既可以方便你的二次編譯，又可以保持的源文件的乾淨。該目標一般放在最後，不可放在最開頭，否則會被當做缺省目標被執行，這很可能不是你的意願。

最後總結一下，Makefile只是告訴了make命令如何來編譯和鏈接程序，告訴make命令生成目標文件需要的文件，具體的編譯鏈接工作是你的目標對應的命令在做。

給一個今天完整的makefile：


```sh
target_bin : main.o debug.o ipc.o timer.o tools.o  
>---gcc -o target_bin main.o debug.o ipc.o timer.o tools.o  
   
main.o: main.c common.h                                                                                                                                                                     
>---gcc -c main.c  
   
debug.o: debug.c debug.h common.h  
>---gcc -c debug.c  
   
ipc.o: ipc.c ipc.h common.h  
>---gcc -c ipc.c  
   
timer.o: timer.c timer.h common.h  
>---gcc -c timer.c  
   
tools.o: tools.c tools.h common.h  
>---gcc -c tools.c  
   
clean:  
>---rm *.o target_bin  
```



