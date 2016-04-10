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
