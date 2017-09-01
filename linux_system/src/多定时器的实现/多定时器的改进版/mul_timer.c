#include "mul_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void CALLBACK traverse_mul_timer(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
static int print_mul_timer(struct _mul_timer_manage *ptimer);

struct _mul_timer_manage *init_mul_timer(unsigned int time_unit)
{
	struct _mul_timer_manage *p;
	
	if( (p = malloc(sizeof(struct _mul_timer_manage))) == NULL)
	{
		return (NULL);
	}
	
	p->etr_head = NULL;
	p->timer_cnt = 0;
	p->time_unit = time_unit;
	p->entry_id = 0;
	
	p->timer_id = timeSetEvent(time_unit, 0, (LPTIMECALLBACK )traverse_mul_timer, (DWORD)p, TIME_PERIODIC);
	
	return(p);
}


timer_handle_t set_timer(struct _mul_timer_manage *ptimer, unsigned int time_out, int (* timer_proc) (void *arg, unsigned int *arg_len), void *arg, unsigned int *arg_len)
{
	struct _mul_timer_entry *p, *prev, *pnew;
	
	if(ptimer == NULL || time_out == 0)
	{
		return (NULL);
	}
	
	
	if( (pnew = malloc(sizeof(struct _mul_timer_entry))) == NULL)
	{
		return (NULL);
	}
	pnew->is_use = 0;
	pnew->arg = arg;
	pnew->arg_len = arg_len;
	pnew->elapse = 0;
	pnew->timer_proc = timer_proc;
	
	p = ptimer->etr_head;
	prev = NULL;
	while(p != NULL)
	{
		if(p->timeout < time_out) /* assume the latest time_proc has higher priority */
		{
			time_out = time_out-p->timeout;
			prev = p;
			p = p->etr_next;
		}
		else
		{
			p->timeout -= time_out;
			break;
		}
	}
	
	pnew->timeout = time_out;
	pnew->etr_next = p;
	pnew->handle.ptr = (unsigned long )pnew;
	pnew->handle.entry_id = ptimer->entry_id;
	ptimer->entry_id++;

	if(prev == NULL)
	{
		ptimer->etr_head = pnew;
	}
	else
	{
		prev->etr_next = pnew;
	}	
	ptimer->timer_cnt++;
	
	return (&pnew->handle);
}


int kill_timer(struct _mul_timer_manage *ptimer, timer_handle_t hdl)
{
	struct _mul_timer_entry *p, *prev;
	
	if(ptimer == NULL)
	{
		return (0);
	}
	
	
	p = ptimer->etr_head;
	prev = NULL;
	while(p != NULL)
	{
		if(p->handle.ptr == hdl->ptr && p->handle.entry_id == hdl->entry_id)
		{
			break;
		}

		prev = p;
		p = p->etr_next;
	}
	
	/* no such timer or timer is in use,  return 0 */
	if(p == NULL || (p != NULL && p->is_use == 1)) 
	{
		return (0); 
	}
	
	/* has found the timer */
	if(prev == NULL)
	{
		ptimer->etr_head = p->etr_next;
	}
	else
	{
		prev->etr_next = p->etr_next;
	}
	
	/* revise timeout */
	if(p->etr_next != NULL)
	{
		p->etr_next->timeout += p->timeout;
	}
	
	/* delete the timer */
	free(p);
	p = NULL;
	ptimer->timer_cnt--;
	
	return (1);
}


int get_timeout_byhdl(struct _mul_timer_manage *ptimer, timer_handle_t hdl)
{
	struct _mul_timer_entry *p;
	unsigned int timeout;
	
	if(ptimer == NULL || (struct _mul_timer_entry *)(hdl) == NULL)
	{
		return (-1);
	}
	
	
	timeout = 0;
	p = ptimer->etr_head;
	while(p != NULL)
	{
	    if(p->handle.ptr == hdl->ptr && p->handle.entry_id == hdl->entry_id)
		{
			break;
		}

		timeout += p->timeout;
		p = p->etr_next;
	}
	
	if(p == NULL)
	{
		return (-1);
	}
	else
	{
		return ((int)timeout+p->timeout);
	}	
	
}


int get_timeout_bytimeproc(struct _mul_timer_manage *ptimer, int (* timer_proc) (void *arg, unsigned int *arg_len))
{
	struct _mul_timer_entry *p;
	unsigned int timeout;
	
	if(ptimer == NULL || timer_proc == NULL)
	{
		return (-1);
	}
	
	p = ptimer->etr_head;
	while((p != NULL) && (p->timer_proc != timer_proc))
	{
		timeout += p->timeout;
		p = p->etr_next;
	}
	
	if(p == NULL)
	{
		return (-1);
	}
	else
	{
		return (timeout+p->timeout);
	}	
}


