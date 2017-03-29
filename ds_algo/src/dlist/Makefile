CC	= gcc
CFLAGS	= -Wall
PROGS	= list_test

all: $(PROGS)

%: %.c list.h
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	$(RM) $(PROGS) $(wildcard *.h.gch)
