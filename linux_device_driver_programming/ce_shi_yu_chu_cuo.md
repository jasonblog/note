# 測試與除錯


開發驅動程式的測試與除錯方法，與開發應用程式時有著很大的差異。 



##16-1、裝置驅動程式的測試與除錯 
應用程式遇到 segmentation fault 時，可以用 gdb 解析 core dump，就能還原當機當時的堆疊，而且，應用程式也很容易在磁碟上留下運作記錄，分析起來較為方便。

但驅動程式是跟 kernel 一起運作的，如果驅動程式碰了不對的記憶體位址，則可能跟 kernel 一起當掉，當然最近的 linux kernel 變得更強固，驅動程式用了 NULL 指標並不會讓整個 kernel 當掉，但這個驅動程式還是無法繼續運作。 

如果要留下驅動程式的運作記錄，在 kernel 之內也沒有能直接取用的檔案系統，也就是說驅動程式只能在記憶體上運作，不保證能確實把記錄內容存成檔案。 

linux kernel 本身因為 kernel panic 當掉時，只會在螢幕上顯示 oops 訊息，無法像商用作業系統可以把整塊記憶體內容存到磁碟。

因此驅動程式的開發者須看著稀少的資料，一邊閱讀原始碼，尋找導致當機的原因。 



##16-2、建構工程 
linux 2.6 是以 make 建構驅動程式，以下列的驅動程式為例： 

```c
/*
 * main.c
 *
 */
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static int sample_init(void)
{
    printk("sample driver installed\n");

    return 0;
}

static void sample_exit(void)
{
    printk("sample driver removed\n");
}

module_init(sample_init);
module_exit(sample_exit);

```
make 時只需要一般使用者身份，要清除建構出來的檔案時，只需要執行： 
```sh
make clean 
```
如果要得知詳細的建構過程，可以在 make 時加上「V=1」引數： 
```sh
make V=1 
```
或是設定 KBUILD_VERBOSE 環境變數，也能開「verbose」模式： 
```sh
export KBUILD_VERBOSE=1 
```
另外在編譯驅動程式時，gcc 一定會接到「-Os」打開最佳化，這是無法關掉的，因為關掉 gcc 最佳化的話，就不會展開 inline 函式，會導致 kernel 無法正常運作。 
所以 kernel 開發者須特別留意：
- gcc 的最佳化臭蟲

##16-3、編譯選項 
想看驅動程式原始碼時，可在 Makefile 的 CFAGS 指定「-S」，如此就會把「-S」傳給 gcc，使得編譯工作在中途停止，並把產生的組合語言碼存到 -o 指定的檔案：


```sh
CFLAGS += -E -P
CFILES = main.c 

obj-m += sample.o
sample-objs := $(CFILES:.c=.o)

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

要把許多原始碼編譯成同一個 kernel module 時，所有的 .c 檔都會以同樣的編譯選項編譯，如果要為某個特定的 .c 檔指定特別的 CFLAGS，可以在 makefile 寫「CFLAGS_$F.o」來支援： 

```sh

CFILES = main.c sub.c

CFLAGS_sub.o := -DDEBUG
obj-m += sample.o
sample-objs := $(CFILES:.c=.o)

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

```

如此，就會只有在編譯「sub.c」時，會傳「-DDEBUG」給 gcc。 

##16-4、前置處理器 
gcc 在編譯時，會從指定的目錄內尋找 include 標頭檔載入，預設的路徑可以這個指令列出： 

```sh
cpp -v /dev/null 
```

可以看出 #include<...> 的搜尋路徑包含這三個： 
```sh
/usr/local/include 
/usr/lib/gcc/i386-redhat-linux/4.1.2/include 
/usr/include 
```

加上 -nostdinc 選項的話，就會略過這三個目錄，而要明確指定 include 標頭檔搜尋目錄的話，可以透過 -I 選項達成。 

gcc 預先定義的巨集可透過這個指令全部顯示出來： 

```sh
cpp -dM /dev/null 
```

驅動程式所使用的 kernel 函式，有不少都是巨集，如果要查閱前置處理器怎麼展開 C 原始碼巨集的話，可以為 gcc 加上「-E -P」選項： 

```c

CFLAGS += -E -P
CFILES = main.c 

obj-m += sample.o
sample-objs := $(CFILES:.c=.o)

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

