# kgdb for RPI

```sh
CONFIG_MAGIC_SYSRQ=y
CONFIG_DEBUG_KERNEL=y
CONFIG_DEBUG_INFO=y
CONFIG_KGDB_SERIAL_CONSOLE=y 
CONFIG_KGDB_KEYBOARD=y
CONFIG_FRAME_POINTER=y
CONFIG_KALLSYMS=y
CONFIG_KALLSYMS_ALL=y
CONFIG_DEBUG_BUGVERBOSE=y
CONFIG_STACKTRACE=y
```


- ./bootable/configs/cmdline.txt

- add `kgdboc=ttyAMA0,115200 kgdbwait` to cmdline.txt

```sh
dwc_otg.lpm_enable=0 console=ttyAMA0,115200 
kgdboc=ttyAMA0,115200 kgdbwait console=tty1 
root=/dev/mmcblk0p2 rootfstype=ext4 
elevator=deadline fsck.repair=yes rootwait
```

- kdb `go` 

```sh
$3#33  -> go 進系統
```

```sh
echo g > /proc/sysrq-trigger  -> kgdb wait gdb 連進來
```

```sh
cgdb -d arm-linux-gnueabihf-gdb ./out/target/HMD/RPi2/obj/KERNEL_OBJ/vmlinux
```

- gdbinit

```sh
set remotebaud 115200
target remote /dev/ttyUSB1
```