# send pid to kernel by ioctl and kernel send_sig to user space

- devone.c

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/oom.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include "header.h"

MODULE_LICENSE("Dual BSD/GPL");

static int major_no;
static int vrmonitor_pid;
static int cccc = 100;
static int send_sig_flag = 1;

static struct task_struct* brook_tsk;
static int vrmonitor_sig_handler(void* arg);

static int vrmonitor_sig_handler(void* arg)
{
    struct task_struct* p;
    struct pid* pp;

    printk(KERN_EMERG "cccc addr=%p ,cccc=%d\n", &cccc, cccc++);

    pp = find_vpid(vrmonitor_pid);
    p = pid_task(pp, PIDTYPE_PID);

    while (send_sig_flag) {
        msleep(1000);
        send_sig(SIGUSR2, p, 0);
        printk(KERN_EMERG "vrmonitor_sig_handler current pid=%d send to PID=%d \n",
               current->pid, vrmonitor_pid);
    }

    send_sig_flag = 1;

    return 0;
}

static int device_open(struct inode* inode, struct file* file)
{
    printk(KERN_EMERG "pid=%d, Node Opened\n", current->pid);
    return 0;
}

int device_ioctl(struct file* filp,
                 unsigned int cmd,
                 unsigned long args)
{
    int ret;

    switch (cmd) {
    case IOCTL_CMD:
        printk(KERN_EMERG "IOCTL_CMD");
        break;

    case IOCTL_SEND_PID:
        printk(KERN_EMERG "appln PID=%u , current kernel pid=%d\n", (unsigned int)args,
               current->pid);
        vrmonitor_pid = (unsigned int)args;
        brook_tsk = kthread_create(vrmonitor_sig_handler, NULL, "brook");

        if (IS_ERR(brook_tsk)) {
            ret = PTR_ERR(brook_tsk);
            brook_tsk = NULL;
            goto out;
        }

        wake_up_process(brook_tsk);
        break;

    case IOCTL_STOP_SIG:
        send_sig_flag = 0;
        printk(KERN_EMERG "IOCTL_STOP_SIG send_sig_flag=%d\n", send_sig_flag);
        break;

    default:
        printk(KERN_EMERG "Illegal ioctl command word\n");
        break;
    }

    return 0;

out:
    return ret;
}

static int device_release(struct inode* inode, struct file* file)
{
    printk(KERN_EMERG "Module Released \n");
    return 0;
}

static struct class* my_class;

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl
};

static int hello_init(void)
{
    major_no = register_chrdev(0, DEVICE_NAME, &fops);
    printk(KERN_EMERG "Module Major No : %d\n", major_no);

    my_class = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(my_class, NULL, MKDEV(major_no, 0), NULL, DEVICE_NAME);
    printk(KERN_EMERG "Module loaded in kernel\n");
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_EMERG "Device is Released or closed \n");
    device_destroy(my_class, MKDEV(major_no, 0));
    class_unregister(my_class);
    class_destroy(my_class);
    unregister_chrdev(major_no, DEVICE_NAME);
}

module_init(hello_init);
module_exit(hello_exit);

```

- Makefile

```sh

CFILES = devone.c 

obj-m += sample.o
sample-objs := $(CFILES:.c=.o)

all:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

```

- header.h

```c
#include <linux/ioctl.h>
#include <linux/kdev_t.h> /* for MKDEV */

#define DEVICE_NAME "my_dev"
#define DEVICE_PATH "/dev/my_dev"

#define MAGIC_NO '4'
/*
 * Set the message of the device driver
 */
#define IOCTL_CMD _IOR(MAGIC_NO, 0, char *)
#define IOCTL_SEND_PID _IOR(MAGIC_NO, 1, char *)
#define IOCTL_STOP_SIG _IOR(MAGIC_NO, 2, char *)
```

- appln.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "header.h"

static int fd; 

void stop_sig(int sig)
{
    unsigned int sig_flag = 0;

    ioctl(fd, IOCTL_STOP_SIG, sig_flag);
    printf("stop_sig=%d, pid=%d\n", sig, getpid());
    (void) signal(SIGINT, SIG_DFL);

#if 1
    kill(getpid(), SIGINT);
#endif
}


void signal_handler(int signum)
{
    printf("signal_handler %d, pid=%d\n", signum, getpid());

    if (signum == SIGUSR2) {
        printf("SIGUSR2\n");
    } else if (signum == SIGUSR1) {
        printf("SIGUSR1\n");
    }
}

int main()
{
    pid_t pid = getpid();

    printf("PID=%u\n", pid);

    signal(SIGUSR2, signal_handler);
    signal(SIGINT, stop_sig);

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd == -1) {
        printf("open fail\n");
        exit(-1);
    }

    ioctl(fd, IOCTL_SEND_PID, pid);
    printf("Ioctl executed\n");

    while(1) {}

    close(fd);
    return 0;
}
```

```c
sudo insmod  sample.ko
sudo ./appln  //  must add sudo
// crtl + c 透過ioctrl 讓 kernel 停止發送 sig , 再一次 ctrl + c 停止 process
```
