CFLAGS =-g -Wall
LDFLAGS =
PROCS = const_pointer align getopt getopt_long libdl

all: ${PROCS}

libdl: libdl.o
	gcc ${LDFLAGS} -rdynamic -ldl -o $@ $<

%: %.o
	gcc ${LDFLAGS} -o $@ $<
%.o: %.c
	gcc ${CFLAGS} -o $@ $< -c
clean:
	rm -rf ${PROCS} ${TRACE} *.o
