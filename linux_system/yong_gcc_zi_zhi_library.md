# 用gcc 自製 Library


## Library可分成三種，static、shared與dynamically loaded。


### Static libraries


Static 程式庫用於靜態連結，簡單講是把一堆object檔用ar(archiver)包裝集合起來，檔名以 `.a' 結尾。優點是執行效能通常會比後兩者快，而且因為是靜態連結，所以不易發生執行時找不到library或版本錯置而無法執行的問題。缺點則是檔案較大，維護度較低；例如library如果發現bug需要更新，那麼就必須重新連結執行檔。



- 編譯

編譯方式很簡單，先例用 `-c' 編出 object 檔，再用 ar 包起來即可。


- hello.c

```c
#include <stdio.h>
void hello(){ 
    printf("Hello "); 
}
```

- world.c

```c
#include <stdio.h>
void world(){ 
    printf("world."); 
}
```

- mylib.h


```c
void hello();
void world();
```

```sh
$ gcc -c hello.c world.c             /* 編出 hello.o 與 world.o */
$ ar rcs libmylib.a hello.o world.o  /* 包成 limylib.a */
```

這樣就可以建出一個檔名為 libmylib.a 的檔。輸出的檔名其實沒有硬性規定，但如果想要配合 gcc 的 '-l' 參數來連結，一定要以 `lib' 開頭，中間是你要的library名稱，然後緊接著 `.a' 結尾。

### 使用

- main.c

```c
#include "mylib.h"
int main() {
    hello();
    world();
}
```
使用上就像與一般的 object 檔連結沒有差別。
```c
$ gcc main.c libmylib.a
```

也可以配合 gcc 的 `-l` 參數使用


```sh
$ gcc main.c -L. -lmylib
```

`-Ldir' 參數用來指定要搜尋程式庫的目錄，`.' 表示搜尋現在所在的目錄。通常預設會搜 /usr/lib 或 /lib 等目錄。
`-llibrary' 參數用來指定要連結的程式庫 ，'mylib' 表示要與mylib進行連結，他會搜尋library名稱前加`lib'後接`.a'的檔案來連結。

### Shared libraries


Shared library 會在程式執行起始時才被自動載入。因為程式庫與執行檔是分離的，所以維護彈性較好。有兩點要注意，shared library是在程式起始時就要被載入，而不是執行中用到才載入，而且在連結階段需要有該程式庫才能進行連結。

首先有一些名詞要弄懂，soname、real name與linker name。

soname 用來表示是一個特定 library 的名稱，像是 libmylib.so.1 。前面以 `lib' 開頭，接著是該 library 的名稱，然後是 `.so' ，接著是版號，用來表名他的介面；如果介面改變時，就會增加版號來維護相容度。

real name 是實際放有library程式的檔案名稱，後面會再加上 minor 版號與release 版號，像是 libmylib.so.1.0.0 。

一般來說，版號的改變規則是(印象中在 APress-Difinitive Guide to GCC中有提到，但目前手邊沒這本書)，最尾碼的release版號用於程式內容的修正，介面完全沒有改變。中間的minor用於有新增加介面，但相舊介面沒改變，所以與舊版本相容。最前面的version版號用於原介面有移除或改變，與舊版不相容時。

linker name 是用於連結時的名稱，是不含版號的 soname ，如: libmylib.so。通常 linker name與 real name是用 ln 指到對應的 real name ，用來提供彈性與維護性。


### 編譯

shared library的製作過程較複雜。

```sh
$ gcc -c -fPIC hello.c world.c
```


編譯時要加上 `-fPIC` 用來產生 position-independent code。也可以用 -fpic 參數。 (不太清楚差異，只知道 -fPIC 較通用於不同平台，但產生的code較大，而且編譯速度較慢)。


```sh
$ gcc -shared -Wl,-soname,libmylib.so.1 -o libmylib.so.1.0.0 \
  hello.o world.o
```

-shared 表示要編譯成 shared library
-Wl 用於參遞參數給linker，因此-soname與libmylib.so.1會被傳給linker處理。
-soname 用來指名 soname 為 limylib.so.1
library 會被輸出成libmylib.so.1.0.0 (也就是real name)

