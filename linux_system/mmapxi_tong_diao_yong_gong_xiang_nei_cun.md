# mmap系統調用共享內存


前面講到socket的進程間通信方式，這種方式在進程間傳遞數據時首先需要從進程1地址空間中把數據拷貝到內核，內核再將數據拷貝到進程2的地址空間中，也就是數據傳遞需要經過內核傳遞。這樣在處理較多數據時效率不是很高，而讓多個進程共享一片內存區則解決了之前socket進程通信的問題。共享內存是最快的進程間通信 ，將一片內存映射到多個進程地址空間中，那麼進程間的數據傳遞將不在涉及內核。


共享內存並不是從某一進程擁有的內存中劃分出來的；進程的內存總是私有的。共享內存是從系統的空閒內存池中分配的，希望訪問它的每個進程連接它。這個連接過程稱為映射，它給共享內存段分配每個進程的地址空間中的本地地址。


mmap()系統調用使得進程之間通過映射同一個普通文件實現共享內存。普通文件被映射到進程地址空間後，進程可以向訪問普通內存一樣對文件進行訪問，不必再調用read()，write()等操作。函數原型為：

```c
#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags,  int fd, off_t offset); 
```

其中參數addr為描述符fd應該被映射到進程空間的起始地址，當指定為NULL時內核將自己去選擇起始地址，無論addr是為NULL，函數返回值都是fd所映射到內存的起始地址；

len是映射到調用進程地址空間的字節數，它 從被映射文件開頭offset個字節開始算起，offset通常設置為0；
prot 參數指定共享內存的訪問權限。可取如下幾個值的或：PROT_READ（可讀） , PROT_WRITE （可寫）, PROT_EXEC （可執行）, PROT_NONE（不可訪問），該值常設置為PROT_READ | PROT_WRITE 。

flags由以下幾個常值指定：MAP_SHARED , MAP_PRIVATE , MAP_FIXED，其中，MAP_SHARED（變動是共享的，對共享內存的修改所有進程可見） , MAP_PRIVATE（變動是私有的，對共享內存修改只對該進程可見）   必選其一，而MAP_FIXED則不推薦使用 。

munmp() 刪除地址映射關係，函數原型如下：

```c
#include <sys/mman.h>
int munmap(void *addr, size_t length);
```

參數addr是由mmap返回的地址，len是映射區大小。

進程在映射空間的對共享內容的改變並不直接寫回到磁盤文件中，往往在調用munmap（）後才執行該操作。可以通過調用msync()實現磁盤上文件內容與共享內存區的內容一致。 msync()函數原型為：

```c
#include <sys/mman.h>
int msync(void *addr, size_t length, int flags);
```

參數addr和len代表內存區，flags有以下值指定，MS_ASYNC（執行異步寫）， MS_SYNC（執行同步寫），MS_INVALIDATE（使高速緩存失效）。其中MS_ASYNC和MS_SYNC兩個值必須且只能指定一個，一旦寫操作排入內核，MS_ASYNC立即返回，MS_SYNC要等到寫操作完成後才返回。如果還指定了MS_INVALIDATE，那麼與其最終拷貝不一致的文件數據的所有內存中拷貝都失效。


在使用open函數打開一個文件之後調用mmap把文件內容映射到調用進程的地址空間，這樣我們操作文件內容只需要對映射的地址空間進行操作，而無需再使用open，write等函數。

使用共享內存的步驟基本是：
open()創建內存段；


用 ftruncate()設置它的大小；
用mmap() 把它映射到進程內存，執行其他參與者需要的操作；
當使用完時，原來的進程調用 munmap()然後退出。
下面來看一個實現：

server程序創建內存並向共享內存寫入數據：

```c
int sln_shm_get(char* shm_file, void** shm, int mem_len)
{
    int fd;
    fd = open(shm_file, O_RDWR | O_CREAT, 0644);//1. 創建內存段

    if (fd < 0) {
        printf("open <%s> failed: %s\n", shm_file, strerror(errno));
        return -1;
    }

    ftruncate(fd, mem_len);//2.設置共享內存大小
    *shm = mmap(NULL, mem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                0); //mmap映射系統內存池到進程內存

    if (MAP_FAILED == *shm) {
        printf("mmap: %s\n", strerror(errno));
        return -1;
    }

    close(fd);
    return 0;
}

int main(int argc, const char* argv[])
{
    char* shm_buf = NULL;
    sln_shm_get(SHM_IPC_FILENAME, (void**)&shm_buf, SHM_IPC_MAX_LEN);
    snprintf(shm_buf, SHM_IPC_MAX_LEN, "hello share memory ipc! i'm server.");
    return 0;
}
```

client程序映射共享內存並讀取其中數據：

```c
int main(int argc, const char* argv[])
{
    char* shm_buf = NULL;
    sln_shm_get(SHM_IPC_FILENAME, (void**)&shm_buf, SHM_IPC_MAX_LEN);
    printf("ipc client get: %s\n", shm_buf);
    munmap(shm_buf, SHM_IPC_MAX_LEN);
    return 0;
}
```

先執行server程序向共享內存寫入數據，再運行客戶程序，運行結果如下：

```sh
# ./server
# ./client
ipc client get: hello share memory ipc! i'm server.
```

共享內存不像socket那樣本身具有同步機制，它需要通過增加其他同步操作來實現同步，比如信號量等。同步相關操作在後面會有相關專欄詳細敘述。

本節源碼下載：
http://download.csdn.net/detail/gentleliu/8140487
