# Linux 的輸出入系統


Linux 的輸出入系統會透過硬體模組介面，以管理各式各樣的驅動程式。Linux 將硬體裝置分為『區塊、字元、網路』等三種類型，這三種類型的驅動程式都必須支援檔案存取的介面，因為在 Linux 當中裝置是以檔案的方式呈現的。像是 /dev/hda1, /dev/sda1, /dev/tty1 等，程式可以透過開檔 open()、讀檔 read()、寫檔 write() 的方式存取裝置，就像存取一個檔案一樣。

因此，所有的驅動程式必須支援檔案 (file) 的操作 (file_operations)，以便將裝置偽裝成檔案，以供作業系統與應用程式進行呼叫。這種將裝置偽裝成檔案的的方式，是從 UNIX 所承襲下來的一種相當成功的模式。

字元類的裝置 (Character Device) 是較為簡單的，常見的字元裝置有鍵盤、滑鼠、印表機等，這些裝置所傳遞的並非一定要是字元訊息，只要可以用串流型式表式即可。因此字元裝置又被稱為串流裝置 (Stream Device)。字元裝置必須支援基本的檔案操作，像是 open(), read(), ioctl() 等。

區塊裝置式形成檔案系統的基礎，除了基本的檔案操作外，區塊裝置還必須支援區塊性的操作 (block_device_operations)。而網路裝置由於必須支援網路定址等特性，因此成為一類獨立的裝置。舉例而言，網路裝置通常必須支援 TCP/IP，以形成網路子系統，因此具有獨特且複雜的操作。像是必須支援封包傳送機制、網路位址的ARP, RARP 協定、MAC Address 等，所以網路裝置的驅動程式也是最複雜的。

由於2.6 版的 Linux 採用模組 (module) 的方式掛載驅動程式，因此必須先透過 module_init(xxx_init_module) 與 module_exit(xxx_cleanup_module) 的方式，將驅動程式掛載到 Linux 中。這種掛載的機制仍然是一種『註冊-反向呼叫機制』，但由於核心程式乃是先編譯好的，因此必須透過一個動態載入器將模組掛載到核心中，原理並不困難，但細節卻很繁瑣。範例 1 顯示了一個最基本的模組 HelloModule.c，該模組透過 module_init(hello_init) 與 module_exit(hello_exit) 兩行巨集，將hello_init() 與 hello_exit() 函數包裝成 linux 可使用的模組介面形式，讓模組載入器得以順利載入該模組。

範例 1. 最基本的模組 (Module) 程式範例 – HelloModule.c

```c
#include <linux/init.h>                     引用檔案
#include <linux/module.h>                   
#include <linux/kernel.h>                   

static int __init hello_init(void) {        模組起始函數
  printk(KERN_ALERT "hello_init()");        印出 hello_init()
  return 0;                                 
}                                           

static void __exit hello_exit(void) {       模組結束函數
  printk(KERN_ALERT "hello_exit()");        印出 hello_exit()
}                                           

module_init(hello_init);                    模組掛載(巨集)
module_exit(hello_exit);                    模組清除(巨集)
```

當您在 Linux 中撰寫了一個模組織之後，可以利用 gcc 編譯該模組，編譯成功後再利用 insmod指令將該模組掛載到核心中，最後透過 rmmod 指令將該模組從核心中移除。範例 2 顯示了這個操作過程。

範例 2 模組的編譯、掛載與清除過程

```sh
gcc -c -O -W -DMODULE -D__KERNEL__ HelloModule.c -o HelloModule.ko
insmod ./helloModule.ko
hello_init()
rmmod ./helloModule
hello_exit()
```

Linux 的驅動程式是一個具有特定結構的模組，必需將裝置的存取函數包裝成檔案存取的形式，讓裝置的存取就像檔案的存取一般。在驅動程式的內部，仍然必須採用『註冊-反向呼叫』機制，將這些存取函數掛載到檔案系統當中，然後就可以透過檔案操作的方式讀取 (read) 或寫入 (write) 這些裝置，或者透過 ioctl() 函數操控這些裝置。範例 3 顯示了 Linux 當中裝置驅動程式的結構，該範例是一個字元裝置 device1 的驅動程式片段，其中的 Linux 中的cdev 是字元裝置結構 (struct)，該驅動程式必需實作出 device1_read(), device1_write(), device1_ioctl(), device1_open(), device1_release() 等檔案操作函數，然後封裝在 file_operations 結構的 device1_fops 變數中，透過指令 cdev_init(&dev->cdev, &device1_fops) 向 Linux 註冊，以將這些實作函數掛載到 Linux 系統中。

範例 3 Linux 中裝置驅動程式的結構範例

```c
int device1_init_module(void) {                                           模組掛載函數
 register_chrdev_region(dev, 1, "device1");                               設定裝置代號
 …                                                                        
 device1_devices = kmalloc(…);                                            分配記憶體(slab)
 …                                                                        
 cdev_init(&dev->cdev, &device1_fops);                                    註冊檔案操作函數群fops
 dev->cdev.owner = THIS_MODULE;                                           
 dev->cdev.ops = &device1_fops;                                           
 err = cdev_add (&dev->cdev, devno, 1);                                   
…                                                                         
fail:                                                                     
  device1_cleanup_module();                                               若掛載失敗則執行清除函數
}                                                                         

int device1_open(struct inode *inode, struct file *filp) {...}            裝置開啟函數
int device1_release(struct inode *inode, struct file *filp) {...}         裝置釋放函數
ssize_t device1_read(struct file *filp, …){...}                           裝置讀取函數
ssize_t device1_write(struct file *filp,…) {...}                          裝置寫入函數
int device1_ioctl(struct inode *inode, struct file *filp…) {…}            裝置控制函數

struct file_operations device1_fops = {                                   裝置的檔案操作函數群，
 .owner = THIS_MODULE,                                                    包含 read(), write(), ioctl(), 
 .read = device1_read,                                                    open(), release() 等。
 .write = device1_write,                                                  
 .ioctl = device1_ioctl,                                                  
 .open = device1_open,                                                    
 .release = device1_release,                                              
};                                                                        

module_init(device1_init_module);                                         模組掛載(巨集)
module_exit(device1_cleanup_module);                                      模組清除(巨集)
```

走筆至此，我們已經介紹完 Linux 中的行程管理、記憶體管理、輸出入系統與檔案系統，完成了我們對 Linux 作業系統的介紹。但是 Linux 是個龐大的系統，筆者無法進行太詳細與深入的介紹，有興趣的讀者請進一步參考相關書籍。