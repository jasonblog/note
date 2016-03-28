# X86_QEMU

- build gdb


```c
如果gdb提示：GDB Remote 'g' packet reply is too long
修改gdb/remote.c文件，屏蔽process_g_packet函數中的下列兩行：

#if 0
    if (buf_len > 2 * rsa->sizeof_g_packet) {
        error(_("Remote 'g' packet reply is too long: %s"), rs->buf);
    }
#else
    if (buf_len > 2 * rsa->sizeof_g_packet) {
        rsa->sizeof_g_packet = buf_len ;

        for (i = 0; i < gdbarch_num_regs(gdbarch); i++) {
            if (rsa->regs[i].pnum == -1) {
                continue;
            }

            if (rsa->regs[i].offset >= rsa->sizeof_g_packet) {
                rsa->regs[i].in_g_packet = 0;
            } else {
                rsa->regs[i].in_g_packet = 1;
            }
        }
    }
#endif
```

```sh
./configure --prefix=/usr/local/gdb-7.9.1
make -j24
sudo make install
```
-  tftp server

```sh
sudo apt-get install tftp-hpa tftpd-hpa

/etc/default/tftpd-hpa


# /etc/default/tftpd-hpa
TFTP_USERNAME="tftp"
TFTP_DIRECTORY="/home/shihyu/tftpboot"
TFTP_ADDRESS="0.0.0.0:69"
TFTP_OPTIONS="-l -c -s"
```

```sh
mkdir ~/tftpboot
chmod 777 ~/tftpboot
```
- restart service

```sh
sudo service tftpd-hpa restart
```


- qemu using tftp

```sh
#tftp上傳 , 下面操作都是在 qemu shell 執行 command

tftp -p -l init  192.168.100.9
-p：Put file
-l ：Local FILE
init：上傳的檔案
192.168.100.9：tftp server的IP

tftp -g -r test.c   192.168.100.9
-g： Get file
-r ：Remote FILE
test.c：下載的檔案
192.168.100.9：tftp server的IP
```

- tftp_get & tftp_push

```sh
/ # tftp_push init
init                 100% |*******************************|   325   0:00:00 ETA
```

- x86_build.sh

