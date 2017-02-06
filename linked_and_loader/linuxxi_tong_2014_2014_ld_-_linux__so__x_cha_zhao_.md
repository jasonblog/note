# ld-linux.so.X查找和加載共享動態庫的順序


## ld-linux.so查找共享庫的順序：

Glibc安裝的庫中有一個為ld-linux.so.X，其中X為一個數字，在不同的平臺上名字也會不同。可以用ldd查看：

```sh
#ldd /bin/cat
linux-gate.so.1 => (0x00bfe000)
libc.so.6 => /lib/libc.so.6 (0x00a4a000)
/lib/ld-linux.so.2 (0x00a28000)
```

最後一個沒有「=>」的就是。其中第一個不是實際的庫文件，你是找不到的，它是一個虛擬庫文件用於和kernel交互。

/lib/ld-linux.so.2以及它的64位版本/lib64/ld-linux-x86-64.so.2雖然看起來是共享庫文件，但實際上他們可以獨立運行。他們的功能是負責動態加載。
它們通過讀取可執行文件的頭部信息來確定哪些庫文件是必須的，以及哪些需要加載。
加載完成後，它會通過修正執行文件裡的相關的地址指針來和加載的庫文件完成動態鏈接，此時程序就可以運行了。


ld-linux.so是專門負責尋找庫文件的庫。
以cat為例，cat首先告訴ld-linux.so它需要libc.so.6這個庫文件，ld-linux.so將按一定順序找到libc.so.6庫再給cat調用。

那ld-linux.so又是怎麼找到的呢？
其實不用找，ld-linux.so的位置是寫死在程序中的，gcc在編譯程序時就寫死在裡面了。Gcc寫到程序中ld-linux.so的位置是可以改變的，通過修改gcc的spec文件。


###編譯時，ld-linux.so查找共享庫的順序
```sh
（1）ld-linux.so.6由gcc的spec文件中所設定 
（2）gcc --print-search-dirs所打印出的路徑，主要是libgcc_s.so等庫。可以通過GCC_EXEC_PREFIX來設定 
（3）LIBRARY_PATH環境變量中所設定的路徑，或編譯的命令行中指定的-L/usr/local/lib 
（4）binutils中的ld所設定的缺省搜索路徑順序，編譯binutils時指定。（可以通過「ld --verbose | grep SEARCH」來查看） 
（5）二進製程序的搜索路徑順序為PATH環境變量中所設定。一般/usr/local/bin高於/usr/bin
（6）編譯時的頭文件的搜索路徑順序，與library的查找順序類似。一般/usr/local/include高於/usr/include
```

###運行時，ld-linux.so查找共享庫的順序：
```sh
（1）ld-linux.so.6在可執行的目標文件中被指定，可用readelf命令查看 
（2）ld-linux.so.6缺省在/usr/lib和lib中搜索；當glibc安裝到/usr/local下時，它查找/usr/local/lib
（3）LD_LIBRARY_PATH環境變量中所設定的路徑 
（4）/etc/ld.so.conf（或/usr/local/etc/ld.so.conf）中所指定的路徑，由ldconfig生成二進制的ld.so.cache中
```




如何查找和加載Linux應用程序需要的動態庫？
具體的：
確定程序需要的庫文件
系統如何查找共享庫文件
加載共享庫文件

##靜態和動態鏈接
Linux系統裡有兩種類型的可執行程序：
靜態鏈接的可執行程序。這種程序文件本身包含了運行所需要的所有庫函數的代碼。程序自身就可以運行，而不依賴於額外的庫文件。靜態鏈接的程序的一個優點就是安裝時不需要額外安裝依賴庫。
動態鏈接的可執行程序。動態鏈接程序只為調用的庫函數設置了佔位符，並沒有真正把函數代碼鏈接到程序本身中，所以這種程序體積小。而且往往多個程序依賴於同一個共享庫文件，這樣多個程序運行時，通過虛擬內存的機制，物理內存中之需要保留一份共享庫代碼，大大節約了內存空間。其缺點是自身運行需要依賴外部庫文件，安裝時如果所依賴的庫文件不存在，需要額外安裝庫文件。總的來說，其優點大於缺點，所以目前系統上大多數程序都是動態鏈接的。


