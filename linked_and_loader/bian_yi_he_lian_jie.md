# 編譯和連結


## Hello World

```c
#include <stdio.h>

int main()
{
    printf("Hello World\n");    
    return 0;
}
```

編譯一個最簡單的 Hello World 檔案指令為

```sh
$ gcc hello.c
$ ./a.out
$ Hello World
```

編譯過程分為 4 個步驟

```sh
1.前置處理 = 預處理 = 前編譯 (Preprocessing)
2.編譯 (Compilation)
3.組譯 (Assemby)
4.連結 (Linking)
```

##前置處理、預處理、前編譯
以下兩種指令只對 source code 做前置處理


```sh
$ gcc -E hello.c -o hello.i
或
$ cpp hello.c > hello.i
```


