# 在Raspberry pi 上建立自己的system call


正在讀恐龍本(OS聖經)，第2章的程式設計作業就是自己弄個system call。

不過恐龍本用的kernel版本很舊(2.x)，實在沒法照做。

所以上網找了一個範例，用Raspberry pi來做。

  

使用kernel版本：linux-rpi-3.19.y 

 

###1. 在 kernel 目錄下建立 helloworld.c, helloworld.h

- helloworld.c

```c
#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/random.h>
#include "helloworld.h"

asmlinkage long sys_helloworld()
{
    printk(KERN_EMERG "hello world!");
    return get_random_int() * 4;
}
```

- helloworld.h

```c
#ifndef HELLO_WORLD_H 
#define HELLO_WORLD_H 
asmlinkage long sys_helloworld(void); 
#endif 
```

###2. 修改 arch/arm/kernel/calls.S 
```c
CALL(sys_helloworld)
```


###3. 修改 arch/arm/include/uapi/asm/unistd.h
```c
#define __NR_helloworld                 (__NR_SYSCALL_BASE+388)
```
 

###4. 修改 arch/arm/include/asm/unistd.h
```c
#define __NR_syscalls  (392)
```

###5. Test file : test.c

```c
#include <linux/unistd.h>
#include <stdio.h>
#include <sys/syscall.h>

int main(int argc, char* argv[])
{
    int i = atoi(argv[1]);
    int j = -1;
    printf("invocing kernel function %i\n", i);
    j = syscall(i); /* 350 is our system calls offset number */
    printf("invoked. Return is %i. Bye.\n", j);

    return 0;
}
```
###6. compile後執行： 
```sh
gcc test.c -o test
./test 388
```

##參考資料：

http://stackoverflow.com/questions/21554267/extending-the-rasbian-kernel-linux-kernel-3-10-28-for-arm-raspberry-pi-how

 

 