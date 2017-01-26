# 基礎範例




```sh
CC = gcc

all: dl.so 
	$(CC) -o main tdl.c -L./ -ldl 
	#$(CC) -o main tdl.c ./libdl.so 
dl.so:
	$(CC) -O -fPIC -shared -o libdl.so d2.c 
       
.PHONY: clean
clean:                             
	@rm -rf *.o *.so main    
```


- 執行期 share library path 未定需要使用 LD_LIBRARY_PATH 指令

```sh
readelf -d main  

Dynamic section at offset 0xe18 contains 25 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libdl.so]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
```