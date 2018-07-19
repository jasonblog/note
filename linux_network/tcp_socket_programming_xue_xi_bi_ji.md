# TCP Socket Programming 學習筆記




有別於 IPC ，Socket 是用於網路上不同程序的互相溝通，比如說流覽器要怎麼跟 Web Server 拿取資料、Messenger 訊息的收發、 ftp 檔案的上傳與下載等等，在現今的網路編程中，Socket 可以說是無所不在。

至今 Socket 也應不同的需求或 OS 衍生出了不少版本，這篇筆計主要是討論 Linux 的 socket ，並專注在實現 TCP 編程：

![](./images/4a83b89243c39.png)


##先從建立一個 Socket 出發
使用socket(int,int,int)，它能幫助我們在kernel中建立一個socket，並傳回對該socket的檔案描述符。

###Prototype

```c
int socket(int domain, int type, int protocol);
```

###Arguments
####domain
定義了socket要在哪個領域溝通，常用的有2種：

- AF_UNIX/AF_LOCAL：用在`本機程序與程序`間的傳輸，讓兩個程序共享一個檔案系統(file system)
- AF_INET , AF_INET6 ：讓兩台主機`透過網路`進行資料傳輸，AF_INET使用的是IPv4協定，而AF_INET6則是IPv6協定。

####type
說明這個socket是傳輸的手段為何：

- SOCK_STREAM：提供一個序列化的連接導向位元流，可以做位元流傳輸。對應的protocol為TCP。
- SOCK_DGRAM：提供的是一個一個的資料包(datagram)，對應的protocol為UDP

####protocol
設定socket的協定標準，一般來說都會設為0，讓kernel選擇type對應的默認協議。

####Return Value

成功產生socket時，會返回該socket的檔案描述符(socket file descriptor)，我們可以透過它來操作socket。若socket創建失敗則會回傳-1(INVALID_SOCKET)。

#### Example

```c
#include<stdio.h>
#include<sys/socket.h>

int main(int argc , char *argv[])
{
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_fd == -1){
        printf("Fail to create a socket.");
    }

    return 0;
}
```

### 從Client連向Server

客戶端要連向伺服端，需要先知道並儲存伺服端的IP及port，netinet/in.h已經為我們定義好了一個struct sockaddr_in來儲存這些資訊：


```c
// IPv4 AF_INET sockets:
// IPv6參見 sockaddr_in6
struct sockaddr_in {
    short            sin_family;   // AF_INET,因為這是IPv4;
    unsigned short   sin_port;     // 儲存port No
    struct in_addr   sin_addr;     // 參見struct in_addr
    char             sin_zero[8];  // Not used, must be zero */
};

struct in_addr {
    unsigned long s_addr;          // load with inet_pton()
};
```

有了IP跟port，我們就能使用connect(int struct sockaddr, int)進行客戶端與伺服端之間的連線。

###Prototype

```c
int connect(int sd, struct sockaddr *server, int addr_len);
```

### Arguments

#### sd
sd是socket的描述符，即是前個Example的sockfd

#### server

負責提供關於這個socket的所有信息，以下是一個簡單的設定例子：

```c
struct sockaddr_in info;

bzero(&info,sizeof(info));//初始化，將struct涵蓋的bits設為0
info.sin_family = PF_INET;//sockaddr_in為Ipv4結構
info.sin_addr.s_addr = inet_addr("123.123.13.12");//IP address
info.sin_port = htons(8080);
```

### iner_addr()是什麼東東？


```sh
210.25.132.181屬於IP地址的ASCII表示法，也就是字符串形式。英語叫做IPv4 numbers-and-dots notation。

如果把210.25.132.181轉換為整數形式，是3524887733，這個就是整數形式的IP地址。
英語叫做binary data。（其實binary是二進制的意思）
– 出自《Linux大棚》
```

inet_addr() `便負責將字串型式的IP轉換為整數型式的IP`。

### 那htons()又做了什麼？

首先，我們要知道網路端的字節序與本機端的字節序可能不一致。

網路端總是用Big endian，而本機端卻要視處理器體系而定，比如x86就跟網路端的看法不同，使用的是Little endian。

而htons()就是Host TO Network Short integer的縮寫，它將本機端的字節序(endian)轉換成了網路端的字節序。根據這個命名法來推斷，htons()應該還有一群好朋友：htonl()、ntohl()、ntohs()，不過這又是另一個故事了。

###addr_len
它的意義簡單明瞭，就是*server的大小

###Return value
嗯……如果成功了就回傳0，不然就回傳-1


###Example
乘接上面所有內容，目前進度已經達到2/5。

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc , char *argv[])
{
	//socket的建立
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線
    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(8080);
    
    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error");
    }
    
    return 0;
}
```

##接收資料
Socket存在多種接收方式，從最基本的read()到其衍生的recv()、recvfrom()、recvfmsg()等。這邊主要介紹`recv()`的功能

### Prototype

```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

### Arguments

#### sockfd

sd是socket的描述符，即是前個Example的sockfd

#### buf
一個緩衝區，讓Socket能把接收到的資料塞進裡頭。

#### len
即是buf的大小

#### flags
相比最基本的read()，recv()的參數中多了這個旗標，flags代表接收的相關細節，通常是設定為0，也存在其他巨集處理一些特殊要求，比如blocking/nonblocking與超額接收等等，這部分細節可參見man-pages。

