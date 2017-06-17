# Linux kernel 調試打log流和斷點流


debug分為兩大流派。打log流和斷點流。

##printk

對於小程序來說，打log太爽了，我可以花式打log，每行插一行log，一次加一行log。
在頭文件` <linux/kernel.h>` 中定義了 8 種可用的日誌級別字符串：KERN_EMERG，KERN_ALERT，KERN_CRIT，KERN_ERR，KERN_WARNING，KERN_NOTICE，KERN_INFO，KERN_INFO。共有 8 種優先級，用戶可以根據需要進行配置

```sh
KERN_EMERG    /* system is unusable                   */
KERN_ALERT    /* action must be taken immediately     */
KERN_CRIT     /* critical conditions                  */
KERN_ERR      /* error conditions                     */
KERN_WARNING  /* warning conditions                   */
KERN_NOTICE   /* normal but significant condition     */
KERN_INFO     /* informational                        */
KERN_DEBUG    /* debug-level messages                 */
```

打log：

```c
printk(KERN_DEBUG "%d", HZ);
```

為了防止log太多撐爆ring buffer，啟動時使用參數 `log_buf_len=104857600` 來指定buffer大小，這裡為100M。
注意啟動參數不要帶 quite 和 splash。


##QEMU + GDB

打log固然爽，然而對於kernel這種"程序"，每次rebuild可以等半天。每次調試都要不斷加printk和rebuild顯然不實際，於是考慮使用GDB進行動態調試。由於是調試kernel，在不考慮kGDB等方法的情況下，最簡單的是跑一個虛擬機，然後GDB遠程連上去。QEMU為此提供了較好的支持。

##編譯kernel

為了能夠動態調試kernel，在編譯前需要進行相應的配置，流程如下：


```sh
make mrproper
make x86_64_defconfig

cat <<EOF >.config-fragment
CONFIG_DEBUG_INFO=y
CONFIG_GDB_SCRIPTS=y
EOF

./scripts/kconfig/merge_config.sh .config .config-fragment

make -j $(nproc)
```

這裡首先清理掉編譯生存的文件，重新產生一個配置文件(.config)，並和配置了DEBUG的.config-fragment進行合併，最後有多少個核就開多少路進行編譯。
注意.config很重要，先前我直接拷貝了/boot/config-xxx(當前kernel的配置)到.config，最後編譯後發現無論是break(通過修改內存產生debug異常來斷點)還是hbreak(通過debug寄存器來設置斷點)都無法斷點。只能通過 make x86_64_defconfig 生成一份新的配置，才能成功GDB。不知道因為哪個選項導致在gdb中無法斷點，苦查無果......


##2017.4.11更新

今晚洗澡的時候回想起這個問題，打算花點時間解決之。手頭有兩份文件，一份是原kernel的配置文件 .config_old ，無法斷點；另一份是通過 make x86_64_defconfig 新生成的配置文件 .config_new ，可以斷點，但缺乏一些配置導致編譯的kernel無法帶起物理機。於是想通過對比的方式找出問題所在。


結果一看， .config_old 有8000行， .config_new 有4000行，用diff一比發現一大堆不同，如果研究每一個diff那今晚不用睡了。於是決定暴力地使用二分查找法：每次用 .config_old 替換掉 .config_new 一半的配置條目，看編譯後能否成功斷點。

經過若干次查找後發現問題出在 Performance monitoring 裡面，懷疑是以下幾行出了問題：


```sh
CONFIG_RELOCATABLE=y
CONFIG_RANDOMIZE_BASE=y
CONFIG_X86_NEED_RELOCS=y
CONFIG_PHYSICAL_ALIGN=0x1000000
CONFIG_RANDOMIZE_MEMORY=y
CONFIG_RANDOMIZE_MEMORY_PHYSICAL_PADDING=0xa
```

是否是因為ASLR的原因可能會導致gdb無法斷到正確的位置？Google一下發現這篇文章： https://www.phoronix.com/scan.php?page=news_item&px=Linux-4.8-ASLR-Kernel-Mem-Sects ，說是4.8引進了 CONFIG_RANDOMIZE_MEMORY 的新特性：

randomizing the virtual address space of kernel memory sections, the goal is to mitigate predictable memory locations.


於是利用 make menuconfig把 Processor type and features -> Randomize the kernel memory sections 關了，重新編譯後發現依然無法斷點。乾脆把其父級選項 Randomize the address of the kernel image (KASLR) 也關了，這時終於好了。此時查看 .config 發現少了以下兩行配置：