在許多Linux系統中存在一個有趣的例子：
/bin/ln命令。這個命令用來創建軟鏈接或者硬鏈接（注意此處的鏈接與程序的動態鏈接是兩個不同的概念）。ln這個程序也是動態鏈接的，所以其運行依賴動態加載機制。加載動態庫時往往使用的是符號鏈接（比如使用ls-linux-86-64.so.2這個符號鏈接而不是真正的文件ld-2.11.1.so），所以動態加載依賴於這種符號鏈接機制。
想想如果這個軟鏈接有問題或者被刪除了，該怎麼辦呢？解決方式就是重新建一個。此時問題來了，為能動態加載而新建鏈接文件使用的是ln命令，而ln本身運行時的動態加載又可能依賴於這個出問題符號鏈接，這就容易陷入死循環了。為瞭解決這個問題，一些Linux發行版裡額外提供了ln的靜態鏈接版本sln（像Debian或Ubuntu這樣的系統，你可能找不到sln程序，你也需要檢查一下/sbin/ldconfig是否存在）。
Fedora12 64bit系統中的這兩個文件如下所示：

```sh
Listing 1. Sizes of sln and ln
[ian@echidna ~]$ ls -l /sbin/sln /bin/ln
-rwxr-xr-x. 1 root root 47384 2010-01-12 09:35 /bin/ln
-rwxr-xr-x. 1 root root 603680 2010-01-04 09:07 /sbin/sln
```

可以看出，同樣的程序靜態鏈接時，文件體積要大很多。



##需要哪些庫文件？
現在很多的Linux系統所在的機器能夠同時支持32位和64位程序的運行。因此，很多庫文件都同時存在32位和64位兩個版本。其中的64位版本存放在目錄/lib64中，32位版本存放在/lib中。你很可能會在一個典型的64位Linux系統中發現同時存在/lib/libc-2.11.1.so and /lib64/libc-2.11.1.so這兩個庫文件。


##ldd命令
除了知道一個靜態鏈接的文件體積大之外，你能分辨一個程序到底是靜態鏈接還是動態鏈接的嗎？如果是動態鏈接的，你知道它依賴於哪些庫文件嗎？ldd命令可以幫助你回答這兩個問題。如果你正在運行一個像Debian或Ubuntu這樣的系統，你可能找不到sln程序，你也需要檢查一下/sbin/ldconfig是否存在。
本例子使用的是Fedora12 64位系統。為了做個對比，32位系統上的輸出也同時列出。
Listing 2. Output of ldd for sln and ln

```sh
//64-bit版本：
[ian@echidna ~]#Fedora 12 64-bit         
[ian@echidna ~]#Fedora 12 64-bit         
[ian@echidna ~] ldd /sbin/sln /sbin/ldconfig /bin/ln
/sbin/sln:
not a dynamic executable
/sbin/ldconfig:
not a dynamic executable
/bin/ln:
linux-vdso.so.1 => (0x00007fff644af000)
libc.so.6 => /lib64/libc.so.6 (0x00000037eb800000)
/lib64/ld-linux-x86-64.so.2 (0x00000037eb400000)

//32-bit版本：
[ian@pinguino ~]# Fedora 8 32-bit         
[ian@pinguino ~]# Fedora 8 32-bit         
[ian@pinguino ~] ldd /bin/ln
linux-gate.so.1 => (0x00110000)
libc.so.6 => /lib/libc.so.6 (0x00a57000)
/lib/ld-linux.so.2 (0x00a38000)
```

ldd命令的功能是查看動態鏈接的信息，所以對於sln,ldconfig這樣的靜態鏈接程序，僅僅提示"not a dynamic executable"。而對於動態鏈接的ln文件則給出了所依賴的三個動態鏈接庫文件。.so後綴名是shared object的意思，也稱為動態鏈接庫。
上面的輸出結果還顯示了三種不同類型的依賴文件：
linux-vsdo.so.1 是Linux Virtual Dynamic Shared Object，一會我們會講到這個。在Fedora8中則是 linux-gate.so.1。
libc.so.6 指向/lib64/libc.so.6
/lib64/ld-linux-x86-64.so.2 是一個庫文件的絕對路徑

