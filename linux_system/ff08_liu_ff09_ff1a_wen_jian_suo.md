# （六）：文件鎖


當一個系統中存在多個進程同時操作同一個文件時，為了保證數據的正確， 一般會將文件上鎖來避免共享文件產生的競爭狀態。在linux系統下文件上鎖可以使用fcntl函數來實現。
函數fcntl原型如下：


```c
#include <unistd.h>  
#include <fcntl.h>  
int fcntl(int fd, int cmd, ... /* arg */ ); 
```

函數對所打開的文件描述符fd，根據不同的cmd命令執行不同的操作，針對文件鎖的命令有如下：
F_GTELK, F_SETLK, F_SETLKW三種，分別為獲取鎖，設置鎖，同步獲取鎖。該函數在使用文件鎖時第三個參數為一個指向結構體struct flock的指針，該結構體定義大致如下：

```c
struct flock {  
    ...  
    short l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */   
    short l_whence;  /* How to interpret l_start:  SEEK_SET, SEEK_CUR, SEEK_END */   
    off_t l_start;   /* Starting offset for lock */  
    off_t l_len;     /* Number of bytes to lock */  
    pid_t l_pid;     /* PID of process blocking our lock (F_GETLK only) */   
    ...  
};  
```


成員l_whence、l_start、l_len指定了我們期望加鎖的文件範圍；<br>
l_whence可取值：SEEK_SET、SEEK_CUR、SEEK_END，一般設為SEEK_SET。<br>
l_start為文件加鎖開始的偏移處，一般設置為0，為開始處。<br>
l_len為指定加鎖的字節數，如果為0則表示從開始處一直到文件末尾。<br>
l_pid為持有鎖的進程ID號。<br>
l_type為要加鎖的類型，可以取值F_RDLCK（讀鎖）、F_WRLCK（寫鎖）、F_UNLCK （釋放鎖）。<br>

任何數量的進程都可以對同一個文件區域持有一個讀鎖，但只能有一個進程對其持有寫鎖（排它鎖）。單個進程只能對同一文件區域持有一種鎖，如果新的鎖應用在同一片已經使用了所得文件區域，那麼之前存在的鎖被覆蓋。

F_STELK在l_type為F_RDLCK或F_WRLCK時請求一個鎖，在l_type為F_UNLCK是釋放一個鎖，如果此時有另外的進程已經提前和獲取到鎖時，該調用立即返回-1。

F_SETLKW和F_STELK類似，去別在於當之前存在鎖時，該函數調用將阻塞直到鎖釋放為止。如果阻塞期被中斷信號打斷將立即返回。

F_GTELK針對指向struct flock結構體類型鎖去試圖加鎖，如果該鎖可以加上（實際沒有做加鎖的操作），fcntl()函數將結構體中l_type置為F_UNLCK，其餘成員保持不變。如果存在不兼容的鎖阻止該種類型的鎖加鎖，fcntl將已經存在鎖的詳細信息寫入結構體中，其中l_pid為持有該鎖的進程ID。 

如果想對文件區域加讀鎖，那麼文件描述應當是以可讀方式打開，類似地，如果將對文件夾寫鎖，文件應當是以可寫的方式打開。

文件鎖釋放有三種途徑，一種是顯示的使用F_UNLCK類型加鎖，第二種是進程終止，第三種是所有針對該文件打開的文件描述全部關閉。

文件鎖不會被通過fork()產生的子進程繼承。

下面是通過包裹fcntl函數實現的加鎖、同步加鎖、解鎖、測試讀寫鎖api：

