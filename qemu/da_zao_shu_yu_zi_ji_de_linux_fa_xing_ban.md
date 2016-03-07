# 打造屬於自己的linux發行版


linux的啟動過程，包括BIOS的加電自檢POST，拷貝MBR的信息（啟動BootLoader），加載內核，掛載根文件安系統這幾大步熟悉grub的話會知道linux啟動時grub中有三項：`root，kernel，initrd`。其三項的作用分別是：

```sh
1.指定內核所在的目錄
2.指定內核的名稱，以及掛載根目錄的方式，還有向內核傳遞一定的參數
3.initrd實際就是個小的linux系統，在一般的系統中initrd的作用是：啟動一個很小的linux用來掛載真實的linux。
```

今天的目的就是從內核開始，打造一個屬於自己的linux。



環境：Ubuntu13.04 gcc4.7.3<br>
相關的準備工作：
內核的編譯<br> 
qemu的安裝<br>

##製作根系統目錄

1）創建init程序 

首先創建一個init.c文件，代碼如下:

```c
#include<stdio.h>
int main()
{
    printf("Welcome! My student No. is sa*****310.\n");
    return 0;
}
```
靜態編譯成一個可執行文件。

```sh
gcc -static -o init init.c
```

2）建立引導根目錄映像

終端運行：
```sh
dd if=/dev/zero of=initrd.img bs=4096 count=1024
mkfs.ext3 initrd.img
```
有提示，輸入y。


###關於dd
dd 是 Linux/UNIX 下的一個非常有用的命令，作用是用指定大小的塊拷貝一個文件，並在拷貝的同時進行指定的轉換。<br>

```sh
語法：dd [選項] 
if =輸入文件（或設備名稱）。 
of =輸出文件（或設備名稱）。 
ibs = bytes 一次讀取bytes字節，即讀入緩衝區的字節數。 
...
```

3）創建rootfs目錄，並掛載

```sh
mkdir rootfs
sudo mount -o loop initrd.img rootfs
```

4）在rootfs中添加一些文件
將init拷貝到initrd4M.img的目標根目錄下（因為linux啟動後期會在根目錄中尋找一個應用程序來運行，在根目錄下提供init是一種可選方案）
```sh
cp init rootfs/
```

準備dev目錄：

```sh
sudo mkdir rootfs/dev
```

linux啟動過程中會啟用console設備:
sudo mknod rootfs/dev/console c 5 1
另外需要提供一個linux根設備，我們使用ram:

```sh
sudo mknod rootfs/dev/ram b 1 0
sudo umount rootfs
```
至此，一個包含簡單應用程序的根目錄initrd4M.img映像就準備好。

mknod 
用於製作字符或塊相關文件


用qemu跑一下：
```sh
qemu -kernel ../linux-3.9.2/arch/x86/boot/bzImage -initrd initrd.img -append "root=/dev/ram init=/init"
```

![](./images/20130616201444750)


## 整合busybox

busybox簡介
BusyBox 是一個集成了一百多個最常用linux命令和工具的軟件。BusyBox 包含了一些簡單的工具，例如ls、cat和echo等等，還包含了一些更大、更復雜的工具，例如grep、find、mount以及telnet。有些人將 BusyBox 稱為 Linux 工具裡的瑞士軍刀。簡單的說BusyBox就好像是個大工具箱，它集成壓縮了 Linux 的許多工具和命令，也包含了 Android 系統的自帶的shell。
BusyBox 將許多具有共性的小版本的UNIX工具結合到一個單一的可執行文件。這樣的集合可以替代大部分常用工具比如的GNU fileutils ， shellutils等工具，BusyBox提供了一個比較完善的環境，可以適用於任何小的或嵌入式系統。

下載源碼：http://www.busybox.net/
這裡選擇1.20穩定版。
解壓，終端進入目錄執行：
```sh
make menuconfig
```

勾選下面的選項：
```sh
Build Options
Build BusyBox as a static binary (no shared libs)
```

![](./images/20130615122706234)

這個選項是一定要選擇的,這樣才能把busybox編譯成靜態鏈接的可執行文件,運行時才獨立於其他函數庫.否則必需要其他庫文件才能運行,在單一個linux內核不能使他正常工作.

現在直接make的話會報錯：`‘RLIMIT_FSIZE’ undeclared`
論壇上的回答是沒有包含  sys/resource.h，則在include/libbb.h 中添加：

```c
#include <sys/resource.h>  
```

接下來執行：
```sh
#編譯busybox
make 
#安裝busybox
make install 
```
安裝好之後在文件夾下出現一個_install文件夾，編譯完成。

![](./images/20130615124957765)

