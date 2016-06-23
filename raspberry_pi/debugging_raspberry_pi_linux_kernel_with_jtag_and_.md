# Debugging Raspberry Pi Linux kernel with JTAG and GDB



This page explains how the Raspberry Pi Linux kernel can be debugged using JTAG. Most probably there is a better and more elegant way of doing this, but this is what I used.

##Background
As part of the "Linux in Embedded Systems" seminar at Aalto University my task was to look at the Linux kernel boot sequence. Though there is a lot of information about the Linux kernel, it seems to be difficult to find the exact Linux boot sequence - what does the kernel do at what order after it starts at the main. There were basically two possibilities to go through the kernel boot sequence - read through the kernel source code or step through the kernel. I selected the later approach. In order to step through the kernel code, I got myself an Olimex JTAG programmer and installed it on my MacBook Pro (OlimexJTAG). A fellow student made a JTAG capable kernel setup for Raspberry Pi (Remote Debugging). The it was up to me to set up the kernel for debugging.
##Compiling the kernel
In order to use GDB to debug the Linux kernel, the kernel has to be compiled with the debug symbol information. Therefore, the kernel compilation configuration file needs to contain at least the following:
```sh
CONFIG_DEBUG_INFO=y
CONFIG_KGDB=y
CONFIG_KGDB_SERIAL_CONSOLE=y
```

The first one has the magic for the debug data. The others allow debugging through the kernel inbuilt debugger, which might come handy if you're debugging the kernel anyways.
##Setting the debug environment
When you boot the Raspberry Pi with the kernel compiled it stops in an eternal loop and does not really do much. The Linux does not boot at this point to make sure there is time to configure the debugging environment. First thing is to start OpenOCD as described in (OlimexJTAG). Remember that you will need to enable the gdb debugging in the raspi.cfg file as described in that page.
OpenOCD can be started as follows:
openocd -f olimex-arm-usb-ocd-h.cfg -f raspi.cfg
You will have to halt the Raspberry PI's eternal looping and set the Program Counter to the beginning of the Kernel. This is done best by using the OpenOCD interface:
```sh
telnet localhost 4444
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Open On-Chip Debugger
> 
Now the Raspberry Pi should be halted using the OpenOCD interface.
>halt
target state: halted
target halted in ARM state due to debug-request, current mode: Supervisor
cpsr: 0x800001d3 pc: 0x00008014
Now we have to step to the beginning of the Linux kernel:
> step 0x8190
target state: halted
target halted in ARM state due to single-step, current mode: Supervisor
cpsr: 0x800001d3 pc: 0x00008194
Now the kernel is ready to be debugged.

```

##Using GDB to debug the kernel

To be able to debug the kernel, you need the file vmlinux and the whole linux kernel source tree in the directory where you start the kernel debugger. I am using the arm-none-eabi-gdb to debug the kernel.
```sh
$ arm-none-eabi-gdb
```

GNU gdb (GDB) 7.5.1
Copyright (C) 2012 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "--host=x86_64-apple-darwin10.8.0 --target=arm-none-eabi".
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.

```sh
(gdb) file vmlinux
```

Reading symbols from /Users/jonsoini/Documents/Files/PhD/RaspberrryPikurssi/armjtag/raspberrypi/armjtag/linux-rpi-3.6.y/vmlinux...done.
First we need to set up the remote target for GDB. The target is the localhost and the port number configured at the raspi.cfg file:
```sh
(gdb) target remote :5555
```

Remote debugging using :5555
stext () at arch/arm/kernel/head.S:86
86        setmode    PSR_F_BIT | PSR_I_BIT | SVC_MODE, r9 @ ensure svc mode
Then we will have to load the vmlinux (the Linux kernel) with the symbol information. Please, note this will take some time. Thus, some patience is needed here...

```sh
(gdb) load vmlinux 
```

```
Loading section .head.text, size 0x160 lma 0xc0010190
Loading section .text, size 0x3a186c lma 0xc00102f0
Loading section .rodata, size 0xfbf40 lma 0xc03b2190
Loading section __bug_table, size 0x61f8 lma 0xc04ae0d0
Loading section __ksymtab, size 0x6100 lma 0xc04b42c8
Loading section __ksymtab_gpl, size 0x3d08 lma 0xc04ba3c8
Loading section __kcrctab, size 0x3080 lma 0xc04be0d0
Loading section __kcrctab_gpl, size 0x1e84 lma 0xc04c1150
Loading section __ksymtab_strings, size 0x15ed8 lma 0xc04c2fd4
Loading section __param, size 0xce0 lma 0xc04d8eac
Loading section __modver, size 0x604 lma 0xc04d9b8c
Loading section .ARM.unwind_idx, size 0x1c338 lma 0xc04da190
Loading section .ARM.unwind_tab, size 0x3fcc lma 0xc04f64c8
Loading section .init.text, size 0x19854 lma 0xc04fb190
Loading section .exit.text, size 0xcf0 lma 0xc05149e4
Loading section .init.proc.info, size 0x34 lma 0xc05156d4
Loading section .init.arch.info, size 0x44 lma 0xc0515708
Loading section .init.tagtable, size 0x48 lma 0xc051574c
Loading section .init.data, size 0x68a8 lma 0xc0515794
Loading section .data, size 0x34240 lma 0xc051c190
Loading section .notes, size 0x24 lma 0xc05503d0
Start address 0xc0008000, load size 5500384
Transfer rate: 76 KB/sec, 15670 bytes/write.
```

Now you are actually already debugging the kernel and can single step through it or set break points... Have fun!

##OpenOCD bugs
You may encounter a bug when after continuing execution the program counter ends up around address 0 instead of the next instruction. The workaround for this bug is simple: before you begin debugging, run OpenOCD, ensure that it recognizes the JTAG target, exit it by hitting Ctrl-C and restart it again. Now you can attach GDB without having the continue problem.
##Using an IDE to debug the kernel
Once the basic GDB debugging has been setup, you can configure an IDE to step through the source code visually and show the relevant variable values:
If you are using Eclipse, follow this tutorial on setting it up
If you are using Visual Studio, follow the VisualKernel Raspberry PI tutorial