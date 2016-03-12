# 如何在linux kernel 中加入system call

建立一個叫helloworld.c的程式，這個就是我們的system call。

- kernel 底下 建立  helloworld.c

```c
#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage int sys_helloworld(void);

asmlinkage int sys_helloworld(void)
{
    printk(KERN_EMERG "hello world!");
    return 1;
}
```

- include/linux/syscalls.h  增加

```c
asmlinkage int sys_helloworld(void);
```


syscall_32.tbl，在最後加上
```c
350     i386    helloworld              sys_helloworld
```

注意：編號不一定為350，如果希望用在64bit的kernel上，請把syscall_32.tbl改成syscall_64.tbl，並把這行改成：

```c
350     common  helloworld              sys_helloworld
```

kernel/Makefile，加這一行：

```c
obj-y                   += helloworld.o
```

```c
#include <syscall.h>
#include <sys/types.h>

int main(void) {
    int a=syscall(350);
    return 0;
}
```

編譯及執行測試程式：

```sh
gcc -o test test.c
./test
```


- dmesg 可以看到

```
<0>[  327.782077] hello world!
```