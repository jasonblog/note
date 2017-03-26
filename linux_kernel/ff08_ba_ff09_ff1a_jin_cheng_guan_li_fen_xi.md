# （八）：进程管理分析


进程其实就是程序的执行时的实例，是处于执行期的程序。在linux内核中，进程列表被存放在一个双向循环链表中，链表中每一项都是类型为task_struct的结构，该结构称作进程描述符，进程描述符包含一个具体进程的所有信息，这个结构就是我们在操作系统中所说的PCB（Process Control Block）。该结构定义于`<include/linux/sched.h>`文件中：


```c
struct task_struct {
    volatile long state;    /* -1 unrunnable, 0 runnable, >0 stopped */
    void* stack;
    atomic_t usage;
    unsigned int flags; /* per process flags, defined below */
    unsigned int ptrace;

    int lock_depth;     /* BKL lock depth */

    ......

    int prio, static_prio, normal_prio;
    unsigned int rt_priority;
    const struct sched_class* sched_class;
    struct sched_entity se;
    struct sched_rt_entity rt;
    ......
    struct task_struct* parent; /* recipient of SIGCHLD, wait4() reports */
    struct list_head children;  /* list of my children */
    struct list_head sibling;   /* linkage in my parent's children list */
    ......
};
```

该结构体中包含的数据可以完整的描述一个正在执行的程序：打开的文件、进程的地址空间、挂起的信号、进程的状态、以及其他很多信息。

在系统运行过程中，进程频繁切换，所以我们需要一种方式能够快速获得当前进程的task_struct，于是进程内核堆栈底部存放着struct thread_info。该结构中有一个成员指向当前进程的task_struct。在x86上，struct thread_info在文件`<arch/x86/include/asm/thread_info.h>`中定义：

```c
struct thread_info {
    struct task_struct*
        task;      /* 该指针存放的是指向该任务实际task_struct的指针 */
    struct exec_domain*  exec_domain;   /* execution domain */
    __u32           flags;      /* low level flags */
    __u32           status;     /* thread synchronous flags */
    __u32           cpu;        /* current CPU */
    int         preempt_count;  /* 0 => preemptable,
                           <0 => BUG */
    mm_segment_t        addr_limit;
    struct restart_block    restart_block;
    void __user*     sysenter_return;
#ifdef CONFIG_X86_32
    unsigned long           previous_esp;   /* ESP of the previous stack in
                           case of nested (IRQ) stacks
                        */
    __u8            supervisor_stack[0];
#endif
    int         uaccess_err;
};
```

使用current宏就可以获得当前进程的进程描述符。

每一个进程都有一个父进程，每个进程管理自己的子进程。每个进程都是init进程的子进程，init进程在内 核系统启动的最后阶段启动init进程，该进程读取系统的初始化脚本并执行其他相关程序，最终完成系统启动的整个过程。每个进程有0个或多个子进程，进程间的关系存放在进程描述符中。task_struct中有一个parent的指针，指向其父进程；还有个children的指针指向其子进程的链表。所以，对于当前进程，可以通过current宏来获得父进程和子进程的进程描述符。

下面程序打印当前进程、父进程信息和所有子进程信息：


```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>

#include <linux/sched.h>

void sln_taskstruct_do(void)
{
    struct task_struct*  cur,
               *parent,
               *task;
    struct list_head*    first_child,
               *child_list,
               *cur_chd;

    //获取当前进程信息
    cur = current;

    printk(KERN_ALERT"Current: %s[%d]\n",
           cur->comm, cur->pid);

    //获取父进程信息
    parent = current->parent;
    printk(KERN_ALERT"Parent: %s[%d]\n",
           parent->comm, parent->pid);

    //获取所有祖先进程信息
    for (task = cur; task != &init_task; task = task->parent) {
        printk(KERN_ALERT"ancestor: %s[%d]\n",
               task->comm, task->pid);
    }

    //获取所有子进程信息
    child_list = &cur->children;
    first_child = &cur->children;

    for (cur_chd = child_list->next;
         cur_chd != first_child;
         cur_chd = cur_chd->next) {
        task = list_entry(child_list, struct task_struct, sibling);

        printk(KERN_ALERT"Children: %s[%d]\n",
               task->comm, task->pid);
    }

}

static int __init sln_init(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);

    sln_taskstruct_do();
    return 0;
}

static void __exit sln_exit(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);
}

module_init(sln_init);
module_exit(sln_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");
```