```c
/**
 * \brief     acquire, release or test for the existence of record locks
 * \details   if a conficling is held on the file, then return -1.
 *
 * \param     fd - file descriptor
 * \param     locktype - lock type: F_RDLCK, F_WRLCK, F_UNLCK.
 *
 * \return    0 is success, < 0 is failed.
 */
static int sln_filelock_set(int fd, short locktype)
{
    struct flock    lock;
    lock.l_type = locktype;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLK, &lock) < 0) {
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}
/**
 * \brief     acquire, release or test for the existence of record locks
 * \details   if a conficling is held on the file, then wait for that
 *            lock to be release
 *
 * \param     fd - file descriptor
 * \param     locktype - lock type: F_RDLCK, F_WRLCK, F_UNLCK.
 *
 * \return    0 is success, < 0 is failed.
 */
int sln_filelock_wait_set(int fd, short locktype)
{
    struct flock    lock;
    lock.l_type = locktype;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();

    if (fcntl(fd, F_SETLKW, &lock) < 0) {
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}
int sln_file_wrlock(int fd)
{
    return sln_filelock_set(fd, F_WRLCK);
}
int sln_file_rdlock(int fd)
{
    return sln_filelock_set(fd, F_RDLCK);
}
int sln_file_wait_wrlock(int fd)
{
    return sln_filelock_wait_set(fd, F_WRLCK);
}
int sln_file_wait_rdlock(int fd)
{
    return sln_filelock_wait_set(fd, F_RDLCK);
}
int sln_file_unlock(int fd)
{
    return sln_filelock_set(fd, F_UNLCK);
}
/**
 * \brief     test for the existence of record locks on the file
 * \details   none
 *
 * \param     fd - file descriptor
 * \param     locktype - lock type: F_RDLCK, F_WRLCK.
 *
 * \return    0 is success, < 0 is failed.
 */
static int sln_filelock_test(int fd, short locktype)
{
    struct flock    lock;
    lock.l_type = locktype;
    lock.l_whence = 0;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = 0;

    if (fcntl(fd, F_GETLK, &lock) < 0) {
        fprintf(stderr, "fcntl: %s\n", strerror(errno));
        return -1;
    }

    if (lock.l_type != F_UNLCK) { //file is locked
        if (F_WRLCK == lock.l_type) {
            printf("write lock hold by process <%d>, lock_type: %d\n", lock.l_pid,
                   lock.l_type);
        } else if (F_RDLCK == lock.l_type) {
            printf("read lock hold by process <%d>, lock_type: %d\n", lock.l_pid,
                   lock.l_type);
        }

        return lock.l_pid; //return the pid of process holding that lock
    } else {
        printf("Unlock, lock type: %d\n", lock.l_type);
        return 0;
    }
}
int sln_file_wrlock_test(int fd)
{
    return sln_filelock_test(fd, F_WRLCK);
}
int sln_file_rdlock_test(int fd)
{
    return sln_filelock_test(fd, F_RDLCK);
}
```

下面來實現三個進程，三個進程針對同一個文件進行讀、寫、測試鎖操作：
寫進程：

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "filelock.h"
int main(int argc, const char* argv[])
{
    int             fd;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <write_str>\n", argv[0]);
        return -1;
    }

    fd = open("filelock.txt", O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    if (sln_file_wait_wrlock(fd) < 0) {
        printf("lock write failed!\n");
        close(fd);
        return -1;
    }

    printf("process <%d> holding write lock ok!\n", getpid());
    write(fd, argv[1], strlen(argv[1]));
    sleep(10);
    sln_file_unlock(fd);
    printf("release lock!\n");
    close(fd);
    return 0;
}
```

讀進程：

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "filelock.h"

int main(int argc, const char* argv[])
{
    int             fd;
    char            buf[1024];

    fd = open("filelock.txt", O_RDONLY | O_CREAT, 0644);

    if (fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    if (sln_file_wait_rdlock(fd) < 0) {
        printf("lock read failed!\n");
        close(fd);
        return -1;
    }

    printf("process <%d> holding read lock ok!\n", getpid());
    sleep(10);
    read(fd, buf, sizeof(buf));
    printf("read buf: %s\n", buf);
    sln_file_unlock(fd);
    printf("release lock!\n");

    close(fd);
    return 0;
}
```

測試讀寫鎖進程：

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "filelock.h"

