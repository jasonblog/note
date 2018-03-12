CFLAGS = -g -Wall
LDFLAGS = -lrt
PROCS = posix shmv

all: ${PROCS}

%.o: %.c
	gcc ${CFLAGS} -o $@ $< -c
clean:
	rm -rf ${PROCS} *.o