执行结果如下：

```sh
 # insmod task.ko  
===sln_init===  
Current: insmod[4315]  
Parent: bash[4032]  
ancestor: insmod[4315]  
ancestor: bash[4032]  
ancestor: login[2563]  
ancestor: init[1]  
```

linux操作系统提供产生进程的机制，在Linux下的fork()使用写时拷贝(copy-on-write)页实现。这种技术原理是：内存并不复制整个进程地址空间，而是让父进程和子进程共享同一拷贝，只有在需要写入的时候，数据才会被复制。也就是资源的复制只是发生在需要写入的时候才进行，在此之前都是以只读的方式共享。

linux通过clone()系统调用实现fork()，然后clone()去调用do_fork()，do_fork()完成创建中大部分工作。库函数vfork()、__clone()都根据各自需要的参数标志去调用clone()。fork()的实际开销就是复制父进程的页表以及给子进程创建唯一的进程描述符。

用户空间的fork()经过系统调用进入内核，在内核中对应的处理函数为sys_fork()，定义于`<arch/x86/kernel/process.c>`文件中。


```c
int sys_fork(struct pt_regs* regs)
{
    return do_fork(SIGCHLD, regs->sp, regs, 0, NULL, NULL);
}

long do_fork(unsigned long clone_flags,
             unsigned long stack_start,
             struct pt_regs* regs,
             unsigned long stack_size,
             int __user* parent_tidptr,
             int __user* child_tidptr)
{
    struct task_struct* p;
    int trace = 0;
    long nr;
    ......
    p = copy_process(clone_flags, stack_start, regs, stack_size,
                     child_tidptr, NULL, trace);
    ......
    return nr;
}

static struct task_struct* copy_process(unsigned long clone_flags,
                                        unsigned long stack_start,
                                        struct pt_regs* regs,
                                        unsigned long stack_size,
                                        int __user* child_tidptr,
                                        struct pid* pid,
                                        int trace)
{
    int retval;
    struct task_struct* p;
    int cgroup_callbacks_done = 0;

    if ((clone_flags & (CLONE_NEWNS | CLONE_FS)) == (CLONE_NEWNS | CLONE_FS)) {
        return ERR_PTR(-EINVAL);
    }

    /*
     * Thread groups must share signals as well, and detached threads
     * can only be started up within the thread group.
     */
    if ((clone_flags & CLONE_THREAD) && !(clone_flags & CLONE_SIGHAND)) {
        return ERR_PTR(-EINVAL);
    }

    /*
     * Shared signal handlers imply shared VM. By way of the above,
     * thread groups also imply shared VM. Blocking this case allows
     * for various simplifications in other code.
     */
    if ((clone_flags & CLONE_SIGHAND) && !(clone_flags & CLONE_VM)) {
        return ERR_PTR(-EINVAL);
    }

    /*
     * Siblings of global init remain as zombies on exit since they are
     * not reaped by their parent (swapper). To solve this and to avoid
     * multi-rooted process trees, prevent global and container-inits
     * from creating siblings.
     */
    if ((clone_flags & CLONE_PARENT) &&
        current->signal->flags & SIGNAL_UNKILLABLE) {
        return ERR_PTR(-EINVAL);
    }

    retval = security_task_create(clone_flags);

    if (retval) {
        goto fork_out;
    }

    retval = -ENOMEM;
    p = dup_task_struct(current);

    if (!p) {
        goto fork_out;
    }

    ftrace_graph_init_task(p);
    rt_mutex_init_task(p);
#ifdef CONFIG_PROVE_LOCKING
    DEBUG_LOCKS_WARN_ON(!p->hardirqs_enabled);
    DEBUG_LOCKS_WARN_ON(!p->softirqs_enabled);
#endif
    retval = -EAGAIN;

    if (atomic_read(&p->real_cred->user->processes) >=
        p->signal->rlim[RLIMIT_NPROC].rlim_cur) {
        if (!capable(CAP_SYS_ADMIN) && !capable(CAP_SYS_RESOURCE) &&
            p->real_cred->user != INIT_USER) {
            goto bad_fork_free;
        }
    }

    retval = copy_creds(p, clone_flags);

    if (retval < 0) {
        goto bad_fork_free;
    }

    /*
     * If multiple threads are within copy_process(), then this check
     * triggers too late. This doesn't hurt, the check is only there
     * to stop root fork bombs.
     */
    retval = -EAGAIN;

    if (nr_threads >= max_threads) {
        goto bad_fork_cleanup_count;
    }

    if (!try_module_get(task_thread_info(p)->exec_domain->module)) {
        goto bad_fork_cleanup_count;
    }

    p->did_exec = 0;
    delayacct_tsk_init(p);  /* Must remain after dup_task_struct() */
    copy_flags(clone_flags, p);
    INIT_LIST_HEAD(&p->children);
    INIT_LIST_HEAD(&p->sibling);
    rcu_copy_process(p);
    p->vfork_done = NULL;
    spin_lock_init(&p->alloc_lock);
    init_sigpending(&p->pending);
    p->utime = cputime_zero;
    p->stime = cputime_zero;
    p->gtime = cputime_zero;
    p->utimescaled = cputime_zero;
    p->stimescaled = cputime_zero;
    p->prev_utime = cputime_zero;
    p->prev_stime = cputime_zero;
    p->default_timer_slack_ns = current->timer_slack_ns;
    task_io_accounting_init(&p->ioac);
    acct_clear_integrals(p);
    posix_cpu_timers_init(p);
    p->lock_depth = -1;     /* -1 = no lock */
    do_posix_clock_monotonic_gettime(&p->start_time);
    p->real_start_time = p->start_time;
    monotonic_to_bootbased(&p->real_start_time);
    p->io_context = NULL;
    p->audit_context = NULL;
    cgroup_fork(p);
#ifdef CONFIG_NUMA
    p->mempolicy = mpol_dup(p->mempolicy);

    if (IS_ERR(p->mempolicy)) {
        retval = PTR_ERR(p->mempolicy);
        p->mempolicy = NULL;
        goto bad_fork_cleanup_cgroup;
    }

    mpol_fix_fork_child_flag(p);
#endif
#ifdef CONFIG_TRACE_IRQFLAGS
    p->irq_events = 0;
#ifdef __ARCH_WANT_INTERRUPTS_ON_CTXSW
    p->hardirqs_enabled = 1;
#else
    p->hardirqs_enabled = 0;
#endif
    p->hardirq_enable_ip = 0;
    p->hardirq_enable_event = 0;
    p->hardirq_disable_ip = _THIS_IP_;
    p->hardirq_disable_event = 0;
    p->softirqs_enabled = 1;
    p->softirq_enable_ip = _THIS_IP_;
    p->softirq_enable_event = 0;
    p->softirq_disable_ip = 0;
    p->softirq_disable_event = 0;
    p->hardirq_context = 0;
    p->softirq_context = 0;
#endif
#ifdef CONFIG_LOCKDEP
    p->lockdep_depth = 0; /* no locks held yet */
    p->curr_chain_key = 0;
    p->lockdep_recursion = 0;
#endif
#ifdef CONFIG_DEBUG_MUTEXES
    p->blocked_on = NULL; /* not blocked yet */
#endif
    p->bts = NULL;
    p->stack_start = stack_start;
    /* Perform scheduler related setup. Assign this task to a CPU. */
    sched_fork(p, clone_flags);
    retval = perf_event_init_task(p);

    if (retval) {
        goto bad_fork_cleanup_policy;
    }

    if ((retval = audit_alloc(p))) {
        goto bad_fork_cleanup_policy;
    }

    /* copy all the process information */
    if ((retval = copy_semundo(clone_flags, p))) {
        goto bad_fork_cleanup_audit;
    }

    if ((retval = copy_files(clone_flags, p))) {
        goto bad_fork_cleanup_semundo;
    }

    if ((retval = copy_fs(clone_flags, p))) {
        goto bad_fork_cleanup_files;
    }

    if ((retval = copy_sighand(clone_flags, p))) {
        goto bad_fork_cleanup_fs;
    }

    if ((retval = copy_signal(clone_flags, p))) {
        goto bad_fork_cleanup_sighand;
    }

    if ((retval = copy_mm(clone_flags, p))) {
        goto bad_fork_cleanup_signal;
    }

    if ((retval = copy_namespaces(clone_flags, p))) {
        goto bad_fork_cleanup_mm;
    }

    if ((retval = copy_io(clone_flags, p))) {
        goto bad_fork_cleanup_namespaces;
    }

    retval = copy_thread(clone_flags, stack_start, stack_size, p, regs);

    if (retval) {
        goto bad_fork_cleanup_io;
    }

    if (pid != &init_struct_pid) {
        retval = -ENOMEM;
        pid = alloc_pid(p->nsproxy->pid_ns);

        if (!pid) {
            goto bad_fork_cleanup_io;
        }

        if (clone_flags & CLONE_NEWPID) {
            retval = pid_ns_prepare_proc(p->nsproxy->pid_ns);

            if (retval < 0) {
                goto bad_fork_free_pid;
            }
        }
    }

    p->pid = pid_nr(pid);
    p->tgid = p->pid;

    if (clone_flags & CLONE_THREAD) {
        p->tgid = current->tgid;
    }

    if (current->nsproxy != p->nsproxy) {
        retval = ns_cgroup_clone(p, pid);

        if (retval) {
            goto bad_fork_free_pid;
        }
    }

    p->set_child_tid = (clone_flags & CLONE_CHILD_SETTID) ? child_tidptr : NULL;
    /*
     * Clear TID on mm_release()?
     */
    p->clear_child_tid = (clone_flags & CLONE_CHILD_CLEARTID) ? child_tidptr : NULL;
#ifdef CONFIG_FUTEX
    p->robust_list = NULL;
#ifdef CONFIG_COMPAT
    p->compat_robust_list = NULL;
#endif
    INIT_LIST_HEAD(&p->pi_state_list);
    p->pi_state_cache = NULL;
#endif

    /*
     * sigaltstack should be cleared when sharing the same VM
     */
    if ((clone_flags & (CLONE_VM | CLONE_VFORK)) == CLONE_VM) {
        p->sas_ss_sp = p->sas_ss_size = 0;
    }

    /*
     * Syscall tracing should be turned off in the child regardless
     * of CLONE_PTRACE.
     */
    clear_tsk_thread_flag(p, TIF_SYSCALL_TRACE);
#ifdef TIF_SYSCALL_EMU
    clear_tsk_thread_flag(p, TIF_SYSCALL_EMU);
#endif
    clear_all_latency_tracing(p);
    /* ok, now we should be set up.. */
    p->exit_signal = (clone_flags & CLONE_THREAD) ? -1 : (clone_flags & CSIGNAL);
    p->pdeath_signal = 0;
    p->exit_state = 0;
    /*
     * Ok, make it visible to the rest of the system.
     * We dont wake it up yet.
     */
    p->group_leader = p;
    INIT_LIST_HEAD(&p->thread_group);
    /* Now that the task is set up, run cgroup callbacks if
     * necessary. We need to run them before the task is visible
     * on the tasklist. */
    cgroup_fork_callbacks(p);
    cgroup_callbacks_done = 1;
    /* Need tasklist lock for parent etc handling! */
    write_lock_irq(&tasklist_lock);
    /*
     * The task hasn't been attached yet, so its cpus_allowed mask will
     * not be changed, nor will its assigned CPU.
     *
     * The cpus_allowed mask of the parent may have changed after it was
     * copied first time - so re-copy it here, then check the child's CPU
     * to ensure it is on a valid CPU (and if not, just force it back to
     * parent's CPU). This avoids alot of nasty races.
     */
    p->cpus_allowed = current->cpus_allowed;
    p->rt.nr_cpus_allowed = current->rt.nr_cpus_allowed;

    if (unlikely(!cpu_isset(task_cpu(p), p->cpus_allowed) ||
                 !cpu_online(task_cpu(p)))) {
        set_task_cpu(p, smp_processor_id());
    }

    /* CLONE_PARENT re-uses the old parent */
    if (clone_flags & (CLONE_PARENT | CLONE_THREAD)) {
        p->real_parent = current->real_parent;
        p->parent_exec_id = current->parent_exec_id;
    } else {
        p->real_parent = current;
        p->parent_exec_id = current->self_exec_id;
    }

    spin_lock(¤t->sighand->siglock);
    /*
     * Process group and session signals need to be delivered to just the
     * parent before the fork or both the parent and the child after the
     * fork. Restart if a signal comes in before we add the new process to
     * it's process group.
     * A fatal signal pending means that current will exit, so the new
     * thread can't slip out of an OOM kill (or normal SIGKILL).
     */
    recalc_sigpending();

    if (signal_pending(current)) {
        spin_unlock(¤t->sighand->siglock);
        write_unlock_irq(&tasklist_lock);
        retval = -ERESTARTNOINTR;
        goto bad_fork_free_pid;
    }

    if (clone_flags & CLONE_THREAD) {
        atomic_inc(¤t->signal->count);
        atomic_inc(¤t->signal->live);
        p->group_leader = current->group_leader;
        list_add_tail_rcu(&p->thread_group, &p->group_leader->thread_group);
    }

    if (likely(p->pid)) {
        list_add_tail(&p->sibling, &p->real_parent->children);
        tracehook_finish_clone(p, clone_flags, trace);

        if (thread_group_leader(p)) {
            if (clone_flags & CLONE_NEWPID) {
                p->nsproxy->pid_ns->child_reaper = p;
            }

            p->signal->leader_pid = pid;
            tty_kref_put(p->signal->tty);
            p->signal->tty = tty_kref_get(current->signal->tty);
            attach_pid(p, PIDTYPE_PGID, task_pgrp(current));
            attach_pid(p, PIDTYPE_SID, task_session(current));
            list_add_tail_rcu(&p->tasks, &init_task.tasks);
            __get_cpu_var(process_counts)++;
        }

        attach_pid(p, PIDTYPE_PID, pid);
        nr_threads++;
    }

    total_forks++;
    spin_unlock(¤t->sighand->siglock);
    write_unlock_irq(&tasklist_lock);
    proc_fork_connector(p);
    cgroup_post_fork(p);
    perf_event_fork(p);
    return p;
bad_fork_free_pid:

    if (pid != &init_struct_pid) {
        free_pid(pid);
    }

bad_fork_cleanup_io:
    put_io_context(p->io_context);
bad_fork_cleanup_namespaces:
    exit_task_namespaces(p);
bad_fork_cleanup_mm:

    if (p->mm) {
        mmput(p->mm);
    }

bad_fork_cleanup_signal:

    if (!(clone_flags & CLONE_THREAD)) {
        __cleanup_signal(p->signal);
    }

bad_fork_cleanup_sighand:
    __cleanup_sighand(p->sighand);
bad_fork_cleanup_fs:
    exit_fs(p); /* blocking */
bad_fork_cleanup_files:
    exit_files(p); /* blocking */
bad_fork_cleanup_semundo:
    exit_sem(p);
bad_fork_cleanup_audit:
    audit_free(p);
bad_fork_cleanup_policy:
    perf_event_free_task(p);
#ifdef CONFIG_NUMA
    mpol_put(p->mempolicy);
bad_fork_cleanup_cgroup:
#endif
    cgroup_exit(p, cgroup_callbacks_done);
    delayacct_tsk_free(p);
    module_put(task_thread_info(p)->exec_domain->module);
bad_fork_cleanup_count:
    atomic_dec(&p->cred->user->processes);
    exit_creds(p);
bad_fork_free:
    free_task(p);
fork_out:
    return ERR_PTR(retval);
}
```

