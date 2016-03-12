#! /bin/bash

BUSYBOX_SRC_URL="http://busybox.net/downloads/busybox-1.23.2.tar.bz2"
KERNEL_SRC_URL="https://www.kernel.org/pub/linux/kernel/v4.x/testing/linux-4.3-rc3.tar.xz"

BUSYBOX="busybox_src"
KERNEL="linux_src"

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

    mkdir "$KERNEL"
        
    wget http://adtrepo.yoctoproject.org/2.0.0/rootfs/qemux86-64/core-image-sato-qemux86-64.tar.gz
    wget -P /tmp/ "$KERNEL_SRC_URL" && tar xJf "/tmp/`basename $KERNEL_SRC_URL`" -C "$KERNEL" --strip-components=1

    dd if=/dev/zero of=./busybox.img bs=1M count=4096
    'yes' | mkfs.ext3 busybox.img
    mkdir disk
    sudo mount -o loop busybox.img disk
    sudo tar xvf core-image-sato-qemux86-64.tar.gz -C disk

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
    -hda ./busybox.img \
    -boot c \
    -append "root=/dev/sda" \
    -redir tcp:5555::22
}

gdb() {
    qemu-system-x86_64 -s -S \
    -localtime \
    -kernel obj/$LINUX_PALTFORM/arch/x86_64/boot/bzImage \
    -hda ./busybox.img \
    -boot c \
    -append "root=/dev/sda" \
    -redir tcp:5555::22
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
