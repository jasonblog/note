#include <ev.h>       // a single header file is required
#include <time.h>       // a single header file is required
#include <stdio.h>    // for puts



//ev_periodic tick;
//static void clock_cb (struct ev_loop *loop, ev_periodic *w, int revents)
//{
//    puts("periodic call back");
//    ev_break (EV_A_ EVBREAK_ALL);
//}
//
//int main (void)
//{
//    // use the default event loop unless you have special needs
//    struct ev_loop *loop = EV_DEFAULT; /* OR ev_default_loop(0) */
//
//    ev_periodic_init (&tick, clock_cb, 10., 5., 0); // for 5s
//    //ev_periodic_init (&tick, clock_cb, 0., 5., 0); // for 5s
//    ev_periodic_start (loop, &tick);
//
//    // now wait for events to arrive
//    ev_run (loop, 0);
//
//    // break was called, so exit
//    return 0;
//}
//

//#include <ev.h>
//#include <pthread.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <math.h> // for fmod
//
//#define TIMEOUT 4.
//
//struct ev_loop *loop = NULL;
//ev_periodic periodic_watcher;
//
//static void periodic_cb(struct ev_loop *loop, ev_periodic *w, int revents)
//{
//    printf("periodic_cb() call\n");
//}
//
//static ev_tstamp scheduler_cb(ev_periodic *w, ev_tstamp now)
//{
//    double mod = fmod(now, TIMEOUT);
//printf("scheduler_cb() call, now = %lf, mod = %lf\n", now, mod);
//return now + (TIMEOUT - mod);
//}
//
//void *ev_periodic_create(void *p)
//{
//    loop = ev_loop_new(EVFLAG_AUTO);
//
//// 下面三种初始化方法都可以，实现的效果是一样的。
////ev_periodic_init(&periodic_watcher, periodic_cb, 0., TIMEOUT, 0); // periodic_cb每隔TIMEOUT秒被调用一次
//
//    // periodic_cb每隔TIMEOUT秒被调用一次
//    ev_periodic_init(&periodic_watcher, periodic_cb, 0., 0., scheduler_cb);
//
////ev_periodic_init(&periodic_watcher, periodic_cb, fmod(ev_now(loop), TIMEOUT), TIMEOUT, 0); // periodic_cb每隔TIMEOUT秒被调>用一次
//
//// me:对于上面第二种初始化方法，执行下面这个方法后会主动去调用一次scheduler_cb函数(但此时并不触发periodic_cb函数)，以后就是每隔TIMEOUT秒后才调用scheduler_cb，并且触发periodic_cb。
//
//    ev_periodic_start(loop, &periodic_watcher);
//    printf("ev_periodic_create() call, after start!\n");
//    ev_run(loop, 0);
//}
//
//int main(void)
//{
//    pthread_t tid;
//    pthread_create(&tid, NULL, ev_periodic_create, NULL);
//
//    while(1) {
//        static int count = 0;
//        printf("count = %d\n", count++);
//        sleep(1);
//    }
//
//    return 0;
//}
//
////编译命令：gcc periodic2.c -lev -lpthread -lm
////-lm表示需要math函数库的支持














ev_periodic pw;

void periodic_action(struct ev_loop *main_loop,ev_periodic *timer_w,int e)
{
    time_t now = time(NULL);
    printf("current time is %s", ctime(&now));
}

static ev_tstamp rescheduler (ev_periodic *w, ev_tstamp now)
{
    return now + 60;
}

int main(void)
{
    time_t now = time(NULL);
    struct ev_loop *main_loop = ev_default_loop(0);

    // 调用rescheduler()返回下次执行的时间，如果存在回调函数，则会忽略其它参数
    // 包括offset+interval，其输出示例如下：
    //     begin time is Fri Apr 14 21:51:47 2016
    //   current time is Fri Apr 14 21:52:47 2016
    //   current time is Fri Apr 14 21:53:47 2016
    //   current time is Fri Apr 14 21:54:47 2016
    ev_periodic_init(&pw, periodic_action, 0, 1, rescheduler);

    // 一般offset在[0, insterval]范围内，如下，也就是在最近的一个5秒整触发第一
    // 次回调函数，其输出示例如下：
    //     begin time is Fri Apr 21 23:24:18 2016
    //   current time is Fri Apr 21 23:24:25 2016
    //   current time is Fri Apr 21 23:24:35 2016
    //   current time is Fri Apr 21 23:24:45 2016
    //ev_periodic_init(&pw, periodic_action, 5, 10, NULL);

    // 如下只执行一次，也就是在20秒后触发
    //     begin time is Fri Apr 21 23:27:04 2016
    //   current time is Fri Apr 21 23:27:24 2016
    //ev_periodic_init(&pw, periodic_action, now+20, 0, NULL);      //3

    ev_periodic_start(main_loop, &pw);

    printf("  begin time is %s", ctime(&now));

    ev_run(main_loop,0);
    return 0;
}
