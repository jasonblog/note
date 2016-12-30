# 查看正在運行的動態鏈接的程序中，某個動態庫中函數的虛擬地址


 目前，大部分可執行程序為了節省內存空間等目的，都是動態鏈接的， 動態鏈接的程序在每次運行時，所依賴的動態庫的地址總是變化的，那麼庫中的函數地址也跟著變化。如何在程序（動態鏈接的）運行期間，查看某個庫函數的虛擬地址？

方法很簡單，首先確定，你需要查看的函數，在哪個動態庫中，並且確定該函數相對於該動態庫的相對地址。其次，在程序運行期間，查看程序的映射表，找到動態庫的加載地址。最後將這兩個地址相加，就是你要的庫函數在運行時的虛擬地址。

我的系統是64位Ubuntu 12.04.1 LTS。

舉例如下：

##1、編寫測試程序hello.c：


```c
#include <stdio.h>
int main()
{
        printf("Hello world\n");
        sleep(-1);
        return 0;
}
```

加sleep的原因是，我們需要看到該程序運行時的狀態，執行完了就看不到其鏈接狀態了。printf就是動態庫libc-2.15.so中的一個函數（在你的系統上也有可能是其他庫的名字比如libc-2.13.so）。

##2、編譯運行

```sh
# gcc hello.c  (不加-static，默認就是動態鏈接)

# ./a.out & (後台運行，並且打印出程序pid)
[3] 1049   

```

##3、打印進程映射信息，得到進程運行時的動態庫基地址：



root@Heron:/home# cat /proc/1049/maps<br>
00400000-00401000 r-xp 00000000 08:01 2364347                         /home/zhangqianlong/Lib_test/a.out<br>
00600000-00601000 r--p 00000000 08:01 2364347                            /home/zhangqianlong/Lib_test/a.out<br>
00601000-00602000 rw-p 00001000 08:01 2364347                            /home/zhangqianlong/Lib_test/a.out<br>
`7fa5b9f44000-7fa5ba0f9000 r-xp 00000000 08:01 5243800                    /lib/x86_64-Linux-gnu/libc-2.15.so`<br>
7fa5ba0f9000-7fa5ba2f8000 ---p 001b5000 08:01 5243800                    /lib/x86_64-linux-gnu/libc-2.15.so<br>
7fa5ba2f8000-7fa5ba2fc000 r--p 001b4000 08:01 5243800                    /lib/x86_64-linux-gnu/libc-2.15.so<br>
7fa5ba2fc000-7fa5ba2fe000 rw-p 001b8000 08:01 5243800                    /lib/x86_64-linux-gnu/libc-2.15.so<br>
7fa5ba2fe000-7fa5ba303000 rw-p 00000000 00:00 0 
7fa5ba303000-7fa5ba325000 r-xp 00000000 08:01 5243815                    /lib/x86_64-linux-gnu/ld-2.15.so<br>
7fa5ba508000-7fa5ba50b000 rw-p 00000000 00:00 0 
7fa5ba522000-7fa5ba525000 rw-p 00000000 00:00 0 
7fa5ba525000-7fa5ba526000 r--p 00022000 08:01 5243815                    /lib/x86_64-linux-gnu/ld-2.15.so<br>
7fa5ba526000-7fa5ba528000 rw-p 00023000 08:01 5243815                    /lib/x86_64-linux-gnu/ld-2.15.so<br>
7ffff2260000-7ffff2281000 rw-p 00000000 00:00 0                          [stack]<br>
7ffff2383000-7ffff2384000 r-xp 00000000 00:00 0                          [vdso]<br>
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0                  [vsyscall]<br>



以上內容，紅色部分就是動態庫libc-2.15.so的運行時基地址

對於maps文件的解析：


```sh
一共有6列
第一列代表內存段的虛擬地址
第二列代表執行權限，r，w，x不必說，p=私有 s=共享
不用說，heap和stack段不應該有x，否則就容易被xx，不過這個跟具體的版本有關
第三列代表在進程地址裡的偏移量
第四列映射文件的主設備號和次設備號
通過 cat /proc/devices
得知fd是253 device-mapper

第五列映像文件的節點號，即inode

第六列是映像文件的路徑
```














##4、查看動態庫中，printf函數的相對地址：

```sh
# nm -D libc-2.15.so |grep -w printf  
```
`0000000000053840`T printf

nm意思不懂得自己去man以下，-w是抓取一個字，以上紅色部分就是printf函數對於動態庫libc-2.15.so的相對地址

##5、計算函數printf在動態運行時的虛擬地址：

`0000000000053840 +7fa5b9f44000=7FA5B9F97840`




`不同的系統，得出的結果有可能是不一樣的，同一個系統，測試兩次結果肯定是不一樣的，因為動態鏈接的庫，每次的加載地址都不一樣。`

