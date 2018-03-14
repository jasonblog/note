.SUFFIXES:.c .o
CC=gcc
CFLAGS = -g -Wall
LDFLAGS = -lrt

POSIX_SRC = posix.c 
POSIX_OBJ = $(POSIX_SRC:.c=.o)  
POSIX_EXE = posix

SHMV_SRC = shmv.c
SHMV_OBJ = $(SHMV_SRC:.c=.o)  
SHMV_EXE = shmv

all: $(POSIX_OBJ) $(SHMV_OBJ) 
	$(CC) -o $(POSIX_EXE) $(POSIX_OBJ) $(LDFLAGS)
	$(CC) -o $(SHMV_EXE) $(SHMV_OBJ) $(LDFLAGS)


%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -rf ${POSIX_EXE} $(SHMV_EXE) *.o
