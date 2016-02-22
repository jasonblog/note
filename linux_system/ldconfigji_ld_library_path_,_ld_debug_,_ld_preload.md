# ldconfig及 LD_LIBRARY_PATH,LD_DEBUG ,LD_PRELOAD



1. 往/lib和/usr/lib裡面加東西，是不用修改/etc/ld.so.conf的，但是完了之後要調一下ldconfig，不然這個library會找不到

2. 想往上面兩個目錄以外加東西的時候，一定要修改/etc/ld.so.conf，然後再調用ldconfig，不然也會找不到
比 如安裝了一個mysql到/usr/local/mysql，mysql有一大堆library在/usr/local/mysql/lib下面，這時就 需要在/etc/ld.so.conf下面加一行/usr/local/mysql/lib，保存過後ldconfig一下，新的library才能在程 序運行時被找到。

3. 如果想在這兩個目錄以外放lib，但是又不想在/etc/ld.so.conf中加東西（或者是沒有權限加東西）。那也可以，就是export一個全局變 量LD_LIBRARY_PATH，然後運行程序的時候就會去這個目錄中找library。一般來講這只是一種臨時的解決方案，在沒有權限或臨時需要的時 候使用。

4. ldconfig做的這些東西都與運行程序時有關，跟編譯時一點關係都沒有。編譯的時候還是該加-L就得加，不要混淆了。

5. 總之，就是不管做了什麼關於library的變動後，最好都ldconfig一下，不然會出現一些意想不到的結果。不會花太多的時間，但是會省很多的事。



##幾個特殊的環境變量：
 
###LD_DEBUG的用法：

```sh 
[root@ljj c_c++]# LD_DEBUG=help ls
Valid options for the LD_DEBUG environment variable are:
  libs        display library search paths
  reloc       display relocation processing
  files       display progress for input file
  symbols     display symbol table processing
  bindings    display information about symbol binding
  versions    display version dependencies
  all         all previous options combined
  statistics  display relocation statistics
  unused      determined unused DSOs
  help        display this help message and exit
To direct the debugging output into a file instead of standard output
a filename can be specified using the LD_DEBUG_OUTPUT environment variable.
[root@ljj c_c++]#`
```

```sh
Linux支持共享庫已經有悠久的歷史了，不再是什麼新概念了。大家都知道如何編譯、連接以及動態加載(dlopen/dlsym/dlclose) 共享庫。但是，可能很多人，甚至包括一些高手，對共享庫相關的一些環境變量認識模糊。當然，不知道這些環境變量，也可以用共享庫，但是，若知道它們，可能就會用得更好。下面介紹一些常用的環境變量，希望對家有所幫助：

 

LD_LIBRARY_PATH 這個環境變量是大家最為熟悉的，它告訴loader：在哪些目錄中可以找到共享庫。可以設置多個搜索目錄，這些目錄之間用冒號分隔開。在linux下，還提供了另外一種方式來完成同樣的功能，你可以把這些目錄加到/etc/ld.so.conf中，或則在/etc/ld.so.conf.d裡創建一個文件，把目錄加到這個文件裡。當然，這是系統範圍內全局有效的，而環境變量只對當前shell有效。按照慣例，除非你用上述方式指明，loader是不會在當前目錄下去找共享庫的，正如shell不會在當前目前找可執行文件一樣。

 

LD_PRELOAD 這個環境變量對於程序員來說，也是特別有用的。它告訴loader：在解析函數地址時，優先使用LD_PRELOAD裡指定的共享庫中的函數。這為調試提供了方便，比如，對於C/C++程序來說，內存錯誤最難解決了。常見的做法就是重載malloc系列函數，但那樣做要求重新編譯程序，比較麻煩。使用LD_PRELOAD機制，就不用重新編譯了，把包裝函數庫編譯成共享庫，並在LD_PRELOAD加入該共享庫的名稱，這些包裝函數就會自動被調用了。在linux下，還提供了另外一種方式來完成同樣的功能，你可以把要優先加載的共享庫的文件名寫在/etc/ld.so.preload裡。當然，這是系統範圍內全局有效的，而環境變量只對當前shell有效。

 

LD_ DEBUG 這個環境變量比較好玩，有時使用它，可以幫助你查找出一些共享庫的疑難雜症（比如同名函數引起的問題）。同時，利用它，你也可以學到一些共享庫加載過程的知識。它的參數如下：

  libs display library search paths

  reloc display relocation processing

  files display progress for input file

  symbols display symbol table processing

  bindings display information about symbol binding

  versions display version dependencies

  all all previous options combined

  statistics display relocation statistics

  unused determined unused DSOs

  help display this help message and exit


BIND_NOW 這個環境變量與dlopen中的flag的意義是一致，只是dlopen中的flag適用於顯示加載的情況，而BIND_NOW/BIND_NOT適用於隱式加載。

 

