# 内核和用户空间共享内存的实现例程-proc和mmap


之所以想写这篇帖子，是有两个方面原因。其一是内核版有一个关于《内核可以从线性地址直接计算物理地址，用来做什么呢？》的讨论，偶说计算出物理地址可以共享给用户空间读写。dreamice兄说能否说一下详细的应用。其二是alb*版主提到wheelz曾经写过这样一个例程，拜读了一把，发现在传递物理地址和内存大小上，wheelz的例程还有些不够灵活。alb*版主提到可以通过文件的方式实现动态的传递。

因此，偶也写了这样一个例程，可以动态的将内核空间的物理地址和大小传给用户空间。

整个内核模块，在模块插入时建立proc文件，分配内存。卸载模块的时候将用户空间写入的内容打印出来。
以下是内核模块的代码和用户空间的测试代码。


```c
/*This program is used to allocate memory in kernel
and pass the physical address to userspace through proc file.*/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>

#define PROC_MEMSHARE_DIR                                "memshare"
#define PROC_MEMSHARE_PHYADDR                        "phymem_addr"
#define PROC_MEMSHARE_SIZE                                "phymem_size"

/*alloc one page. 4096 bytes*/
#define PAGE_ORDER                                0
/*this value can get from PAGE_ORDER*/
#define PAGES_NUMBER                                1

struct proc_dir_entry* proc_memshare_dir ;
unsigned long kernel_memaddr = 0;
unsigned long kernel_memsize = 0;

static int proc_read_phymem_addr(char* page, char** start, off_t off, int count)
{
    return sprintf(page, "%08lx\n", __pa(kernel_memaddr));
}
static int proc_read_phymem_size(char* page, char** start, off_t off, int count)
{
    return sprintf(page, "%lu\n", kernel_memsize);
}

static int __init init(void)
{
    /*build proc dir "memshare"and two proc files: phymem_addr, phymem_size in the dir*/
    proc_memshare_dir = proc_mkdir(PROC_MEMSHARE_DIR, NULL);
    create_proc_info_entry(PROC_MEMSHARE_PHYADDR, 0, proc_memshare_dir,
                           proc_read_phymem_addr);
    create_proc_info_entry(PROC_MEMSHARE_SIZE, 0, proc_memshare_dir,
                           proc_read_phymem_size);

    /*alloc one page*/
    kernel_memaddr = __get_free_pages(GFP_KERNEL, PAGE_ORDER);

    if (!kernel_memaddr) {
        printk("Allocate memory failure!\n");
    } else {
        SetPageReserved(virt_to_page(kernel_memaddr));
        kernel_memsize = PAGES_NUMBER * PAGE_SIZE;
        printk("Allocate memory success!. The phy mem addr=%08lx, size=%lu\n",
               __pa(kernel_memaddr), kernel_memsize);
    }

    return 0;
}

static void __exit fini(void)
{
    printk("The content written by user is: %s\n", (unsigned char*) kernel_memaddr);
    ClearPageReserved(virt_to_page(kernel_memaddr));
    free_pages(kernel_memaddr, PAGE_ORDER);
    remove_proc_entry(PROC_MEMSHARE_PHYADDR, proc_memshare_dir);
    remove_proc_entry(PROC_MEMSHARE_SIZE, proc_memshare_dir);
    remove_proc_entry(PROC_MEMSHARE_DIR, NULL);

    return;
}
module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Godbach (nylzhaowei@163.com)");
MODULE_DESCRIPTION("Kernel memory share module.");
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s string\n", argv[0]);
        return 0;
    }

    unsigned long phymem_addr, phymem_size;
    char* map_addr;
    char s[256];
    int fd;

    /*get the physical address of allocated memory in kernel*/
    fd = open("/proc/memshare/phymem_addr", O_RDONLY);

    if (fd < 0) {
        printf("cannot open file /proc/memshare/phymem_addr\n");
        return 0;
    }

    read(fd, s, sizeof(s));
    sscanf(s, "%lx", &phymem_addr);
    close(fd);

    /*get the size of allocated memory in kernel*/
    fd = open("/proc/memshare/phymem_size", O_RDONLY);

    if (fd < 0) {
        printf("cannot open file /proc/memshare/phymem_size\n");
        return 0;
    }

    read(fd, s, sizeof(s));
    sscanf(s, "%lu", &phymem_size);
    close(fd);

    printf("phymem_addr=%lx, phymem_size=%lu\n", phymem_addr, phymem_size);
    /*memory map*/
    int map_fd = open("/dev/mem", O_RDWR);

    if (map_fd < 0) {
        printf("cannot open file /dev/mem\n");
        return 0;
    }

    map_addr = mmap(0, phymem_size, PROT_READ | PROT_WRITE, MAP_SHARED, map_fd,
                    phymem_addr);
    strcpy(map_addr, argv[1]);
    munmap(map_addr, phymem_size);
    close(map_fd);
    return 0;

}
```


```sh
debian:/home/km/memshare# insmod memshare_kernel.ko
debian:/home/km/memshare# ./memshare_user 'hello,world!'
phymem_addr=e64e000, phymem_size=4096
debian:/home/km/memshare# cat /proc/memshare/phymem_addr
0e64e000
debian:/home/km/memshare# cat /proc/memshare/phymem_size
4096
debian:/home/km/memshare# rmmod memshare_kernel
debian:/home/km/memshare# tail /var/log/messages
Sep 27 18:14:24 debian kernel: [50527.567931] Allocate memory success!. The phy mem addr=0e64e000, size=4096
Sep 27 18:15:31 debian kernel: [50592.570986] The content written by user is: hello,world!
```