上面执行完以后，回到do_fork()函数，如果copy_process()函数成功返回。新创建的子进程被唤醒并让其投入运行。内核有意选择子进程先运行。因为一般子进程都会马上调用exec()函数，这样可以避免写时拷贝的额外开销。如果父进程首先执行的话，有可能会开始向地址空间写入。

线程机制提供了在同一程序内共享内存地址空间运行的一组线程。线程机制支持并发程序设计技术，可以共享打开的文件和其他资源。如果你的系统是多核心的，那多线程技术可保证系统的真正并行。在linux中，并没有线程这个概念，linux中所有的线程都当作进程来处理，换句话说就是在内核中并没有什么特殊的结构和算法来表示线程。在linux中，线程仅仅是一个使用共享资源的进程。每个线程都拥有一个隶属于自己的task_struct。所以说线程本质上还是进程，只不过该进程可以和其他一些进程共享某些资源信息。

内核有时需要在后台执行一些操作，这种任务可以通过内核线程完成，内核线程独立运行在内核空间的标准进程。内核线程和普通的进程间的区别在于内核线程没有独立的地址空间。它们只在讷河空间运行，从来不切换到用户空间去。内核进程和普通进程一样，可以被调度，也可以被抢占。内核线程也只能由其它内核线程创建，内核是通过从kthreadd内核进程中衍生出所有新的内核线程来自动处理这一点的。在内核中创建一个的内核线程方法如下：


