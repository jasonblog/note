# buildroot & dropbear


```sh
https://github.com/wayling/buildroot.git

git clone git://git.buildroot.net/buildroot

```


## make menuconfig
gdb/gdbserver/strace 應該是基本款,蠻好用的,可以追glibc實際呼叫的syscall
debug選項記得要開
```
[Toolchain]->[glibc (2.23)]
[Toolchain] -> [GCC compiler Version (gcc 4.8.x)]
[Build options]-> [strip command for binaries on target (none)]
[Build options]->[build packages with debugging symbols]
[Target packages] -> [Debugging, profiling and benchmark]->[gdb]
[Target packages] -> [Debugging, profiling and benchmark]->[gdbserver]
[Target packages] -> [Debugging, profiling and benchmark]->[full debugger]
[Target packages] -> [Debugging, profiling and benchmark]->[strace]
[Target packages] -> [System tools] -> [htop]

Symbol: BR2_PACKAGE_DROPBEAR [=y]                                          │  
  │ Type  : boolean                                                            │  
  │ Prompt: dropbear                                                           │  
  │   Location:                                                                │  
  │     -> Target packages                                                     │  
  │ (1)   -> Networking applications   
  
  
```


## make linux-menuconfig

```sh
[Kernel hacking]->[Kernel debugging]
[Compile-time checks and compiler options]->[Compile the kernel with debug info]
```

- make all


## Run 

```sh
qemu-system-x86_64 --kernel output/images/bzImage \
                   --hda output/images/rootfs.ext2 \
                   --append "root=/dev/sda" \
                   -net nic,model=virtio \
                   -net user \
                   -redir tcp:5556:10.0.2.15:22
```



- qemu.sh

```sh
#! /bin/bash

qemu() {
	qemu-system-x86_64 --kernel output/images/bzImage \
					   --hda output/images/rootfs.ext2 \
					   --append "root=/dev/sda" \
					   -net nic,model=virtio \
					   -net user \
					   -redir tcp:5556:10.0.2.15:22
}

gdb() {
	qemu-system-x86_64 -s -S \
                       --kernel output/images/bzImage \
					   --hda output/images/rootfs.ext2 \
					   --append "root=/dev/sda" \
					   -net nic,model=virtio \
					   -net user \
					   -redir tcp:5556:10.0.2.15:22
}

while true; do
    case "$1" in
        -r|--run) qemu; exit 0;;
        -d|--debug) gdb; exit 0;;
        --) shift; break;;
    esac
    shift
done
```

```
./qemu.sh -r
```

## ssh login

```sh
qemu 裡面建立  
mkdir -p /home/yshihyu
adduser yshihyu , password xxxx
reboot // save
```

```sh
ssh -p 5556 yshihyu@localhost


passwd 更改 root 密碼就可以讓 root 登入
ssh -p 5556 root@localhost
```

- scp

```sh
scp atexit.exe shihyu@10.0.2.2:/home/shihyu  // guest qemu to host pc
scp -P 5556 ttt.c shihyu@localhost:/home/shihyu  // host pc to guest qemu

scp -P 5556 test root@localhost:/root
```
