# 編譯內核+BusyBox定製一個Linux提供ssh和web服務


- 1、添加一塊空閒磁盤
- 2、下載編譯內核
- 3、並為空閒磁盤安裝grub

前提準備：

```sh
[root@soul ~]# fdisk /dev/sdb    分區
Command (m for help): p
Disk /dev/sdb: 10.7 GB, 10737418240 bytes
255 heads, 63 sectors/track, 1305 cylinders
Units = cylinders of 16065 * 512 = 8225280 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disk identifier: 0x4e977ad9
   Device Boot      Start         End      Blocks   Id  System
/dev/sdb1               1           7       56196   83  Linux
/dev/sdb2               8          73      530145   83  Linux
/dev/sdb3              74         107      273105   82  Linux swap / Solaris
#格式化操作
[root@soul ~]# mke2fs -t ext4 /dev/sdb1
[root@soul ~]# mke2fs -t ext4 /dev/sdb2
[root@soul ~]# mkswap /dev/sdb3
[root@soul ~]# mkdir -pv /mnt/{boot,sysroot}
[root@soul ~]# mount /dev/sdb1 /mnt/boot/
[root@soul ~]# mount /dev/sdb2 /mnt/sysroot/
[root@soul ~]# grub-install --root-directory=/mnt /dev/sdb    安裝grub
[root@soul ~]# ls /mnt/boot/
grub  lost+found
[root@soul ~]# 上述信息顯示安裝成功
```

其中需要一個腳本來移植命令和所依賴的庫文件：

```sh
#腳本可能不完善；不過可用
#!/bin/bash
aimDir=/mnt/sysroot
cmdInput() {
    if which $cmd &> /dev/null;then
        cmdPath=`which --skip-alias $cmd`
    else
        echo "No such command."
        return 5
    fi
}
cpCmd() {
    cmdDir=`dirname $cmdPath`
    [ -d ${aimDir}${cmdDir} ] || mkdir -p ${aimDir}${cmdDir}
    [ -f $cmdPath ] && cp $cmdPath ${aimDir}${cmdDir}
}
cpLib() {
    for libPath in `ldd $cmdPath | grep -o "/[^[:space:]]\{1,\}"`;do
        libDir=`dirname $libPath`
        [ -d ${aimDir}${libDir} ] || mkdir -p ${aimDir}${libDir}
        [ -f $libPath ] && cp $libPath ${aimDir}${libDir}
    done
}
echo "You can input [q|Q] quit."
while true;do
  read -p "Enter a command: " cmd
  if [[ "$cmd" =~ \(|q|Q|\) ]];then
    echo "You choose quit."
    exit 0
  fi
    cmdInput
    [ $? -eq 5 ] && continue
    cpCmd
    cpLib
    [ $? -eq 0 ] && echo -e "\033[36mCopy successful.\033[0m"
done
```

## 一、編譯內核

下載地址：https://www.kernel.org/
```sh
[root@soul ~]# ls
anaconda-ks.cfg  install.log  install.log.syslog  linux-3.13.8.tar.xz
[root@soul ~]# 這裡下載的是目前最新的穩定版
[root@soul ~]# tar xf linux-3.13.8.tar.xz -C /usr/src/
[root@soul ~]# ln -sv /usr/src/linux-3.13.8/ /usr/src/linux    創建鏈接
`/usr/src/linux' -> `/usr/src/linux-3.13.8/'
[root@soul ~]# cd /usr/src/linux
[root@soul linux]#
#編譯
[root@soul linux]# make allnoconfig    清除所有選擇；然後重新選擇定製
  HOSTCC  scripts/basic/fixdep
  HOSTCC  scripts/kconfig/conf.o
  SHIPPED scripts/kconfig/zconf.tab.c
  SHIPPED scripts/kconfig/zconf.lex.c
  SHIPPED scripts/kconfig/zconf.hash.c
  HOSTCC  scripts/kconfig/zconf.tab.o
  HOSTLD  scripts/kconfig/conf
