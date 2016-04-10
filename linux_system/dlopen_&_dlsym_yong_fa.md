# dlopen &  dlsym 用法


```c
/*gcc -o dl_call dll.c -ldl*/

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

int main(void) {
    typedef void (*myShowMsg)();
    int (*my_printf)(const char*, ...);
    void *handle;
    char* error;
    void* handle1 = 0;
    myShowMsg show_msg = 0;

    /* Clear any existing error */
    dlerror();

    /* 開啟 shared library 'libc-2.15.so' */
    handle = dlopen("/lib/x86_64-linux-gnu/libc-2.19.so", RTLD_LAZY);


    /* 在 handle 指向的 shared library 裡找到 "printf" 函數,
     *      * 並傳回他的 memory address
     **/

    *(void **)(&my_printf) = dlsym(handle, "printf");
    my_printf("printf()'s address is [%p],handle is [%p]\n", my_printf, handle);
    dlclose(handle);

    printf("my test\n");
    dlerror();

    printf("main pid=%d\n", getpid());
    /* 開啟 shared library 'libmyfunc.so' */
    if ((handle1 = dlopen("./libmyfunc.so", RTLD_LAZY)) == NULL) {
        printf("open libmyfunc.so error.the handle1 is [%p]\n", handle1);
        exit(1);
    }

    /* 在 handle 指向的 shared library 裡找到 "showMsg" 函數,
     *      * 並傳回他的 memory address
     **/
    if ((*(void **)(&show_msg) = dlsym(handle1, "showMsg")) == NULL) {
        printf("can not find showMsg.the showMsg is [%p]\n", *(void**)&show_msg);
        exit(2);
    }

    if ((error = dlerror()) != NULL) {
        printf("error message:[%s]\n", error);
        exit(3);
    }

    my_printf("showMsg()'s address is [%p],handle is [%p]\n", show_msg, handle1);

    /* indirect function call (函數指標呼叫),
     * 呼叫所指定的函數
     */
    show_msg();
    printf("main pid=%d\n", getpid());
    dlclose(handle1);

    return 0;
}
```

- 需要照系統改路徑版本

```sh
handle = dlopen("/lib/x86_64-linux-gnu/libc-2.19.so", RTLD_LAZY);
```

```sh
gcc -o dl_call dll.c -ldl
```

- myfunc.c

```c
/* gcc -fpic -shared -o libmyfunc.so myfunc.c */
#include <stdio.h>
#include <unistd.h>

void showMsg() {
    printf("showMsg pid=%d\n", getpid());
    printf("hello, this is mytest\n");
}
```


```sh
gcc -fpic -shared -o libmyfunc.so myfunc.c
```


```sh
printf()'s address is [0x7f325f31a400],handle is [0x7f325fab0b00]
my test
main pid=2317
showMsg()'s address is [0x7f325f0c4770],handle is [0x123a0b0]
showMsg pid=2317
hello, this is mytest
main pid=2317
```


- Tips

```
int (*my_printf)(const char*, ...);

void *dlopen(const char *filename, int flag);

*(void **)(&my_printf) = dlsym(handle, "my_printf");

先&my_printf，再转换成void **，最后是把该指针指向的地方赋值

void **p = (void **)&my_printf;
*p = dlsym(...

任何指针都可以转换成void *
```