LD_PROFILE/LD_PROFILE_OUTPUT：為指定的共享庫產生profile數據，LD_PROFILE指定共享庫的名稱，LD_PROFILE_OUTPUT指定輸出profile文件的位置，是一個目錄，且必須存在，默認的目錄為/var/tmp/或/var/profile。通過profile數據，你可以得到一些該共享庫中函數的使用統計信息。
```


Linux 運行的時候，是如何管理共享庫(*.so)的？在 Linux 下面，共享庫的尋找和加載是由 /lib/ld.so 實現的。 ld.so 在標準路經(/lib, /usr/lib) 中尋找應用程序用到的共享庫。


但是，如果需要用到的共享庫在非標準路經，ld.so 怎麼找到它呢？


目前，Linux 通用的做法是將非標準路經加入 /etc/ld.so.conf，然後運行 ldconfig 生成 /etc/ld.so.cache。 ld.so 加載共享庫的時候，會從 ld.so.cache 查找。


傳 統上， Linux 的先輩 Unix 還有一個環境變量 - LD_LIBRARY_PATH 來處理非標準路經的共享庫。ld.so 加載共享庫的時候，也會查找這個變量所設置的路經。但是，有不少聲音主張要避免使用 LD_LIBRARY_PATH 變量，尤其是作為全局變量。這些聲音是：


* LD_LIBRARY_PATH is not the answer - http://prefetch.net/articles/linkers.badldlibrary.html

* Why LD_LIBRARY_PATH is bad - http://xahlee.org/UnixResource_dir/_/ldpath.html 

* LD_LIBRARY_PATH - just say no - http://blogs.sun.com/rie/date/20040710
解決這一問題的另一方法是在編譯的時候通過 -R<path> 選項指定 run-time path。


```sh
1. 往/lib和/usr/lib裡面加東西，是不用修改/etc/ld.so.conf的，但是完了之後要調一下ldconfig，不然這個library會找不到


2. 想往上面兩個目錄以外加東西的時候，一定要修改/etc/ld.so.conf，然後再調用ldconfig，不然也會找不到
比如安裝了一個mysql到/usr/local/mysql，mysql有一大堆library在/usr/local/mysql/lib下面，這時就需要在/etc/ld.so.conf下面加一行/usr/local/mysql/lib，保存過後ldconfig一下，新的library才能在程序運行時被找到。


3. 如果想在這兩個目錄以外放lib，但是又不想在/etc/ld.so.conf中加東西（或者是沒有權限加東西）。那也可以，就是export一個全局變量LD_LIBRARY_PATH，然後運行程序的時候就會去這個目錄中找library。一般來講這只是一種臨時的解決方案，在沒有權限或臨時需要的時候使用。


4. ldconfig做的這些東西都與運行程序時有關，跟編譯時一點關係都沒有。編譯的時候還是該加-L就得加，不要混淆了。


5. 總之，就是不管做了什麼關於library的變動後，最好都ldconfig一下，不然會出現一些意想不到的結果。不會花太多的時間，但是會省很多的事。

```
http://hi.baidu.com/fanzier/blog/item/222ba3ec74cb9a2463d09fb5.html




LD_LIBRARY_PATH 這個環境變量是大家最為熟悉的，它告訴loader：在哪些目錄中可以找到共享庫。可以設置多個搜索目錄，這些目錄之間用冒號分隔開。在linux下，還提供了另外一種方式來完成同樣的功能，你可以把這些目錄加到/etc/ld.so.conf中，或則在/etc/ld.so.conf.d裡創建一個文件，把目錄加到這個文件裡。當然，這是系統範圍內全局有效的，而環境變量只對當前shell有效。按照慣例，除非你用上述方式指明，loader是不會在當前目錄下去找共享庫的，正如shell不會在當前目前找可執行文件一樣。
 


LD_DEBUG 是 glibc 中的 loader 為了方便自身調試而設置的一個環境變量。通過設置這個環境變量，可以方便的看到 loader 的加載過程。 以在 64位 centos 上編譯安裝 memcached 為例，安裝階段順利，執行 memcached 命令時出現錯誤：

```sh
#memcached –h
/usr/local/memcached/bin/memcached: error while loading shared libraries: libevent-1.4.so.2:
cannot open shared object file: No such file or directory
#find / -name libevent-1.4.so.2
/usr/lib/libevent-1.4.so.2
```

##設置 LD_DEBUG 變量獲得更多信息:

```sh
#LD_DEBUG=help ls
Valid options for the LD_DEBUG environment variable are:

  libs        display library search paths
  reloc       display relocation processing
  files       display progress for input file
  symbols     display symbol table processing
  bindings    display information about symbol binding
  versions    display version dependencies
  all         all previous options combined
  statistics  display relocation statistics
  unused      determined unused DSOs
  help        display this help message and exit

To direct the debugging output into a file instead of standard output
a filename can be specified using the LD_DEBUG_OUTPUT environment variable.
```

##庫依賴的問題，使用 libs 參數:

```sh
#LD_DEBUG=libs /usr/local/memcached/bin/memcached –h
9808:      find library=libevent-1.4.so.2 [0]; searching
9808:      search cache=/etc/ld.so.cache
9808:      search path=/lib64/tls/x86_64:/lib64/tls:/lib64/x86_64:/lib64:/usr/lib64/tls/x86_64
/usr/lib64/tls:/usr/lib64/x86_64:/usr/lib64
(system search path)
9808:      trying file=/lib64/tls/x86_64/libevent-1.4.so.2
9808:      trying file=/lib64/tls/libevent-1.4.so.2
9808:      trying file=/lib64/x86_64/libevent-1.4.so.2
9808:      trying file=/lib64/libevent-1.4.so.2
9808:      trying file=/usr/lib64/tls/x86_64/libevent-1.4.so.2
9808:      trying file=/usr/lib64/tls/libevent-1.4.so.2
9808:      trying file=/usr/lib64/x86_64/libevent-1.4.so.2
9808:      trying file=/usr/lib64/libevent-1.4.so.2
9808:      /usr/local/memcached/bin/memcached: error while loading shared libraries: libevent-1.4.so.2: cannot open shared object file: No such file or directory
```


