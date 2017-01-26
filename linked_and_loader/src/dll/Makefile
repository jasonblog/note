export  LD_LIBRARY_PATH=LD_LIBRARY_PATH:./

CC = gcc

all: dl.so 
	#$(CC) -o main tdl.c -L./ -ldl 
	$(CC) -o main tdl.c ./libdl.so 
dl.so:
	$(CC) -O -fPIC -shared -o libdl.so d2.c 
       
.PHONY: clean
clean:                             
	@rm -rf *.o *.so main    
