# 基礎範例

- d2.c

```c
#include <stdio.h>

int p = 3;
void print()
{
    printf("This is the second dll src!\n");
}
```

- tdl.c

```c
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    print();

    return 0;
}
```

```sh
export  LD_LIBRARY_PATH=LD_LIBRARY_PATH:./

CC = gcc

all: dl.so 
	#$(CC) -o main tdl.c -L./ -ldl 
	$(CC) -o main tdl.c -L./ -ldl -Wl,-rpath=/home/shihyu/dll 
	#$(CC) -o main tdl.c ./libdl.so 
dl.so:
	$(CC) -O -fPIC -shared -o libdl.so d2.c 
       
.PHONY: clean
clean:                             
	@rm -rf *.o *.so main    
```

```sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/xxxx/share_lib_path
```

## -L./ -ldl 


### 使用 readelf -d 查詢ELF 的 share lib 路徑 

```sh
readelf -d main  

Dynamic section at offset 0xe18 contains 25 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libdl.so]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]

```

`執行期 Shared library: [libdl.so]未指定路徑, 需要使用 LD_LIBRARY_PATH 指令`

## ./libdl.so // 編譯跟執行期綁死在目前路徑 

`gcc編譯指定寫死路徑 Shared library: [./libdl.so] 靈活性不好`

```sh
readelf -d main  

Dynamic section at offset 0xe18 contains 25 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [./libdl.so]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
```


## -Wl,-rpath=/home/shihyu/dll

- `動態庫路徑寫入到elf檔案 //  (RPATH)              Library rpath: [/home/shihyu/dll]`

```sh
readelf -d main 

Dynamic section at offset 0xe08 contains 26 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libdl.so]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x000000000000000f (RPATH)              Library rpath: [/home/shihyu/dll]
```


### -Wl,-rpath 使用時機
在C/C++程序裡經常會調用到外部庫函數，最常用的方法莫過於export LD_LIBRARY_PATH，不過使用它存在一些弊端，可能會影響到其它程序的運行。在經歷的大項目中就遇到過，兩個模塊同時使用一外部動態庫，而且版本還有差異，導致其中一模塊出錯，兩模塊是不同時期不同人員分別開發，修正起來費時費力。

對於上述問題，一個比較好的方法是在程序`編譯的時候加上參數-Wl,-rpath，指定編譯好的程序在運行時動態庫的目錄。這種方法會將動態庫路徑寫入到elf文件中去`。

