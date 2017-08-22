/* This file provides an interface of multiple timers. it can't be used in multithreading environment.
 * Author:bripengandre
 * Date:2009-07-19
 */

#ifndef _MUL_TIMER_H_
#define _MUL_TIMER_H_
#include <windows.h>

typedef struct _timer_handle
{
		unsigned long ptr;
		unsigned long entry_id;
}*timer_handle_t;

/* timer entry */
typedef struct _mul_timer_entry
{
	char is_use; /* 0, not; 1, yes */
	struct _timer_handle handle;
	unsigned int timeout;
	unsigned int elapse; /*  */
	int (* timer_proc) (void *arg, unsigned int *arg_len); /* callback function */
	void *arg;
	unsigned int *arg_len;
	struct _mul_timer_entry *etr_next;
}mul_timer_entry_t;

typedef struct _mul_timer_manage
{
	unsigned long  entry_id;
	unsigned int timer_cnt;
	unsigned int time_unit;
	struct _mul_timer_entry *etr_head;
	UINT timer_id;
};



struct _mul_timer_manage *init_mul_timer(unsigned int time_unit);
timer_handle_t set_timer(struct _mul_timer_manage *ptimer, unsigned int time_out, int (* timer_proc) (void *arg, unsigned int *arg_len), void *arg, unsigned int *arg_len);
int kill_timer(struct _mul_timer_manage *ptimer, timer_handle_t hdl);
int get_timeout_byhdl(struct _mul_timer_manage *ptimer, timer_handle_t hdl);
int get_timeout_bytimeproc(struct _mul_timer_manage *ptimer, int (* timer_proc) (void *arg, unsigned int *arg_len));
int release_mul_timer(struct _mul_timer_manage *ptimer);

int is_valid_time_hdl(timer_handle_t hdl);

#endif /* _MUL_TIMER_H_ */
