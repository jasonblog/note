# Makefile範例教學


Makefile和GNU make可能是linux世界裡最重要的檔案跟指令了。編譯一個小程式，可以用簡單的command來進行編譯；稍微複雜一點的程式，可以用shell script來幫忙進行編譯。如今的程式(如Apache, Linux Kernel)可能動輒數百萬行程式碼，數萬個標頭檔(headers)、函式庫(libraries)以及程式碼(source code)，如果只是針對幾個檔案進行修改，卻要用shell script整個程式重新編譯，不但浪費時間也相當沒有效率。GNU make或是其他make程式的用途就在這裡： 當程式有些許變動時，我們需要一個程式幫助我們判斷哪些需要重新編譯，哪些不用；因此，撰寫一個好的Makefile便是相當重要的能力。

不過話說回來，是不是每一隻程式都需要一個Makefile呢？其實撰寫Makefile是有益無害的，只是如果你的程式就只有兩三個source codes需要編譯，其實忘掉Makefile也沒關係。本文的目的是希望以範例的方式能夠讓讀者能看得懂，並且有能力撰寫並修改Makefile，也順便當作自己的筆記。

## 傳統的編譯：

```sh
gcc foo1.c -o foo1
```

事實上，上面的這個編譯方式可以拆解成：
```sh
gcc foo1.c -c
gcc foo1.o -o foo1
```

編譯的過程是將原始碼(foo1.c)先利用-c參數編譯成.o(object file)，然後再鏈結函式庫成為一個binary。-c即compile之意。

```sg
gcc foo1.c $SACLIB/sacio.a -O3 -g -Wall -ansi -o foo1
```

開始有趣了。編譯的參數開始變多：
- -c :編譯但不進行鏈結，會產生一個跟原始碼相同名稱但副檔名為.o的目的檔。

- -O ：表示最佳化的程度
-O預設就是-O1，你可以指定成-O2或-O3，數字越大表示最佳化程度越好，但是會增加編譯時間。

- -g ：把偵錯資訊也編譯進去
當你有需要使用GDB軟體進行偵錯，必須加入-g使GDB能夠讀取。一般情況下可以不用-g，因為他也會增加binary大小。

- -Wall ：顯示警告訊息
使用這個參數會在編譯時顯示更多的警告訊息。這個參數相當有用，特別是找不到libs/headers之類的問題。

- -ansi ：使用相容ANSI標準的編譯方式
ANSI是American National Standards Institute，即美國國家標準學會的簡稱。-ansi可以增加程式的可移植性。

- 其中的$SACLIB就是一個變數名稱，她必須被指定正確的值。
執行這個命令前必須先確定這個變數是有被指派正確的值才行。.a檔是一個靜態函式(static library)，關於靜態跟共享的觀念稍候解釋。


再來更多吧！假設你今天要編譯main這隻程式，他的source codes有main.c, foo.c, target.h，並且需要/usr/local/moreFoo/lib/libpthread.so這個共享函式，以及/usr/local/moreFoo/include裡面的headers；這麼複雜的情況又該怎麼作呢？


```sh
gcc main.c foo.c -I /usr/local/moreFoo/include -lpthread -L /usr/local/moreFoo/lib -O3 -ansi -o main
```

新的參數意義如下：
- -I ：需要include某些headers所在的目錄
通常include目錄都放置headers，利用-I使編譯器知道去哪裡找原始碼裡宣告的header。gcc預設會去尋找headers的目錄大致有：
    - /usr/include
    - /usr/local/include
    - /usr/src/linux-headers-`uname -r`/include
    - /usr/lib/gcc/i486-linux-gnu/UR_GCC_VERSION/include
當前目錄

因此，當原始碼內有宣告
```c
#include <fakeFoo.h> 
```
但fakeFoo.h並不在上述的資料夾內，就需要利用-I引導gcc找到她。至於target.h因為在當前目錄，因此不必額外宣告。
當然，可以利用多個-I來指定多個headers的路徑。

- -l :表示編譯過程需要一個library。
-lpthread代表需要一個名為libpthread.so的函式。

- -L :需要額外鏈結函式庫所在的目錄
有時候程式碼經常會呼叫一些函數(methods, functions或是subroutines)，而這些函數是使用其他人預先寫好的、已經編譯成函式(例如libpthread.so)供人使用的話，我們就不必自己從頭寫過。gcc預設會去找函式的目錄大致有：

