CFLAGS = -g -Wall
PROCS = accept epoll reuseport

all: ${PROCS}

%.o: %.c
	gcc ${CFLAGS} -o $@ $< -c
clean:
	rm -rf ${PROCS} *.o
