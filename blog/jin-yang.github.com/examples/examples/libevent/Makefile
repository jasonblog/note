CFLAGS = -g -Wall
LDFLAGS = -levent
PROCS = simple #evbuffer

all: ${PROCS}

#evbuffer: evbuffer.o

%: %.o
	gcc ${LDFLAGS} -o $@ $<
%.o: %.c
	gcc ${CFLAGS} -o $@ $< -c

clean:
	rm -rf ${PROCS} ${TRACE} *.o
