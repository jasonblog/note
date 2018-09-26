CPP = g++-4.9
CPPFLAGS = -g -std=c++0x
OFLAG = -o
.SUFFIXES : .o .cpp .c
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<
.c.o :
	$(CPP) $(CPPFLAGS) -c $<

PROGS = stack stack_hp queue_spsc queue_mpmc 

all: $(PROGS)

stack: stack.o 
	$(CPP) $(OFLAG) stack stack.o -lpthread

stack_hp: stack_hp.o 
	$(CPP) $(OFLAG) stack_hp stack_hp.o -lpthread

stack_sp: stack_sp.o 
	$(CPP) $(OFLAG) stack_sp stack_sp.o -lpthread

stack_ref: stack_ref.o 
	$(CPP) $(OFLAG) stack_ref stack_ref.o -lpthread

queue_spsc: queue_spsc.o 
	$(CPP) $(OFLAG) queue_spsc queue_spsc.o -lpthread

queue_mpmc: queue_mpmc.o 
	$(CPP) $(OFLAG) queue_mpmc queue_mpmc.o -lpthread

clean:
	rm -f ${PROGS} *.o

