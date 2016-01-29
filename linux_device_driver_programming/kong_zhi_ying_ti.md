# 控制硬體


驅動程式最主要的任務 - 控制硬體。 



###7-1、控制硬體 
驅動程式的工作，就是幫助 kernel 控制硬體，kernel 將不認識的硬體交給驅動程式控制，而驅動程式再與 kernel 溝通後，即可讓 kernel 認識硬體，如此， user process 即可透過系統呼叫介面來操作硬體。 

各種硬體都有獨特的控制方式，但大多數硬體都可透過驅動程式讀寫「暫存器」來操控。 
暫存器是硬體(裝置)內部的記憶空間，但屬於斷電後內容就會消失的揮發性記憶體。 

驅動程式這類軟體去讀寫裝置的暫存器，也就是 CPU 去讀寫裝置，因為軟體是在 CPU 上面運行的。 
具體的讀寫方式隨著裝置而有不同，一般來說可歸為「I/O mapped I/O」與「Memory mapped I/O」這兩類。 



###7-2、I/O Mapped I/O 
「I/O mapped I/O」指的是經由通訊埠讀寫，只是為了與「Memory mapped I/O」對應，才如此描述。 
很久以前的裝置，多半都各自擁有 I/O port(通訊埠)，這是用來將資料寫給硬體、或是從硬體讀出資料的介面，代表只有硬體擁有 I/O port，就可以由驅動程式透過 I/O port 進行控制。 

為了通作 I/O port，系統準備了「I/O space」這個特別的空間，它與 memory space(記憶體空間) 是各自獨立的，I/O space 裡面的位址稱為「I/O port space」或直稱「port」。 
各個裝置都擁有自己的 I/O port address 範圍，硬體設計者必須確保系統內的硬體不會重複用到同一段位址。 
Linux 的 I/O port address 分配狀況可透過`「cat /proc/ioports」`指令來查閱。 

I/O port address 的讀寫方式有下面幾種：
以 1 byte、2 bytes 或 4 bytes 為單位執行讀寫

因此大量資料不適合經由 I/O port 傳送，另外硬體如果要提供 I/O port 的話，會佔用寶貴的電路板空間，所以最近的硬體多半完全不透過 I/O port 來通訊。 

為了讓驅動程式之類的軟體可以操作 I/O port，CPU 提供了 I/O port 專用的 mnemonic(方便記憶的組合語言指令寫法)，而 linux kernel 為了讓驅動程式不必動用組合語言指令，所以提供了 C 語言可以取用的函式，這些函式定義在「asm/io.h」檔案內。 
函式名稱	功能

```c
inb()	從 port 讀出 1 byte
inw()	從 port 讀出 2 byte
inl()	從 port 讀出 4 byte
outb()	從 port 寫入 1 byte
outw()	從 port 寫入 2 byte
outl()	從 port 寫入 4 byte
```


在讀寫 I/O port 的時候需要注意的是，近來 CPU 的性能愈來愈高，因此裝置有可能跟不上 CPU 的處理速度，因此 Linux 準備了會等待的函式，末尾的 「p」 就代表「pause」的意思：
```c
u8 inb_p(unsigned long port); 
u16 inw_p(unsigned long port); 
u32 inl_p(unsigned long port); 
void outb_p(u8 b, unsigned long port); 
void outw_p(u16 w, unsigned long port); 
void outl_p(u32 l, unsigned long port); 
```

如果 kernel 與好幾個驅動程式同時使用同一段 I/O port 的話，硬體運作可能會出問題，因此，要讀寫 I/O port 的 module 要先用 request_region() 把一塊位址空間保留下來，若保留成功，就會在 /proc/ioports 顯示驅動程式的名稱： 

```c
struct resource *request_region(resource_size_t start, resource_size_t n, const char *name);
```
I/O port 的開始位址透過 start 引數傳入，想讀寫的 port 個數則透過 n 引數傳入，name 是在 /proc/ioports 顯示的名字，不過 name 引數的指標會被 kernel 留著使用，所以不能傳入區域變數的指標。 

另外，保留 memory mapped I/O (MMIO) 位址空間的函式叫做 request_mem_region()。 

在不需要用到 I/O port 之後，一定要呼叫 release_region() 釋放先前保留的空間，否則在卸載驅動程式後，直到重開機之後，都無法再使用這塊 I/O port： 
```c
void release_region(resource_size_t start, resource_size_t n); 
```


### 7-3、Memory Mapped I/O 
以記憶體讀寫動作代替 I/O port 在硬體層級操縱裝置的手法就稱為「Memory Mapped I/O」(記憶體映射讀寫)，經常縮寫為「MMIO」。 

驅動程式只要向預先定義的位址範圍 (MMIO範圍) 進行讀寫動作，就會變成對特定硬體讀寫暫存器的動作。 
也因為只要以 C 語言的指標就能簡單辦到，所以成為近代的標準作法。 

一般常見的 IA-32 個人電腦架構中，是把緊鄰 4GB 以下的 1GB 範圍當成 MMIO 範圍。 

讀寫 MMIO 範圍時，只要用指標就可以了，但是不能直接把記憶體位址的數值當成指標來用，否則可能會造成「kernel panic」。 
因為驅動程式是在虛擬記憶體空間運作的緣故，這跟 user space 的應用程式讀寫不正確的記憶體位址而導致「segmentation fault」是一樣的。 

驅動程式想讀寫 MMIO 範圍時，必須透過 ioremap() 把物理位址映射到 kernel 的虛擬記憶體空間才行，定義在 「asm/io.h」： 

```c
void __iomem * ioremap(unsigned long offset, unsigned long size); 
```

