# 動態載入函式庫 (Dynamically Lodaded Libraries)


##簡介
動態載入函式庫是一種在需要的時候才載入的函式庫。在實作外掛或者模組時特別有用，當程式需要用到的時候才載入外掛或模組。舉例來說，Pluggable Authenticaltion Modules (PAM) 使用動態載入函式庫來允許管理者設定或重新設定認證。

從 Linux 檔案格式的觀點來看，動態載入函式庫與共用函式庫並沒有什麼差別，它被編譯成標準的目的檔或標準的共用函式庫。主要的不同在於當程式連結或啟動時，函式庫並不會自動被載入，相反的，是由 API 來開啟，尋找符號，處理錯誤與關閉函式庫，C 語言需要加入` <dlfcn.h>` 這個標頭檔來使用這些 API。

Linux 使用的介面基本上跟 Solaris 一樣，都是 dlopen()，但是並不是所有的平臺都支援。HP-UX 使用 shl_load() 的機制而 Windows 使用完全不同的 DLLs 介面，如果考慮到平臺間的可攜性，可能就必須考慮使用隱藏不同平臺間差異的包裝函式庫，一種方法是使用 glib，glib 使用平臺基本的動態載入函式實作可攜性的介面，在 glib 的網站可以得到更多的相關資訊 [Dynamic Loading of Modules](http://developer.gnome.org/glib/stable/glib-Dynamic-Loading-of-Modules.html)，另一種方法是使用 libltdl，它是 GNU libtool 的一部分，想知道更多資訊的話可以去尋找 CORBA Object Request Broker (ORB) 的資料，這篇文章主要介紹 Linux 和 Solaris 提供的介面。


###dlopen()<br>
dlopen 函式開啟一個函式庫以供使用，在 C 的原型是：<br>

```c
void* dlopen(const char* filename, int flag);
```

第一個參數是函式庫的路徑，如果 filename 是由 “/“ 開始 (絕對路徑)，dlopen() 會直接嘗試開啟，不會再去其它資料夾尋找。否則的話，dlopen() 會根據以下的順序尋找函式庫

- 使用者設定的 LD_LIBRARY_PATH 環境變數，每個目錄之間由冒號隔開

- /etc/ld.so.cache 裡面所存的目錄 (由 /etc/ld.so.conf 產生)

- /lib 然後是 /usr/lib，在這裡有一個値得注意的地方，有些舊的 a.out 載入器搜尋的順序是相反的，它會先搜尋 /usr/lib 再去 /lib 找，這在正常情況下應該是沒有問題的，因為一個函式庫應該只會出現在其中一個目錄下，如果不同的函式庫而有同樣的名稱，可能會造成一些無法預期的錯誤。


第二個參數 flag 的值一定要是 RTLD_LAZY (函式符號被用到才會重定位) 或 RTLD_NOW (在 dlopen() 返回前，重定位所有未定義的符號，失敗會回傳錯誤) 其中一種。RTLD_GLOBAL 可以與 flag 同時使用，代表在這個函式庫重定位的符號，之後可以給其他函式庫使用。在除錯的時候，傾向使用 RTLD_NOW，因為沒被定義的符號會馬上被發現，RTLD_LAZY 可能會造成無法預期的錯誤。使用 RTLD_NOW 會讓函式庫開起的時間稍微增加 (但是之後尋找的速度會加快)。

dlopen() 的回傳值是一個 handle 值，對其他函式庫來說應該是不可見的。一旦函式庫開啟失敗，dlopen() 會傳回 NULL，所以必須檢查傳回的值。如果同樣的函式庫被 dlopen() 打開不只一次，會傳回同樣的 handle。

開啟的函式庫如果有 _init 這個符號，會在 dlopen() 回傳之前執行 _init()。但是，函式庫不應該匯出 _init 這個符號，因為這些機制是過時的，而且可能造成無法預期的行為。相反的，函式庫應該使用 __attribute__((constructor)) 這個函式屬性來匯出函式 (假設使用 gcc)。

如果函式庫依賴於其它的函式庫 (X 依賴 Y)，必須先載入被依賴的函式庫 (先載入 Y，再載入 X)。


###dlerror()
呼叫 dlerror() 可以取得相關的錯誤資訊，會回傳一個字串描述最近一次呼叫的 dlopen()，dlsym()，或者 dlclose()。一個比較特別的行為是當呼叫了 dlerror() 之後，之後呼叫 dlerror() 都會傳回 NULL 直到另一個錯誤發生。

###dlsym()
載入一個函式庫而不去使用是沒有意義的，主要使用函式庫的函式是 dlsym，這個函式會在已經打開的函式庫內尋找符號，函式被定義成：

```c
void* dlsym(void* handle, char* symbol);
```

handle 是從 dlopen 回傳的値，symbol 是一個由 ‘\0’ 結束的字串。如果可以避免的話，不要把 dlsym() 回傳的値存成 void* pointer，因為這樣會導致之後每次需要使用時都必須轉型一次 (對之後要維護的人來說可能會遺失一些資訊)。

如果符號沒有被找到，dlsym() 會回傳 NULL。如果知道符號不會是 NULL 或 0，那可能沒有問題，否則可能會有模稜兩可的情況，如果回傳值是 NULL，代表沒有找到符號，還是這個符號的值就是 NULL 呢，標準的做法是呼叫 dlerror() 函式 (需要先清空之前發生的錯誤)，然後再呼叫 dlsym() 尋找符號，最後再呼叫 dlerror() 看是否有錯誤發生。程式片段看起來可能像這樣：

```c
dlerror();     /* clear error code */
s = (actual_type) dlsym(handle, symbol_being_searched_for);

if ((err = dlerror()) != NULL) {
    /* handle error, the symbol wasn't found */
} else {
    /* symbol found, its value is in s */
}
```
###dlclose()
dlopen() 的相反是 dlclose()，也就是關閉一個函式庫。如果一個函式庫被釋放，_fini() 函式會被呼叫 (如果存在的話)，前面有提到這是一個過時的機制，因此不應該繼續使用。相反的，函式庫應該使用 __attribute((destructor)) 這個函式屬性來匯出函式。

Note：dlclose() 成功會回傳 0，失敗傳回非 0 値，一些 Linux 的 man page 沒有提到這個。

###DL Library Example
這個範例開啟第一個參數指定的函式庫，尋找第二個參數指定的符號，並且在每個步驟中都確認是否有錯誤發生

- dltest.c

```c
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char const* argv[])
{
    void* handle;
    char* (*fp)();
    char* error;

    handle = dlopen(argv[1], RTLD_LAZY);

    if (handle == NULL) {
        fprintf(stderr, "Load error %s: %s\n", argv[1], dlerror());
        exit(1);
    }

    dlerror(); /* clear error */

    fp = (char* (*)()) dlsym(handle, argv[2]);

    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "dlsym error %s: %s\n", argv[2], error);
        exit(1);
    }

    printf("%s\n", (*fp)());

    dlclose(handle);
}
```

建立執行檔
```sh
gcc -o dltest dltest.c -ldl
```

測試
```sh
./dltest libc.so.6 tmpnam
```


