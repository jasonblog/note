CPP = g++
CPPFLAGS = -g -std=c++0x
OFLAG = -o
.SUFFIXES : .o .cpp .c
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<
.c.o :
	$(CPP) $(CPPFLAGS) -c $<

PROGS = stack queue queuelist queuelist2 list

all: $(PROGS)

stack: stack.o 
	$(CPP) $(OFLAG) stack stack.o -lpthread

queue: queue.o 
	$(CPP) $(OFLAG) queue queue.o -lpthread

queuelist: queuelist.o 
	$(CPP) $(OFLAG) queuelist queuelist.o -lpthread

queuelist2: queuelist2.o 
	$(CPP) $(OFLAG) queuelist2 queuelist2.o -lpthread

lookuptable: lookuptable.o 
	$(CPP) $(OFLAG) lookuptable lookuptable.o -lpthread

list: list.o 
	$(CPP) $(OFLAG) list list.o -lpthread

clean:
	rm -f ${PROGS} *.o

