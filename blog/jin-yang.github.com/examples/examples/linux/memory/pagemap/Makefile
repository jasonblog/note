CFLAGS = -g -Wall
LDFLAGS =
PROCS = pagemap foobar

all: ${PROCS}

%.o: %.c
	gcc ${CFLAGS} -o $@ $< -c
clean:
	rm -rf ${PROCS} *.o
