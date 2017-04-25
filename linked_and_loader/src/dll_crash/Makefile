
CC=gcc
all: dl.so 
	#$(CC) -o main tdl.c -L./libs -ldl 
	$(CC) -o main tdl.c -L./libs -ldl -Wl,-rpath=`pwd`/libs 
	#$(CC) -o main tdl.c ./libdl.so 
dl.so:
	$(CC) -O -fPIC -shared -o libs/libdl.so d2.c 
       
.PHONY: clean
clean:                             
	@rm -rf *.o libs/*.so main    
