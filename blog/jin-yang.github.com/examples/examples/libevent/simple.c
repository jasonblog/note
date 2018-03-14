#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <event.h>

void on_time(int sock, short event, void *arg)
{
    struct timeval tv;
    printf("Hello World!\n");

    tv.tv_sec = 1;
    tv.tv_usec = 0;
    event_add((struct event*)arg, &tv);  // 重新添加定时事件，默认会自动删除
}

int main(void)
{
    struct event ev_time;
    struct timeval tv;

    event_init();                             // 初始化
    evtimer_set(&ev_time, on_time, &ev_time); // 设置定时事件

    tv.tv_sec = 1;
    tv.tv_usec = 0;
    event_add(&ev_time, &tv);                 // 添加定时事件

    event_dispatch();                         // 事件循环

    return 0;
}