```sh
CONFIG_X86_NEED_RELOCS=y
CONFIG_RANDOMIZE_MEMORY=y
```


個人猜測是內存的重新佈局導致無法成功斷點。雖然開48核編譯kernel每次只需幾分鐘，但前後調試還是花費了兩個小時，蛋疼。

##調試kernel

通過QEMU跑一個VM來加載kernel，同時啟動gdbserver提供調試信息。在宿主機中通過連接該server來進行調試。命令為：

```sh
sudo qemu-system-x86_64 -m 2048 \
                        -kernel /home/binss/work/GDB-Kernel/arch/x86/boot/bzImage \
                        -initrd ~/work/initrd.img-4.4.0-66-generic -gdb tcp::8889 \
                        -nographic -serial mon:stdio -append 'console=ttyS0' -S \
                        --enable-kvm
```

其中kernel用來指定kernel的鏡像文件，initrd用來指定initramfs，gdb用於啟動gdbserver並指定監聽的端口(也可以用-s來監聽1234端口)， -nographic -serial mon:stdio -append 'console=ttyS0' 用來指將輸出重定向到當前終端，便於觀察kernel運行時的輸出。S表示在開始的時候停止直到通過gdb輸入c才繼續運行。

在運行過程中隨時可以通過Ctrl-A + C 來切換到qemu monitor進行操作(重複操作退出qemu monitor)，如輸入quit可以結束當前VM。

啟動後，在另一個shell中cd到編譯kernel的目錄下，啟動gdb，依次執行以下命令：

```sh
add-auto-load-safe-path /home/binss/work/GDB-Kernel/
file /home/binss/work/GDB-Kernel/vmlinux
directory /home/binss/work/GDB-Kernel
target remote:8889
```

這裡從當前目錄的vmlinux(帶有符號信息的kernel，巨達幾百M)中加載符號表。也可以把以上命令保存到當前目錄( /home/binss/work/GDB-Kernel/ )的.gdbinit中，然後在 ~/.gdbinit 中添加：

```sh
add-auto-load-safe-path /home/binss/work/GDB-Kernel/.gdbinit
```

這樣在gdb啟動時就會自動執行以上命令。

然後我們就能夠通過函數名進行斷點了，比如斷在入口：

```sh
hbreak start_kernel
c
```

注意對於QEMU模擬的VM，可以使用break，但對於KVM模擬的VM，需要使用hbreak。
##掛載磁盤

前面的指令拉起的VM會掛在initramfs，因為沒有指定要掛載的磁盤，可以通過hda掛載已有磁盤並配置root參數，從而成功進入某個虛擬機：


```sh
sudo qemu-system-x86_64 -m 2048 \
                        -kernel /home/binss/work/KVM-Learning/arch/x86/boot/bzImage \
                        -initrd ~/work/initrd.img-4.4.0-66-generic -hda myvm2.img \
                        -gdb tcp::8889 -nographic -serial mon:stdio \
                        -append 'root=/dev/sda1 console=ttyS0' \
                        --enable-kvm
```

當然為了加強魯棒性，建議使用UUID來指定root設備，UUID可以在進入系統後查詢/boot/grub/grub.cfg得到。


```sh
sudo qemu-system-x86_64 -m 2048 \
                        -kernel /home/binss/work/KVM-Learning/arch/x86/boot/bzImage \
                        -initrd ~/work/initrd.img-4.4.0-66-generic -hda myvm2.img \
                        -gdb tcp::8889 -nographic -serial mon:stdio \
                        -append 'root=UUID=02cf5ccd-f57f-4b25-b923-add3adb5d6c3 console=ttyS0' \
                        --enable-kvm
```

##調試模塊

對於內核模塊，我們同樣能夠通過虛擬機的方式對其進行GDB。首先需要確保模塊已被加載，對於自行編譯的模塊，可以通過scp等方式將文件發到guest中，通過insmod進行安裝。注意需要保證是在當前kerenl的目錄下編譯模塊，確保它們的版本相同。

然後需要定位模塊地址(可能沒有data和bss)：

```sh
sudo cat /sys/module/kvm/sections/.text
sudo cat /sys/module/kvm/sections/.data
sudo cat /sys/module/kvm/sections/.bss
```

結果：

```sh
[email protected]
```

```sh
:~$ sudo cat /sys/module/kvm/sections/.text 0xffffffffa00e4000[emailprotected]
:~$ sudo cat /sys/module/kvm/sections/.data 0xffffffffa0143000 [email protected]
:~$ sudo cat /sys/module/kvm/sections/.bss 0xffffffffa0152140
```