下面來整合根文件系統。
新建一個文件夾，終端cd進去，將之前的initrd.img拷貝進來。
```sh
#創建文件夾
mkdir rootfs
#掛載鏡像
sudo mount -o loop initrd.img rootfs/ 
#將busybox添加進來
cd ../busybox-1.20.2/
sudo make CONFIG_PREFIX=../Opuntu/rootfs/ install
#查看rootfs中結構
cd ../Opuntu
ls rootfs
 #卸載分區
sudo umount rootfs/
```

`運行命令時注意目錄結構！`<br>
linux的系統下的目錄都是幹嘛的？<br>
linux下的文件結構，看看每個文件夾都是幹嗎用的

```sh
/bin 二進制可執行命令 
/dev 設備特殊文件 
/etc 系統管理和配置文件 
/etc/rc.d 啟動的配置文件和腳本 
/home 用戶主目錄的基點，比如用戶user的主目錄就是/home/user，可以用~user表示 
/lib 標準程序設計庫，又叫動態鏈接共享庫，作用類似windows裡的.dll文件 
/sbin 系統管理命令，這裡存放的是系統管理員使用的管理程序 
/tmp 公用的臨時文件存儲點 
/root 系統管理員的主目錄（呵呵，特權階級） 
/mnt 系統提供這個目錄是讓用戶臨時掛載其他的文件系統。 
...
```
##qemu測試
```sh
qemu -kernel ../linux-3.9.2/arch/x86/boot/bzImage -initrd initrd.img -append "root=/dev/ram init=/bin/sh"
```

裝載好之後可以在qemu中運行busybox的命令，效果如下：

![](./images/20130616205424343)

##整合grub
關於Grub
GNU GRUB（簡稱“GRUB”）是一個來自GNU項目的多操作系統啟動程序。GRUB是多啟動規範的實現，它允許用戶可以在計算機內同時擁有多個操作系統，並在計算機啟動時選擇希望運行的操作系統。GRUB可用於選擇操作系統分區上的不同內核，也可用於向這些內核傳遞啟動參數。
首先來測試一下grub.
從ftp://alpha.gnu.org/gnu/grub/下載GRUB Legacy的最後一個版本0.97的編譯好的文件grub-0.97-i386-pc.tar.gz.
解壓之後中端cd進去，執行下面的命令。

```sh
#建立軟盤映像：
dd if=/dev/zero of=boot.img bs=512 count=2880
#在boot.img中安裝grub：
sudo losetup /dev/loop0 boot.img
sudo dd if=./grub-0.97-i386-pc/boot/grub/stage1 of=/dev/loop0 bs=512 count=1
sudo dd if=./grub-0.97-i386-pc/boot/grub/stage2 of=/dev/loop0 bs=512 seek=1
sudo losetup -d /dev/loop0
```

在qemu中測試是否可以進入grub

```sh
qemu -fda boot.img
```
![](./images/20130616110258062)
OK.
接下來將grub，kernel，busybox一起整合，離勝利只還有一步 - - 。
先你自己發行版取一個庫一些的名字，比如Opuntu...(Opensuse+ubuntu),創建一個以它命名的文件夾。

```sh
#拷貝boot.img到當前目錄
sudo cp ../grub-0.97-i386-pc/boot.img ./
#創建rootfs文件夾
mkdir rootfs
#創建一個32M的磁盤鏡像文件
dd if=/dev/zero of=/dev/zero of=Opuntu.img bs=4096 count=8192
#給磁盤映像分區
fdisk -C 16065 -H 255 -S 63 Opuntu.img
```

解釋：設置Opuntu.img的磁頭數為255、磁道數為16065、扇區數為63，同時給磁盤分區。
這裡我們只分一個區，並設置該分區為引導分區。示意圖如下：

![](./images/20130616183140578)
```sh
#格式化分區
sudo losetup -o 1048576 /dev/loop0 Opuntu.img
sudo mkfs.ext3 -m 0 /dev/loop0
```
解釋：我們把前面的2048個扇區（0~2047）作為引導扇區使用，格式化分區從第2048個扇區開始，所以1048576=2048*512

```sh
#拷貝之前做好的initrd.img和bzImage.img到rootfs
sudo mount /dev/loop0 rootfs/
sudo cp ../linux-3.9.2/arch/x86/boot/bzImage ./rootfs/
sudo cp ../rootfs/initrd.img ./rootfs/
```

```sh
#添加grub
sudo mkdir rootfs/boot
sudo mkdir rootfs/boot/grub
sudo cp ../grub-0.97-i386-pc/boot/grub/* ./rootfs/boot/grub
sudo vi ./rootfs/boot/grub/menu.lst
```

內容：

