/* This file provides an interface of multiple timers. for convenience, it simplify signal processing.
 * Also, it can't be used in multithreading environment.
 * Author:bripengandre
 * Date:2009-04-29
 */
#ifndef _MUL_TIMER_H_
#define _MUL_TIMER_H_

#include <sys/time.h>

#define MAX_TIMER_CNT 10
#define MUL_TIMER_RESET_SEC 10
#define TIMER_UNIT 60
#define MAX_FUNC_ARG_LEN 100
#define INVALID_TIMER_HANDLE (-1)

typedef int timer_handle_t;

typedef struct _timer_manage
{
    struct  _timer_info
    {
        int state; /* on or off */
        int interval;
        int elapse; /* 0~interval */
        int (* timer_proc) (void *arg, int arg_len);
        char func_arg[MAX_FUNC_ARG_LEN];
        int arg_len;
    }timer_info[MAX_TIMER_CNT];

    void (* old_sigfunc)(int);
    void (* new_sigfunc)(int);
    struct itimerval value, ovalue;
}_timer_manage_t;

/* success, return 0; failed, return -1 */
int init_mul_timer(void);
/* success, return 0; failed, return -1 */
int destroy_mul_timer(void);
/* success, return timer handle(>=0); failed, return -1 */
timer_handle_t set_a_timer(int interval, int (* timer_proc) (void *arg, int arg_len), void *arg, int arg_len);
/* success, return 0; failed, return -1 */
int del_a_timer(timer_handle_t handle);

#endif /* _MUL_TIMER_H_ */