- /lib
- /usr/lib
- /lib/modules/`uname -r`/kernel/lib
- /usr/src/linux-headers-`uname -r`/lib
- /usr/local/lib

當前目錄

因此編譯時，利用-L指定目錄告訴編譯器可以該路徑下尋找libpthread.so。因此，若使用了-l，則必須確定所使用的lib有在預設尋找的目錄中，否則就必須利用-L來指定路徑給編譯器。
當然，可以利用多個-L來指定多個lib路徑。


# 靜態、共享與動態鏈結函式庫

我們已經知道：輪子不必重複發明 -- 人家寫好的方法我們可以直接拿來用。不過很多時候，這些方法可能因為某些因素，希望提供給別人使用卻又不希望公佈原始碼，這時候編譯成libraries是最好的選擇


##靜態函式(static libraries)
靜態函式其實就是將一系列.o檔打包起來，因此她可以直接視為一個巨大的.o檔。打造出一個靜態函式的方法很簡單：

```sh
gcc operator.c -c
ar crsv liboperator.a operator.o
```

或者
```sh
gcc -static operator.c -loperator
```

兩種方法皆能產生liboperator.a。假設這個靜態函式在/usr/local/foo/lib/裡，編譯時要與靜態函式作鏈結也很容易：
```sh
gcc main.c /usr/local/foo/lib/liboperator.a -o main
```

把靜態函式當成一般的.o檔一起納入binary，也可以像這樣：
```sh
gcc main.c -L /usr/local/foo/lib -loperator -o main
```

靜態函式將所有的功能全部打包在一起，因此binary會變得很巨大，但是執行這個程式的所有功能都已滿足，不會再有libraries相依性的問題。但是缺點在於當某些libraries的功能有所更新時，這個程式就必須重新編譯，無法享受到即時更新的優點。通常商業軟體以及嵌入式系統等功能異動較少的程式，會傾向使用靜態函式。

##共享函式(shared libraries)
共享函式跟靜態函式的觀念剛好相反，程式在執行時必須能夠找到相依的函式，否則執行時會出現錯誤訊息。製作一個共享函式的方法也很簡單：
```sh
gcc -shared operator.c -o liboperator.so
```

或是先編譯出目的檔再進行鏈結：
```sh
gcc -c operator.c
gcc -shared operator.o -o liboperator.so
```

產生出liboperator.so。假設這個共享函式在/usr/local/foo/lib/裡，使用共享函式進行鏈結也很容易：
```sh
gcc main.c /usr/local/foo/lib/liboperator.so -o main
```

也可以像這樣：
```sh
gcc main.c -L /usr/local/foo/lib -loperator -o main
```

共享函式在`程式啟動時期會檢查是否存在`。以一個分別鏈結了靜態函式與共享函式的binary而言，執行的結果大有差別。以靜態函式鏈結的main程式可以順利執行，但是假設系統預設尋找函式庫的路徑裡找不到liboperator.so，以共享函式鏈結的main程式則會出現錯誤訊息：

`./main: error while loading shared libraries: liboperator.so: cannot open shared object file: No such file or directory`

這時解決的方法有四種：
- 把liboperator.so複製或是作一個連結到/usr/lib裡。

- 修改/etc/ld.so.conf，把/usr/local/foo/lib加進系統libraries的搜尋範圍內。

- 設定LD_LIBRARY_PATH變數，累加該路徑進來：
如果你不是系統管理員，前兩個方法根本沒辦法執行。我們只好自己加到~/.profile裡：

- export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/foo/lib
改用靜態函式進行鏈結。

共享函式經常出現在開放原始碼的linux世界裡，`由於使用所有函式皆是共享的`，因此許多程式都可以重複利用既有的功能；有新功能或是bug也能簡單的替換掉該函式，所有程式都可以即時享受到這樣的改變，也是最為常見的函式型態。

##動態函式(dynamic libraries)
動態函式跟共享函式非常類似，`唯一的差別在於程式執行時期並不會去檢查該函式是否存在，而是程式執行到某功能時才進行檢查`。這種動態載入的技術最常用在瀏覽器或是大型程式的外掛程式，當有需要用到這個功能時才載入進來。
製作一個動態函式比較麻煩。

```sh
gcc -c -fPIC operator.c
gcc -shared operator.o -o liboperator.so
```

其中的`-fPIC是產生position-independent code`，也可以用-fpic。詳細的用法已經超過筆者的理解範圍，撰寫呼叫動態函式的程式碼也需要傳入相關參數。關於更多dynamic libraries的用法請參考這裡。 


