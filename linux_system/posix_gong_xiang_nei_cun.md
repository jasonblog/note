# posix 共享內存


上一節講了由open函數打開一個內存映射文件，再由mmap函數把得到的`描述符`映射到當前進程地址空間中來。這一節說說另外一種類似的共享內存方法，即有shm_open函數打開一個Posix.1 IPC名字（也許是文件系統中的一個路徑名），所返回的描述符由函數mmap映射到當前進程地址空間。

posix共享內存和尚上一節類似，首先需要制定一個名字參數調用shm_open ，以創建一個新的共享內存對象或打開一個已存在的共享內存對象；然後再調用mmap把共享內存區映射到調用進程的地址空間中。
shm_open函數原型如下：

```c
#include <sys/mman.h>  
#include <sys/stat.h> /* For mode constants */  
#include <fcntl.h> /* For O_* constants */  
  
int shm_open(const char *name, int oflag, mode_t mode);  
  
Link with -lrt.  
```


參數name為將要被打開或創建的共享內存對象，需要指定為/name的格式。
參數oflag必須要有O_RDONLY（只讀）、標誌O_RDWR(讀寫)，除此之外還可以指定O_CREAT（沒有共享對象則創建）、O_EXCL（如果O_CREAT指定，但name不存在，就返回錯誤）、O_TRUNC（如果共享內存存在，長度就截斷為0）。
參數mode為指定權限位，在指定了O_CREAT的前提下使用，如果沒有指定O_CREAT，mode可以指定為0.

該函數返回一個描述符，在mmap函數（見上一節）第五個參數使用。

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

編譯執行結果如下：

```sh
# ./server   
# ./client   
ipc client get: hello posix share memory ipc! This is write by server.  
```

shm_open創建的文件位置位於 /dev/shm/ 目錄下：

```sh
# ll /dev/shm/   
-rw-r--r-- 1 root root 8193 Oct 30 14:13 sln_shm_file   
```

posix共享內存同步可以使用posix信號量來實現，具體在後面同步相關專欄會有詳細講解。

本節源碼下載：
http://download.csdn.net/detail/gentleliu/8140869