scripts/kconfig/conf --allnoconfig Kconfig
#
# configuration written to .config
#
[root@soul linux]# make menuconfig
#下面的選擇沒辦法列出來；給個大概
1、選擇CPU類型
2、支持動態模塊裝載
3、PCI總線支持
4、硬盤驅動
5、文件系統
6、可執行文件格式
7、I/O驅動；USB驅動
8、devtmpfs支持
9、選擇網絡支持以及網卡驅動
#結束後備份下配置文件；然後編譯成bzImage格式
[root@soul linux]# cp .config /root/config-3.13.8-x86_64
[root@soul linux]# make bzImage
[root@soul linux]# cp arch/x86/boot/bzImage /mnt/boot/
```

## 二、安裝BusyBox

BusyBox 是一個遵循GPL協議、以自由軟件形式發佈的應用程序。Busybox在單一的可執行文件中提供了精簡的Unix工具集(例如shell、init、getty、login...)，可運行於多款POSIX環境的操作系統，例如Linux（包括Android）、Hurd、FreeBSD等等。

1、安裝；官方下載地址：http://www.busybox.net/

```sh
#因為稍後需要編譯busybox為靜態二進製程序；所以需要實現安裝glibc-static和libmcrypt-devel；
#glibc-static在安裝光盤的第二張光盤上；可以掛在安裝；也可以到網上下載
[root@soul busybox-1.22.1]# mount /dev/cdrom /media/
mount: block device /dev/sr0 is write-protected, mounting read-only
[root@soul busybox-1.22.1]# yum -y install /media/Packages/glibc-static-2.12-1.132.el6.x86_64.rpm
[root@soul ~]# ls
anaconda-ks.cfg         config-3.13.8-x86_64  install.log.syslog
busybox-1.22.1.tar.bz2  install.log           linux-3.13.8.tar.xz
[root@soul ~]#
[root@soul ~]# tar xf busybox-1.22.1.tar.bz2
[root@soul ~]# cd busybox-1.22.1    #安裝可以查看INSTALL文件說明
[root@soul busybox-1.22.1]# make menuconfig
Busybox Settings  --->
    Build Options  --->
        [*] Build BusyBox as a static binary (no shared libs) #選中這項
#其他選項都不需要改動了
                                                                                                      
[root@soul busybox-1.22.1]# make
perl: warning: Setting locale failed.
perl: warning: Please check that your locale settings:
    LANGUAGE = (unset),
    LC_ALL = (unset),
    LANG = "en"
#根據提示設置下
[root@soul busybox-1.22.1]# export LANGUAGE=en_US.UTF-8
[root@soul busybox-1.22.1]# export LANG=en_US.UTF-8
[root@soul busybox-1.22.1]# export LC_ALL=en_US.UTF-8
[root@soul busybox-1.22.1]# make    再次make通過
[root@soul busybox-1.22.1]# make install
--------------------------------------------------
You will probably need to make your busybox binary
setuid root to ensure all configured applets will
work properly.
--------------------------------------------------
[root@soul busybox-1.22.1]#
[root@soul busybox-1.22.1]# ls _install/
bin  linuxrc  sbin  usr
[root@soul busybox-1.22.1]# cp -a _install/* /mnt/sysroot/
[root@soul busybox-1.22.1]# cd /mnt/sysroot/
[root@soul sysroot]# ls
bin  linuxrc  lost+found  sbin  usr
[root@soul sysroot]# mkdir -pv etc/rc.d var/log root proc sys srv boot mnt tmp home dev lib lib64
```

2、提供一個grub.conf文件
```sh
[root@soul sysroot]# vim /mnt/boot/grub/grub.conf
default=0
timeout=5
title Mini Linux (3.13.8-soul)
        root (hd0,0)
        kernel /bzImage ro root=/dev/sda2 init=/sbin/init
#完成後sync同步下；掛起宿主機；然後創建一個新的虛擬機使用之前添加的磁盤
#即可測試啟動
```


