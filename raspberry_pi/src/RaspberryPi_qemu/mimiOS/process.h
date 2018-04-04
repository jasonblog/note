
#ifndef PROCESS_H
#define	PROCESS_H

#define PROCESS_STATUS_RUNNING		0	// In the processor
#define PROCESS_STATUS_WAITING		1	// Waiting its time on the processor
#define PROCESS_STATUS_ZOMBIE		2	// Dead, but waiting child process to terminate
#define PROCESS_STATUS_TERMINATED	3	// Terminated gracefully
#define PROCESS_STATUS_ABORTED		4	// Terminated abnormally

typedef struct {
    unsigned int 	id;
    char * 			name;
    unsigned int 	parent;
    unsigned long 	stack_pointer;
	unsigned long 	pc;
    unsigned int 	times_loaded;
    unsigned int 	status;
} process;

extern void sample_process_1();

extern void sample_process_2();

#endif	/* PROCESS_H */

