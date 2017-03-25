# Linux 下 IPC 的運作方式


IPC溝通的有許多方式，例如：Shared Memory, Message Queue, PIPE, FIFO, Unix Socket 等等。
下面將會整理 Linux 下常見的 IPC 的運作方式舉例。



##一、Shared Memory
主要有四個 API： 

- int shmget(key_t key, size_t size, int shmflg);
- int shmctl(int shmid, int cmd, struct shmid_ds *buf); 
- void *shmat(int shmid, const void *shmaddr, int shmflg); 
- int shmdt(const void *shmaddr); 

用法說明：

- 使用函數 key_t ftok(char *name, int id) 取得一個在此系統內唯一的值，假設為 vKey。
- 使用函數 vShmId = shmget(vKey, 1024, 0600)取得一個大小為1024bytes, 權限為 0600(read+write) 的Shared Memory 。
- 使用函數 pShmPtr = shmat(vShmId,0,0) 取得 Shared Memory 對應的指標，以供後續操作。
- 當不需要使用時，使用 shmctl(vShmId, IPC_RMID, 0) 移除此 Shared Memory 。


舉例： 
請參考 http://www.cs.cf.ac.uk/Dave/C/node27.html 

- shm_server.c

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define SHMSZ     27

main()
{
    char c;
    int shmid;
    key_t key;
    char* shm, *s;

    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char*) - 1) {
        perror("shmat");
        exit(1);
    }

    /*
     * Now put some things into the memory for the
     * other process to read.
     */
    s = shm;

    for (c = 'a'; c <= 'z'; c++) {
        *s++ = c;
    }

    *s = NULL;

    /*
     * Finally, we wait until the other process
     * changes the first character of our memory
     * to '*', indicating that it has read what
     * we put there.
     */
    while (*shm != '*') {
        sleep(1);
    }

    exit(0);
}
```

- shm_client.c

```c
/*
 * shm-client - client program to demonstrate shared memory.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define SHMSZ     27

main()
{
    int shmid;
    key_t key;
    char* shm, *s;

    /*
     * We need to get the segment named
     * "5678", created by the server.
     */
    key = 5678;

    /*
     * Locate the segment.
     */
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char*) - 1) {
        perror("shmat");
        exit(1);
    }

    /*
     * Now read what the server put in the memory.
     */
    for (s = shm; *s != NULL; s++) {
        putchar(*s);
    }

    putchar('\n');

    /*
     * Finally, change the first character of the
     * segment to '*', indicating we have read
     * the segment.
     */
    *shm = '*';

    exit(0);
}

```

##二、Message Queue 的基本用法
參考  http://tldp.org/LDP/lpg/node27.html，主要有四個API
int msgget ( key_t key, int msgflg );
int msgsnd ( int msqid, struct msgbuf *msgp, int msgsz, int msgflg );
int msgrcv ( int msqid, struct msgbuf *msgp, int msgsz, long mtype, int msgflg );
int msgctl ( int msgqid, int cmd, struct msqid_ds *buf );
用法說明：

- 使用函數 key_t ftok(char *name, int id) 取得一個在此系統內唯一的值，假設為 vKey。當然這個值也可以自行指定。
- 使用函數 vQueueId = msgget( vKey, IPC_CREAT | 0660 ) 取得此 message queue 對應的 ID。
- 定義將傳送的 message 資料結構，假設是 tMsgBuf vxMsgBuf。此處需注意的是第一個欄位需定義為 long mtype, 舉例如下:

```c
struct msgbuf
{
  long mtype;
  char mtext[1];
};
```

 
- 使用函數 msgsnd(vQueueId, &vxMsgBuf, sizeof(tMsgBuf), 0) 將資料送往 message queue。
- 使用函數 msgrcv(vQueueId, &vxMsgBuf, sizeof(tMsgBuf), mtype, 0)) 取得 message queue 內, 定義為 mtype 的資料。

舉例：

```c
#define MSG_KEY 9999
#define MSG_SIZE 1024
struct tMsgBuf
{
     long mtype;
     char mtext[MSG_SIZE];
};

typedef struct tMsgBuf tMessageBuffer; 

