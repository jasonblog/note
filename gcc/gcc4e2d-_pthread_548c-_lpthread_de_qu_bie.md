# gcc中-pthread和-lpthread的區別


最近在使用linux mint15，裡面自帶的gcc時4.7的，當我編譯多線程程序時，使用-lpthread居然說沒有找到線程庫函數！！！然後man了一下，才發現在gcc 4.7中鏈接線程庫使用-pthread，而不是用-lpthread，為了搞清楚他們之間的區別，我進行了以下測試（測試方法我查找了好多網上的資料）：
用gcc編譯使用了POSIX thread的程序時通常需要加額外的選項，以便使用thread-safe的庫及頭文件，一些老的書裡說直接增加鏈接選項 -lpthread 就可以了，像這樣：


```sh
gcc -c x.c  
gcc x.o -ox -lpthread  
```


而gcc手冊裡則指出應該在編譯和鏈接時都增加 -pthread 選項，像這樣：

```sh
gcc -pthread -c x.c  
gcc x.o -ox -pthread  
```


那麼 -pthread 相比於 -lpthread 鏈接選項究竟多做了什麼工作呢？我們可以在verbose模式下執行一下對應的gcc命令行看出來。下面是老式的直接加 -lpthread 鏈接選項的輸出結果：


```sh
$ gcc -v -c x.c  
...  
/usr/lib/gcc/i486-linux-gnu/4.2.4/cc1 -quiet -v x.c -quiet -dumpbase x.c  
-mtune=generic -auxbase x -version -fstack-protector -fstack-protector -o /tmp/cch4ASTF.s  
...  
as --traditional-format -V -Qy -o x.o /tmp/cch4ASTF.s  
...  
$ gcc -v x.o -ox -lpthread  
...  
 /usr/lib/gcc/i486-linux-gnu/4.2.4/collect2 --eh-frame-hdr -m elf_i386 --hash-style=both  
-dynamic-linker /lib/ld-linux.so.2 -ox  
/usr/lib/gcc/i486-linux-gnu/4.2.4/../../../../lib/crt1.o  
/usr/lib/gcc/i486-linux-gnu/4.2.4/../../../../lib/crti.o  
/usr/lib/gcc/i486-linux-gnu/4.2.4/crtbegin.o  
-L/opt/intel/Compiler/11.1/046/tbb/ia32/cc4.1.0_libc2.4_kernel2.6.16.21/lib/../lib  
-L/usr/lib/gcc/i486-linux-gnu/4.2.4  
-L/usr/lib/gcc/i486-linux-gnu/4.2.4  
-L/usr/lib/gcc/i486-linux-gnu/4.2.4/../../../../lib  
-L/lib/../lib  
-L/usr/lib/../lib  
-L/opt/intel/Compiler/11.1/046/lib/ia32  
-L/opt/intel/Compiler/11.1/046/tbb/ia32/cc4.1.0_libc2.4_kernel2.6.16.21/lib  
-L/usr/lib/gcc/i486-linux-gnu/4.2.4/../../..  
x.o -lpthread -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc  
--as-needed -lgcc_s --no-as-needed  
/usr/lib/gcc/i486-linux-gnu/4.2.4/crtend.o /usr/lib/gcc/i486-linux-gnu/4.2.4/../../../../lib/crtn.o  
```

下面是在編譯和鏈接時分別指定 -pthread 選項的輸出結果：
```sh
$ gcc -v -pthread -c x.c  
...  
/usr/lib/gcc/i486-linux-gnu/4.2.4/cc1 -quiet -v <strong>-D_REENTRANT</strong>  
 x.c -quiet -dumpbase x.c  
-mtune=generic -auxbase x -version -fstack-protector -fstack-protector -o /tmp/cc205IQf.s  
...  
as --traditional-format -V -Qy -o x.o /tmp/cc205IQf.s  
...  
$ gcc -v x.o -ox -pthread  
/usr/lib/gcc/i486-linux-gnu/4.2.4/collect2 --eh-frame-hdr -m elf_i386 --hash-style=both  
-dynamic-linker /lib/ld-linux.so.2 -ox  
/usr/lib/gcc/i486-linux-gnu/4.2.4/../../../../lib/crt1.o  
/usr/lib/gcc/i486-linux-gnu/4.2.4/../../../../lib/crti.o  
/usr/lib/gcc/i486-linux-gnu/4.2.4/crtbegin.o  
-L/opt/intel/Compiler/11.1/046/tbb/ia32/cc4.1.0_libc2.4_kernel2.6.16.21/lib/../lib  
-L/usr/lib/gcc/i486-linux-gnu/4.2.4  
-L/usr/lib/gcc/i486-linux-gnu/4.2.4  
-L/usr/lib/gcc/i486-linux-gnu/4.2.4/../../../../lib  
-L/lib/../lib  
-L/usr/lib/../lib  
-L/opt/intel/Compiler/11.1/046/lib/ia32  
-L/opt/intel/Compiler/11.1/046/tbb/ia32/cc4.1.0_libc2.4_kernel2.6.16.21/lib  
-L/usr/lib/gcc/i486-linux-gnu/4.2.4/../../..  
x.o -lgcc --as-needed -lgcc_s --no-as-needed <strong>-lpthread</strong>  
 -lc -lgcc  
--as-needed -lgcc_s --no-as-needed  
/usr/lib/gcc/i486-linux-gnu/4.2.4/crtend.o /usr/li
```

 可見編譯選項中指定 -pthread 會附加一個宏定義` -D_REENTRANT`，該宏會導致 libc 頭文件選擇那些thread-safe的實現；鏈接選項中指定 -pthread 則同 -lpthread 一樣，只表示鏈接 POSIX thread 庫。由於 libc 用於適應 thread-safe 的宏定義可能變化，因此在編譯和鏈接時都使用 -pthread 選項而不是傳統的 -lpthread 能夠保持向後兼容，並提高命令行的一致性。


