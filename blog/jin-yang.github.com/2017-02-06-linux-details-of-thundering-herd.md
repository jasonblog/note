---
title: Linux 惊群详解
layout: post
comments: true
language: chinese
category: [linux,program,misc]
keywords: 惊群,accept,epoll,epoll_wait
description: 简言之，惊群现象就是当多个进程或线程在同时阻塞等待同一个事件时，如果这个事件发生，会唤醒所有的进程，但最终只可能有一个进程/线程对该事件进行处理，其他进程/线程会在失败后重新休眠，这种性能浪费就是惊群。这里对 Linux 中的惊群现象进行简单介绍。
---

简言之，惊群现象就是当多个进程或线程在同时阻塞等待同一个事件时，如果这个事件发生，会唤醒所有的进程，但最终只可能有一个进程/线程对该事件进行处理，其他进程/线程会在失败后重新休眠，这种性能浪费就是惊群。

这里对 Linux 中的惊群现象进行简单介绍。

<!-- more -->

## 惊群

关于惊群的解释可以查看 Wiki 的解释 [Thundering herd problem](https://en.wikipedia.org/wiki/Thundering_herd_problem) 。

### accept()

常见的场景如下。

主进程执行 `socket()+bind()+listen()` 后，`fork()` 多个子进程，每个子进程都通过 `accept()` 循环处理这个 socket；此时，每个进程都阻塞在 `accpet()` 调用上，当一个新连接到来时，所有的进程都会被唤醒，但其中只有一个进程会 `accept()` 成功，其余皆失败，重新休眠。这就是 accept 惊群。

如果只用一个进程去 accept 新连接，并通过消息队列等同步方式使其他子进程处理这些新建的连接，那么将会造成效率低下；因为这个进程只能用来 accept 连接，该进程可能会造成瓶颈。

而实际上，对于 Linux 来说，这只是历史上的问题，现在的内核都解决该问题，也即只会唤醒一个进程。可以通过如下程序进行测试，只会激活一个进程。

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

然后，通过 `telnet 127.1 1234` 或者 `nc 127.1 1234` 测试链接即可。

### epoll()

另外还有一个是关于 `epoll_wait()` 的，目前来仍然存在惊群现象。

主进程仍执行 `socket()+bind()+listen()` 后，将该 socket 加入到 epoll 中，然后 fork 出多个子进程，每个进程都阻塞在 `epoll_wait()` 上，如果有事件到来，则判断该事件是否是该 socket 上的事件，如果是，说明有新的连接到来了，则进行 accept 操作。

为了简化处理，忽略后续的读写以及对 accept 返回的新的套接字的处理，直接断开连接。

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

注意：上述的处理中添加了 `sleep()` 函数，实际上，如果很快处理完了这个 `accept()` 请求，那么其余进程可能还没有来得及被唤醒，内核队列上已经没有这个事件，无需唤醒其他进程。

那么，为什么只解决了 `accept()` 的惊群问题，而没有解决 `epoll()` 的？

当接收到一个报文后，显然只能由一个进程处理 (accept)；而 `epoll()` 却不同，因为内核不知道对应的触发事件具体由哪些进程处理，那么只能是唤醒所有的进程，然后由不同的进程进行处理。

## Nginx 解决

如上所述，如果采用 epoll，则仍然存在该问题，nginx 就是这种场景的一个典型，我们接下来看看其具体的处理方法。

nginx 的每个 worker 进程都会在函数 `ngx_process_events_and_timers()` 中处理不同的事件，然后通过 `ngx_process_events()` 封装了不同的事件处理机制，在 Linux 上默认采用 `epoll_wait()`。

主要在 `ngx_process_events_and_timers()` 函数中解决惊群现象。

{% highlight text %}
void ngx_process_events_and_timers(ngx_cycle_t *cycle)
{
    ... ...
    // 是否通过对accept加锁来解决惊群问题，需要工作线程数>1且配置文件打开accetp_mutex
    if (ngx_use_accept_mutex) {
        // 超过配置文件中最大连接数的7/8时，该值大于0，此时满负荷不会再处理新连接，简单负载均衡
        if (ngx_accept_disabled > 0) {
            ngx_accept_disabled--;
        } else {
            // 多个worker仅有一个可以得到这把锁。获取锁不会阻塞过程，而是立刻返回，获取成功的话
            // ngx_accept_mutex_held被置为1。拿到锁意味着监听句柄被放到本进程的epoll中了，如果
            // 没有拿到锁，则监听句柄会被从epoll中取出。
            if (ngx_trylock_accept_mutex(cycle) == NGX_ERROR) {
                return;
            }
            if (ngx_accept_mutex_held) {
                // 此时意味着ngx_process_events()函数中，任何事件都将延后处理，会把accept事件放到
                // ngx_posted_accept_events链表中，epollin|epollout事件都放到ngx_posted_events链表中
                flags |= NGX_POST_EVENTS;
            } else {
                // 拿不到锁，也就不会处理监听的句柄，这个timer实际是传给epoll_wait的超时时间，修改
                // 为最大ngx_accept_mutex_delay意味着epoll_wait更短的超时返回，以免新连接长时间没有得到处理
                if (timer == NGX_TIMER_INFINITE || timer > ngx_accept_mutex_delay) {
                    timer = ngx_accept_mutex_delay;
                }
            }
        }
    }
    ... ...
    (void) ngx_process_events(cycle, timer, flags);   // 实际调用ngx_epoll_process_events函数开始处理
    ... ...
    if (ngx_posted_accept_events) { //如果ngx_posted_accept_events链表有数据，就开始accept建立新连接
        ngx_event_process_posted(cycle, &ngx_posted_accept_events);
    }

    if (ngx_accept_mutex_held) { //释放锁后再处理下面的EPOLLIN EPOLLOUT请求
        ngx_shmtx_unlock(&ngx_accept_mutex);
    }

    if (delta) {
        ngx_event_expire_timers();
    }

    ngx_log_debug1(NGX_LOG_DEBUG_EVENT, cycle->log, 0, "posted events %p", ngx_posted_events);
	// 然后再处理正常的数据读写请求。因为这些请求耗时久，所以在ngx_process_events里NGX_POST_EVENTS标
    // 志将事件都放入ngx_posted_events链表中，延迟到锁释放了再处理。
}
{% endhighlight %}

关于 `ngx_use_accept_mutex`、`ngx_accept_disabled` 的修改可以直接 grep 查看。

## SO_REUSEPORT

Linux 内核的 3.9 版本带来了 `SO_REUSEPORT` 特性，该特性支持多个进程或者线程绑定到同一端口，提高服务器程序的性能，允许多个套接字 `bind()` 以及 `listen()` 同一个 TCP 或者 UDP 端口，并且在内核层面实现负载均衡。

在未开启 `SO_REUSEPORT` 时，由一个监听 socket 将新接收的链接请求交给各个 worker 处理。

![thunder reuseport]({{ site.url }}/images/network/thunder-reuseport-01.png "thunder reuseport"){: .pull-center width="60%" }

在使用 `SO_REUSEPORT` 后，多个进程可以同时监听同一个 IP:Port ，然后由内核决定将新链接发送给那个进程，显然会降低各个 worker 接收新链接时锁竞争。

![thunder reuseport]({{ site.url }}/images/network/thunder-reuseport-02.png "thunder reuseport"){: .pull-center width="60%" }

<!--
现在我们再来看一下程序执行的效果，拿出证据证明上述操作确实避免了惊群问题：
我们启动一个程序，该程序会创建两个子进程并同时监听9001端口。然后利用telnet向9001进行连接，其中一个子进程唤醒了epool的监听，而另一个没有。
http://www.verycto.com/code/cpp/15.html
-->

## 参考

关于 Nginx 使用 `SO_REUSEPORT` 的介绍 [Socket Sharding in NGINX Release 1.9.1](https://www.nginx.com/blog/socket-sharding-nginx-release-1-9-1/) 。

<!--
epoll()惊群实例
http://blog.csdn.net/spch2008/article/details/18301357
epoll()讨论
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
