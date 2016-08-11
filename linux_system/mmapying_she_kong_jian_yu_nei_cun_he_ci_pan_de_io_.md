# mmap映射空間與內存和磁盤的IO性能對比


mmap：
功能描述：mmap將一個文件或者其它對象映射進內存。文件被映射到多個頁上，如果文件的大小不是所有頁的大小之和，最後一個頁不被使用的空間將會清零。munmap執行相反的操作，刪除特定地址區域的對象映射。 基於文件的映射，在mmap和munmap執行過程的任何時刻，被映射文件的st_atime可能被更新。如果st_atime字段在前述的情況下沒有得到更新，首次對映射區的第一個頁索引時會更新該字段的值。用PROT_WRITE 和 MAP_SHARED標誌建立起來的文件映射，其st_ctime 和 st_mtime
在對映射區寫入之後，但在msync()通過MS_SYNC 和 MS_ASYNC兩個標誌調用之前會被更新。
用法：

```c
#include <sys/mman.h>
void *mmap(void *start, size_t length, int prot, int flags,
int fd, off_t offset);
int munmap(void *start, size_t length);
```

mmap能夠把磁盤文件映射進內存，這樣對文件的操作就可以直接像操作內存那樣，而不用read()/write()函數了，那麼通過mmap映射的空間和內存、磁盤的IO性能上有何差異呢？為此，寫瞭如下程序測試mmap映射的空間、內存已經磁盤的讀寫速度。

- 程序代碼如下（mmap_test.c）：


```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

#define SIZE 50*1024*1024
int main(int argc, char** argv)
{
    FILE* fp;
    char c = 'a';
    int i;
    char* pmap;
    int fd;
    char* mem;
    struct timeval t1, t2;
    long spend_us;
    printf("write size: %dMB/n", SIZE / 1024 / 1024);

    if ((fp = fopen("disk_file", "w")) == NULL) {
        printf("fopen error!/n");
        return -1;
    }

    gettimeofday(&t1, NULL);

    for (i = 0; i < SIZE; i++) {
        if (fwrite(&c, sizeof(char), 1, fp) != 1) {
            printf("file write error!/n");
        }
    }

    gettimeofday(&t2, NULL);
    spend_us = 1000000 * (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec);
    printf("write disk file,spend time:%ld us/n", spend_us);
    fclose(fp);

    fd = open("mmap_file", O_RDWR);

    if (fd < 0) {
        perror("open mmap_file");
        return -1;
    }

    pmap = (char*)mmap(NULL, sizeof(char) * SIZE, PROT_READ | PROT_WRITE,
                       MAP_SHARED, fd, 0);

    if (pmap == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    gettimeofday(&t1, NULL);

    for (i = 0; i < SIZE; i++) {
        memcpy(pmap + i, &c, 1);
    }

    gettimeofday(&t2, NULL);
    spend_us = 1000000 * (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec);
    printf("write mmap file,spend time:%ld us/n", spend_us);
    close(fd);
    munmap(pmap, sizeof(char) * SIZE);
    mem = (char*)malloc(sizeof(char) * SIZE);

    if (!mem) {
        printf("mem malloc error!/n");
        return -1;
    }

    gettimeofday(&t1, NULL);

    for (i = 0; i < SIZE; i++) {
        memcpy(mem + i, &c, 1);
    }

    gettimeofday(&t2, NULL);
    spend_us = 1000000 * (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec);
    printf("write mem,spend time:%ld us/n", spend_us);
    free(mem);
    return 0;
}
```

編譯程序：
```sh
gcc –o mmap_test mmap_test.c
```

運行之前在當前目錄創建disk_file和mmap_file

```sh
touch disk_file
dd if=/dev/zero of=mmap_file bs=1M count=50
```

運行程序：./mmap_test
運行結果：

```sh
write size: 50MB
write disk file,spend time:3020481 us
write mmap file,spend time:793141 us
write mem,spend time:399923 us
```

計算寫的速度：

```sh
寫磁盤速度：16.55M/s
寫mmap映射的空間速度：63.04M/s
寫內存的速度：125.02M/s
```

由此可見，由mmap映射的空間的讀寫速度比直接讀寫磁盤的速度快。
那麼，為什麼會快呢？這就需要從mmap實現的原理來分析了。
在此圖中，「起始地址」是mmap的返回值。在圖中，映射存儲區位於堆和棧之間。
以下摘自博客
http://hi.baidu.com/%D4%BC%D0%DE%D1%C7ing/blog/item/ee1664d899820ae138012f11.html
內核怎樣保證各個進程尋址到同一個共享內存區域的內存頁面

```
1、 page cache及swap cache中頁面的區分：一個被訪問文件的物理頁面都駐留在page cache或swap cache中，一個頁面的所有信息由struct page來描述。struct page中有一個域為指針mapping ，它指向一個struct address_space類型結構。page cache或swap cache中的所有頁面就是根據address_space結構以及一個偏移量來區分的。
2、文件與 address_space結構的對應：一個具體的文件在打開後，內核會在內存中為之建立一個struct inode結構，其中的i_mapping域指向一個address_space結構。這樣，一個文件就對應一個address_space結構，一個 address_space與一個偏移量能夠確定一個page cache 或swap cache中的一個頁面。因此，當要尋址某個數據時，很容易根據給定的文件及數據在文件內的偏移量而找到相應的頁面。
3、進程調用mmap()時，只是在進程空間內新增了一塊相應大小的緩衝區，並設置了相應的訪問標識，但並沒有建立進程空間到物理頁面的映射。因此，第一次訪問該空間時，會引發一個缺頁異常。
4、對於共享內存映射情況，缺頁異常處理程序首先在swap cache中尋找目標頁（符合address_space以及偏移量的物理頁），如果找到，則直接返回地址；如果沒有找到，則判斷該頁是否在交換區 (swap area)，如果在，則執行一個換入操作；如果上述兩種情況都不滿足，處理程序將分配新的物理頁面，並把它插入到page cache中。進程最終將更新進程頁表。
註：對於映射普通文件情況（非共享映射），缺頁異常處理程序首先會在page cache中根據address_space以及數據偏移量尋找相應的頁面。如果沒有找到，則說明文件數據還沒有讀入內存，處理程序會從磁盤讀入相應的頁面，並返回相應地址，同時，進程頁表也會更新。
5、所有進程在映射同一個共享內存區域時，情況都一樣，在建立線性地址與物理地址之間的映射之後，不論進程各自的返回地址如何，實際訪問的必然是同一個共享內存區域對應的物理頁面。
```