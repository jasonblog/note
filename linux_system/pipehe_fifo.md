# pipe和FIFO


在unix系統上最早的IPC形式為管道，管道的創建使用pipe函數：

```sh
#include <unistd.h>
int pipe(int pipefd[2]);
```

該函數創建一個單向的管道，返回兩個描述符 pipefd[0],和pipefd[1]，pipefd[0]用於讀操作，pipefd[1]用於寫操作。該函數一般應用在父子進程（有親緣關係的進程）之間的通信，先是一個進程創建管道，再fork出一個子進程，然後父子進程可以通過管道來實現通信。

管道具有以下特點：
管道是半雙工的，數據只能向一個方向流動；需要雙方通信時，需要建立起兩個管道；
只能用於父子進程或者兄弟進程之間（具有親緣關係的進程）；

單獨構成一種獨立的文件系統：管道對於管道兩端的進程而言，就是一個文件，但它不是普通的文件，它不屬於某種文件系統，而是自立門戶，單獨構成一種文件系統，並且只存在與內存中。

數據的讀出和寫入：一個進程向管道中寫的內容被管道另一端的進程讀出。寫入的內容每次都添加在管道緩衝區的末尾，並且每次都是從緩衝區的頭部讀出數據。


函數pipe一般使用步驟如下：

```sh
1.pipe創建管道；
2.fork創建子進程；
3.父子進程分別關閉掉讀和寫（或寫和讀）描述符；
4.讀端在讀描述符上開始讀（或阻塞在讀上等待寫端完成寫），寫端開始寫，完成父子進程通信過程。
一個簡單的通信實現（來自linux man手冊的修改）：
```


```c
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char* argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf[128];
    int readlen;


    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return -1;
    }

    if (pipe(pipefd) < 0) {
        fprintf(stderr, "pipe: %s\n", strerror(errno));
        return -1;
    }

    cpid = fork();

    if (cpid < 0) {
        fprintf(stderr, "fork: %s\n", strerror(errno));
        return -1;
    }

    if (0 == cpid) { /* 子進程 */
        close(pipefd[1]); /* 子進程關閉寫端 */
        readlen = read(pipefd[0], buf,
                       128); //子進程阻塞在讀上，等待父進程寫

        if (readlen < 0) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            return -1;
        }

        write(STDOUT_FILENO, buf, readlen);
        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]); //讀完之後關閉讀描述符
        return 0;
    } else { /* 父進程 */
        close(pipefd[0]); /*父進程關閉沒用的讀端 */
        sleep(2);
        write(pipefd[1], argv[1], strlen(argv[1])); //父進程開始寫
        close(pipefd[1]); /* 父進程關閉寫描述符 */
        wait(NULL); /* 父進程等待子進程退出，回收子進程資源 */
        return 0;
    }
}
```

運行時將打印命令行輸入參數，打印將在父進程睡眠2秒之後，子進程將阻塞在讀，直到父進程寫完數據，可見管道是有同步機制的，不需要自己添加同步機制。如果希望兩個進程雙向數據傳輸，那麼需要建立兩個管道來實現。

管道最大的劣勢就是隻能在擁有共同祖先進程的進程之間通信，在無親緣關係的兩個進程之間沒有辦法使用，不過有名管道FIFO解決了這個問題。FIFO類似於pipe，也是隻能單向傳輸數據，不過和pipe不同的是他可以在無親緣關係的進程之間通信，它提供一個路徑與之關聯，所以只要能訪問該路徑的進程都可以建立起通信，類似於前面的共享內存，都提供一個路徑與之關聯。

```sh
#include <sys/types.h>
#include <sys/stat.h>
int mkfifo(const char *pathname, mode_t mode);
```

pathname 為系統路徑名，mode為文件權限位，類似open函數第二個參數。

打開或創建一個新的fifo是先調用mkfifo，當指定的pathname已存在fifo時，mkfifo返回EEXIST錯誤，此時再調用open函數。

下面來使用mkfifo實現一個無親緣關係進程間的雙向通信，此時需要建立兩個fifo，分別用於讀寫。服務進程循環的讀並等待客戶進程寫，之後打印客戶進程傳來數據並向客戶進程返回數據；客戶進程向服務器寫數據並等待讀取服務進程返回的數據。

server process：


```c
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "slnipc.h"
int main(int argc, const char* argv[])
{
    int rc;
    int wr_fd, rd_fd;
    char sendbuf[128];
    char recvbuf[128];
    rc = mkfifo(SLN_IPC_2SER_PATH, O_CREAT | O_EXCL); //建立服務進程讀的fifo

    if ((rc < 0) && (errno != EEXIST)) {
        fprintf(stderr, "mkfifo: %s\n", strerror(errno));
        return -1;
    }

    rc = mkfifo(SLN_IPC_2CLT_PATH, O_CREAT | O_EXCL); //建立服務進程寫的fifo

    if ((rc < 0) && (errno != EEXIST)) {
        fprintf(stderr, "mkfifo: %s\n", strerror(errno));
        return -1;
    }

    wr_fd = open(SLN_IPC_2CLT_PATH, O_RDWR, 0);

    if (wr_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    rd_fd = open(SLN_IPC_2SER_PATH, O_RDWR, 0);

    if (rd_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }

    for (;;) {
        rc = read(rd_fd, recvbuf,
                  sizeof(recvbuf)); //循環等待接受客戶進程數據

        if (rc < 0) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            continue;
        }

        printf("server recv: %s\n", recvbuf);
        snprintf(sendbuf, sizeof(sendbuf), "Hello, this is server!\n");
        rc = write(wr_fd, sendbuf, strlen(sendbuf));

        if (rc < 0) {
            fprintf(stderr, "write: %s\n", strerror(errno));
            continue;
        }
    }

    close(wr_fd);
    close(rd_fd);
    return 0;
}
```

client process：

```c
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "slnipc.h"
int main(int argc, const char *argv[])
{
    int rc;
    int rd_fd, wr_fd;
    char recvbuf[128];
    char sendbuf[128];
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return -1;
    }
    snprintf(sendbuf, sizeof(sendbuf), "%s", argv[1]);
    wr_fd = open(SLN_IPC_2SER_PATH, O_RDWR, 0);
    if (wr_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }
    rd_fd = open(SLN_IPC_2CLT_PATH, O_RDWR, 0);
    if (rd_fd < 0) {
        fprintf(stderr, "open: %s\n", strerror(errno));
        return -1;
    }
    rc = write(wr_fd, sendbuf, strlen(sendbuf));
    if (rc < 0) {
        fprintf(stderr, "write: %s\n", strerror(errno));
        return -1;
    }
    rc = read(rd_fd, recvbuf, sizeof(recvbuf));
    if (rc < 0) {
        fprintf(stderr, "write: %s\n", strerror(errno));
        return -1;
    }
    printf("client read: %s\n", recvbuf);
    close(wr_fd);
    close(rd_fd);
    return 0;
}
```

服務器先啟動運行，之後運行客戶端，運行結果

```sh
# ./server 
server recv: hi,this is fifo client 
```

```sh
# ./client "hi,this is fifo client" 
client read: Hello, this is server! 
```


這裡有一些類似於socket實現進程間通信過程，只是fifo的讀寫描述符是兩個，socket的讀寫使用同一個描述符。fifo的出現克服了管道的只能在有親緣關係的進程之間的通信。和其他的進程間通信一直，fifo傳送的數據也是字節流，需要自己定義協議格式來解析通信的數據，可以使用socket章節介紹的方式來實現的通信協議。

本節源碼下載：
http://download.csdn.net/detail/gentleliu/8183027
