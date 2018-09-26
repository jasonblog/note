CPP = g++
CPPFLAGS = -g -std=c++0x
OFLAG = -o
.SUFFIXES : .o .cpp .c
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<
.c.o :
	$(CPP) $(CPPFLAGS) -c $<

PROGS = queue quicksort

all: $(PROGS)

queue: queue.o 
	$(CPP) $(OFLAG) queue queue.o -lpthread

quicksort: quicksort.o 
	$(CPP) $(OFLAG) quicksort quicksort.o -lpthread

clean:
	rm -f ${PROGS} *.o