```sh
#! /bin/bash

BUSYBOX_SRC_URL="http://busybox.net/downloads/busybox-1.23.2.tar.bz2"
KERNEL_SRC_URL="https://www.kernel.org/pub/linux/kernel/v4.x/testing/linux-4.3-rc3.tar.xz"
DROPBEAR_SRC_URL="https://matt.ucc.asn.au/dropbear/dropbear-2016.73.tar.bz2"

BUSYBOX="busybox_src"
KERNEL="linux_src"
DROPBEAR="dropbear_src"

BUSYBOX_PALTFORM="busybox-x86"
LINUX_PALTFORM="linux-x86-basic"


usage() {
cat <<USAGE

Usage:
    bash $0 <TARGET_PRODUCT> [OPTIONS]

Description:
    Builds Android tree for given TARGET_PRODUCT

OPTIONS:
    -c, --clean_build
        Clean build - build from scratch by removing entire out dir

    -d, --debug
        Enable debugging - captures all commands while doing the build

    -h, --help
        Display this help message

    -i, --image
        Specify image to be build/re-build (bootimg/sysimg/usrimg)

    -j, --jobs
        Specifies the number of jobs to run simultaneously (Default: 8)

    -k, --kernel_defconf
        Specify defconf file to be used for compiling Kernel

    -l, --log_file
        Log file to store build logs (Default: <TARGET_PRODUCT>.log)

    -m, --module
        Module to be build

    -p, --project
        Project to be build

    -s, --setup_ccache
        Set CCACHE for faster incremental builds (true/false - Default: true)

    -u, --update-api
        Update APIs

    -v, --build_variant
        Build variant (Default: userdebug)

USAGE
}

clean_build() {
    TOP=`pwd`
    echo $TOP

    mkdir "$BUSYBOX"
    mkdir "$KERNEL"
    mkdir "$DROPBEAR"
        
    wget -P /tmp/ "$BUSYBOX_SRC_URL" && tar xjf "/tmp/`basename $BUSYBOX_SRC_URL`" -C "$BUSYBOX" --strip-components=1
    wget -P /tmp/ "$KERNEL_SRC_URL" && tar xJf "/tmp/`basename $KERNEL_SRC_URL`" -C "$KERNEL" --strip-components=1
    wget -P /tmp/ "$DROPBEAR_SRC_URL" && tar xjf "/tmp/`basename $DROPBEAR_SRC_URL`" -C "$DROPBEAR" --strip-components=1

    ## build busybox
    cd $TOP/$BUSYBOX
    mkdir -pv ../obj/$BUSYBOX_PALTFORM
    make mrproper
    make  O=../obj/$BUSYBOX_PALTFORM defconfig
    sed -i 's/.*CONFIG_STATIC.*/CONFIG_STATIC=y/' ../obj/$BUSYBOX_PALTFORM/.config
    sed -i 's/.*CONFIG_INETD.*/CONFIG_INETD=n/' ../obj/$BUSYBOX_PALTFORM/.config
    make clean
    make  O=../obj/$BUSYBOX_PALTFORM -j12 2>&1 | tee  ../busybox_build.log
    make  O=../obj/$BUSYBOX_PALTFORM install

    # initramfs
    mkdir -p $TOP/initramfs/"$BUSYBOX_PALTFORM"
    cd $TOP/initramfs/"$BUSYBOX_PALTFORM"
    mkdir -pv {bin,sbin,etc/init.d,proc,sys,dev,lib,usr/{bin,sbin}}
    cp -av $TOP/obj/$BUSYBOX_PALTFORM/_install/* .

    cd dev
    sudo mknod -m 660 null c 1 3
    ln -sf null tty2
    ln -sf null tty3
    ln -sf null tty4
    cd ..

    # x86_64 lib
    cp -a /lib/x86_64-linux-gnu/ lib/
    cp -a /lib64/ .

    echo "tftp -p -l \$1 192.168.100.9" > bin/tftp_push
    echo "tftp -g -r \$1 192.168.100.9" > bin/tftp_get
    chmod 755 bin/tftp_push
    chmod 755 bin/tftp_get

cd $TOP/initramfs/"$BUSYBOX_PALTFORM"

# root::0:0:root:/:/bin/sh  // 不用密碼
# f0409
cat << EOF >>etc/passwd
root:95OnzVR4r6HAM:0:0:root:/root:/bin/sh
EOF

cat << EOF >>init
#!/bin/sh

/sbin/ifconfig lo 127.0.0.1
ifconfig eth0 down
ifconfig eth0 hw ether 08:90:90:59:62:21
ifconfig eth0 192.168.100.5 netmask 255.255.255.0 up
route add default gw 192.168.100.1

mount -t proc none /proc
mount -t sysfs none /sys
mount -t ramfs none /dev
mdev -s

# tty & ssh
mkdir -p /dev/pts
mount -t devpts devpts /dev/pts
dropbear -p 2222

# mount devpts in order to use telnetd
#mkdir /dev/pts
#mount -t devpts devpts /dev/pts
/usr/sbin/telnetd -l /bin/login

echo -e "\nBoot took $(cut -d" " -f1 /proc/uptime) seconds\n"
exec /bin/sh
EOF

    chmod +x init

    # build dropbear
    cd $TOP/$DROPBEAR
    ./configure --prefix="$TOP"/initramfs/"$BUSYBOX_PALTFORM"
    make PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp"
    make PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" install
    cd $TOP/initramfs/"$BUSYBOX_PALTFORM"
    mkdir -p etc/dropbear
    rm -rf share
    cd bin
    ./dropbearkey -t dss -f ../etc/dropbear/dropbear_dss_host_key
    ./dropbearkey -t rsa -s 1024 -f ../etc/dropbear/dropbear_rsa_host_key
    cd ..

    find . -print0 \
        | cpio --null -ov --format=newc \
        | gzip -9 > $TOP/obj/initramfs-$BUSYBOX_PALTFORM.cpio.gz

    # build kernel
    cd $TOP
    mkdir -pv obj/$LINUX_PALTFORM
    make -C $KERNEL O=../obj/$LINUX_PALTFORM mrproper
    make -C $KERNEL O=../obj/$LINUX_PALTFORM x86_64_defconfig
    sed -i 's/.*CONFIG_EXPERIMENTAL.*/CONFIG_EXPERIMENTAL=y/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_DEBUG_INFO.*/CONFIG_DEBUG_INFO=y/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_KGDB.*/CONFIG_KGDB=y/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_KGDB_LOW_LEVEL_TRAP.*/CONFIG_KGDB_LOW_LEVEL_TRAP=y/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_FRAME_POINTER.*/CONFIG_FRAME_POINTER=y/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_MAGIC_SYSRQ.*/CONFIG_MAGIC_SYSRQ=y/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_8139CP.*/CONFIG_8139CP=y/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_DEBUG_SET_MODULE_RONX.*/CONFIG_DEBUG_SET_MODULE_RONX=n/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_DEBUG_RODATA.*/CONFIG_DEBUG_RODATA=n/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_MODULE_FORCE_LOAD.*/CONFIG_MODULE_FORCE_LOAD=y/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_MODULE_UNLOAD.*/CONFIG_MODULE_UNLOAD=y/' obj/$LINUX_PALTFORM/.config
    sed -i 's/.*CONFIG_MODULE_FORCE_UNLOAD.*/CONFIG_MODULE_FORCE_UNLOAD=y/' obj/$LINUX_PALTFORM/.config

cat << EOF >> obj/$LINUX_PALTFORM/.config
CONFIG_KPROBES_ON_FTRACE=y
CONFIG_UPROBES=y
CONFIG_TRACE_IRQFLAGS=y
CONFIG_TRACER_MAX_TRACE=y
CONFIG_RING_BUFFER_ALLOW_SWAP=y
CONFIG_FTRACE=y
CONFIG_FUNCTION_TRACER=y
CONFIG_FUNCTION_GRAPH_TRACER=y
CONFIG_IRQSOFF_TRACER=y
CONFIG_SCHED_TRACER=y
CONFIG_FTRACE_SYSCALLS=y
CONFIG_TRACER_SNAPSHOT=y
CONFIG_TRACER_SNAPSHOT_PER_CPU_SWAP=y
CONFIG_STACK_TRACER=y
CONFIG_UPROBE_EVENT=y
CONFIG_DYNAMIC_FTRACE=y
CONFIG_DYNAMIC_FTRACE_WITH_REGS=y
CONFIG_FUNCTION_PROFILER=y
CONFIG_FTRACE_MCOUNT_RECORD=y
CONFIG_FTRACE_SELFTEST=y
CONFIG_FTRACE_STARTUP_TEST=y
CONFIG_EVENT_TRACE_TEST_SYSCALLS=y
CONFIG_MMIOTRACE=y
CONFIG_PERCPU_RWSEM=y
EOF

    yes '' | make -C $KERNEL O=../obj/$LINUX_PALTFORM oldconfig
    make -C $KERNEL O=../obj/$LINUX_PALTFORM clean
    time make -C $KERNEL O=../obj/$LINUX_PALTFORM -j12 2>&1 | tee kernel_build.log
}

kernel_build() {
    make -C $KERNEL O=../obj/$LINUX_PALTFORM clean
    time make -C $KERNEL O=../obj/$LINUX_PALTFORM -j12 2>&1 | tee kernel_build.log
}

qemu() {
    qemu-system-x86_64 \
    -enable-kvm -m 1024 \
    -localtime \
    -kernel obj/$LINUX_PALTFORM/arch/x86_64/boot/bzImage \
    -initrd obj/initramfs-$BUSYBOX_PALTFORM.cpio.gz \
    -serial stdio \
    -net nic \
    -net tap,ifname=tap0,script=no \
    -append "root=/dev/ram rdinit=/init console=ttyS0 noapic"
}

gdb() {
    qemu-system-x86_64 -s -S \
    -kernel obj/"$LINUX_PALTFORM"/arch/x86_64/boot/bzImage \
    -initrd obj/initramfs-"$BUSYBOX_PALTFORM".cpio.gz \
    -serial stdio \
    -append "console=ttyS0"
}

initramfs() {
    TOP=`pwd`
    cd $TOP/initramfs/"$BUSYBOX_PALTFORM"

    #find ./ | cpio -o -H newc | gzip > $TOP/obj/initramfs-$BUSYBOX_PALTFORM.cpio.gz

    find . -print0 \
    | cpio --null -ov --format=newc \
    | gzip -9 > $TOP/obj/initramfs-$BUSYBOX_PALTFORM.cpio.gz
}

while true; do
    case "$1" in
        -c|--clean_build) clean_build; exit 0;;
        -h|--help) usage; exit 0;;
        -r|--run) qemu; exit 0;;
        -i|--initramfs) initramfs; exit 0;;
        -d|--debug) gdb; exit 0;;
        -k|--kernel) kernel_build; exit 0;;
        --) shift; break;;
    esac
    shift
done
```

