# 補充 现代Linux有两种共享内存机制


现代Linux有两种共享内存机制：

- POSIX共享内存（shm_open()、shm_unlink()）
- System V共享内存（shmget()、shmat()、shmdt()）

其中，System V共享内存历史悠久，一般的UNIX系统上都有这套机制；而POSIX共享内存机制接口更加方便易用，一般是结合内存映射mmap使用。

mmap和System V共享内存的主要`区别`在于：

- sysv shm是持久化的，除非被一个进程明确的删除，否则它始终存在于内存里，直到系统关机；

- mmap映射的内存在不是持久化的，如果进程关闭，映射随即失效，除非事先已经映射到了一个文件上。


内存映射机制mmap是POSIX标准的系统调用，有匿名映射和文件映射两种。

- 匿名映射使用进程的虚拟内存空间，它和malloc(3)类似，实际上有些malloc实现会使用mmap匿名映射分配内存，不过匿名映射不是POSIX标准中规定的。

- 文件映射有MAP_PRIVATE和MAP_SHARED两种。前者使用COW的方式，把文件映射到当前的进程空间，修改操作不会改动源文件。后者直接把文件映射到当前的进程空间，所有的修改会直接反应到文件的page cache，然后由内核自动同步到映射文件上。

相比于IO函数调用，基于文件的mmap的一大优点是把文件映射到进程的地址空间，避免了数据从用户缓冲区到内核page cache缓冲区的复制过程；当然还有一个优点就是不需要频繁的read/write系统调用。

由于接口易用，且可以方便的persist到文件，避免主机shutdown丢失数据的情况，所以在现代操作系统上一般偏向于使用mmap而不是传统的System V的共享内存机制。

建议仅把mmap用于需要大量内存数据操作的场景，而不用于IPC。因为IPC总是在多个进程之间通信，而通信则涉及到同步问题，如果自己手工在mmap之上实现同步，容易滋生bug。推荐使用socket之类的机制做IPC，基于socket的通信机制相对健全很多，有很多成熟的机制和模式，比如epoll, reactor等。

sysv shm的实现可以参考`glibc源码，shm_open(3)` 打开一个名为abc的共享内存，等价于open("/dev/shm/abc", ..)，其中 /dev/shm 是Linux下sysv共享内存的默认挂载点。shm_open调用返回一个文件描述符，其实也可以给` mmap(2)` 使用，作为named share memory使用。

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

- 如果使用 MAP_SHARED 的文件映射方式，由于内核自动同步page cache到文件，如果进程意外crash，是否可能出现内存数据没有及时同步到文件的情况。

- 同样的情况，如果没有主动调用shm_unlink，那么文件在内存中的映射是否自动消失？因为原来就是映射到进程空间的，但这个进程已经死了。还是说内核自动维护page cache，与具体的进程无关。
