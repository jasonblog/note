# rpi, serial and kgdb


###  configurations for the kernel.


```sh
CONFIG_KGDB=y
CONFIG_KGDB_KDB=y
CONFIG_KGDB_SERIAL_CONSOLE=y 
CONFIG_MAGIC_SYSRQ=y 
CONFIG_KGDB_KEYBOARD=y
CONFIG_DEBUG_KERNEL=y
```
Here's a short documentation on how to use the serial port on the rpi with 
console and gdb:

* Build your kernel with debugging symbols and kgdb (over serial) enabled.


* Deploy it, boot it.


* Change the kernel cmdline and add:

console=ttyAMA0,115200 kgdboc=ttyAMA0,115200

1st is the serial console, 2nd is the gdb connection.


* download and build agent-proxy
git clone http://git.kernel.org/pub/scm/utils/kernel/kgdb/agent-proxy.git
cd agent-proxy
make
./agent-proxy 5550^5551 0 /dev/ttyUSB0,115200 


Now we have console on port 5550 and gdb on port 5551.
Boot the board and ...

```sh
a) telnet 127.0.0.1:5550  -> console
b) gdb:

gdb vmlinux
- set remotebaud 115200

```


## 參考

https://sergioprado.org/acessando-a-console-serial-na-raspberry-pi/

http://lists.linuxfoundation.org/pipermail/llvmlinux/2013-January/000122.html

https://www.raspberrypi.org/forums/viewtopic.php?t=69843&p=507579