offset 引數是物理記憶體位址，size 引數是以 byte 單位指定的映射範圍大小，Linux kernel 也把物理記憶體位址稱為「bus address」。 
函式執行成功的話會傳回 kernel 的虛擬記憶體位址，否則傳回 NULL。 

透過 ioremap() 映射的動作，在某些架構上可能會導致問題，這是因為被 cache 影響的緣故。 
驅動程式在讀寫位於 MMIO 範圍內的暫存器時，有時會從 cache 讀回先前的資料，而不會真得去讀硬體，寫入資料時也會發生類似的情形，因此擾亂 memory mapped I/O 的工作。 
這時就要把用到 ioremap() 的地方改用 ioremap_nocache()： 

```c
void __iomem * ioremap_nocache(unsigned long phys_addr, unsigned long size); 
```
在不需要 memory mapped I/O 時，驅動程式必須呼叫 iounmap() 明確取消映射： 

```c
void iounmap(volatile void __iomem *addr); 
```

```c
/*
 * main.c
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>

MODULE_LICENSE("Dual BSD/GPL");

static int sample_init(void)
{
    char *reg;

    printk("sample driver installed.\n");

    reg = ioremap_nocache(0xFEC00000, 4);
    if (reg != NULL) {
        printk("%x\n", *reg);
        iounmap(reg);
    }

    return 0;
}

static void sample_exit(void)
{
    printk("sample driver removed.\n");
}

module_init(sample_init);
module_exit(sample_exit);

```

讀寫 MMIO 範圍是可以直接 ioremap()，但為了避免衝突，還是建議事先利用 request_mem_region() 保留位址範圍： 
```c
struct resource *request_mem_region(resource_size_t start, resource_size_t n, const char *name);
```

start 引數是 MMIO 範圍開頭的物理記憶體位址，n引數則是 byte 單位的範圍大小，name 是引數裝置名稱，函式呼叫成功後，會回傳 resource 結構的指標，否則傳回 NULL。 
保留成功後，就能在 /proc/iomem 看到裝置名稱。 
在不需要讀寫 MMIO 範圍後，呼叫 iounmap() 解除映射後，還需要呼叫 release_mem_region 取消保留： 

```c
void release_mem_region(resource_size_t start, resource_size_t n); 
```

start 引數跟 request_mem_region() 同樣是 MMIO 範圍開頭的物理記憶體位址，n引數則是 byte 單位的大小。 

### 7-4、PCI 裝置的泛用介面 
讀寫 PCI 裝置的暫存器時，可以透過 I/O port 也可以透過 MIMO，每個裝置都可能使用不同的讀寫方式，控制這些裝置的驅動程式必須根據裝置來切換讀寫方式。 
如此會讓驅動程式的工作變得更複雜，所以 Linux 提供了不必區分 I/O port 與 MMIO 的抽象介面 pci_iomap()，定義在「asm/iomap.h」：

```c
void __iomem *pci_iomap(struct pci_dev *dev, int bar, unsigned long maxlen); 
```

呼叫後會自動切換 I/O port 或 MMIO 讀寫方式，並傳回指定 BAR(Base Address Register) 的 kernel 虛擬位址。 
需要透田 I/O port 讀寫時，內部會用 ioport_map() 進行映射，使它能跟 MMIO 透過同樣的方式讀寫。 

不需要讀寫後，要呼叫 pci_iounmap() 解除映射： 

```c
void pci_iounmap(struct pci_dev *dev, void __iomem *addr); 
```

而透過 pci_iomap() 映射的位址讀寫暫存器時，必須使用專用的函式，一樣是定義在「asm/io.h」： 

```c
unsigned int ioread8(void __iomem *addr); 
unsigned int ioread16(void __iomem *addr); 
unsigned int ioread32(void __iomem *addr); 
unsigned int iowrite8(u8 b, void __iomem *addr); 
unsigned int iowrite16(u16 b, void __iomem *addr); 
unsigned int iowrite32(u32 b, void __iomem *addr); 
```

這些函式會判斷對象是 I/O port 還是 MMIO，並配合做適當的處理。 

### 7-5、以 sparse 做靜態程式碼檢查 
前面介紹的函式 prototype 多半都有「__iomem」的標記，查看標頭檔定義： 

```c
#ifdef __CHECKER__ 
# define __iomem __attribute__((noderef, address_space(2))) 
#else 
# define __iomem 
#endif 
```

平常編譯驅動程式時，沒有定義 __CHECKER__，因此 __iomem 這個巨集的內容是空的，這個巨集要在透過 sparse 這個靜態程式碼檢查工具檢查程式碼時才會起作用。 
__iomem 巨集會透過 gcc 擴充功能指定下列屬性：
不能透過 *(解參考運算子)存取指標(noderef)
位於 I/O 位址空間 (I/O mapped I/O 或 memory mapped I/O; address_space(2))


address_space() 括弧內的數字意義：
數值	意義

<table>
<tbody><tr><td>數值</td><td>意義</td></tr>
<tr><td>0</td><td>Kernel space</td></tr>
<tr><td>1</td><td>User space</td></tr>
<tr><td>2</td><td>I/O space</td></tr>
</tbody></table>



sparse 安裝完成後，就可以建構驅動程式，以往都是在命令列執行「make」，現在要多指定「C=1」或「C=2」，「C=1」會檢查需要重新編譯的檔案，「C=2」則會檢查所有的檔案。 

在執行 「make C=2」編譯 7-3 的範例程式後，會出現些許 warning，需修改程式碼如下： 
char *reg; => char __iomem *reg; 
printk(KERN_ALERT "%x\n", *reg); => printk(KERN_ALERT "%x\n", ioread8(reg)); 
