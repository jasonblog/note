---
title: Linux 驚群詳解
layout: post
comments: true
language: chinese
category: [linux,program,misc]
keywords: 驚群,accept,epoll,epoll_wait
description: 簡言之，驚群現象就是當多個進程或線程在同時阻塞等待同一個事件時，如果這個事件發生，會喚醒所有的進程，但最終只可能有一個進程/線程對該事件進行處理，其他進程/線程會在失敗後重新休眠，這種性能浪費就是驚群。這裡對 Linux 中的驚群現象進行簡單介紹。
---

簡言之，驚群現象就是當多個進程或線程在同時阻塞等待同一個事件時，如果這個事件發生，會喚醒所有的進程，但最終只可能有一個進程/線程對該事件進行處理，其他進程/線程會在失敗後重新休眠，這種性能浪費就是驚群。

這裡對 Linux 中的驚群現象進行簡單介紹。

<!-- more -->

## 驚群

關於驚群的解釋可以查看 Wiki 的解釋 [Thundering herd problem](https://en.wikipedia.org/wiki/Thundering_herd_problem) 。

### accept()

常見的場景如下。

主進程執行 `socket()+bind()+listen()` 後，`fork()` 多個子進程，每個子進程都通過 `accept()` 循環處理這個 socket；此時，每個進程都阻塞在 `accpet()` 調用上，當一個新連接到來時，所有的進程都會被喚醒，但其中只有一個進程會 `accept()` 成功，其餘皆失敗，重新休眠。這就是 accept 驚群。

如果只用一個進程去 accept 新連接，並通過消息隊列等同步方式使其他子進程處理這些新建的連接，那麼將會造成效率低下；因為這個進程只能用來 accept 連接，該進程可能會造成瓶頸。

而實際上，對於 Linux 來說，這只是歷史上的問題，現在的內核都解決該問題，也即只會喚醒一個進程。可以通過如下程序進行測試，只會激活一個進程。

{% highlight c %}
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netinet/in.h>

#define PROCESS_NUM 10

int main()
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    int connfd;
    int pid, i, status;
    char sendbuff[1024];
    struct sockaddr_in serveraddr;

    printf("Listening 0.0.0.0:1234\n");
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(1234);
    bind(fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    listen(fd, 1024);
    for(i = 0; i < PROCESS_NUM; i++) {
        pid = fork();
        if(pid == 0) {
            while(1) {
                connfd = accept(fd, (struct sockaddr*)NULL, NULL);
                snprintf(sendbuff, sizeof(sendbuff), "accept PID is %d\n", getpid());

                send(connfd, sendbuff, strlen(sendbuff) + 1, 0);
                printf("process %d accept success!\n", getpid());
                close(connfd);
            }
        }
    }
    wait(&status);
    return 0;
}
{% endhighlight %}

然後，通過 `telnet 127.1 1234` 或者 `nc 127.1 1234` 測試鏈接即可。

### epoll()

另外還有一個是關於 `epoll_wait()` 的，目前來仍然存在驚群現象。

主進程仍執行 `socket()+bind()+listen()` 後，將該 socket 加入到 epoll 中，然後 fork 出多個子進程，每個進程都阻塞在 `epoll_wait()` 上，如果有事件到來，則判斷該事件是否是該 socket 上的事件，如果是，說明有新的連接到來了，則進行 accept 操作。

為了簡化處理，忽略後續的讀寫以及對 accept 返回的新的套接字的處理，直接斷開連接。

{% highlight c %}
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PROCESS_NUM 10
#define MAXEVENTS 64

int main (int argc, char *argv[])
{
    int sfd, efd;
    int flags;
    int n, i, k;
    struct epoll_event event;
    struct epoll_event *events;
    struct sockaddr_in serveraddr;

    sfd = socket(PF_INET, SOCK_STREAM, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi("1234"));
    bind(sfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    flags = fcntl (sfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl (sfd, F_SETFL, flags);

    if (listen(sfd, SOMAXCONN) < 0) {
        perror ("listen");
        exit(EXIT_SUCCESS);
    }

    if ((efd = epoll_create(MAXEVENTS)) < 0) {
        perror("epoll_create");
        exit(EXIT_SUCCESS);
    }

    event.data.fd = sfd;
    event.events = EPOLLIN; // | EPOLLET;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event) < 0) {
        perror("epoll_ctl");
        exit(EXIT_SUCCESS);
    }

    /* Buffer where events are returned */
    events = (struct epoll_event*)calloc(MAXEVENTS, sizeof event);

    for(k = 0; k < PROCESS_NUM; k++) {
        if (fork() == 0) { /* children process */
            while (1) {    /* The event loop */
                n = epoll_wait(efd, events, MAXEVENTS, -1);
                printf("process #%d return from epoll_wait!\n", getpid());
                sleep(2);  /* sleep here is very important!*/
                for (i = 0; i < n; i++) {
                    if ((events[i].events & EPOLLERR) ||
                        (events[i].events & EPOLLHUP) ||
                        (!(events[i].events & EPOLLIN))) {
                        /* An error has occured on this fd, or the socket is not
                         * ready for reading (why were we notified then?)
                         */
                        fprintf (stderr, "epoll error\n");
                        close (events[i].data.fd);
                        continue;
                    } else if (sfd == events[i].data.fd) {
                        /* We have a notification on the listening socket, which
                         * means one or more incoming connections.
                         */
                        struct sockaddr in_addr;
                        socklen_t in_len;
                        int infd;
                        //char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                        in_len = sizeof in_addr;

                        infd = accept(sfd, &in_addr, &in_len);
                        if (infd == -1) {
                            printf("process %d accept failed!\n", getpid());
                            break;
                        }
                        printf("process %d accept successed!\n", getpid());

                        /* Make the incoming socket non-blocking and add it to the
                        list of fds to monitor. */
                        close(infd);
                    }
                }
            }
        }
    }
    int status;
    wait(&status);
    free (events);
    close (sfd);
    return EXIT_SUCCESS;
}
{% endhighlight %}

注意：上述的處理中添加了 `sleep()` 函數，實際上，如果很快處理完了這個 `accept()` 請求，那麼其餘進程可能還沒有來得及被喚醒，內核隊列上已經沒有這個事件，無需喚醒其他進程。

那麼，為什麼只解決了 `accept()` 的驚群問題，而沒有解決 `epoll()` 的？

當接收到一個報文後，顯然只能由一個進程處理 (accept)；而 `epoll()` 卻不同，因為內核不知道對應的觸發事件具體由哪些進程處理，那麼只能是喚醒所有的進程，然後由不同的進程進行處理。

## Nginx 解決

如上所述，如果採用 epoll，則仍然存在該問題，nginx 就是這種場景的一個典型，我們接下來看看其具體的處理方法。

nginx 的每個 worker 進程都會在函數 `ngx_process_events_and_timers()` 中處理不同的事件，然後通過 `ngx_process_events()` 封裝了不同的事件處理機制，在 Linux 上默認採用 `epoll_wait()`。

主要在 `ngx_process_events_and_timers()` 函數中解決驚群現象。

{% highlight text %}
void ngx_process_events_and_timers(ngx_cycle_t *cycle)
{
    ... ...
    // 是否通過對accept加鎖來解決驚群問題，需要工作線程數>1且配置文件打開accetp_mutex
    if (ngx_use_accept_mutex) {
        // 超過配置文件中最大連接數的7/8時，該值大於0，此時滿負荷不會再處理新連接，簡單負載均衡
        if (ngx_accept_disabled > 0) {
            ngx_accept_disabled--;
        } else {
            // 多個worker僅有一個可以得到這把鎖。獲取鎖不會阻塞過程，而是立刻返回，獲取成功的話
            // ngx_accept_mutex_held被置為1。拿到鎖意味著監聽句柄被放到本進程的epoll中了，如果
            // 沒有拿到鎖，則監聽句柄會被從epoll中取出。
            if (ngx_trylock_accept_mutex(cycle) == NGX_ERROR) {
                return;
            }
            if (ngx_accept_mutex_held) {
                // 此時意味著ngx_process_events()函數中，任何事件都將延後處理，會把accept事件放到
                // ngx_posted_accept_events鏈表中，epollin|epollout事件都放到ngx_posted_events鏈表中
                flags |= NGX_POST_EVENTS;
            } else {
                // 拿不到鎖，也就不會處理監聽的句柄，這個timer實際是傳給epoll_wait的超時時間，修改
                // 為最大ngx_accept_mutex_delay意味著epoll_wait更短的超時返回，以免新連接長時間沒有得到處理
                if (timer == NGX_TIMER_INFINITE || timer > ngx_accept_mutex_delay) {
                    timer = ngx_accept_mutex_delay;
                }
            }
        }
    }
    ... ...
    (void) ngx_process_events(cycle, timer, flags);   // 實際調用ngx_epoll_process_events函數開始處理
    ... ...
    if (ngx_posted_accept_events) { //如果ngx_posted_accept_events鏈表有數據，就開始accept建立新連接
        ngx_event_process_posted(cycle, &ngx_posted_accept_events);
    }

    if (ngx_accept_mutex_held) { //釋放鎖後再處理下面的EPOLLIN EPOLLOUT請求
        ngx_shmtx_unlock(&ngx_accept_mutex);
    }

    if (delta) {
        ngx_event_expire_timers();
    }

    ngx_log_debug1(NGX_LOG_DEBUG_EVENT, cycle->log, 0, "posted events %p", ngx_posted_events);
	// 然後再處理正常的數據讀寫請求。因為這些請求耗時久，所以在ngx_process_events裡NGX_POST_EVENTS標
    // 志將事件都放入ngx_posted_events鏈表中，延遲到鎖釋放了再處理。
}
{% endhighlight %}

關於 `ngx_use_accept_mutex`、`ngx_accept_disabled` 的修改可以直接 grep 查看。

## SO_REUSEPORT

Linux 內核的 3.9 版本帶來了 `SO_REUSEPORT` 特性，該特性支持多個進程或者線程綁定到同一端口，提高服務器程序的性能，允許多個套接字 `bind()` 以及 `listen()` 同一個 TCP 或者 UDP 端口，並且在內核層面實現負載均衡。

在未開啟 `SO_REUSEPORT` 時，由一個監聽 socket 將新接收的鏈接請求交給各個 worker 處理。

![thunder reuseport]({{ site.url }}/images/network/thunder-reuseport-01.png "thunder reuseport"){: .pull-center width="60%" }

在使用 `SO_REUSEPORT` 後，多個進程可以同時監聽同一個 IP:Port ，然後由內核決定將新鏈接發送給那個進程，顯然會降低各個 worker 接收新鏈接時鎖競爭。

![thunder reuseport]({{ site.url }}/images/network/thunder-reuseport-02.png "thunder reuseport"){: .pull-center width="60%" }

<!--
現在我們再來看一下程序執行的效果，拿出證據證明上述操作確實避免了驚群問題：
我們啟動一個程序，該程序會創建兩個子進程並同時監聽9001端口。然後利用telnet向9001進行連接，其中一個子進程喚醒了epool的監聽，而另一個沒有。
http://www.verycto.com/code/cpp/15.html
-->

## 參考

關於 Nginx 使用 `SO_REUSEPORT` 的介紹 [Socket Sharding in NGINX Release 1.9.1](https://www.nginx.com/blog/socket-sharding-nginx-release-1-9-1/) 。

<!--
epoll()驚群實例
http://blog.csdn.net/spch2008/article/details/18301357
epoll()討論
https://www.zhihu.com/question/24169490
http://blog.csdn.net/mumumuwudi/article/details/47051235
http://blog.csdn.net/russell_tao/article/details/7204260
http://blog.csdn.net/liujiyong7/article/details/43346829
http://www.cnblogs.com/Anker/p/7071849.html
http://simohayha.iteye.com/blog/561424


http://pureage.info/2015/12/22/thundering-herd.html
-->

{% highlight text %}
{% endhighlight %}
