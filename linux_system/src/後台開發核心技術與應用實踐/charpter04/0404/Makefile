OBJS = file1.o file2.o
XX = g++
CFLAGS = -Wall -O -g

helloworld : $(OBJS)
	$(XX) $(OBJS) -o helloworld

file2.o : file2.cpp file1.h
	$(XX) $(CFLAGS) -c file2.cpp -o file2.o

file1.o : file1.cpp file1.h
	$(XX) $(CFLAGS) -c file1.cpp -o file1.o

clean:
	rm -rf *.o helloworld
