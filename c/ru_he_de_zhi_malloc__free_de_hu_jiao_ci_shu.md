
# 貌似簡單的問題：「如何得知 malloc/free 的呼叫次數？」

### 簡單的作法

* 定義全域變數來紀錄: int malloc_count = 0, free_count = 0;
* 透過巨集 `#define MALLOC(x) do { if (malloc(x)) malloc_count++; } while (0)`
    * 技巧: [談Linux Kernel巨集 do{…}while(0) 的撰寫方式](http://loda.hala01.com/2011/11/%E8%AB%87linux-kernel%E5%B7%A8%E9%9B%86-do-while0-%E7%9A%84%E6%92%B0%E5%AF%AB%E6%96%B9%E5%BC%8F/)，避免 dangling-else
    * 因此迴避 “goto” 的使用，對結構化編程有幫助
* 這有什麼問題？
    * 要改寫原始程式碼，將 malloc 換成 MALLOC 巨集，沒更換到的話，就不會追蹤到
    * 對 C++ 不適用，即便底層 [libstdc++](https://gcc.gnu.org/onlinedocs/libstdc++/) 也用 malloc()/free() 來實做 new 和 delete
    * 使用到的函式庫 (靜態和動態) 裡頭若呼叫到 malloc()/free()，也無法追蹤到

* 要徹底解決這問題，其實就需要理解動態連結器 (dynamic linker) 的協助

* 以 GNU/Linux 搭配 [glibc](https://www.gnu.org/software/libc/) 為例
* File: **malloc_count.c**

- malloc_count.c

```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

void* malloc(size_t size)
{
    char buf[32];
    static void* (*real_malloc)(size_t) = NULL;

    if (real_malloc == NULL) {
        *(void **)(&real_malloc) = dlsym(RTLD_NEXT, "malloc");
    }

    sprintf(buf, "malloc called, size = %zu\n", size);
    write(2, buf, strlen(buf));
    return real_malloc(size);
}
```

## 編譯和執行:

```sh
$ gcc -D_GNU_SOURCE -shared -fPIC -o /tmp/libmcount.so malloc_count.c -ldl
$ LD_PRELOAD=/tmp/libmcount.so ls
```


即可得知每次 `malloc()` 呼叫對應的參數，甚至可以統計記憶體配置，完全不需要變更原始程式碼。這樣的技巧，我們稱為 interpositioning。可能的應用是遊戲破解, 執行時期追蹤, sandboxing / software fault isolation (SFI), profiling，或者效能最佳化的函式庫 (如 [TCMalloc](http://goog-perftools.sourceforge.net/doc/tcmalloc.html))。

* 延伸閱讀: [Dynamic linker tricks: Using LD_PRELOAD to cheat, inject features and investigate programs](https://rafalcieslak.wordpress.com/2013/04/02/dynamic-linker-tricks-using-ld_preload-to-cheat-inject-features-and-investigate-programs/)
* Unrandomize (暫時不「亂」的亂數)

透過設定 LD_PRELOAD 環境變數，glibc 的 dynamic linker (ld-linux.so) 會在載入和重定位 (relocation) libc.so 之前，載入我們撰寫的 /tmp/libmcount.so 動態連結函式庫，如此一來，我們實做的 malloc 就會在 libc.so 提供的 malloc 函式之前被載入。當然，我們還是需要「真正的」 malloc，否則無法發揮作用，所以透過 dlsym 去從 libc.so 載入 malloc 程式碼，這裡 RTLD_NEXT 參數告知動態連結器，我們想從下一個載入的動態函式庫載入 malloc 的程式碼位址。

* GNU ld 有個選項 `-Bsymbolic-functions` 會影響 LD_PRELOAD 的行為
* [Symbolism and ELF files (or, what does -Bsymbolic do?)](https://blog.flameeyes.eu/2012/10/symbolism-and-elf-files-or-what-does-bsymbolic-do)