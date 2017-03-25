# posix 共享内存


上一节讲了由open函数打开一个内存映射文件，再由mmap函数把得到的`描述符`映射到当前进程地址空间中来。这一节说说另外一种类似的共享内存方法，即有shm_open函数打开一个Posix.1 IPC名字（也许是文件系统中的一个路径名），所返回的描述符由函数mmap映射到当前进程地址空间。

posix共享内存和尚上一节类似，首先需要制定一个名字参数调用shm_open ，以创建一个新的共享内存对象或打开一个已存在的共享内存对象；然后再调用mmap把共享内存区映射到调用进程的地址空间中。
shm_open函数原型如下：

```c
#include <sys/mman.h>  
#include <sys/stat.h> /* For mode constants */  
#include <fcntl.h> /* For O_* constants */  
  
int shm_open(const char *name, int oflag, mode_t mode);  
  
Link with -lrt.  
```


参数name为将要被打开或创建的共享内存对象，需要指定为/name的格式。
参数oflag必须要有O_RDONLY（只读）、标志O_RDWR(读写)，除此之外还可以指定O_CREAT（没有共享对象则创建）、O_EXCL（如果O_CREAT指定，但name不存在，就返回错误）、O_TRUNC（如果共享内存存在，长度就截断为0）。
参数mode为指定权限位，在指定了O_CREAT的前提下使用，如果没有指定O_CREAT，mode可以指定为0.

该函数返回一个描述符，在mmap函数（见上一节）第五个参数使用。

```c
#define SHM_IPC_FILENAME "sln_shm_file"  
#define SHM_IPC_MAX_LEN 1024   
```


- server

```c
int sln_shm_get(char *shm_file, void **shm, int mem_len)  
{   
    int fd;  
  
    fd = shm_open(shm_file, O_RDWR | O_CREAT, 0644);  
    if (fd < 0) {  
        printf("shm_open <%s> failed: %s\n", shm_file, strerror(errno));  
        return -1;  
    }  
  
    ftruncate(fd, mem_len);  
  
    *shm = mmap(NULL, mem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);  
    if (MAP_FAILED == *shm) {  
        printf("mmap: %s\n", strerror(errno));  
        return -1;  
    }  
  
    close(fd);  
  
    return 0;  
}  
  
int main(int argc, const char *argv[])  
{  
    char *shm_buf = NULL;  
    sln_shm_get(SHM_IPC_FILENAME, (void **)&shm_buf, SHM_IPC_MAX_LEN);  
    snprintf(shm_buf, SHM_IPC_MAX_LEN, "ipc client get: hello posix share memory ipc! This is write by server.");  
  
    return 0;  
}  
```

编译执行结果如下：

```sh
# ./server   
# ./client   
ipc client get: hello posix share memory ipc! This is write by server.  
```

shm_open创建的文件位置位于 /dev/shm/ 目录下：

```sh
# ll /dev/shm/   
-rw-r--r-- 1 root root 8193 Oct 30 14:13 sln_shm_file   
```

posix共享内存同步可以使用posix信号量来实现，具体在后面同步相关专栏会有详细讲解。

本节源码下载：
http://download.csdn.net/detail/gentleliu/8140869

