SHELL = /bin/bash
CC = gcc
CFLAGS = -g -D_Float32x=float -D_Float64x="long double" -D_Float32=float -D_Float64="long double"
SRC = $(wildcard *.c)
EXE = $(patsubst %.c, %, $(SRC))

all: ${EXE}

%:	%.c
	${CC} ${CFLAGS} $@.c -o $@

clean:
	rm ${EXE}
