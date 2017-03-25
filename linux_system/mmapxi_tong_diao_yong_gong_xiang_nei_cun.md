# mmap系统调用共享内存


前面讲到socket的进程间通信方式，这种方式在进程间传递数据时首先需要从进程1地址空间中把数据拷贝到内核，内核再将数据拷贝到进程2的地址空间中，也就是数据传递需要经过内核传递。这样在处理较多数据时效率不是很高，而让多个进程共享一片内存区则解决了之前socket进程通信的问题。共享内存是最快的进程间通信 ，将一片内存映射到多个进程地址空间中，那么进程间的数据传递将不在涉及内核。


共享内存并不是从某一进程拥有的内存中划分出来的；进程的内存总是私有的。共享内存是从系统的空闲内存池中分配的，希望访问它的每个进程连接它。这个连接过程称为映射，它给共享内存段分配每个进程的地址空间中的本地地址。


mmap()系统调用使得进程之间通过映射同一个普通文件实现共享内存。普通文件被映射到进程地址空间后，进程可以向访问普通内存一样对文件进行访问，不必再调用read()，write()等操作。函数原型为：

```c
#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags,  int fd, off_t offset); 
```

其中参数addr为描述符fd应该被映射到进程空间的起始地址，当指定为NULL时内核将自己去选择起始地址，无论addr是为NULL，函数返回值都是fd所映射到内存的起始地址；

len是映射到调用进程地址空间的字节数，它 从被映射文件开头offset个字节开始算起，offset通常设置为0；
prot 参数指定共享内存的访问权限。可取如下几个值的或：PROT_READ（可读） , PROT_WRITE （可写）, PROT_EXEC （可执行）, PROT_NONE（不可访问），该值常设置为PROT_READ | PROT_WRITE 。

flags由以下几个常值指定：MAP_SHARED , MAP_PRIVATE , MAP_FIXED，其中，MAP_SHARED（变动是共享的，对共享内存的修改所有进程可见） , MAP_PRIVATE（变动是私有的，对共享内存修改只对该进程可见）   必选其一，而MAP_FIXED则不推荐使用 。

munmp() 删除地址映射关系，函数原型如下：

```c
#include <sys/mman.h>
int munmap(void *addr, size_t length);
```

参数addr是由mmap返回的地址，len是映射区大小。

进程在映射空间的对共享内容的改变并不直接写回到磁盘文件中，往往在调用munmap（）后才执行该操作。可以通过调用msync()实现磁盘上文件内容与共享内存区的内容一致。 msync()函数原型为：

```c
#include <sys/mman.h>
int msync(void *addr, size_t length, int flags);
```

参数addr和len代表内存区，flags有以下值指定，MS_ASYNC（执行异步写）， MS_SYNC（执行同步写），MS_INVALIDATE（使高速缓存失效）。其中MS_ASYNC和MS_SYNC两个值必须且只能指定一个，一旦写操作排入内核，MS_ASYNC立即返回，MS_SYNC要等到写操作完成后才返回。如果还指定了MS_INVALIDATE，那么与其最终拷贝不一致的文件数据的所有内存中拷贝都失效。


在使用open函数打开一个文件之后调用mmap把文件内容映射到调用进程的地址空间，这样我们操作文件内容只需要对映射的地址空间进行操作，而无需再使用open，write等函数。

使用共享内存的步骤基本是：
open()创建内存段；


用 ftruncate()设置它的大小；
用mmap() 把它映射到进程内存，执行其他参与者需要的操作；
当使用完时，原来的进程调用 munmap()然后退出。
下面来看一个实现：

server程序创建内存并向共享内存写入数据：

```c
int sln_shm_get(char* shm_file, void** shm, int mem_len)
{
    int fd;
    fd = open(shm_file, O_RDWR | O_CREAT, 0644);//1. 创建内存段

    if (fd < 0) {
        printf("open <%s> failed: %s\n", shm_file, strerror(errno));
        return -1;
    }

    ftruncate(fd, mem_len);//2.设置共享内存大小
    *shm = mmap(NULL, mem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                0); //mmap映射系统内存池到进程内存

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

client程序映射共享内存并读取其中数据：

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

先执行server程序向共享内存写入数据，再运行客户程序，运行结果如下：

```sh
# ./server
# ./client
ipc client get: hello share memory ipc! i'm server.
```

共享内存不像socket那样本身具有同步机制，它需要通过增加其他同步操作来实现同步，比如信号量等。同步相关操作在后面会有相关专栏详细叙述。

本节源码下载：
http://download.csdn.net/detail/gentleliu/8140487
