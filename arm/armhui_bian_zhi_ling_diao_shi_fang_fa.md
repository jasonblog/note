# ARM彙編指令調試方法


學習ARM彙編時，少不了對ARM彙編指令的調試。作為支持多語言的調試器，gdb自然是較好的選擇。調試器工作時，一般通過修改代碼段的內容構造trap軟中斷指令，實現程序的暫停和程序執行狀態的監控。為了在x86平臺上執行ARM指令，可以使用qemu模擬器執行ARM彙編指令。

##一、準備ARM彙編程序

首先，我們構造一段簡單的ARM彙編程序作為測試代碼main.s。

```c
.globl _start
_start:
    mov R0,#0
    swi 0x00900001
```

以上彙編指令完成了0號系統調用exit的調用。mov指令將系統調用號傳入寄存器R0，然後使用0x00900001軟中斷陷入系統調用。

為了運行ARM彙編代碼，需要使用交叉編譯器arm-linux-gcc對ARM彙編代碼進行編譯。下載交叉編譯器安裝完畢後，對ARM彙編代碼進行編譯。

```sh
arm-linux-gcc main.s -o main -nostdlib
```

編譯選項`-nostdlib`表示不使用任何運行時庫文件，編譯生成的可執行文件main只能在ARM體系結構的系統上運行。

##二、編譯安裝qemu模擬器
為了x86的Linux系統內運行ARM體系結構的可執行程序，需要安裝qemu模擬器。

首先下載qemu源碼，然後保證系統已經安裝了flex和bison。

編譯安裝qemu。

```sh
./configure --prefix=/usr
sudo make && make install
```

然後使用qemu的ARM模擬器執行ARM程序。

```sh
qemu ./main
```

##三、編譯安裝arm-gdb
為了調試ARM程序，需要使用gdb的源碼編譯生成arm-gdb。

首先下載gdb源代碼，編譯安裝。

```sh
./configure --target=arm-linux --prefix=/usr/local
sudo make && make install
```

為了和系統的gdb避免衝突，我們將gdb的安裝目錄安裝到/usr/local，然後建立軟鏈接即可。

```sh
ln -s /usr/local/gdb/gdb /usr/bin/arm-gdb
```

之後便可以使用arm-gdb命令調試ARM程序了。

##四、調試ARM程序
首先使用qemu打開遠程調試端口。

```sh
qemu-arm -g 1024 ./main
```

然後導出環境變量QEMU_GDB。

```sh
export QEMU_GDB=/usr/local/gdb
```

最後，進入gdb調試。

```sh
arm-gdb ./main
```

進入arm-gdb後，首先連接遠程調試端口。

```sh
(gdb) target remote localhost:1024
```

然後使用gdb常用的調試命令調試代碼即可。

```sh
(gdb) disassemble           // 查看反彙編
(gdb) x /8xw 0x0000808e     // 查看內存
(gdb) info register         // 查看寄存器
(gdb) continue              // 繼續執行
(gdb) stepi                 // 彙編級逐過程
(gdb) nexti                 // 彙編級逐語句
```

##參考資料
http://blog.sina.com.cn/s/blog_59fd92c40100h4v2.html
http://wenku.baidu.com/link?url=g8nb4PdsjaS4LLg6bDO3fo8LZjSVOFvglIi1b6OLgYbl_1Nav8_0QuDiWY4gTCByklxriyo-6s7s_JCErU6_RNcAP2FtB1FMpax1ATnUZn3