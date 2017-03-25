# system V共享内存


system V共享内存和posix共享内存类似，system V共享内存是调用shmget函数和shamat函数。   
shmget函数创建共享内存区，或者访问一个存在的内存区，类似系统调用共享内存的open和posix共享内存shm_open函数。shmget函数原型为：


```c
#include <sys/ipc.h>  
#include <sys/shm.h>  
  
int shmget(key_t key, size_t size, int shmflg); 
```

