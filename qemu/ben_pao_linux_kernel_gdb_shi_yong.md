# 奔跑 linux kernel GDB 使用



## qemu

```sh
https://github.com/qemu/qemu
```

```sh
libcap-dev libattr1-dev
```

```sh
./configure --prefix=/home/shihyu/.mybin/qemu
make -j8
make install
```


## ~/.bashrc

```sh
export PATH="/home/shihyu/.mybin/qemu/bin:$PATH"
```

## linaro  工具链 

須下載 `x86_64` 才支援ubuntu 18,04 的 64位元, ubuntu 18.04不支援32位元

```sh
gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu
```

## 编译ARM64内核：

```sh
git clone https://github.com/figozhang/runninglinuxkernel_4.0
```

```sh
$ export ARCH=arm64
$ export CROSS_COMPILE=aarch64-linux-gnu-
$ cd _install_arm64 && mkdir dev
$ sudo mknod console c 5 1  (注意，不要遗漏该步骤)
$ cd runninglinuxkernel_4.0 
$ make defconfig (在runninglinuxkernel_4.0目录下输入make命令)
$ make -j8
```



## RUN + GDB 

```
cd /home/shihyu/.mybin/linaro  && source build.env
./run.sh arm64 debug
```

```
cd /home/shihyu/.mybin/linaro  && source build.env
cgdb -d aarch64-linux-gnu-gdb vmlinux
target remote localhost:1234
b start_kernel
c
```