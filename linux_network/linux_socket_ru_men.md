# Linux Socket 入門


在兩台LINUX系統下使用socket進行傳輸。

首先介紹client端:

既然要使用socket那麼當然要先include它的函式庫

![](./images/4a83b89243c39.png)

```c
#include <sys/socket.h>
#include <netinet/in.h>

```

## client的程式碼

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
using namespace std;

int main()
{
    char ip[32] = "127.0.0.1";
    struct sockaddr_in dest;
    char buffer[256];
    char res[256] = "client-res";

    //初始
    bzero(&dest, sizeof(dest));
    dest.sin_family = PF_INET;
    dest.sin_port = htons(8889);//port
    dest.sin_addr.s_addr = inet_addr(ip);//ip
    printf("--------initial!--------\n");

    int sockfd;
    //建立
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    //連線
    connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));
    printf("---connection success---\n");
    //發送訊息
    send(sockfd, res, sizeof(res), 0);
    //接收回傳訊息
    bzero(buffer, 256);//清除
    recv(sockfd, buffer, sizeof(buffer), 0);//接收
    printf("server: %s\n", buffer);

    close(sockfd);
    return 0;

}
```

## server的程式碼

```c
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netdb.h>
#include <strings.h>
#include <unistd.h>
using namespace std;

int main()
{
    int sockfd, res;
    struct sockaddr_in dest;
    char Hello[256] = "Hello client!";
    char buffer[256];

    //建立
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    //初始化
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(8889);//port
    dest.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr*)&dest, sizeof(struct sockaddr));
    listen(sockfd, 20);
    printf("--------initial!--------\n");
    int clientfd;
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    //等待連線
    printf("wait..\n");
    clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
    printf("---connection success---\n");
    //接收
    res = recv(clientfd, buffer, sizeof(buffer), 0);
    printf("client: %s, %d bytes\n", buffer, res);
    //回傳
    send(clientfd, Hello, sizeof(Hello), 0);
    //close(clientfd);
    close(sockfd);
    return 0;
}
```

##執行結果

###client端:
```
--------initial!--------
---connection success---
server: Hello client!
```
 

###Server端:

```
--------initial!--------
wait..
---connection success---
client: client-res, 256 bytes
```