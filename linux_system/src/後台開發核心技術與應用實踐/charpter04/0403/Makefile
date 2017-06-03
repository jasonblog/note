helloworld:file1.o file2.o
	g++ file1.o file2.o -o helloworld

file2.o:file2.cpp
	g++ -c file2.cpp -o file2.o

file1.o:file1.cpp file1.h
	g++ -c file1.cpp -o file1.o

clean:
	rm -rf *.o helloworld
