CFLAGS =-g -Wall
LDFLAGS =-lev
PROCS = misc example timer signal child filestat periodic #io_timer_signal

all: ${PROCS}

### examples fro ptrace() system call.
#misc: misc.o
#example: example.o
#timer: timer.o
#signal: signal.o
#child: child.o
#filestat: filestat.o
#periodic: periodic.o
#io_timer_signal: io_timer_signal.o

%: %.o
	gcc ${LDFLAGS} -o $@ $<
%.o: %.c
	gcc ${CFLAGS} -o $@ $< -c
clean:
	rm -rf ${PROCS} ${TRACE} *.o