int main(int argc, const char* argv[])
{
    int             fd, pid;
    fd = open("filelock.txt", O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    pid = sln_file_wrlock_test(fd);

    if (pid > 0) {
        printf("write locked!\n");
    } else {
        printf("write unlock!\n");
    }

    pid = sln_file_rdlock_test(fd);

    if (pid > 0) {
        printf("read locked!\n");
    } else {
        printf("read unlock!\n");
    }

    close(fd);
    return 0;
}
```


【CSDN 日報】| 2.17-3.17 上榜作者排行出爐      同步博客至 CSDN ，讓更多開發者看到你的文章      看微博技術大咖解析互聯網應用架構實戰
Linux互斥與同步應用（六）：文件鎖
標籤： mutexfcntl
2015-01-06 23:43 952人閱讀 評論(0) 收藏 舉報
分類：
linux同步與互斥應用系列（6）

版權聲明：本文為博主原創文章，未經博主允許不得轉載。如果您覺得文章對您有用，請點擊文章下面“頂”。
        【版權聲明：尊重原創，轉載請保留出處：blog.csdn.net/shallnet 或 .../gentleliu，文章僅供學習交流，請勿用於商業用途】
            當一個系統中存在多個進程同時操作同一個文件時，為了保證數據的正確， 一般會將文件上鎖來避免共享文件產生的競爭狀態。在linux系統下文件上鎖可以使用fcntl函數來實現。
函數fcntl原型如下：
[cpp] view plain copy
在CODE上查看代碼片派生到我的代碼片

    #include <unistd.h>  
    #include <fcntl.h>  
    int fcntl(int fd, int cmd, ... /* arg */ );  

函數對所打開的文件描述符fd，根據不同的cmd命令執行不同的操作，針對文件鎖的命令有如下：
F_GTELK, F_SETLK, F_SETLKW三種，分別為獲取鎖，設置鎖，同步獲取鎖。該函數在使用文件鎖時第三個參數為一個指向結構體struct flock的指針，該結構體定義大致如下：
[cpp] view plain copy
在CODE上查看代碼片派生到我的代碼片

    struct flock {  
        ...  
        short l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */   
        short l_whence;  /* How to interpret l_start:  SEEK_SET, SEEK_CUR, SEEK_END */   
        off_t l_start;   /* Starting offset for lock */  
        off_t l_len;     /* Number of bytes to lock */  
        pid_t l_pid;     /* PID of process blocking our lock (F_GETLK only) */   
        ...  
    };  

成員l_whence、l_start、l_len指定了我們期望加鎖的文件範圍；
l_whence可取值：SEEK_SET、SEEK_CUR、SEEK_END，一般設為SEEK_SET。
l_start為文件加鎖開始的偏移處，一般設置為0，為開始處。
l_len為指定加鎖的字節數，如果為0則表示從開始處一直到文件末尾。
l_pid為持有鎖的進程ID號。
l_type為要加鎖的類型，可以取值F_RDLCK（讀鎖）、F_WRLCK（寫鎖）、F_UNLCK （釋放鎖）。

任何數量的進程都可以對同一個文件區域持有一個讀鎖，但只能有一個進程對其持有寫鎖（排它鎖）。單個進程只能對同一文件區域持有一種鎖，如果新的鎖應用在同一片已經使用了所得文件區域，那麼之前存在的鎖被覆蓋。

F_STELK在l_type為F_RDLCK或F_WRLCK時請求一個鎖，在l_type為F_UNLCK是釋放一個鎖，如果此時有另外的進程已經提前和獲取到鎖時，該調用立即返回-1。
F_SETLKW和F_STELK類似，去別在於當之前存在鎖時，該函數調用將阻塞直到鎖釋放為止。如果阻塞期被中斷信號打斷將立即返回。
F_GTELK針對指向struct flock結構體類型鎖去試圖加鎖，如果該鎖可以加上（實際沒有做加鎖的操作），fcntl()函數將結構體中l_type置為F_UNLCK，其餘成員保持不變。如果存在不兼容的鎖阻止該種類型的鎖加鎖，fcntl將已經存在鎖的詳細信息寫入結構體中，其中l_pid為持有該鎖的進程ID。 

如果想對文件區域加讀鎖，那麼文件描述應當是以可讀方式打開，類似地，如果將對文件夾寫鎖，文件應當是以可寫的方式打開。

文件鎖釋放有三種途徑，一種是顯示的使用F_UNLCK類型加鎖，第二種是進程終止，第三種是所有針對該文件打開的文件描述全部關閉。

文件鎖不會被通過fork()產生的子進程繼承。

下面是通過包裹fcntl函數實現的加鎖、同步加鎖、解鎖、測試讀寫鎖api：
[cpp] view plain copy
在CODE上查看代碼片派生到我的代碼片

    /** 
     * \brief     acquire, release or test for the existence of record locks 
     * \details   if a conficling is held on the file, then return -1. 
     * 
     * \param     fd - file descriptor 
     * \param     locktype - lock type: F_RDLCK, F_WRLCK, F_UNLCK. 
     * 
     * \return    0 is success, < 0 is failed. 
     */  
    static int sln_filelock_set(int fd, short locktype)  
    {  
        struct flock    lock;  
        lock.l_type = locktype;  
        lock.l_whence = SEEK_SET;  
        lock.l_start = 0;  
        lock.l_len = 0;  
        lock.l_pid = getpid();  
        if (fcntl(fd, F_SETLK, &lock) < 0) {  
            fprintf(stderr, "fcntl: %s\n", strerror(errno));  
            return -1;  
        }  
        return 0;  
    }  
    /** 
     * \brief     acquire, release or test for the existence of record locks 
     * \details   if a conficling is held on the file, then wait for that 
     *            lock to be release 
     * 
     * \param     fd - file descriptor 
     * \param     locktype - lock type: F_RDLCK, F_WRLCK, F_UNLCK. 
     * 
     * \return    0 is success, < 0 is failed. 
     */  
    int sln_filelock_wait_set(int fd, short locktype)  
    {  
        struct flock    lock;  
        lock.l_type = locktype;  
        lock.l_whence = SEEK_SET;  
        lock.l_start = 0;  
        lock.l_len = 0;  
        lock.l_pid = getpid();  
        if (fcntl(fd, F_SETLKW, &lock) < 0) {  
            fprintf(stderr, "fcntl: %s\n", strerror(errno));  
            return -1;  
        }  
        return 0;  
    }  
    int sln_file_wrlock(int fd)  
    {  
        return sln_filelock_set(fd, F_WRLCK);  
    }  
    int sln_file_rdlock(int fd)  
    {  
        return sln_filelock_set(fd, F_RDLCK);  
    }  
    int sln_file_wait_wrlock(int fd)  
    {  
        return sln_filelock_wait_set(fd, F_WRLCK);  
    }  
    int sln_file_wait_rdlock(int fd)  
    {  
        return sln_filelock_wait_set(fd, F_RDLCK);  
    }  
    int sln_file_unlock(int fd)  
    {  
        return sln_filelock_set(fd, F_UNLCK);  
    }  
    /** 
     * \brief     test for the existence of record locks on the file 
     * \details   none 
     * 
     * \param     fd - file descriptor 
     * \param     locktype - lock type: F_RDLCK, F_WRLCK. 
     * 
     * \return    0 is success, < 0 is failed. 
     */  
    static int sln_filelock_test(int fd, short locktype)  
    {  
        struct flock    lock;  
        lock.l_type = locktype;  
        lock.l_whence = 0;  
        lock.l_start = 0;  
        lock.l_len = 0;  
        lock.l_pid = 0;  
        if (fcntl(fd, F_GETLK, &lock) < 0) {  
            fprintf(stderr, "fcntl: %s\n", strerror(errno));  
            return -1;  
        }  
        if (lock.l_type != F_UNLCK) { //file is locked  
            if (F_WRLCK == lock.l_type) {  
                printf("write lock hold by process <%d>, lock_type: %d\n", lock.l_pid, lock.l_type);  
            } else if (F_RDLCK == lock.l_type) {  
                printf("read lock hold by process <%d>, lock_type: %d\n", lock.l_pid, lock.l_type);  
            }  
            return lock.l_pid; //return the pid of process holding that lock  
        } else {  
            printf("Unlock, lock type: %d\n", lock.l_type);  
            return 0;  
        }  
    }  
    int sln_file_wrlock_test(int fd)  
    {  
        return sln_filelock_test(fd, F_WRLCK);  
    }  
    int sln_file_rdlock_test(int fd)  
    {  
        return sln_filelock_test(fd, F_RDLCK);  
    }  


下面來實現三個進程，三個進程針對同一個文件進行讀、寫、測試鎖操作：
寫進程：
[cpp] view plain copy
在CODE上查看代碼片派生到我的代碼片

    #include <stdio.h>  
    #include <sys/types.h>  
    #include <unistd.h>  
    #include <fcntl.h>  
    #include <string.h>  
    #include <errno.h>  
    #include "filelock.h"  
    int main(int argc, const char *argv[])  
    {  
        int             fd;  
        if (argc != 2) {  
            fprintf(stderr, "Usage: %s <write_str>\n", argv[0]);  
            return -1;  
        }  
        fd = open("filelock.txt", O_RDWR | O_CREAT, 0644);  
        if (fd < 0) {  
            fprintf(stderr, "open: %s\n", strerror(errno));  
            return -1;  
        }  
        if (sln_file_wait_wrlock(fd) < 0) {  
            printf("lock write failed!\n");  
            close(fd);  
            return -1;  
        }  
        printf("process <%d> holding write lock ok!\n", getpid());  
        write(fd, argv[1], strlen(argv[1]));  
        sleep(10);  
        sln_file_unlock(fd);  
        printf("release lock!\n");  
        close(fd);  
        return 0;  
    }  


讀進程：
[cpp] view plain copy
在CODE上查看代碼片派生到我的代碼片

    #include <stdio.h>  
    #include <sys/types.h>  
    #include <unistd.h>  
    #include <fcntl.h>  
    #include <string.h>  
    #include <errno.h>  
      
    #include "filelock.h"  
      
    int main(int argc, const char *argv[])  
    {  
        int             fd;  
        char            buf[1024];  
      
        fd = open("filelock.txt", O_RDONLY | O_CREAT, 0644);  
        if (fd < 0) {  
            fprintf(stderr, "open: %s\n", strerror(errno));  
            return -1;  
        }  
      
        if (sln_file_wait_rdlock(fd) < 0) {  
            printf("lock read failed!\n");  
            close(fd);  
            return -1;  
        }  
        printf("process <%d> holding read lock ok!\n", getpid());  
        sleep(10);  
        read(fd, buf, sizeof(buf));  
        printf("read buf: %s\n", buf);  
        sln_file_unlock(fd);  
        printf("release lock!\n");  
      
        close(fd);  
        return 0;  
    }  

測試讀寫鎖進程：
[cpp] view plain copy
在CODE上查看代碼片派生到我的代碼片

    #include <stdio.h>  
    #include <sys/types.h>  
    #include <unistd.h>  
    #include <fcntl.h>  
    #include <string.h>  
    #include <errno.h>  
    #include "filelock.h"  
    int main(int argc, const char *argv[])  
    {  
        int             fd, pid;  
        fd = open("filelock.txt", O_RDWR | O_CREAT, 0644);  
        if (fd < 0) {  
            fprintf(stderr, "open: %s\n", strerror(errno));  
            return -1;  
        }  
        pid = sln_file_wrlock_test(fd);  
        if (pid > 0) {  
            printf("write locked!\n");  
        } else {  
            printf("write unlock!\n");  
        }  
        pid = sln_file_rdlock_test(fd);  
        if (pid > 0) {  
            printf("read locked!\n");  
        } else {  
            printf("read unlock!\n");  
        }  
        close(fd);  
        return 0;  
    }  


編譯運行三個程序。
為了測試效果，在進程讀和寫sleep 10秒鐘。

```sh
1. 在寫鎖存在是，讀鎖是無法持有的。測試：先運行寫進程，在運行讀進程，讀進程將在寫進程釋放鎖時獲得鎖。
2. 在讀鎖存在時，寫鎖是無法持有的。測試：先運行讀進程，再運行寫進程，寫進程將在讀進程釋放鎖時獲得鎖。
3. 在讀鎖存在時，另外的讀鎖可以繼續持有。測試：先後運行兩個讀進程，後運行的讀進程可以立即獲得讀鎖。
4.上面的測試也可可以通過測試讀寫鎖進程得到結果。
```

http://download.csdn.net/detail/gentleliu/8332973