下表中，我們使用ls -l命令顯示了上面最後的兩個庫文件其實是具體版本庫文件的符號鏈接。
在Fedora12系統中，結果如下：
Listing 3. Library symbolic links

[ian@echidna ~]$ ls -l /lib64/libc.so.6 /lib64/ld-linux-x86-64.so.2
lrwxrwxrwx. 1 root root 12 2010-01-14 14:24 /lib64/ld-linux-x86-64.so.2 -> ld-2.11.1.so
lrwxrwxrwx. 1 root root 14 2010-01-14 14:24 /lib64/libc.so.6 -> libc-2.11.1.so


Linux虛擬動態共享對象
在早期X86的年代裡，用戶程序和內核空間程序的通信是通過軟中斷。隨著處理器速度的提高，這種方式成為了嚴重的瓶頸。從Pentium II開始，Intel引進了一種叫做Fast System Call的機制來加速系統調用的執行。這種新的方式使用的專門的SYSENTER和SYSEXIT指令而不是中斷。
上面提到的linux-vdso.so.1是一個虛擬庫文件或者虛擬動態共享對象，這個文件駐留在每一個程序進程的地址空間。老的系統裡文件名是linux-gate.so.1。這個虛擬庫提供必要的邏輯來讓用戶程序在特定的處理器上選擇最快的系統函數調用方式，也許是軟中斷，但在大多數新的CPU中都是使用fast system call。


##動態加載
也許你會感到奇怪，上面提到的/lib/ld-linux.so.2以及它的64位版本/lib64/ld-linux-x86-64.so.2雖然看起來是共享庫文件，但實際上他們可以獨立運行。他們的功能是負責動態加載。它們通過讀取可執行文件的頭部信息來確定哪些庫文件是必須的，以及哪些需要加載。加載完成後，它會通過修正執行文件裡的相關的地址指針來和加載的庫文件完成動態鏈接，此時程序就可以運行了。

```sh
ld-linux.so的幫助手冊還描述了ld.so這個文件，ld.so針對的是早期的
a.out格式的可執行程序。ld-linux.so.2的--list參數可以顯示
和ldd命令同樣的信息，如下表：
Listing 4. Using ld-linux.so to display library requirements

[ian@echidna ~]$ /lib64/ld-linux-x86-64.so.2 --list /bin/ln
linux-vdso.so.1 => (0x00007fffc9fff000)
libc.so.6 => /lib64/libc.so.6 (0x00000037eb800000)
/lib64/ld-linux-x86-64.so.2 (0x00000037eb400000)

[ian@pinguino ~]$ /lib/ld-linux.so.2 --list /bin/ln
linux-gate.so.1 => (0x00110000)
libc.so.6 => /lib/libc.so.6 (0x00a57000)
/lib/ld-linux.so.2 (0x00a38000)
```

注意：上面的地址部分`可能每次運行都會有所不同`。


##動態庫配置
從可執行文件頭可以讀取依賴的庫文件信息，那麼動態加載器去哪裡尋找這些庫文件呢？還是Linux世界的慣例，存在一個配置文件描述這些信息。
實際上，有兩個配置文件，一個是/etc/ld.so.conf，另一個是/etc/ld.so.cache。下表顯示了/etc/ld.so.conf的內容。注意/etc/ld.so.conf文件指定了ld.so.conf.d目錄下的所有配置文件都會被包含進來。老系統上的/etc/ld.so.conf文件可能本身就包含了很多配置項而不是從子目錄中加載配置項。
Listing 5. Content of /etc/ld.so.conf