### Return value
recv()會回傳接收到了多少個位元組，若在接收時發生的錯誤則會傳回-1。

我想值得一提的是回傳0的情形，可以考慮為以下三者：

- 就真的是 0 bytes
- 兩方Socket設定的domain不一致，比如一方為網路一方為本機
- 當一方在正常情況下結束連線，也會回傳 0 (end-of-file)

### Example

```c
char buffer[100];
recv(sockfd,buf,sizeof(buf),0);
```

## 傳送資料



`Socket的傳送與接收大同小異`，同樣有很多傳送手段(如write()、send()、sendto()等等)，也都是給定三個東西：

- Socket 描述符
- 一個緩衝區
- 緩衝區的大小

`就差在接收是把資料往緩衝區裡塞，而傳送是把緩衝區向外倒而已`。這裡主要介紹

### Prototype

```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

### Arguments


####sockfd
sockfd是socket的描述符，即是前個Example的sockfd

####buf
一個緩衝區，讓Socket能把接收到的資料塞進裡頭。

####len
即是buf的大小

####flags
相比最基本的write()，send()的參數中多了這個旗標，flags代表接收的相關細節，通常是設定為0，也存在其他巨集處理一些特殊要求，比如blocking/nonblocking與超額傳輸等等，這部分細節可參見man-pages。


###Return value
如果輸送成功，會回傳共送出了多少個位元組，傳輸失敗則回傳-1。

###Example

```c
char buf[] = "Hi there!";
send(sockfd,buf,sizeof(buf),0);
```

##綁定Server端的地址
`connect()讓我們到別人家去取用資料，bind()則是把自己家地址綁在Socket身上`。不論當Clinet或當Server，我們都需要給Socket一份地址，同樣的，能以結構sockaddr_in來儲存資料。



###Prototype

```c
int bind(int sockfd, struct sockaddr* addr, int addrlen);
```

###Arguments
####sockfd
socket的描述符

####addr
與connetc()的 server 的設定大同小異，值得一提的是s_addr，這部分常有人設定為info.sin_addr.s_addr = INADDR_ANY，INADDR_ANY表示我不在乎loacl IP是什麼，讓kernel替我決定就好。

####addrlen
即是 *addr的大小

###Return value
0表示綁定成功，-1則表失敗


## 設置Server的監聽隊列
如果說IP對應到一座城市，Port就是城市的港口。在bind()告訴別人我們住在哪裡，並該從哪座港口登陸後，我們必需反覆去查看客人來了沒有，這個過程就稱為監聽，對應到了Socket的listen()。

由於Server一次只能服務一個人，當港口出入頻繁時，我們得讓來客照拜訪的先後排成隊列，即是說每當一個請求送到Server，Socket就會把它丟到監聽隊列的尾端。


###Prototype

```c
int listen(int sockfd, int backlog);
```

###Argument
####sockfd
sockfd是socket的描述符，即是前個Example的sockfd

####backlog
規定最多能有幾個人能連入server，即時說這個隊列究竟有多大

###Return value
成功為0，產生錯誤則回傳-1

###Example

```c
listen(sockfd, 5);
```

## Server 接收請求
Socket在港口等了又等，終於有客人拜訪了，我們可以用函式accept()去接見這名客人。當accept()被調用時，它會為該請求產生出一個新的Socket，並把這個請求從監聽隊列剔除掉。

###Prototype

```c
int accept(int sockfd, struct sockaddr addr, socklen_t addrlen);
```

###Argument
####sockfd
毫無反應，就是個socket的描述符

####addr
一樣是個描述Socket資訊的結構，不過他是一個空容器，用於儲存接收到的Client端相關資訊，比如port、IP等等。

####addrlen
描述的是addr的大小

###Return value
它會傳回一個新的Socket描述符，以後和Client端交談的是這個新創出的Socket，如果失敗則傳回-1(INVALID_SOCKET)

###Example

```c
int forClientSockfd;
struct sockaddr_in client_info;
int addrlen = sizeof(client_addr);

forClientSockfd = accept(sockfd,(structsockaddr*) &client_addr, &addrlen);
```

## Server 範例程式

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc , char *argv[])

{
    //socket的建立
    char inputBuffer[256] = {};
    char message[] = {"Hi,this is server.\n"};
    int sockfd = 0,forClientSockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線
    struct sockaddr_in serverInfo,clientInfo;
    int addrlen = sizeof(clientInfo);
    bzero(&serverInfo,sizeof(serverInfo));

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(8700);
    bind(sockfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));
    listen(sockfd,5);

    while(1){
        forClientSockfd = accept(sockfd,(struct sockaddr*) &clientInfo, &addrlen);
        send(forClientSockfd,message,sizeof(message),0);
        recv(forClientSockfd,inputBuffer,sizeof(inputBuffer),0);
        printf("Get:%s\n",inputBuffer);
    }
    return 0;
}
```

## Client 範例程式

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc , char *argv[])
{

    //socket的建立
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    //localhost test
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(8700);


    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error");
    }


    //Send a message to server
    char message[] = {"Hi there"};
    char receiveMessage[100] = {};
    send(sockfd,message,sizeof(message),0);
     recv(sockfd,receiveMessage,sizeof(receiveMessage),0);

    printf("%s",receiveMessage);
    printf("close Socket\n");
    close(sockfd);
    return 0;
}
```



