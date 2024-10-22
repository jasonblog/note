# dlopen() 與 GCC -rdynamic 選項


之前讀某個 JIT Compiler 的程式碼。只要正確輸入函式原型，這個 JIT Compiler 可以讓使用者呼叫外部的 C 函式。

要實作類似的功能並不困難，我們只要寫一個「名稱—位址」的對照表，JIT Compiler 就能輕易地實作這個功能。舉例來說，如果我希望使用者可以使用 foo 與 bar 二個函式，我們需要定義一個簡單的函式表：

```c
struct { const char *, void * } func_table[] = {
  { "foo", (void *)&foo },
  { "bar", (void *)&bar },
  { NULL, NULL },
};
```

當使用者輸入 foo()，我們就可以從函式表取得 foo 的位址，進而呼叫 foo 函式。不過我卻沒有辦法在 JIT Compiler 的程式碼找到類似的東西。這激起了我的興趣。

其實 JIT Compiler 的執行檔確實是有這樣的函式表。如果仔細看該 JIT Compiler 的 Makefile，我們就可以發現他在編譯 JIT Compiler 的時候，額外加上 -rdynamic 選項。根據 GCC 使用手冊，-rdynamic 的用途為：

Pass the flag -export-dynamic to the ELF linker, on targets that support it. This instructs the linker to add all symbols, not only used ones, to the dynamic symbol table. This option is needed for some uses of dlopen or to allow obtaining backtraces from within a program.

簡單的說，這個選項會把有的 non-static 函式視為要 export 的 symbol，讓 Linker 把他們都寫入 Dynamic Symbol Table。之後，我們就可以使用 dlsym() 找到這個 symbol。以下是一個簡單的範例：

```c
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void foo()
{
    printf("called foo()\n");
}

void bar()
{
    printf("called bar()\n");
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s [func]\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Load the function/variable symbol table from executable. */
    void* handle = dlopen(NULL, RTLD_LAZY);

    if (!handle) {
        fprintf(stderr, "ERROR: Failed to load executable\n");
        return EXIT_FAILURE;
    }

    /* Find the function */
    void* func = dlsym(handle, argv[1]);

    if (!func) {
        fprintf(stderr, "ERROR: Function not found: %s\n", argv[1]);
    } else {
        ((void (*)())func)();
    }

    dlclose(handle);

    return EXIT_SUCCESS;
}
```
我們可以分別使用以下二個指令編譯這個範例程式：

```sh
$ gcc example.c -ldl
$ gcc example.c -ldl -rdynamic
```

然後使用 objdump -T 指令觀看執行檔的 Dynamic Symbol Table。

```sh
$ objdump -T a.out
```
以下是簡化過的輸出：

```sh
a.out:     file format elf64-x86-64

DYNAMIC SYMBOL TABLE:
... 略 ... 
0000000000000000      DF *UND*    0000000000000000  GLIBC_2.2.5 fwrite
0000000000000000      DF *UND*    0000000000000000  GLIBC_2.2.5 dlsym
0000000000601048 g    D  *ABS*    0000000000000000  Base        _edata
0000000000400914 g    DF .text    0000000000000010  Base        bar
0000000000601038 g    D  .data    0000000000000000  Base        __data_start
0000000000400904 g    DF .text    0000000000000010  Base        foo
... 略 ... 
0000000000400924 g    DF .text    00000000000000f6  Base        main
0000000000400788 g    DF .init    0000000000000000  Base        _init
0000000000601048 g    DO .bss    0000000000000008  GLIBC_2.2.5 stderr
... 略 ...
```

其中藍色的字就是加上 -rdynamic 選項多出來的資料。執行以下指令，我們就可以注意到範例程式可以呼叫對應的函式：

```sh
$ ./a.out foo
called foo() 
$ ./a.out bar
called bar() 
```

我們要怎麼取得 Dynamic Symbol Table 裡面的資料呢？這就必須藉助 dlopen() 與 dlsym()。根據 man dlopen 的資料，dlopen() 必須要傳入二個參數，第一個參數是要載入的動態函式庫，而第二個參數是函式庫的載入方式。如果我們以 NULL 作為第一個參數，則代表我們要載入執行檔本身的 Dynamic Symbol Table。接著我們就可以用 dlsym() 尋找我們要執行的函式了。

後注：雖然我覺得這個方法很巧妙也很有趣，不過我覺得這個方法不太安全。因為所有的函式都有可能暴露給 JIT Compiler 使用者。我覺得還是手動建函式表會比較好。這樣我們才可以控制要開放的函式與介面。