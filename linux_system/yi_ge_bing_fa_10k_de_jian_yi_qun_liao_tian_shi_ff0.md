# 一個並發10k的簡易群聊天室（使用epoll）


- local.h

```c
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <list>
#include <string.h>

#define BUF_SIZE 1024 //默認緩衝區
#define SERVER_PORT 8888 //監聽端口
//#define SERVER_HOST "192.168.34.15" //服務器IP地址
#define SERVER_HOST "127.0.0.1" //服務器IP地址
#define EPOLL_RUN_TIMEOUT -1 //epoll的超時時間
#define EPOLL_SIZE 10000 //epoll監聽的客戶端的最大數目

#define STR_WELCOME "Welcome to seChat! You ID is: Client #%d"
#define STR_MESSAGE "Client #%d>> %s"
#define STR_NOONE_CONNECTED "Noone connected to server except you!"
#define CMD_EXIT "EXIT"

//兩個有用的宏定義：檢查和賦值並且檢測
#define CHK(eval) if(eval < 0){perror("eval"); exit(-1);}
#define CHK2(res, eval) if((res = eval) < 0){perror("eval"); exit(-1);}

//================================================================================================
//函數名： setnonblocking
//函數描述： 設置socket為不阻塞
//輸入： [in] sockfd socket標示符
//輸出： 無
//返回： 0
//================================================================================================
int setnonblocking(int sockfd);

//================================================================================================
//函數名： handle_message
//函數描述： 處理每個客戶端socket
//輸入： [in] new_fd socket標示符
//輸出： 無
//返回： 返回從客戶端接受的數據的長度
//================================================================================================
int handle_message(int new_fd);
```

- utils.h

```c
#include "local.h"

int setnonblocking(int sockfd)
{
    CHK(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK));
    return 0;
}
```

- server.cpp

```cpp
#include "local.h"
#include "utils.h"

using namespace std;

// 存放客戶端socket描述符的list
list<int> clients_list;

int main(int argc, char* argv[])
{
    int listener; //監聽socket
    struct sockaddr_in addr, their_addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
    socklen_t socklen;
    socklen = sizeof(struct sockaddr_in);

    static struct epoll_event ev, events[EPOLL_SIZE];
    ev.events = EPOLLIN | EPOLLET; //對讀感興趣，邊沿觸發

    char message[BUF_SIZE];

    int epfd; //epoll描述符
    clock_t tStart; //計算程序運行時間

    int client, res, epoll_events_count;

    CHK2(listener, socket(PF_INET, SOCK_STREAM, 0)); //初始化監聽socket
    setnonblocking(listener); //設置監聽socket為不阻塞
    CHK(bind(listener, (struct sockaddr*)&addr,
             sizeof(addr)));  //綁定監聽socket
    CHK(listen(listener, 1)); //設置監聽

    CHK2(epfd, epoll_create(
             EPOLL_SIZE)); //創建一個epoll描述符，並將監聽socket加入epoll
    ev.data.fd = listener;
    CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev));

    while (1) {
        CHK2(epoll_events_count, epoll_wait(epfd, events, EPOLL_SIZE,
                                            EPOLL_RUN_TIMEOUT));
        tStart = clock();

        for (int i = 0; i < epoll_events_count ; i++) {
            if (events[i].data.fd ==
                listener) { //新的連接到來，將連接添加到epoll中，並發送歡迎消息
                CHK2(client, accept(listener, (struct sockaddr*) &their_addr, &socklen));
                setnonblocking(client);
                ev.data.fd = client;
                CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev));

                clients_list.push_back(client); // 添加新的客戶端到list
                bzero(message, BUF_SIZE);
                res = sprintf(message, STR_WELCOME, client);
                CHK2(res, send(client, message, BUF_SIZE, 0));

            } else {
                CHK2(res, handle_message(
                         events[i].data.fd)); //注意：這裡並沒有調用epoll_ctl重新設置socket的事件類型，但還是可以繼續收到客戶端發送過來的信息
            }
        }

        printf("Statistics: %d events handled at: %.2f second(s)\n", epoll_events_count,
               (double)(clock() - tStart) / CLOCKS_PER_SEC);
    }

    close(listener);
    close(epfd);

    return 0;
}

int handle_message(int client)
{
    char buf[BUF_SIZE], message[BUF_SIZE];
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);

    int len;

    CHK2(len, recv(client, buf, BUF_SIZE, 0)); //接受客戶端信息

    if (len ==
        0) { //客戶端關閉或出錯，關閉socket，並從list移除socket
        CHK(close(client));
        clients_list.remove(client);
    } else { //向客戶端發送信息
        if (clients_list.size() == 1) {
            CHK(send(client, STR_NOONE_CONNECTED, strlen(STR_NOONE_CONNECTED), 0));
            return len;
        }

        sprintf(message, STR_MESSAGE, client, buf);
        list<int>::iterator it;

        for (it = clients_list.begin(); it != clients_list.end(); it++) {
            if (*it != client) {
                CHK(send(*it, message, BUF_SIZE, 0));
            }
        }
    }

    return len;
}
```

- 10kclient.cpp

```cpp
#include "local.h"
#include "utils.h"

using namespace std;

char message[BUF_SIZE]; //接受服務器信息
list<int> list_of_clients; //存放所有客戶端
int res;
clock_t tStart;

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_HOST);

    tStart = clock();

    for (int i = 0 ; i < EPOLL_SIZE;
         i++) { //生成EPOLL_SIZE個客戶端，這裡是10000個，模擬高並發
        CHK2(sock, socket(PF_INET, SOCK_STREAM, 0));
        CHK(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0);
        list_of_clients.push_back(sock);

        bzero(&message, BUF_SIZE);
        CHK2(res, recv(sock, message, BUF_SIZE, 0));
        printf("%s\n", message);
    }

    list<int>::iterator it; //移除所有客戶端

    for (it = list_of_clients.begin(); it != list_of_clients.end() ; it++) {
        close(*it);
    }

    printf("Test passed at: %.2f second(s)\n",
           (double)(clock() - tStart) / CLOCKS_PER_SEC);
    printf("Total server connections was: %d\n", EPOLL_SIZE);

    return 0;
}
```


## 參考資料：
google code svn路徑：http://sechat.googlecode.com/svn/trunk/<br>
http://www.cnblogs.com/venow/archive/2012/11/30/2790031.html