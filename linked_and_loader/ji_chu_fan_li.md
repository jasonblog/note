# 基礎範例

###`ld可以叫靜態連接器，ld.so可以叫動態連接器`

###`編譯時用的連結器是ld，而運行時用的連結器是/lib/ld-linux.so.2`.

ld-linux.so是專門負責尋找庫文件的庫。
以cat為例，cat首先告訴ld-linux.so它需要libc.so.6這個庫文件，ld-linux.so將按一定順序找到libc.so.6庫再給cat調用。

那ld-linux.so又是怎麼找到的呢？
其實不用找，ld-linux.so的位置是寫死在程序中的，gcc在編譯程序時就寫死在裡面了。Gcc寫到程序中ld-linux.so的位置是可以改變的，通過修改gcc的spec文件。


###編譯時，ld順序：ld --verbose | grep SEARCH
```sh
（1）ld-linux.so.6由gcc的spec文件中所設定 
（2）gcc --print-search-dirs所打印出的路徑，主要是libgcc_s.so等庫。可以通過GCC_EXEC_PREFIX來設定 
（3）LIBRARY_PATH環境變量中所設定的路徑，或編譯的命令行中指定的-L/usr/local/lib 
（4）binutils中的ld所設定的缺省搜索路徑順序，編譯binutils時指定。（可以通過「ld --verbose | grep SEARCH」來查看） 
（5）二進製程序的搜索路徑順序為PATH環境變量中所設定。一般/usr/local/bin高於/usr/bin
（6）編譯時的頭文件的搜索路徑順序，與library的查找順序類似。一般/usr/local/include高於/usr/include
```

###運行時，ld-linux.so查找共享庫的順序：
```sh
（1）ld-linux.so.6在可執行的目標文件中被指定，可用readelf命令查看 
（2）ld-linux.so.6缺省在/usr/lib和lib中搜索；當glibc安裝到/usr/local下時，它查找/usr/local/lib
（3）LD_LIBRARY_PATH環境變量中所設定的路徑 
（4）/etc/ld.so.conf（或/usr/local/etc/ld.so.conf）中所指定的路徑，由ldconfig生成二進制的ld.so.cache中
```



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

