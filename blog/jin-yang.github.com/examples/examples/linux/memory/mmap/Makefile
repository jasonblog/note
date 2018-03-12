CFLAGS = -g -Wall
LDFLAGS=

PROCS = filemap sharemem1 sharemem2 anonymem

all: ${PROCS}

filemap: 0-filemap.c
	@echo "just for test" > file.txt
	gcc ${CFLAGS} -o $@ $^

sharemem1: 1-sharemem1.c
	gcc ${CFLAGS} -o $@ $^
sharemem2: 1-sharemem2.c
	gcc ${CFLAGS} -o $@ $^

anonymem: 2-anonymem.c
	gcc ${CFLAGS} -o $@ $^

%.o: %.c
	gcc ${CFLAGS} -o $@ $< -c

clean:
	rm -rf ${PROCS} *.o file.txt share.txt