```

##16-5、printk 除錯 
printk() 可以在任何 context 中使用，因此也能用來追縱中斷處理程序。 
只是 printk() 的輸出內容只會存到 kernel buffer(128KB)，如果滿了，會不斷蓋掉舊資料。 

user spacce 的 klogd daemon 會定期讀取 /proc/kmsg ，並將內容傳給 syslogd，而 syslogd 再把這些訊息當成系統記錄存到 /var/log/messages。

所以 kernel buffer 有可能會溢出，導致 daemon 會來不及把這些訊息存到檔案裡。 

以 printk() 除錯時，可以用條件式來切換顯示與否，而要取得 C 語言的函式名的話，可以用`「__FUNCTION__」或「__func__」`： 


```sh
#ifdef DRIVER_DEBUG 
# define DPRINTK(fmt, args...) printk("%s: " fmt, __FUNCTION__, ## args) 
#else 
# define DPRINTK(fmt, args...) 
#endif 
```

有時希望硬體故障時，可以留下記錄，但若硬體持續故障的話，就會留下一大堆訊息，要抑制大量的輸出訊息，可以透過 printk_ratelimit()： 

```c
/*
 * main.c
 *
 */
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static int sample_init(void)
{
    int i;

    printk("sample driver installed\n");

    for (i = 0 ; i < 300 ; i++) {
        if (printk_ratelimit())
            printk("I2C access error\n");
    }

    return 0;
}

static void sample_exit(void)
{
    printk("sample driver removed\n");
}

module_init(sample_init);
module_exit(sample_exit);
```

執行這個範例的話，就只會在 kernel buffer 看到 10 個訊息。 

開始抑制之前的顯示次數可透過 /proc/sys/kernel/printk_ratelimit_burst 設定，而解除抑制的時間可透過 /proc/sys/kernel/printk_ratelimit 設定。 
這些位於 /proc/sys/kernel/* 的設定選項，可在 Documentation/sysctl/kernel.txt 找到詳細說明。 


##16-6、Magic SysRq 鍵 
驅動程式發生問題，而讓整個 kernel 行為怪異的話，可以使用「Magic SysRq 鍵」，Magic SysRq 是透過特定按鍵方式題示暫存器內容、 process 狀態的機制。 
啟動 Magic SysRq 的方法為： 
```sh
# echo 1 > /proc/sys/kernel/sysrq 
```

或是修改 /et/sysctl.conf ，將 「kernel.sysrq」設為「1」： 

```sh
kernel.sysrq = 1 
```

Magic SysRq 的呼叫方法：
主控臺種類	按鍵方式
VGA	Alt+SysRq(Print Screen) + 指令
Serial console	Break訊號 + 指令


Serial console 傳送 break 訊號的方式隨終端機軟體而有所不同， TeraTerm 是透過 Control 選單選擇 send break，minicom 則是按「Ctrl+A」鍵再按「F」。 

Magic SysRq 可用的輔助指令隨架構而異，說明文件位於 Documentation/sysrq.txt。 

## 16-7、oops 訊息

linux kernel 內部發生某些矛盾時，會在主控臺顯示 oops 訊息(panic 訊息)。 
linux kernel 在發現自身矛盾時，會時會判斷無法保證後續正常運作，這時就會明確呼叫 panic()： 

```sh
NORET_TYPE void panic(const char * fmt, ...); 
```

呼叫 panic() 會讓系統停止運作，如果希望系統能自動重開的話，可以在開機時加上「panic=」選項： 
```sg
# cat /proc/cmdline 
ro root=/dev/VolGroup00/LogVol00 rhgb quiet panic=10 
```

上述的例子，會在 panic() 之後10 秒，重新啟動。 

##16-8、分析 kernel panic 情形 
linux kernel 在 panic 後，要從當時的指令指標(EIP) 識別對應的指令時，objdump 可以派上用場。 
執行 objdump 即可將指令指標附近的指令反組譯，這時需要建構 kernel 時產生的「vmlinux」檔，不能用「vmlinuz」： 

```sh
# objdump -d --start-address=0xc00f9e9c --stop-address=0xc00f9eec ./vmlinux 
```

##16-9、結語 
linux 解析問題的機制還不夠充實，因此在開發驅動程式時，須不斷摸索解決問題的方式。 

