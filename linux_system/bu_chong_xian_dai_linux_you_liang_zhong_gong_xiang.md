# 補充 現代Linux有兩種共享內存機制


現代Linux有兩種共享內存機制：

- POSIX共享內存（shm_open()、shm_unlink()）
- System V共享內存（shmget()、shmat()、shmdt()）

其中，System V共享內存歷史悠久，一般的UNIX系統上都有這套機制；而POSIX共享內存機制接口更加方便易用，一般是結合內存映射mmap使用。

mmap和System V共享內存的主要`區別`在於：

- sysv shm是持久化的，除非被一個進程明確的刪除，否則它始終存在於內存裡，直到系統關機；

- mmap映射的內存在不是持久化的，如果進程關閉，映射隨即失效，除非事先已經映射到了一個文件上。


內存映射機制mmap是POSIX標準的系統調用，有匿名映射和文件映射兩種。

- 匿名映射使用進程的虛擬內存空間，它和malloc(3)類似，實際上有些malloc實現會使用mmap匿名映射分配內存，不過匿名映射不是POSIX標準中規定的。

- 文件映射有MAP_PRIVATE和MAP_SHARED兩種。前者使用COW的方式，把文件映射到當前的進程空間，修改操作不會改動源文件。後者直接把文件映射到當前的進程空間，所有的修改會直接反應到文件的page cache，然後由內核自動同步到映射文件上。

相比於IO函數調用，基於文件的mmap的一大優點是把文件映射到進程的地址空間，避免了數據從用戶緩衝區到內核page cache緩衝區的複製過程；當然還有一個優點就是不需要頻繁的read/write系統調用。

由於接口易用，且可以方便的persist到文件，避免主機shutdown丟失數據的情況，所以在現代操作系統上一般偏向於使用mmap而不是傳統的System V的共享內存機制。

建議僅把mmap用於需要大量內存數據操作的場景，而不用於IPC。因為IPC總是在多個進程之間通信，而通信則涉及到同步問題，如果自己手工在mmap之上實現同步，容易滋生bug。推薦使用socket之類的機制做IPC，基於socket的通信機制相對健全很多，有很多成熟的機制和模式，比如epoll, reactor等。

sysv shm的實現可以參考`glibc源碼，shm_open(3)` 打開一個名為abc的共享內存，等價於open("/dev/shm/abc", ..)，其中 /dev/shm 是Linux下sysv共享內存的默認掛載點。shm_open調用返回一個文件描述符，其實也可以給` mmap(2)` 使用，作為named share memory使用。

sysv shm使用示例：

```c
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#define MAP_SIZE 1024

int main(int argc, char* argv[])
{
    int fd;
    void* result;
    fd = shm_open("shm name", O_RDWR | O_CREAT, 0644);
    result = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ftruncate(fd, MAP_SIZE);

    /* ... operate result pointer */

    pause();
    shm_unlink(fd);
    return 0;
}
```

##思考：

- 如果使用 MAP_SHARED 的文件映射方式，由於內核自動同步page cache到文件，如果進程意外crash，是否可能出現內存數據沒有及時同步到文件的情況。

- 同樣的情況，如果沒有主動調用shm_unlink，那麼文件在內存中的映射是否自動消失？因為原來就是映射到進程空間的，但這個進程已經死了。還是說內核自動維護page cache，與具體的進程無關。
