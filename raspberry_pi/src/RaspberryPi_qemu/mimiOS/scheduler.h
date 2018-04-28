
#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include "process.h"

void create_main_process();
void fork(char * name, unsigned long addr);
void schedule_timeout(unsigned long stack_pointer, unsigned long pc);
void terminate_process();
void halt();

#endif	/* SCHEDULER_H */

