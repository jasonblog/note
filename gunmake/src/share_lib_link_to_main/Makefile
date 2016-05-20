CC = gcc
SOFLAGS = -shared -fPIC
LDFLAGS = -lm
CFLAGS = -Wall -pedantic -ggdb3 -O0 -std=c99 -lpthread
 
all: add.so sub.so main

main:
	${CC}  -o main  main.c -L./ -ladd -L./ -lsub ${CFLAGS} ${LDFLAGS} 

add.so: add.c
	$(CC) ${SOFLAGS} -o libadd.so $? 

sub.so: sub.c
	$(CC) ${SOFLAGS} -o libsub.so $? 

.PHONY: clean
clean:
	rm -f main *.so *.o