```c
struct task_struct *kthread_create(int (*threadfn)(void *data),  
                   void *data,                                                            
                   const char namefmt[], ...)  
```
该函数实现如下：

```c
struct task_struct* kthread_create(int (*threadfn)(void* data),
                                   void* data,
                                   const char namefmt[],
                                   ...)
{
    struct kthread_create_info create;
    create.threadfn = threadfn;
    create.data = data;
    init_completion(&create.done);
    spin_lock(&kthread_create_lock);
    list_add_tail(&create.list, &kthread_create_list);
    spin_unlock(&kthread_create_lock);
    wake_up_process(kthreadd_task);
    wait_for_completion(&create.done);

    if (!IS_ERR(create.result)) {
        struct sched_param param = { .sched_priority = 0 };
        va_list args;
        va_start(args, namefmt);
        vsnprintf(create.result->comm, sizeof(create.result->comm),
                  namefmt, args);
        va_end(args);
        /*
         * root may have changed our (kthreadd's) priority or CPU mask.
         * The kernel thread should not inherit these properties.
         */
        sched_setscheduler_nocheck(create.result, SCHED_NORMAL, ¶m);
        set_cpus_allowed_ptr(create.result, cpu_all_mask);
    }
}
```

新的任务是由kthread内核进程通过clone()系统调用而创建的。新的进程将运行threadfn函数，给其传递参数data，新的进程名称为namefmt，新创建的进程处于不可运行状态，需要调用wake_up_process()明确的唤醒它，否则它不会主动运行。也可以通过调用kthread_run()来创建一个进程并让它运行起来。


