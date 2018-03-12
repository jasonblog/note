CFLAGS =-g -Wall -std=c99
LDFLAGS =
PROCS = avltree_test


ifeq ($(coverage), yes)
	CFLAGS += -fprofile-arcs -ftest-coverage -g3
	LDFLAGS += -fprofile-arcs -ftest-coverage
endif

all: ${PROCS}

avltree_test: avltree_test.o avltree.o

%: ^.o
	gcc ${LDFLAGS} -o $@ $<
%.o: %.c
	gcc ${CFLAGS} -o $@ $< -c
clean:
	rm -rf ${PROCS} ${TRACE} *.o *.gcno *.gcda *.gcov
