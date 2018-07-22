# QEMU + GDB + LK4 on Docker


## 安裝Docker
```sh
sudo apt install docker.io
sudo systemctl start docker
sudo systemctl enable docker
docker --version
sudo usermod -aG docker shihyu
```

### Docker 修改默認存儲位置

```sh
sudo /etc/init.d/docker stop

cd /var/lib
sudo mv docker /media/shihyu/ssd/docker
sudo ln -s  /media/shihyu/ssd/docker docker

sudo /etc/init.d/docker start
```

### 進同一個 Docker容器

```sh
docker ps 
docker exec -it CONTAINER ID /bin/bash
```

### 快速刪除多個 docker 容器的方法 
總結了2種批量刪除容器的方法

```sh
docker stop $(docker ps -qa)
docker rm $(docker ps -qa)

docker ps -qa | xargs -n 1 docker stop
docker ps -qa | xargs -n 1 docker rm

docker ps -a
docker stop c8fddb25161f && docker rm c8fddb25161f
```

### 刪除docker image

```sh
docker image rm IMAGE ID
```

### docker commit


```sh
docker ps
docker commit -m "qemu + gdb + linux kernel 4.0" CONTAINER ID  yshihyu/qemu_gdb_lk4
```


### 登入 Docker

```sh
docker login
ID: yshihyu
PW: 2l
```

### Push

```sh
docker images
docker push yshihyu/qemu_gdb_lk4
```

### ssh 方式登入
```sh
docker run -itd -p 10023:22 yshihyu/qemu_gdb_lk4
docker ps 

docker exec -it CONTAINER ID /bin/bash
```

## 建構環境 從0開始

- 下载ubuntu镜像


```sh
docker pull ubuntu:18.04
```

- 启动镜像

```sh
docker run -ti ubuntu:18.04
```

- 安裝需要的套件

```sh

apt-get update
apt-get install qemu build-essential gcc g++ vim wget git automake libncurses5-dev flex bison texinfo subversion libreadline-dev bc libpython2.7-dev net-tools
```

```sh
cd ~/
wget https://releases.linaro.org/components/toolchain/binaries/latest-5/aarch64-linux-gnu/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu.tar.xz


mkdir ~/.mybin
vim ~/.bashrc
export PATH=/root/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/bin:$PATH
export PATH=$HOME/.mybin:$PATH
export PATH=$HOME/.mybin/gdb_python3/bin:$PATH
source ~/.bashrc
```

- 下載 runninglinuxkernel_4

```sh
git clone https://github.com/figozhang/runninglinuxkernel_4.0

export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
cd _install_arm64 && mkdir dev && cd dev
mknod console c 5 1 
cd runninglinuxkernel_4.0 
make defconfig
make -j8
```


### Anaconda3 & GDB & CGDB 安裝

- Anaconda3 安裝

```sh
wget https://repo.anaconda.com/archive/Anaconda3-5.2.0-Linux-x86_64.sh
sh Anaconda3-5.2.0-Linux-x86_64.sh
source ~/.bashrc
```

- GDB 安裝

```sh
git clone https://github.com/bminor/binutils-gdb
```

```sh

gdb 編譯新版修改  // 因為 gdb 會出現  'g' packet reply is too long:
修改gdb/remote.c文件，屏蔽process_g_packet函數中的下列兩行：
if (buf_len > 2 * rsa->sizeof_g_packet)
error (_(“Remote ‘g’ packet reply is too long: %s”), rs->buf);
在其後添加：

if (buf_len > 2 * rsa->sizeof_g_packet) {
    rsa->sizeof_g_packet = buf_len ;
    for (i = 0; i < gdbarch_num_regs (gdbarch); i++)
    {
        if (rsa->regs[i].pnum == -1)
            continue;

        if (rsa->regs[i].offset >= rsa->sizeof_g_packet)
            rsa->regs[i].in_g_packet = 0;
        else
            rsa->regs[i].in_g_packet = 1;
    }
}
```

```sh
export LDFLAGS="-Wl,-rpath,/root/anaconda3/lib -L/root/anaconda3/lib"

mkdir build && cd build

../configure --enable-targets=all \
             --enable-64-bit-bfd  \
             --with-python=python3.6 \
             --with-system-readline \
             --prefix=/root/.mybin/gdb_python3


make -j8 && make install

cd /root/.mybin/gdb_python3
svn co svn://gcc.gnu.org/svn/gcc/trunk/libstdc++-v3/python

git clone https://github.com/cgdb/cgdb
cd cgdb
./autogen.sh
./configure
make 
cp cgdb/cgdb ~/.mybin
```

- CGDB 安裝

`只能使用docker run 那個終端機開啟cgdb 不然會crash 原因不清楚` 

```sh
```