int release_mul_timer(struct _mul_timer_manage *ptimer)
{
	struct _mul_timer_entry *p, *ptmp;
	
	if(ptimer == NULL)
	{
		return (0);
	}
	
	timeKillEvent(ptimer->timer_id);
	/* delete all timers */
	p = ptimer->etr_head;
	while(p != NULL)
	{
		ptmp = p;
		p = p->etr_next;
		free(ptmp);
	}
	/* delete timer_manage */
	free(ptimer);
	ptimer = NULL;
	
	return (1);
}

int is_valid_time_hdl(timer_handle_t hdl)
{
	if(hdl == NULL)
	{
		return (0);
	}
	else
	{
		return (1);
	}	
}


void CALLBACK traverse_mul_timer(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	struct _mul_timer_manage *ptimer;
	struct _mul_timer_entry *p, *ptmp;
	unsigned int timeout;
	
	ptimer = (struct _mul_timer_manage *)dwUser;
	if(ptimer == NULL)
	{
		return;
	}
	
	timeout = ptimer->time_unit;
	p = ptimer->etr_head;
	while(p != NULL)
	{
		if(p->timeout <= timeout)
		{
			p->is_use = 1;
			p->timer_proc(p->arg, p->arg_len);
			ptmp = p;
			timeout -= p->timeout;
			p = p->etr_next;
			free(ptmp);
			ptimer->etr_head = p;
		}
		else
		{
			p->timeout -= timeout;
			p->elapse += timeout;
			ptimer->etr_head = p;
			break;
		}
	}
	if(p == NULL)
	{
		ptimer->etr_head = NULL;
	}	
	
	return; 
}


static int print_mul_timer(struct _mul_timer_manage *ptimer)
{
	struct _mul_timer_entry *p;
	int i;
	
	if(ptimer == NULL)
	{
		return (0);
	}
	
	printf("***************************mul_timer statistics start************************\n");
	printf("this mul_timer's time_unit=%u, etr_head=%p and  has %d timers:\n", ptimer->time_unit, ptimer->etr_head, ptimer->timer_cnt);
	
	p = ptimer->etr_head;
	i = 0;
	while(p != NULL)
	{
		printf("the %d timer: timeout=%u, elapse=%u, timer_proc=%p, arg=%p, arg_len=%p, etr_next=%p\n"
			, i+1, p->timeout, p->elapse, p->timer_proc, p->arg, p->arg_len, p->etr_next);
		p = p->etr_next;
		i++;
	}
	printf("***************************mul_timer statistics end************************\n");
	
	return (1);
}


#define _MUL_TIMER_MAIN

#ifdef _MUL_TIMER_MAIN

static void get_format_time(char *tstr)
{
    time_t t;
    
    t = time(NULL);
    strcpy(tstr, ctime(&t));
    tstr[strlen(tstr)-1] = '\0';
    
    return;
}

timer_handle_t hdl[100];
int call_cnt = 0;
struct _mul_timer_manage *ptimer;

int timer_proc1(void *arg, unsigned int *len)
{
    char tstr[200];
    static int i, ret;
    
    get_format_time(tstr);
    printf("call_cnt=%d, hello %s: timer_proc1 is here.\n", call_cnt, tstr);
    i++;
    call_cnt++;
    
    
    return (1);
}

int timer_proc2(void * arg, unsigned int *len)
{
    char tstr[200];
    static int i, ret;
    
    get_format_time(tstr);
    printf("call_cnt=%d, hello %s: timer_proc2 is here: arg = %s, len = %d.\n", call_cnt, tstr, arg, *len);
    i++;
    call_cnt++;
    
    return (1);
}


int main(void)
{
    char arg[50] = "hello, multiple timers";
    char tstr[200];
    int ret;
	int len = 50, i;
    
    ptimer = init_mul_timer(1000);
	
	for(i = 0; i < 10; i++)
	{
		hdl[i<<1] = set_timer(ptimer, 1000*(i+1), timer_proc1, NULL, NULL);
		printf("hdl[0i<<1=%d, is_valid_hdl=%d\n", hdl[i<<1], is_valid_time_hdl(hdl[i<<1]));
		hdl[(i<<1)+1] = set_timer(ptimer, 3000*(i+1), timer_proc2, arg, &len);
		printf("hdl[i<<1+1]=%d, is_valid_hdl=%d\n", hdl[(i<<1)+1], is_valid_time_hdl(hdl[(i<<1)+1]));
		print_mul_timer(ptimer);
	}

	ret = kill_timer(ptimer, hdl[17]);
	printf("ret=kill_timer=%d\n", ret);
	print_mul_timer(ptimer);	
	printf("hd[19]->timout=%d\n", get_timeout_byhdl(ptimer, hdl[19]));

    while(1)
    {
        if(call_cnt == 15)
        {
            get_format_time(tstr);
            ret = release_mul_timer(ptimer);
            printf("call_cnt=%d, main: %s destroy_mul_timer, ret=%d\n", call_cnt, tstr, ret);
            call_cnt++;
        }
    }
    
    return 0;
}

#endif
