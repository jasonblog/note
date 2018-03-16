#ifndef  LST_TIMER
#define  LST_TIMER

#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

#define BUFFER_SIZE 64
class util_timer; /*前置声明*/

/*用户数据结构：客户端socket地址、socket文件描述符、读缓存和定时器*/
struct client_data
{
    struct sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE];
    util_timer *timer;
};

/*定时器类*/
class util_timer
{
public:
    util_timer():prev(NULL), next(NULL){}
public:
    time_t expire;  /*任务的超时时间，这里使用绝对时间*/
    void (*cb_func)(client_data *); /*任务回调函数*/
    /*回调函数处理的客户数据，由定时器的执行者传递给回调函数*/
    client_data *user_data;
    util_timer  *prev;
    util_timer  *next;
};

/*定时器链表， 它是一个升序、双向链表、且带有头结点和尾节点*/
class sort_timer_lst
{
public:
    sort_timer_lst():head(NULL), tail(NULL){}
    ~sort_timer_lst()
    {
        util_timer *tmp = head;
        while(tmp)
        {
            head = tmp->next;
            delete tmp;
            tmp = head;
        }
    }

    /*将目标定时器timer添加到链表中*/
    void add_timer(util_timer *timer)
    {
        if (!timer) 
        {
            return;
        }
        
        if (!head) 
        {
            head = tail = timer;
            return;
        }

        /* 如果目标定时器的超时时间小于当前链表中所有定时器的超时时间，则把该定时器插入链表头部，
         * 作为链表新的头节点。否则就需要调用重载函数add_timer（util_timer
         * *timer, util_timer *lst_head),把它插入链表中合适的位置，
         * 以保证链表的升序特性
         * */
        if (timer->expire < head->expire) 
        {
            timer->next = head;
            head->prev  = timer;
            head        = timer;
            return;
        }
        add_timer(timer, head);
    }

    /* 当某个定时任务发生变化时，调整对应的定时器在链表中的位置。这个函数只考虑被调整
     * 的定时器的超时时间延长的情况，即该定时器需要往链表的尾部移动
     * */
    void adjust_timer(util_timer *timer)
    {
         if (!timer) 
         {
             return;
         }
         util_timer *tmp = timer->next;

         /* 如果被调整的目标定时器处在链表尾部，或者该定时器新的超时值仍然小于
          * 其下一个定时器的超时值， 则不调整
          */
         if (!tmp || (timer->expire < tmp->expire)) 
         {
             return;
         }

         /*如果目标定时器是链表的头节点，
          * 则将该定时器从链表中取出并重新插入链表*/
         if (timer == head) 
         {
             head = head->next;
             head->prev = NULL;
             timer->next = NULL;
             add_timer(timer, head);
         }

         /*如果目标定时器不是链表的头节点， 则将该定时器从链表中取出，
          * 然后插入其原来所在位置之后的部分链表*/
         else
         {
            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            add_timer(timer, timer->next);
         }
    }

    /*将目标定时器timer从链表中删除*/
    void del_timer(util_timer *timer)
    {
        if (!timer) 
        {
            return;
        }
        
        /*下面这个条件成立表示链表中只有一个定时器， 即目标定时器*/
        if ((timer == head) && (timer == tail)) 
        {
            delete timer;
            head = NULL;
            tail = NULL;
            return;
        }

        /*如果链表中至少有两个定时器，且目标定时器链表的尾节点，则将链表的尾结点重置为原尾节点，然后删除目标定时器*/
        if (timer == head) 
        {
            head = head->next;
            head->prev = NULL;
            delete timer;
            return;
        }

        /*如果链表中至少有两个定时器，且目标定时器是链表的尾节点，
         * 则将链表的尾结点重置为原尾节点的前一个节点， 然后删除目标定时器*/
        if (timer == tail) 
        {
            tail = tail->prev;
            tail->next = NULL;
            delete timer;
            return;
        }

        /*如果目标定时器位于链表的中间， 则把它前后的定时器串联起来，
         * 然后删除目标定时器*/
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        delete timer;
    }
    
    /*SIGALRM信号每次被触发就在其信号处理（如果使用统一事件源，则是主函数）中执行一个tick函数，
     * 以处理链表上到期的任务*/
    void tick()
    {
        if (!head) 
        {
            return;
        }
        fprintf(stdout, "timer tick\n");
        time_t cur = time(NULL); /*获得系统当前的时间*/
        util_timer *tmp = head;
        /*从头结点开始依次处理每个定时器，直到遇到一个尚未到期的定时器，
         * 这就是定时器的核心处理逻辑*/
        while(tmp)
        {
            /*因为每个定时器都使用绝对时间作为超时值，所以我们可以把定时器的超时值和系统当前时间，
             * 比较以判断定时器是否到期*/
            if (cur < tmp->expire) 
            {
                break;
            }

            /*调用定时器的回调函数，以执行定时任务*/
            tmp->cb_func(tmp->user_data);
            /*执行完定时器中的定时任务之后，就将它从链表中删除，并重置链表头结点*/
            head = tmp->next;
            if (head) 
            {
                head->prev = NULL;
            }
            delete tmp;
            tmp = head;
        }
    }

private:
    /*一个重载的辅助函数，它被公有的add_timer函数和adjust_timer函数调用。该函数表示将
     * 目标定时器timer添加到节点lst_head之后的部分链表中*/
    void add_timer(util_timer *timer, util_timer *lst_head)
    {
        util_timer *prev = lst_head;
        util_timer *tmp  = prev->next;

        /*遍历lst_head节点之后的部分链表，
         * 直到找到一个超时时间大于目标定时器的超时时间的
         * 节点，并将目标定时器插入该节点之前*/
        while(tmp)
        {
            if (timer->expire < tmp->expire) 
            {
                prev->next  = timer;
                timer->next = tmp;
                tmp->prev   = timer;
                timer->prev = prev;
                break;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        /*如果遍历完lst_head节点之后的部分链表，仍未找到超时时间大于目标定时器的超时时间的节点，
         * 则将目标定时器插入链表尾部，并把它设置为链表新的尾节点*/
        if (!tmp) 
        {
            prev->next   = timer;
            timer->prev  = prev;
            timer->next  = NULL;
            tail         = timer;
        }
    }

private:
    util_timer *head;
    util_timer *tail;
};
#endif
