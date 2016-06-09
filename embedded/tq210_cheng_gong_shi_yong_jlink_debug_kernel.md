# TQ210 成功使用 JLink debug kernel


##需要編譯 gdb  target 支援 arm

```sh
1. tar xvf gdb-7.8.1.tar.gz
2. cd gdb-7.8.1
3. ./configure --target=arm-linux --prefix=/home/shihyu/data/arm-gdb/arm-gdb
4 make -j8
5. make install
```


```sh
1. 設定 u-boot 讓 u-boot 延長進去 kernel  
setenv bootdelay 45
saveenv
再重開按板子 reset key  

2. 重新啟動到 uboot 選單
3. 再啟動 sudo ./JLinkGDBServer  // uboot 這時候選單會卡住
4. /home/shihyu/data/arm-gdb/arm-gdb/bin/arm-linux-gdb
```


```sh
(gdb)  source gdb_set  // 透過 script

(gdb) target remote localhost:2331
Remote debugging using localhost:2331
0x00000000 in ?? ()
(gdb) monitor endian little
Target endianess set to "little endian"
(gdb) monitor speed 30
Target interface speed set to 30 kHz
(gdb) monitor reset
Resetting target
(gdb) file vmlinux
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Reading symbols from vmlinux...done.
(gdb) b start_kernel
Breakpoint 1 at 0xc000866c: file init/main.c, line 461.
(gdb) c
```


目前設定就成功了～  看起來要設定 30 kHz 之後再 monitor reset 才可以



```
# connect to the J-Link gdb server
target remote localhost:2331
# Set JTAG speed to 30 kHz
monitor endian little
monitor speed 30
# Reset the target
monitor reset
monitor sleep 10
 
   
# Setup GDB for faster downloads
#set remote memory-write-packet-size 1024
monitor speed auto
break _start
load
continue
```