- nettap.sh

` sude ./nettap.sh`

```sh
tunctl -u shihyu -t tap0
ifconfig tap0 192.168.100.9 up
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
iptables -I FORWARD 1 -i tap0 -j ACCEPT
iptables -I FORWARD 1 -o tap0 -m state --state RELATED,ESTABLISHED -j ACCEPT
```

- hello.c

```c
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    while(1) {
        printf("Hello world\n");
        sleep(10);
    }

    return 0;
}
```

- trace_me.sh


```sh
#!/bin/sh

pause() {
    read -n 1 -p "$*" INP
    if [ "$INP" != '' ] ; then
        echo -ne '\b \n'
    fi
}

if [ ! -d "/sys/kernel/debug/tracing" ]; then
mount -t debugfs none /sys/kernel/debug
fi

echo nop > /sys/kernel/debug/tracing/current_tracer
echo 16384 > /sys/kernel/debug/tracing/buffer_size_kb
echo 0 > /sys/kernel/debug/tracing/tracing_on
echo  > /sys/kernel/debug/tracing/trace
pause 'press any key to start capturing...'

echo function_graph > /sys/kernel/debug/tracing/current_tracer
echo funcgraph-proc > /sys/kernel/debug/tracing/trace_options
echo 1 > /sys/kernel/debug/tracing/tracing_on


./fork


echo "Start recordng ftrace data"
pause "Press any key to stop..."
echo "Recording stopped..."
echo 0 > /sys/kernel/debug/tracing/tracing_on

echo "copying [+]"
time cp /sys/kernel/debug/tracing/trace ~/
echo "copying [-]"

FileDate=`date +%Y%m%d_%H%M%S`

tar zcvf  "$FileDate".tar.gz trace
tftp_push "$FileDate".tar.gz
```

- fork.c

```c
#include <unistd.h>
#include <stdio.h>

int main()
{
   fork();
   printf("%d\n",getpid());
   return 0;
}
```

```sh
cat /sys/kernel/debug/tracing/trace
```

- Makefile

```sh
debug:
	@sh build.sh -d

init:
	@sh build.sh -i

build:
	@sh build.sh -c

qemu:
	@sh build.sh -r
```

- gdb

```sh
cgdb ./vmlinux
target remote localhost:1234
b *start_kernel
c
```