```c
#define kthread_run(threadfn, data, namefmt, ...)              \                          
({                                     \  
    struct task_struct *__k                        \  
        = kthread_create(threadfn, data, namefmt, ## __VA_ARGS__); \  
    if (!IS_ERR(__k))                          \  
        wake_up_process(__k);                      \  
    __k;                                   \  
})
```
kthread_run其实就是创建了一个内核线程并且唤醒了。内核线程启动后就一直运行直到调用do_exit()退出或者内核的其他部分调用kthread_stop()退出。

```c
int kthread_stop(struct task_struct *k); 
```

下面为一个使用内核线程的示例：

```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/sched.h>  //schdule_timeout()  
#include <linux/kthread.h>
struct task_struct*  sln_task;
int sln_kthread_func(void* arg)
{
    while (!kthread_should_stop()) {
        printk(KERN_ALERT"===%s===\n", __func__);
        set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(2 * HZ);
    }

    return 0;
}
void sln_init_do(void)
{
    int data = 9527;
    sln_task = kthread_create(sln_kthread_func,
                              &data,
                              "sln_kthread_task");

    if (IS_ERR(sln_task)) {
        printk(KERN_ALERT"kthread_create() failed!\n");
        return;
    }

    wake_up_process(sln_task);
}
void sln_exit_do(void)
{
    if (NULL != sln_task) {
        kthread_stop(sln_task);
        sln_task = NULL;
    }
}
static int __init sln_init(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);
    sln_init_do();
    return 0;
}
static void __exit sln_exit(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);
    sln_exit_do();
}
module_init(sln_init);
module_exit(sln_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");
```