若不指定 soname 的話，在編譯結連後的執行檔會以連時的library檔名為soname，並載入他。否則是載入soname指定的library檔案。

可以利用 objdump 來看 library 的 soname。


```sh
$ objdump -p libmylib.so  | grep SONAME
  SONAME      libmylib.so.1
  
```
若不指名-soname參數的話，則library不會有這個欄位資料。

在編譯後再用 ln 來建立 soname 與 linker name 兩個檔案。

### 使用

與使用 static library 同。

```sh
$ gcc main.c libmylib.so
```
以上直接指定與 libmylib.so 連結。

或用

```sh
$ gcc main.c -L. -lmylib
```

linker會搜尋 libmylib.so 來進行連結。

如果目錄下同時有static與shared library的話，會以shared為主。使用 -static 參數可以避免使用shared連結。

```sh
$ gcc main.c -static -L. -lmylib
```

此時可以用 ldd 看編譯出的執行檔與shared程式庫的相依性


```sh
$ldd a.out
 linux-gate.so.1 =>  (0xffffe000)
 libmylib.so.1 => not found
 libc.so.6 => /lib/libc.so.6 (0xb7dd6000)
 /lib/ld-linux.so.2 (0xb7f07000)
```

輸出結果顯示出該執行檔需要 libmylib.so.1 這個shared library。會顯示 not found 因為沒指定該library所在的目錄，所找不到該library。

因為編譯時有指定-soname參數為 libmylib.so.1 的關係，所以該執行檔會載入libmylib.so.1。否則以libmylib.so連結，執行檔則會變成要求載入libmylib.so


```sh
$ ./a.out
  ./a.out: error while loading shared libraries: libmylib.so.1:
  cannot open shared object file: No such file or directory
  ```


因為找不到 libmylib.so.1 所以無法執行程式。有幾個方式可以處理。

a. 把 libmylib.so.1 安裝到系統的library目錄，如/usr/lib下<br>
b. 設定 /etc/ld.so.conf ，加入一個新的library搜尋目錄，並執行ldconfig更新快取<br>
c. 設定 LD_LIBRARY_PATH 環境變數來搜尋library這個例子是加入目前的目錄來搜尋要載作的library<br>


```sh
$ LD_LIBRARY_PATH=. ./a.out
  Hello world.
```

###3. Dynamically loaded libraries

Dynamicaaly loaded libraries 才是像 windows 所用的 DLL ，在使用到時才載入，編譯連結時不需要相關的library。動態載入庫常被用於像plug-ins的應用。

###3.1 使用方式

動態載入是透過一套 dl function來處理。

```sh
#include <dlfcn.h>
void *dlopen(const char *filename, int flag);
//開啟載入 filename 指定的 library。
void *dlsym(void *handle, const char *symbol);
//取得 symbol 指定的symbol name在library被載入的記憶體位址。
int dlclose(void *handle);
//關閉dlopen開啟的handle。
char *dlerror(void);
//傳回最近所發生的錯誤訊息。
```

- dltest.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
int main()  {
    void *handle;
    void (*f)();
    char *error;
 
    /* 開啟之前所撰寫的 libmylib.so 程式庫 */
    handle = dlopen("./libmylib.so", RTLD_LAZY);
    if( !handle ) {
        fputs( dlerror(), stderr);
        exit(1);
    }
 
    /* 取得 hello function 的 address */
    f = dlsym(handle, "hello");
    if(( error=dlerror())!=NULL) {
        fputs(error, stderr);
        exit(1);
    }
    /* 呼叫該 function */
    f();
    dlclose(handle);
}
```

編譯時要加上 -ldl 參數來與 dl library 連結

```c
$ gcc dltest.c -ldl
```
結果會印出 Hello 字串
```c
$ ./a.out
Hello
```
關於dl的詳細內容請參閱 man dlopen

##參考資料:

Creating a shared and static library with the gnu compiler [gcc]
http://www.adp-gmbh.ch/cpp/gcc/create_lib.html

Program Library HOWTO
http://tldp.org/HOWTO/Program-Library-HOWTO/index.html