然後在host的GDB中用這些地址加載模塊：


```sh
(gdb) add-symbol-file ~/work/GDB-Kernel/arch/x86/kvm/kvm.ko 0xffffffffa00e4000 -s .data 0xffffffffa0143000 -s .bss 0xffffffffa0152140
add symbol table from file "/home/binss/work/GDB-Kernel/arch/x86/kvm/kvm.ko" at
    .text_addr = 0xffffffffa00e4000
    .data_addr = 0xffffffffa0143000
    .bss_addr = 0xffffffffa0152140
(y or n) y
Reading symbols from /home/binss/work/GDB-Kernel/arch/x86/kvm/kvm.ko...done.
```

用同樣的方式加載kvm-intel的符號信息：

```sh
sudo cat /sys/module/kvm_intel/sections/.text
sudo cat /sys/module/kvm_intel/sections/.data
sudo cat /sys/module/kvm_intel/sections/.bss
```

結果：
```sh
[email protected] 
```

```sh
:~$ sudo cat /sys/module/kvm_intel/sections/.text 0xffffffffa01a3000[emailprotected]
:~$ sudo cat /sys/module/kvm_intel/sections/.data 0xffffffffa01cb000 [email protected]
:~$ sudo cat /sys/module/kvm_intel/sections/.bss 0xffffffffa01cbec0
```

加載：


```sh
(gdb) add-symbol-file ~/work/GDB-Kernel/arch/x86/kvm/kvm-intel.ko 0xffffffffa01a3000 -s .data 0xffffffffa01cb000 -s .bss 0xffffffffa01cbec0
add symbol table from file "/home/binss/work/GDB-Kernel/arch/x86/kvm/kvm-intel.ko" at
    .text_addr = 0xffffffffa01a3000
    .data_addr = 0xffffffffa01cb000
    .bss_addr = 0xffffffffa01cbec0
(y or n) y
Reading symbols from /home/binss/work/GDB-Kernel/arch/x86/kvm/kvm-intel.ko...done.
```

然後打斷點：


```sh
(gdb) hb vcpu_enter_guest
Hardware assisted breakpoint 1 at 0xffffffffa0103d37: file ./arch/x86/include/asm/processor.h, line 482.

(gdb) hb vmx_vcpu_run
Hardware assisted breakpoint 2 at 0xffffffffa01b30e0: file arch/x86/kvm/vmx.c, line 8798.

(gdb) c
Continuing.
```

然後就可以進行調試了。在VM中運行：


```sh
qemu-img create -f qcow2 mytest.img 5G

sudo qemu-system-x86_64 -cpu host -hda mytest.img \
                        -boot c \
                        -nographic \
                        -serial mon:stdio \
                        -vnc :1 \
                        -smp 1 \
                        -m 2048 \
                        --enable-kvm
```

回到gdb：

```sh
Thread 2 hit Breakpoint 1, vcpu_run (vcpu=<optimized out>)
    at /home/binss/work/GDB-Kernel/arch/x86/kvm/x86.c:6788
6788                            r = vcpu_enter_guest(vcpu);
(gdb) p vcpu
$1 = <optimized out>
(gdb) c
Continuing.

Thread 2 hit Breakpoint 2, vmx_vcpu_run (vcpu=0xffff8800778a0000)
    at /home/binss/work/GDB-Kernel/arch/x86/kvm/vmx.c:8798
8798    {
(gdb) p vcpu
$5 = (struct kvm_vcpu *) 0xffff8800778a0000
(gdb) n
8799            struct vcpu_vmx *vmx = to_vmx(vcpu);
(gdb) n
8803            if (unlikely(!cpu_has_virtual_nmis() && vmx->soft_vnmi_blocked))
(gdb) p vmx
$6 = (struct vcpu_vmx *) 0xffff8800778a0000
```

缺陷在於編譯kernel時強制採用了 -O2 進行編譯，導致一些值被優化後顯示為` <optimized out>` ，可以考慮`反彙編`。


##參考
http://stackoverflow.com/questions/11408041/how-to-debug-the-linux-kernel-with-gdb-and-qemu
https://wiki.ubuntu.com/Kernel/KernelDebuggingTricks
http://www.elinux.org/Debugging_The_Linux_Kernel_Using_Gdb
https://www.phoronix.com/scan.php?page=news_item&px=Linux-4.8-ASLR-Kernel-Mem-Sects