既然有进程的创建，那就有进程的终结，终结时内核必须释放它所占有的资源。内核终结时，大部分任务都是靠do_exit()（定义于）来完成。


```c
NORET_TYPE void do_exit(long code)
{
    struct task_struct* tsk = current;
    int group_dead;
    profile_task_exit(tsk);
    WARN_ON(atomic_read(&tsk->fs_excl));

    //不可在中断上下文中使用该函数
    if (unlikely(in_interrupt())) {
        panic("Aiee, killing interrupt handler!");
    }

    if (unlikely(!tsk->pid)) {
        panic("Attempted to kill the idle task!");
    }

    tracehook_report_exit(&code);
    validate_creds_for_do_exit(tsk);

    /*
     * We're taking recursive faults here in do_exit. Safest is to just
     * leave this task alone and wait for reboot.
     */
    if (unlikely(tsk->flags & PF_EXITING)) {
        printk(KERN_ALERT
               "Fixing recursive fault but reboot is needed!\n");

        //设置PF_EXITING:表示进程正在退出
        tsk->flags |= PF_EXITPIDONE;
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule();
    }

    exit_irq_thread();
    exit_signals(tsk);  /* sets PF_EXITING */
    /*
     * tsk->flags are checked in the futex code to protect against
     * an exiting task cleaning up the robust pi futexes.
     */
    smp_mb();
    spin_unlock_wait(&tsk->pi_lock);

    if (unlikely(in_atomic()))
        printk(KERN_INFO "note: %s[%d] exited with preempt_count %d\n",
               current->comm, task_pid_nr(current),
               preempt_count());

    acct_update_integrals(tsk);
    group_dead = atomic_dec_and_test(&tsk->signal->live);

    if (group_dead) {
        hrtimer_cancel(&tsk->signal->real_timer);
        exit_itimers(tsk->signal);

        if (tsk->mm) {
            setmax_mm_hiwater_rss(&tsk->signal->maxrss, tsk->mm);
        }
    }

    acct_collect(code, group_dead);

    if (group_dead) {
        tty_audit_exit();
    }

    if (unlikely(tsk->audit_context)) {
        audit_free(tsk);
    }

    tsk->exit_code = code;
    taskstats_exit(tsk, group_dead);
    //调用__exit_mm()函数放弃进程占用的mm_struct,如果没有别的进程使用它们即没被共享，就彻底释放它们
    exit_mm(tsk);

    if (group_dead) {
        acct_process();
    }

    trace_sched_process_exit(tsk);
    exit_sem(tsk);    //调用sem_exit()函数。如果进程排队等候IPC信号，它则离开队列
    //分别递减文件描述符，文件系统数据等的引用计数。当引用计数的值为0时，就代表没有进程在使用这些资源，此时就释放
    exit_files(tsk);
    exit_fs(tsk);
    check_stack_usage();
    exit_thread();
    cgroup_exit(tsk, 1);

    if (group_dead && tsk->signal->leader) {
        disassociate_ctty(1);
    }

    module_put(task_thread_info(tsk)->exec_domain->module);
    proc_exit_connector(tsk);
    /*
     * Flush inherited counters to the parent - before the parent
     * gets woken up by child-exit notifications.
     */
    perf_event_exit_task(tsk);
    //调用exit_notify()向父进程发送信号，将子进程的父进程重新设置为线程组中的其他线程或init进程，并把进程状态设为TASK_ZOMBIE.
    exit_notify(tsk, group_dead);
#ifdef CONFIG_NUMA
    mpol_put(tsk->mempolicy);
    tsk->mempolicy = NULL;
#endif
#ifdef CONFIG_FUTEX

    if (unlikely(current->pi_state_cache)) {
        kfree(current->pi_state_cache);
    }

#endif
    /*
     * Make sure we are holding no locks:
     */
    debug_check_no_locks_held(tsk);
    /*
     * We can do this unlocked here. The futex code uses this flag
     * just to verify whether the pi state cleanup has been done
     * or not. In the worst case it loops once more.
     */
    tsk->flags |= PF_EXITPIDONE;

    if (tsk->io_context) {
        exit_io_context();
    }

    if (tsk->splice_pipe) {
        __free_pipe_info(tsk->splice_pipe);
    }

    validate_creds_for_do_exit(tsk);
    preempt_disable();
    exit_rcu();
    /* causes final put_task_struct in finish_task_switch(). */
    tsk->state = TASK_DEAD;
    schedule();    //调用schedule()切换到其他进程
    BUG();

    /* Avoid "noreturn function does return".  */
    for (;;) {
        cpu_relax();    /* For when BUG is null */
    }
}
```

