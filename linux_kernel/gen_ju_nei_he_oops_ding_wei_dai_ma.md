# 根據內核Oops 定位代碼


內核開發時有時候出現Oops，例如一個野指針會導致內核崩潰，如運行時出現以下log：現在有三種方法可以找出具體出現野指針的地方

```sh
   5.438972] bells bells:  wm5102-aif1 <-> samsung-i2s.0 mapping ok  
[    5.443812] bells bells: Failed to add route OPCLK->Sub CLK_SYS  
[    5.450499] Unable to handle kernel NULL pointer dereference at virtual address 00000010  
[    5.457770] pgd = c0004000  
[    5.460504] [00000010] *pgd=00000000  
[    5.463959] Internal error: Oops: 5 [#1] PREEMPT SMP ARM  
[    5.469249] CPU: 3    Not tainted  (3.4.5-g092c4c5 #75)  
[    5.474457] <span style="color:#990000;">PC is at snd_soc_dai_set_sysclk+0x10/0x84</span>  
[    5.479481] LR is at bells_late_probe+0x60/0x198  
[    5.484133]<span style="color:#FFCC33;"> pc : [<c040faa0>]</span>    lr : [<c0424030>]    psr: 60000013  
[    5.484139] sp : d683bd58  ip : d683bd80  fp : d683bd7c  
[    5.495579] r10: 00000000  r9 : c08a41f8  r8 : 00000000  
[    5.500723] r7 : d62bf400  r6 : 00000000  r5 : d69ab800  r4 : 00000000  
[    5.507284] r3 : 00000000  r2 : 00000000  r1 : 00000002  r0 : 00000000  
[    5.513731] Flags: nZCv  IRQs on  FIQs on  Mode SVC_32  ISA ARM  Segment kernel  
[    5.521074] Control: 10c5387d  Table: 4000406a  DAC: 00000015  
[    5.526799]   
[    5.526802] PC: 0xc040fa20:  
```

- 直接通過addr2line命令獲取，例如：

```sh
$ arm-none-linux-gnueabi-addr2line -e vmlinux c040faa0
```

註：請確保CROSS_COMPILE跟你編譯用的是一樣的前綴，例如上面的arm-none-linux-gnueabi-，你編譯時也必須是這個，不然算出來的行號可能會偏差比較大。
addr2line 代碼如下

- 通過gdb定位

1.1 首先運行gdb,不過需要使用出錯內核的vmlinux 
執行 $ arm-linux-gnueabi-gdb vmlinux 
1.2 設置斷點，即上面log信息中的用黃色重點標記的pc地址 
執行 
```sh
(gdb) b*0xc040faa0
```

Breakpoint 1 at 0xc040faa0: file sound/soc/soc-core.c, line 1070. 
此時，我們知道了在 sound/soc/soc-core.c文件的1070行出錯，這下我們就鎖定了範圍，具體解決了； 
1.3 如果你不想再另打開一個窗口去看該函數，也可以直接在當前窗口查看該函數 

```sh
(gdb) set listsize 50（設置顯示50行的內容）
(gdb) list *0xc040faa0（查看顯示的內容）
```
- 根據查詢內核符號表和反彙編信息定位，它可以不依賴出錯內核的vmlinux

2.1 根據上面紅色標記的log信息，PC is at snd_soc_dai_set_sysclk+0x10/0x84
0x10：表示出錯的偏移位置，0x84表示snd_soc_dai_set_sysclk函數的大小 

2.2 現在就是找到snd_soc_dai_set_sysclk函數的位置，
```sh
$ arm-linux-gnueabi-nm vmlinux | grep snd_soc_dai_set_sysclk
c04116bc T snd_soc_dai_set_sysclk 
```
```sh
$ arm-linux-gnueabi-objdump -S vmlinux –start-address=0xc04116bc –stop-address=0xc04116bc > ~/temp/soc
```

2.3 接下來就去查看vim ～/temp/soc文件， 找到0xc04116bc+0x10的位置即可
