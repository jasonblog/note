# 動手寫 Linux Driver


先前為了一個期末專題花了一點時間研究怎麼在 Linux 作業系統上寫一個 PCI Driver。寫過 Linux 驅動程式之後，覺得 Linux 的架構真的很漂亮！為了怕以後忘記怎麼寫，所以就把他寫下來記錄成一篇文章。

建構編譯環境

首先我們必須要準備開發 Linux 驅動程式所需的環境，在 Debian 上可以用以下的指令達到這個目的：

```sh
$ sudo apt-get install build-essential linux-headers-$(uname -r)
```

其中 build-essential 會安裝 gcc, make 等軟體開發必要的工具，而 linux-headers 會安裝開發 Linux 驅動程式必要的 SDK。因為 linux-headers 會隨核心的版本而有所不同，所以我們要使用 $(uname -r) 取得目前核心的版本。

簡單的驅動程式

所有的 Linux 驅動程式至少要包含一個 MODULE_LICENSE 用以宣告驅動程式的授權，另外還需要一個 init 與一個 exit 函式，分別處理驅動程式的起始與終止。以下就是一個什麼都沒有的空殼：

```c
/* example.c */
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static int example_init(void) {
    printk("<1>EXAMPLE: init\n");
    return 0;
}

static void example_exit(void) {
    printk("<1>EXAMPLE: exit\n");
}

module_init(example_init);
module_exit(example_exit);
```

我們可以注意到裡面有一個 printk，他就相當於驅動程式設計當中的 printf。我們如果需要印任何除錯資訊，可以呼叫 printk，然後使用 sudo dmesg 觀看結果。編譯這個檔案之前，我們要先幫他寫 Makefile：
```sh
obj-m := example.o

ifeq ($(KERNELDIR),)
KERNELDIR=/lib/modules/$(shell uname -r)/build
endif

all:
    make -C $(KERNELDIR) M=$(PWD) modules

clean:
    make -C $(KERNELDIR) M=$(PWD) clean
```
在這個 Makefile 裡面，我們會使用 obj-m 這個變數指定我們要編譯的模組，然後再呼叫 make 讓他載入 SDK 的 Makefile。我們先前安裝的 SDK 就會放在 /lib/modules/$(shell uname -r)/build 裡面。

接下來我們就可以用 make 編譯我們的模組，並使用以下指令載入、卸除模組：

```sh
$ sudo insmod ./example.ko
$ sudo rmmod example
```
如果要看我們的模組有沒有輸出任何訊息，可以使用：

```sh
$ sudo dmesg | tail
```
註冊為 Character Device

在 Unix 的設計哲學當中，所有的東西都是檔案，硬體也不例外。我們寫驅動程式的時候要提供一個檔案操作的介面給 Userspace 的程式。為了達到這個目的，我們必須再引入一個標頭檔：

```c
#include <linux/fs.h>
```

然後定義若干檔案操作與 file_operations 這個資料結構：

```c
static int example_open(struct inode *inode, struct file *filp) {
    printk("<1>EXAMPLE: open\n");
    return 0;
}

static int example_close(struct inode *inode, struct file *filp) {
    printk("<1>EXAMPLE: close\n");
    return 0;
}

static ssize_t example_read(struct file *filp, char *buf, size_t size, loff_t *f_pos) {
    printk("<1>EXAMPLE: read  (size=%zu)\n", size);
    return 0;
}

static ssize_t example_write(struct file *filp, const char *buf, size_t size, loff_t *f_pos) {
    printk("<1>EXAMPLE: write  (size=%zu)\n", size);
    return size;
}

static struct file_operations example_fops = {
    .open = example_open,
    .release = example_close,
    .read = example_read,
    .write = example_write,
};
```

然後在 example_init() 當中用 register_chrdev 把這個驅動程式註冊為一個 Character Device。

```c
#define EXAMPLE_MAJOR 60
#define EXAMPLE_NAME "example"

static int example_init(void) {
    int result;
    printk("<1>EXAMPLE: init\n");

    /* Register character device */
    result = register_chrdev(EXAMPLE_MAJOR, EXAMPLE_NAME, &example_fops);
    if (result < 0) {
        printk("<1>EXAMPLE: Failed to register character device\n");
        return result;
    }

    return 0;
}
```

值得一提的是第一個參數 EXAMPLE_MAJOR 可以是 60, 61, 62。如果是正式要釋出的 Driver，就必須要從 Documentation/devices.txt 選取適當的 Major ID。當然，在 example_exit() 我們也必需加上對應的 unregister：

```c
static void example_exit(void) {
    printk("<1>EXAMPLE: exit\n");

    /* Unregister character device */
    unregister_chrdev(EXAMPLE_MAJOR, EXAMPLE_NAME);
}
```

在重新編譯之後，我們可以用 insmod 載入驅動程式，然後使用 mknod 建立 Device File。然後我們就可以在 User Space 使用一般的檔案讀寫操作這個 Device。

```sh
$ sudo insmod ./example.ko
$ sudo mknod /dev/example c 60 0
# /dev/example 是我們要存放檔案的路徑，c 代表 Character Device，60 是這個驅動程式的 Major ID，0 是驅動程式的 Minor ID。
$ sudo chmod 666 /dev/example
# 為了方便測試，我們把這個 Device 改成所有人都可以讀寫。
$ echo -n 'abcd' > /dev/example
$ sudo dmesg | tail
```

讀取 User Space 的資料

在前一節當中我們提供了一個 API 讓 User Space 可以操作 Driver。但是其實我們是不能直接存取 buf 的內容。因為 Kernel Space 與 User Space 有不同的位址空間，所以不能直接存取他們。我們必須藉助 copy_from_user 這個 API。

在使用這個 API 之前，我們必需引入 <asm/uaccess.h>：

```c
#include <asm/uaccess.h>
```
然後我們就可以使用 copy_from_user 來存取 User Space 的位址空間，舉例來說：

```c
ssize_t example_write(struct file *filp, const char *buf, size_t size, loff_t *f_pos) {
    size_t pos;
    uint8_t byte;
    printk("<1>EXAMPLE: write  (size=%zu)\n", size);
    for (pos = 0; pos < size; ++pos) {
        if (copy_from_user(&byte, buf + pos, 1) != 0) {
            break;
        }
        printk("<1>EXAMPLE: write  (buf[%zu] = %02x)\n", pos, (unsigned)byte);
    }
    return pos;
}
```

值得注意的是 copy_from_user() 會回傳剩下未完成的 byte 數。所以一般來說這個回傳值必須是 0 才是成功地讀入資料。要把資料從 Kernel Space 複製到 User Space 則是使用 copy_to_user() 函式，至於使用方法就不再贅述。

```sh
$ echo -n 'abcd' > /dev/example
$ sudo dmesg | tail 
```
小結

透過這個小練習，我們可以知道要怎麼開始寫一個 Linux Driver。在下一結我們會從 QEMU 的角度出發，建立一個 QEMU 的虛擬裝置，讓 QEMU Guest OS 的驅動程式可以和外面的 QEMU 虛擬裝置相互溝通。

##參考資料
Linux Device Driver 3
Linux Kernel Module Programming Guide
Writing device drivers in Linux: A brief tutorial