进程终结时所需的清理工作和进程描述符的删除被分开执行，这样尽管在调用了do_exit()之后，线程已经僵死不能允许情况下，系统还是保留了它的进程描述符。在父进程获得已经终结的子进程信息后，子进程的task_struct结构才被释放。Linux中有一系列wait()函数，这些函数都是基于系统调用wait4()实现的。它的动作就是挂起调用它的进程直到其中的一个子进程退出，此时函数会返回该退出子进程的PID。 最终释放进程描述符时，会调用release_task()。


```c
void release_task(struct task_struct* p)
{
    struct task_struct* leader;
    int zap_leader;
repeat:
    tracehook_prepare_release_task(p);
    /* don't need to get the RCU readlock here - the process is dead and
     * can't be modifying its own credentials */
    atomic_dec(&__task_cred(p)->user->processes);
    proc_flush_task(p);
    write_lock_irq(&tasklist_lock);
    tracehook_finish_release_task(p);
    __exit_signal(
        p);    //释放目前僵死进程所使用的所有剩余资源，并进行统计记录

    zap_leader = 0;
    leader = p->group_leader;

    //如果进程是线程组最后一个进程，并且领头进程已经死掉，那么就通知僵死的领头进程的父进程
    if (leader != p && thread_group_empty(leader) &&
        leader->exit_state == EXIT_ZOMBIE) {
        BUG_ON(task_detached(leader));
        do_notify_parent(leader, leader->exit_signal);
        zap_leader = task_detached(leader);

        if (zap_leader) {
            leader->exit_state = EXIT_DEAD;
        }
    }

    write_unlock_irq(&tasklist_lock);
    release_thread(p);
    call_rcu(&p->rcu, delayed_put_task_struct);
    p = leader;

    if (unlikely(zap_leader)) {
        goto repeat;
    }
}
```

子进程不一定能保证在父进程前边退出，所以必须要有机制来保证子进程在这种情况下能找到一个新的父进程。否则的话，这些成为孤儿的进程就会在退出时永远处于僵死状态，白白的耗费内存。解决这个问题的办法，就是给子进程在当前线程组内找一个线程作为父亲。一旦系统给进程成功地找到和设置了新的父进程，就不会再有出现驻留僵死进程的危险了，init进程会例行调用wait()来等待子进程，清除所有与其相关的僵死进程。

本节源码下载：
http://download.csdn.net/detail/gentleliu/8944331
