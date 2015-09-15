#! /bin/bash

KERNEL=linux-4.1.7
BUSYBOX=busybox-1.23.2

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

    #curl https://www.kernel.org/pub/linux/kernel/v4.x/linux-4.1.7.tar.xz | tar xJf -
    #curl http://busybox.net/downloads/busybox-1.23.2.tar.bz2 | tar xjf -
   
    ## build busybox
    cd $TOP/$BUSYBOX
    mkdir -pv ../obj/busybox-x86
    make mrproper
    make  O=../obj/busybox-x86 defconfig
    sed -i 's/.*CONFIG_STATIC.*/CONFIG_STATIC=y/' ../obj/busybox-x86/.config
    sed -i 's/.*CONFIG_INETD.*/CONFIG_INETD=n/' ../obj/busybox-x86/.config
    make clean
    make  O=../obj/busybox-x86 -j12 2>&1 | tee  ../busybox_build.log
    make  O=../obj/busybox-x86 install

    # initramfs
    mkdir -p $TOP/initramfs/x86-busybox
    cd $TOP/initramfs/x86-busybox
    mkdir -pv {bin,sbin,etc,proc,sys,dev,lib,usr/{bin,sbin}}
    cp -av $TOP/obj/busybox-x86/_install/* .

    cd dev
    sudo mknod -m 660 null c 1 3
    ln -sf null tty2
    ln -sf null tty3
    ln -sf null tty4
    cd ..

    # x86_64 lib
    cp -a /lib/x86_64-linux-gnu/ lib/
    cp -a /lib64/ .

    echo "#!/bin/sh" >> init
    echo "mount -t proc none /proc" >> init
    echo "mount -t sysfs none /sys" >> init
    echo "mount -t ramfs none /dev" >> init
    echo "mdev -s" >> init
    echo 'echo -e "\nBoot took $(cut -d" " -f1 /proc/uptime) seconds\n"' >> init
    echo "exec /bin/sh" >> init

    chmod +x init

    find . -print0 \
        | cpio --null -ov --format=newc \
        | gzip -9 > $TOP/obj/initramfs-busybox-x86.cpio.gz

    # build kernel
    cd $TOP
    mkdir -pv obj/linux-x86-basic
    make -C $KERNEL O=../obj/linux-x86-basic mrproper
    make -C $KERNEL O=../obj/linux-x86-basic x86_64_defconfig
    sed -i 's/.*CONFIG_EXPERIMENTAL.*/CONFIG_EXPERIMENTAL=y/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_DEBUG_INFO.*/CONFIG_DEBUG_INFO=y/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_KGDB.*/CONFIG_KGDB=y/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_KGDB_LOW_LEVEL_TRAP.*/CONFIG_KGDB_LOW_LEVEL_TRAP=y/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_FRAME_POINTER.*/CONFIG_FRAME_POINTER=y/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_MAGIC_SYSRQ.*/CONFIG_MAGIC_SYSRQ=y/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_8139CP.*/CONFIG_8139CP=y/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_DEBUG_SET_MODULE_RONX.*/CONFIG_DEBUG_SET_MODULE_RONX=n/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_DEBUG_RODATA.*/CONFIG_DEBUG_RODATA=n/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_MODULE_FORCE_LOAD.*/CONFIG_MODULE_FORCE_LOAD=y/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_MODULE_UNLOAD.*/CONFIG_MODULE_UNLOAD=y/' obj/linux-x86-basic/.config
    sed -i 's/.*CONFIG_MODULE_FORCE_UNLOAD.*/CONFIG_MODULE_FORCE_UNLOAD=y/' obj/linux-x86-basic/.config

cat << EOF >> obj/linux-x86-basic/.config
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

    yes '' | make -C $KERNEL O=../obj/linux-x86-basic oldconfig
    make -C $KERNEL O=../obj/linux-x86-basic clean
    time make -C $KERNEL O=../obj/linux-x86-basic -j12 2>&1 | tee kernel_build.log
}

qemu() {
    qemu-system-x86_64 \
    -kernel obj/linux-x86-basic/arch/x86_64/boot/bzImage \
    -initrd obj/initramfs-busybox-x86.cpio.gz \
    -serial stdio \
    -append "console=ttyS0" -enable-kvm
}

gdb() {
    #qemu-system-x86_64  -s -S \
    #-kernel obj/linux-x86-basic/arch/x86_64/boot/bzImage \
    #-initrd obj/initramfs-busybox-x86.cpio.gz \
    #-serial stdio \
    #-append "console=ttyS0" 
    #-append "root=/dev/ram rdinit=/sbin/init console=ttyS0"

    #qemu-system-x86_64 -s -S \
    #-kernel obj/linux-x86-basic/arch/x86_64/boot/bzImage \
    #-initrd obj/initramfs-busybox-x86.cpio.gz \
    #-append "root=/dev/ram rdinit=/sbin/init console=ttyS0" \
    #-serial stdio 
    
    qemu-system-x86_64 -s -S \
    -kernel obj/linux-x86-basic/arch/x86_64/boot/bzImage \
    -initrd obj/initramfs-busybox-x86.cpio.gz \
    -serial stdio \
    -append "console=ttyS0"
}

initramfs() {
    TOP=`pwd`
    cd $TOP/initramfs/x86-busybox

    #find ./ | cpio -o -H newc | gzip > $TOP/obj/initramfs-busybox-x86.cpio.gz

    find . -print0 \
    | cpio --null -ov --format=newc \
    | gzip -9 > $TOP/obj/initramfs-busybox-x86.cpio.gz
}

while true; do
    case "$1" in
        -c|--clean_build) clean_build; exit 0;;
        -h|--help) usage; exit 0;;
        -r|--run) qemu; exit 0;;
        -i|--initramfs) initramfs; exit 0;;
        -d|--debug) gdb; exit 0;;
        --) shift; break;;
    esac
    shift
done
