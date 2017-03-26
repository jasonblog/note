CC = gcc
RM = rm -f

SRC = $(wildcard *.c)

PROGRAM = mem_app

OBJ = $(SRC:.c=.o)

CFLAGS = -Wall -g
# LFLAGS = -lssl -Iinclude

all : $(PROGRAM)

$(PROGRAM) : $(OBJ)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJ)
	cp $(PROGRAM) ../

$(OBJ):%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY : all clean
clean :
	$(RM) $(OBJ) $(PROGRAM) ../$(PROGRAM) *~
