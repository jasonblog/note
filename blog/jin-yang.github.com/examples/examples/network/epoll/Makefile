CFLAGS = -g -Wall
PROCS = server client

all: ${PROCS}
server: server.o common.h
client: client.o common.h

%.o: %.c
	gcc ${CFLAGS} -o $@ $< -c
clean:
	rm -rf ${PROCS} *.o
