# KGDB调试LINUX内核


摘要: 
本文介绍了如何在DB12X板子上开启KGDB调试功能.如何运行AGENT-PROXY进行代理串口的访问.如何运行GDB单步跟踪. 

1.环境 
我的环境是: 
一块DB12X ATHEROS AP 板子,MIPS架构,内核LINUX 2.6.31 
一台UBUNTU 14.04电脑. 

2.修改内核配置 
进入SDK的LINUX内核目录 
```sh
cd linux/kernels/mips-linux-2.6.31/arch/mips/configs/ 
```

采用DEBUG版本的配置文件 

```sh
cp db12x-debug_defconf db12x_defconf
```

3.关闭看门狗 
```sh
cd linux/kernels/mips-linux-2.6.31/arch/mips/atheros/ 
vi wdt.c
```

找到70行,然后注释掉: 
```c
//ath_reg_wr(ATH_WATCHDOG_TMR, usec);
```

４.编译内核

５.修改kermit的脚本启动参数

需要增加`kgdboc=ttyS0,115200 kgdbwait` 参数 
`kgdboc` 指的是OVER CONSOLE的KGDB使能 
`KGDBWAIT`指的是启动一开始就等着GDB连接 
如果如下: 

```sh
LINEOUT “setenv bootargs kgdboc=ttyS0,115200 kgdbwait console=ttyS0,115200 root=31:02 rootfstype=squashfs init=/sbin/init mtdparts=ath-nor0:256k(u-boot),256k(u-boot-env),6m(rootfs0),1m(uImage0),6m(rootfs1),1m(uImage1),320k(config),1m(reserved),64k(log),64k(manu),64k(ART)” 
INPUT 10 “ar7240>”
```

6.配置串口代理 
启动agent-proxy,在5550端口侦听,等待ＴＥＬＮＥＴ连接 
在５５５１端口侦听,ＧＤＢ的连接. 
所有连接的数据都代理到串口/dev/ttyUSB0去,波特率１１５２００ 
ubuntu主机上运行的结果如下: 

```sh
sudo ./agent-proxy 5550^5551 0 /dev/ttyUSB0,115200 
[sudo] password for snowpiaop: 
Agent Proxy 1.96 Started with: 5550^5551 0 /dev/ttyUSB0,115200 
Agent Proxy running. pid: 15130
```

telnet 到串口５５５０ 
telnet localhost 5550 
这样就相当于运行minicom连接串口了一样,能够看到串串口的输出.

启动内核 
此时会停在kdb处. 
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


表示,已经进入等待远端GDB连接状态.

９.运行GDB连接内核KGDB 

```sh
/opt/toolchains/mips-linux-uclibc/4.2.4/usr/bin/mips-linux-uclibc-gdb vmlinux 
进入了gdb中,再输入如下命令,(gdb)是提示符: 
(gdb) tar rem localhost:5551 
会有如下输出: 
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

这时,就可以用GDB的命令进行内核的调试.

如果启动参数中没有指定`KGDBWAIT`,也没关系,进入ＤＢ１２Ｘ板子串口执行 

```sh
echo g >/proc/sysrq-trigger
```
以便让GDB停下来,进入到(gdb)提示符下,以便添加断点.