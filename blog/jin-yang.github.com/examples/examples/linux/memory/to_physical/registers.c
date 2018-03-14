#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 3, 0)
#include <asm/switch_to.h>
#else
#include <asm/system.h>
#endif
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static char modname[] = "registers";
struct gdtr_struct{
    short limit;
    unsigned long long address __attribute__((packed));
};

static int registers_proc_show(struct seq_file *m, void *v)
{
    struct mm_struct *mm;
    unsigned long cr0,cr3,cr4;
    struct gdtr_struct gdtr;
    int entry_num;

    seq_printf(m, "----  MM ----\n");
    mm = current->active_mm;
	seq_printf(m, "m->pgd: 0x%p\n", mm->pgd);

    seq_printf(m, "\n----  GDTR ----\n");
    //asm(" sgdt gdtr");
    asm("sgdt %0":"=m"(gdtr));
    entry_num = (gdtr.limit + 1) / 8;
    seq_printf(m, "gdtr_addr: 0x%llx limit:%d entry:%d\n",
               gdtr.address, gdtr.limit, entry_num);

    seq_printf(m, "\n----  Control Registers ----\n");
    cr0 = read_cr0();
    seq_printf(m, "CR0 = 0x%08lx\n", cr0);
    seq_printf(m, "CR0.PG = %d\n", (int)(0x01 & (cr0 >> 31)));

    cr3 = read_cr3();
    seq_printf(m, "CR3 = 0x%08lx\n", cr3);
    seq_printf(m, "CR3 PT Base = 0x%08lx\n", cr3 & 0xFFFFF000);;

    cr4 = read_cr4();
    seq_printf(m, "CR4 = 0x%08lx\n", cr4);
    seq_printf(m, "CR4.PSE = %d\n", (int)(cr4 >> 4) & 0x1);
    seq_printf(m, "CR4.PAE = %d\n", (int)(cr4 >> 5) & 0x1);
    seq_printf(m, "CR4.PCIDE = %d\n", (int)(cr4 >> 17) & 0x1);

    return 0;
}

static int registers_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, registers_proc_show, NULL);
}

static struct proc_dir_entry *registers_proc_file;
static const struct file_operations registers_proc_fops = {
	.open		= registers_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init registers_init( void )
{
    printk(KERN_ALERT "Installing \'%s\' module\n", modname );
    registers_proc_file = proc_create(modname, 0, NULL, &registers_proc_fops);
    return 0;
}

static void __exit registers_exit( void )
{
    proc_remove(registers_proc_file);
    printk(KERN_ALERT "Removing \'%s\' module\n", modname );
}

module_init(registers_init);
module_exit(registers_exit);
MODULE_LICENSE("GPL");
