# KGDB調試LINUX內核


摘要: 
本文介紹瞭如何在DB12X板子上開啟KGDB調試功能.如何運行AGENT-PROXY進行代理串口的訪問.如何運行GDB單步跟蹤. 

1.環境 
我的環境是: 
一塊DB12X ATHEROS AP 板子,MIPS架構,內核LINUX 2.6.31 
一臺UBUNTU 14.04電腦. 

2.修改內核配置 
進入SDK的LINUX內核目錄 
```sh
cd linux/kernels/mips-linux-2.6.31/arch/mips/configs/ 
```

採用DEBUG版本的配置文件 

```sh
cp db12x-debug_defconf db12x_defconf
```

3.關閉看門狗 
```sh
cd linux/kernels/mips-linux-2.6.31/arch/mips/atheros/ 
vi wdt.c
```

找到70行,然後註釋掉: 
```c
//ath_reg_wr(ATH_WATCHDOG_TMR, usec);
```

４.編譯內核

５.修改kermit的腳本啟動參數

需要增加`kgdboc=ttyS0,115200 kgdbwait` 參數 
`kgdboc` 指的是OVER CONSOLE的KGDB使能 
`KGDBWAIT`指的是啟動一開始就等著GDB連接 
如果如下: 

```sh
LINEOUT “setenv bootargs kgdboc=ttyS0,115200 kgdbwait console=ttyS0,115200 root=31:02 rootfstype=squashfs init=/sbin/init mtdparts=ath-nor0:256k(u-boot),256k(u-boot-env),6m(rootfs0),1m(uImage0),6m(rootfs1),1m(uImage1),320k(config),1m(reserved),64k(log),64k(manu),64k(ART)” 
INPUT 10 “ar7240>”
```

6.配置串口代理 
啟動agent-proxy,在5550端口偵聽,等待ＴＥＬＮＥＴ連接 
在５５５１端口偵聽,ＧＤＢ的連接. 
所有連接的數據都代理到串口/dev/ttyUSB0去,波特率１１５２００ 
ubuntu主機上運行的結果如下: 

```sh
sudo ./agent-proxy 5550^5551 0 /dev/ttyUSB0,115200 
[sudo] password for snowpiaop: 
Agent Proxy 1.96 Started with: 5550^5551 0 /dev/ttyUSB0,115200 
Agent Proxy running. pid: 15130
```

telnet 到串口５５５０ 
telnet localhost 5550 
這樣就相當於運行minicom連接串口了一樣,能夠看到串串口的輸出.

啟動內核 
此時會停在kdb處. 
如下: 
```sh
ar7240> boot

Booting image at 9f680000 …
Image Name: Linux Kernel Image 
Created: 2016-01-23 12:33:54 UTC 
Image Type: MIPS Linux Kernel Image (lzma compressed) 
Data Size: 1040561 Bytes = 1016.2 kB 
Load Address: 80002000 
Entry Point: 80230a90 
Verifying Checksum at 0x9f680040 …OK 
Uncompressing Kernel Image … OK 
No initrd 
Transferring control to Linux (at address 80230a90) … 
Giving linux memsize in bytes, 67108864

Starting kernel …

Booting Atheros AR934x 
[1970-01-01 00:00:00:000000]Linux version 2.6.31–LSDK-9.2.0_U7.717(gcc version 4.2.4) #5 Sat Jan 23 20:33:42 CST 2016 
[1970-01-01 00:00:00:116000]kgdb: Waiting for connection from remote gdb…

Entering kdb (current=0x83813aa8, pid 1) due to Keyboard Entry 
kdb>
```


表示,已經進入等待遠端GDB連接狀態.

９.運行GDB連接內核KGDB 

```sh
/opt/toolchains/mips-linux-uclibc/4.2.4/usr/bin/mips-linux-uclibc-gdb vmlinux 
進入了gdb中,再輸入如下命令,(gdb)是提示符: 
(gdb) tar rem localhost:5551 
會有如下輸出: 
/opt/toolchains/mips-linux-uclibc/4.2.4/usr/bin/mips-linux-uclibc-gdb ./vmlinux 
GNU gdb 6.8 
Copyright (C) 2008 Free Software Foundation, Inc. 
License GPLv3+: GNU GPL version 3 or later http://gnu.org/licenses/gpl.html 
This is free software: you are free to change and redistribute it. 
There is NO WARRANTY, to the extent permitted by law. Type “show copying” 
and “show warranty” for details. 
This GDB was configured as “–host=i386-pc-linux-gnu –target=mips-linux-uclibc”… 
(no debugging symbols found) 
(gdb) target remote localhost:5551 
Remote debugging using localhost:5551 
0x80010144 in arch_kgdb_breakpoint () 
(gdb) bt 
0 0x80010144 in arch_kgdb_breakpoint () 
1 0x80057be8 in kgdb_breakpoint () 
2 0x80057efc in kgdb_register_io_module () 
3 0x80162a58 in configure_kgdboc () 
4 0x8000243c in do_one_initcall () 
5 0x802d66ac in kernel_init () 
6 0x80008cc8 in kernel_thread_helper () 
7 0x00000000 in ?? () 
(gdb) c 
Continuing.
```

這時,就可以用GDB的命令進行內核的調試.

如果啟動參數中沒有指定`KGDBWAIT`,也沒關係,進入ＤＢ１２Ｘ板子串口執行 

```sh
echo g >/proc/sysrq-trigger
```
以便讓GDB停下來,進入到(gdb)提示符下,以便添加斷點.