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

```sh
ldd test
	linux-vdso.so.1 =>  (0x00007fff1c720000)
	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f5a69a50000)
	/lib64/ld-linux-x86-64.so.2 (0x000055ae57f4d000)
```

用 qemu 需要把 libc.so.6 ＆ ld-linux-x86-64.so.2 放進 qemu 系統裡面

```sh
scp -P 5555 /lib/x86_64-linux-gnu/libc.so.6 root@localhost:/lib
scp -P 5555 /lib64/ld-linux-x86-64.so.2 root@localhost:/lib64
```

不然編譯要 -static
```sh
gcc -o -static test test.c
```



- dmesg 可以看到

```
<0>[  327.782077] hello world!
```