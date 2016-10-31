# buildroot & dropbear


```sh
https://github.com/wayling/buildroot.git
```

-  Dropbear

```sh
make menuconfig


Symbol: BR2_PACKAGE_DROPBEAR [=y]                                          │  
  │ Type  : boolean                                                            │  
  │ Prompt: dropbear                                                           │  
  │   Location:                                                                │  
  │     -> Target packages                                                     │  
  │ (1)   -> Networking applications   
  
  
```

- make linux-menuconfig


```sh
[Kernel hacking]->[Kernel debugging]
[Compile-time checks and compiler options]->[Compile the kernel with debug info]
```

- make all


- Run 

```sh
qemu-system-x86_64 --kernel output/images/bzImage \
                   --hda output/images/rootfs.ext2 \
                   --append "root=/dev/sda" \
                   -net nic,model=virtio \
                   -net user \
                   -redir tcp:5556:10.0.2.15:22
```

```
qemu 裡面建立  
mkdir -p /home/yshihyu
adduser yshihyu , password xxxx
reboot // save
```

- scp

```sh
scp atexit.exe shihyu@10.0.2.2:/home/shihyu  // guest qemu to host pc
scp -P 5556 ttt.c shihyu@localhost:/home/shihyu  // host pc to guest qemu 
```