```sh
[ian@echidna ~]cat/etc/ld.so.confincludeld.so.conf.d/∗.conf
[ian@echidna ]cat/etc/ld.so.confincludeld.so.conf.d/∗.conf
[ian@echidna ] ls /etc/ld.so.conf.d/*.conf

/etc/ld.so.conf.d/kernel-2.6.31.12-174.2.19.fc12.x86_64.conf
/etc/ld.so.conf.d/kernel-2.6.31.12-174.2.22.fc12.x86_64.conf
/etc/ld.so.conf.d/kernel-2.6.31.12-174.2.3.fc12.x86_64.conf
/etc/ld.so.conf.d/mysql-x86_64.conf
/etc/ld.so.conf.d/qt-x86_64.conf
/etc/ld.so.conf.d/tix-x86_64.conf
/etc/ld.so.conf.d/xulrunner-64.conf
```

程序的加載速度很重要，所以使用ldconfig命令來處理ld.so.conf文件以及它包含的其他配置文件，還有/lib, /usr/lib目錄下的庫文件，以及以命令行參數形式提供的其他庫文件。


ldconfig的作用是為最近使用的庫文件創建必要的符號鏈接和緩存信息，並寫入/etc/ld.so.cache文件。動態加載器利用/etc/ld.so.cache這個文件裡的信息來查找和加載庫文件。如果你改變了ld.so.conf文件或者其包含的子文件，你必須重新運行ldconfig命令來更新/etc/ld.so.cache緩存文件。

正常情況下，你使用不帶任何參數的ldconfig命令來重建ld.so.cache文件。你也可以為ldconfig提供命令來改變這種默認的行為。通常使用man ldconfig來查看更多地信息。下表顯示了使用-p 參數來查看ld.so.cache裡的內容。


Listing 6. Using ldconfig to display ld.so.cache

```sh
[ian@lyrebird ian]$ /sbin/ldconfig -p | less
1602 libs found in cache `/etc/ld.so.cache'
libzip.so.1 (libc6,x86-64) => /usr/lib64/libzip.so.1
libz.so.1 (libc6,x86-64) => /lib64/libz.so.1
libz.so (libc6,x86-64) => /usr/lib64/libz.so
libx86.so.1 (libc6,x86-64) => /usr/lib64/libx86.so.1
libx11globalcomm.so.1 (libc6,x86-64) => /usr/lib64/libx11globalcomm.so.1
libxul.so (libc6,x86-64) => /usr/lib64/xulrunner-1.9.1/libxul.so
libxtables.so.2 (libc6,x86-64) => /usr/lib64/libxtables.so.2
libxslt.so.1 (libc6,x86-64) => /usr/lib64/libxslt.so.1
libxslt.so (libc6,x86-64) => /usr/lib64/libxslt.so
libxpcom.so (libc6,x86-64) => /usr/lib64/xulrunner-1.9.1/libxpcom.so
libxml2.so.2 (libc6,x86-64) => /usr/lib64/libxml2.so.2
libxml2.so (libc6,x86-64) => /usr/lib64/libxml2.so
...
libABRTdUtils.so.0 (libc6,x86-64) => /usr/lib64/libABRTdUtils.so.0
libABRTUtils.so.0 (libc6,x86-64) => /usr/lib64/libABRTUtils.so.0
ld-linux.so.2 (ELF) => /lib/ld-linux.so.2
ld-linux-x86-64.so.2 (libc6,x86-64) => /lib64/ld-linux-x86-64.so.2
```

##加載指定的庫文件
如果你在運行一個依賴於特定老版本共享庫文件的應用程序，或者你在開發測試一個新開發的共享庫或者已存在共享庫的新版本，你就需要加載器使用的默認查找路徑。這種需要也出現在使用特定產品共享庫的腳本中，這個特定的產品共享庫可能安裝到了/opt目錄下。
就像可以通過修改PATH環境變量來指定可執行程序的搜索路徑一樣，你可以通過修改LD_LIBRARY_PATH環境變量來修改動態加載器的查找路徑。LD_LIBRARY_PATH指定的路徑會排在ld.so.cache文件裡的路徑之前。
例如，你可能會使用這樣一些命令：

export LD_LIBRARY_PATH=/usr/lib/oldstuff:/opt/IBM/AgentController/lib