main()
{
     tMessageBuffer  sndmsg={0} ;
     tMessageBuffer  recvmsg={0}  ;
     msqid = msgget(MSG_KEY , PERMS | IPC_CREAT));
     msgsnd(msqid, &sndmsg, MSG_SIZE, 0);
     msgrcv(msqid, &recvmsg, MSG_SIZE, 0, 0)
}
```

##三、PIPE
只需要一個 API，但是其只適用於 parent process 與 child process
int pipe(int fd[2]);
用法說明：

- 使用 pipe() 建立兩個 file descripter，第一個 FD 用來從PIPE讀取資料，第二個FD 用來作為寫入資料至PIPE，如下例： 

```c
#include<unistd.h>
int fds[2];
int pipe(fds);
```

- 接著呼叫 fork ，因為child process 會擁有同樣的 FD，因此便可以透過這兩個 FD 進行溝通。
舉例：
請參考
http://www.vr.ncue.edu.tw/esa/EmbeddedSystemProgramming2010/ch05.htm

##四、FIFO (Named Pipe)
使用以下幾個 API

```c
int mkfifo(const char *pathname, mode_t mode);
int open(const char *pathname, int flags);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int close(int fd);
int unlink(const char *pathname);
```

用法說明：

###1. 定義 FIFO 對應的檔案

```c
int client_to_server;
char *myfifo = "/tmp/client_to_server_fifo";
int server_to_client;
char *myfifo2 = "/tmp/server_to_client_fifo";
```

###2. 使用 mknod 或 mkfifo 建立 FIFO 對應的檔案

```c
mkfifo(myfifo, 0666);
mkfifo(myfifo2, 0666);
```

###3. 開啟檔案以供讀取或寫入

```c
client_to_server = open(myfifo, O_RDONLY);
server_to_client = open(myfifo2, O_WRONLY); 
```

###4. 寫入資料，或讀取資料

```c
read(client_to_server, buf, BUFSIZ);
write(server_to_client,buf,BUFSIZ);
```

### 5. 關閉檔案，並刪除 FIFO 檔案

```c
close(client_to_server);
close(server_to_client);
unlink(myfifo);
unlink(myfifo2);
```

舉例：
請參考
http://stackoverflow.com/questions/8611035/proper-fifo-client-server-connection

##五、Unix Socket

此處socket應用與一般撰寫網路應用相同，只是此時改成使用AF_UNIX address family，設定的資訊由 sockaddr_in 的 port, address 改為 sockaddr_un 的 path 
可能會用到的 API 列舉如下： 

```c
sockfd = socket(int socket_family, int socket_type, int protocol);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int send(int s, const char *msg, int len, int flags),
int recv(int s, char *buf, int len, int flags)
int close(int fd);
```

- server.c

```c
/* server.c */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

int port = 8111;

int main(void)
{
    struct sockaddr_in    sin;
    struct sockaddr_in    pin;
    int   mysock;
    int   tempsock;
    int   addrsize;
    char  str[100], str1[20], str2[20], str3[20];
    char  buf[100];
    int   i, len1, len2;
    float c;

    mysock = socket(AF_INET, SOCK_STREAM, 0);

    if (mysock == -1) {
        perror("call to socket");
        exit(1);
    }

    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);

    if (bind(mysock, (struct sockaddr*)&sin, sizeof(sin)) == -1) {
        perror("call to bind");
        exit(1);
    }

    if (listen(mysock, 20) == -1) {
        perror("call to listen");
        exit(1);
    }

    printf("Accepting connections ...\n");

    while (1) {
        tempsock = accept(mysock, (struct sockaddr*)&pin, &addrsize);

        if (tempsock == -1) {
            perror("call to accept");
            exit(1);
        }

        len1 = recv(tempsock, str, 100, 0);
        printf("\n收到字元數: %d\n", len1);
        str[len1] = 0;

        printf("received from client: %s\n", str);

        if (len1 > 0) {
            strcpy(str1, strtok(str, " "));
            printf("第 1 個字串為: %s\n", str1);
            strcpy(str2, strtok(NULL, " "));
            printf("第 2 個字串為: %s\n", str2);
            strcpy(str3, strtok(NULL, " "));
            printf("第 3 個字串為: %s\n", str3);
            c = atof(str3) * 1.05;
            sprintf(buf, "品號為 %s\n品名為 %s\n含稅價為: %.2f\n", str1, str2, c);
        }

        len2 = strlen(buf);

        if (send(tempsock, buf, len2, 0) == -1) {
            perror("call to send");
            exit(1);
        }

        close(tempsock);
    }

    return 0;
}
```

- client.c

```c
/* client.c */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int port = 8111;

int main(int argc, char* argv[])
{
    struct sockaddr_in    pin;
    int   mysock;
    char  buf[8192];
    char*  str = "A001 電視機 20000.00 ";

    if (argc < 2) {
        printf("使用方法: client 字串\n");
        printf("使用預設字串\n");
    } else {
        str = argv[1];
    }

    bzero(&pin, sizeof(pin));
    pin.sin_family = AF_INET;
    pin.sin_addr.s_addr = inet_addr("192.168.2.163");
    pin.sin_port = htons(port);

    mysock = socket(AF_INET, SOCK_STREAM, 0);

    if (mysock == -1) {
        perror("call to socket");
        exit(1);
    }

    if (connect(mysock, (void*)&pin, sizeof(pin)) == -1) {
        perror("call to connect");
        exit(1);
    }

    printf("Sending message %s to server ...\n", str);

    if (send(mysock, str, strlen(str), 0) == -1) {
        perror("Error in send\n");
        exit(1);
    }


    if (recv(mysock, buf, 8192, 0) == -1) {
        perror("Error in receiving\n");
        exit(1);
    }

    printf("\nResponse from server: \n\n%s\n", buf);

    close(mysock);
    return 0;
}
```

## 法說明：

請直接參考 http://learn.akae.cn/media/ch37s04.html
舉例：

##請參考
http://learn.akae.cn/media/ch37s04.htmlhttp://www.cs.cf.ac.uk/Dave/C/node28.html 

##參考資料：
- Shared Memory http://www.cs.cf.ac.uk/Dave/C/node27.html
- Message Queue http://tldp.org/LDP/lpg/node27.html
- PIPE http://www.vr.ncue.edu.tw/esa/EmbeddedSystemProgramming2010/ch05.htm
 -取得 key http://www.360doc.com/content/11/1125/09/1317564_167200026.shtml                         
