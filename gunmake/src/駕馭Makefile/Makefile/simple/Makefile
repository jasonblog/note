simple.exe: main.o foo.o
	gcc -o simple.exe main.o foo.o
main.o: main.c
	gcc -c main.c -o main.o
foo.o: foo.c
	gcc -c foo.c -o foo.o
clean:
	rm simple.exe main.o foo.o