```sh
default 0
timeout 30
title linux on 32M.img
root (hd0,0)
kernel (hd0,0)/bzImage root=/dev/ram init=/bin/ash
initrd (hd0,0)/initrd.img
```

```sh
#卸載磁盤鏡像
sudo umount rootfs
sudo losetup -d /dev/loop0
```

```sh
#利用grub啟動軟盤，在硬盤映像上添加grub功能
qemu -boot a -fda boot.img -hda Opuntu.img
```

執行圖中的兩步（注意空格）：

![](./images/20130616213616781)


運行成功之後，Opuntu.img就是我們的最終成果了，集成了busybox,grub,linux kernel3.92!

```sh
qemu -hda Opuntu.img
```

用qemu跑起來：

![](./images/20130616214002343)
![](./images/20130616214027078)

##參考：
鳥哥私房菜 第二十二章：開關機流程與loader
製作可用grub引導Linux系統的磁盤映像文件 - http://blog.sina.com.cn/s/blog_70dd169101013gcw.html

詳細講解Linux啟動流程及啟動用到的配置文件及腳本 - http://guodayong.blog.51cto.com/263451/1168731

## 更新內核
直接安裝的系統內核版本一般不是最新，用

```sh
uname -a  
```
命令可以查看內核的版本號，比如我的就是：

![](./images/20130530105838024)

下面來手動更新內核到最新的穩定版本。

1.獲取源碼

最新的穩定版本是3.9.4.
下載好之後解壓到 /usr/src 文件夾下

```sh
sudo tar -xvf linux-3.9.4.tar.xz -C /usr/src/  
```

2.配置內核
將原來的配置文件拷過來

```sh
cp /usr/src/linux-headers-2.6.32-27-generic/.config .config  
```

首先進行一下配置，進入到 /usr/src//linux-3.9.4 文件夾下，執行

```sh
make menuconfig  
```

報錯：
```sh
*** Unable to find the ncurses libraries or the
*** required header files.
```
缺少ncurses庫（一個管理應用程序在字符終端顯示的函數庫），怒裝之：

```sh
sudo apt-get install ncurses-dev  
```
重新執行make menuconfig<br>
選Load，然後Ok，然後Save。<br>



![](./images/20130530193054963)


3.編譯和安裝
終端執行：

```sh
make bzImage #編譯kernel  
make modules #編譯模塊  
make modules_install  #先安裝模塊
make install #安裝內核
```

編譯的時間由機器性能決定。

make install之後，grub已經自動更新，不用再手動設置引導。
重啟，進入ubuntu，更新後的第一次加載會有些慢。
再次查看內核版本，終端運行

```sh
uname -a  
```

![](./images/20130530193611812)


###添加系統調用

系統調用的原理如下：
![](./images/20130530200110780)


源碼目錄下涉及內核的三個文件有：

```sh
/kernel/sys.c                                         //定義系統調用
/arch/x86/syscalls/syscall_32.tbl      //設置系統調用號
/include/linux/syscalls.h                     //系統調用的頭文件
```

下面來實現一個簡單的系統調用。

1）系統調用函數的實現。
在/kernel/sys.c的最後添加下面的代碼：

```c
asmlinkage int sys_callquansilang(int num)
{
	printk("Hi,I'm Quansilang. My student No. is sa*****310!");
	return 1;
}
```

2)設置系統調用號

編輯arch/x86/syscalls/syscall_32.tbl
文件，發現已經由350個定義號的系統調用，照葫蘆畫葫蘆娃，在最後添加自己的系統調用：



```c
351 i386    callquansilang      sys_callquansilang  
```

注意要與之前定義的函數對應。

3）添加系統調用的聲明到頭文件

打開 /include/linux/syscalls.h ，倒數第二行添加

```c
asmlinkage int sys_callquansilang(int num); 
```
4）重新編譯內核並安裝。


```sh
sudo make mrproper  #清除久的編譯文件
sudo make             #編譯
```

安裝：

```sh
make modules_install  #先安裝模塊
make install #安裝內核
```
重啟系統。

5）測試

創建一個main.c

```c
#include <unistd.h>
#include <stdio.h>
int main()
{
    syscall(351, 1);
    return 1;

}
```

編譯運行，然後用

```c
sudo dmesg -c
```

查看系統調用log（寫系統log會有一點延遲）。


![](./images/20130530215124845)


##參考
向linux內核添加系統調用新老內核比較 - http://www.cnblogs.com/albert1017/archive/2013/05/27/3101760.html
ubuntu 12.10 x64 下編譯新內核 + 系統調用方法  -  http://blog.csdn.net/dslovemz/article/details/8744352
Linux添加內核系統調用報告 - http://blog.csdn.net/yming0221/article/details/6559767



