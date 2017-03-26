# （八）：進程管理分析


進程其實就是程序的執行時的實例，是處於執行期的程序。在linux內核中，進程列表被存放在一個雙向循環鏈表中，鏈表中每一項都是類型為task_struct的結構，該結構稱作進程描述符，進程描述符包含一個具體進程的所有信息，這個結構就是我們在操作系統中所說的PCB（Process Control Block）。該結構定義於`<include/linux/sched.h>`文件中：


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

該結構體中包含的數據可以完整的描述一個正在執行的程序：打開的文件、進程的地址空間、掛起的信號、進程的狀態、以及其他很多信息。

在系統運行過程中，進程頻繁切換，所以我們需要一種方式能夠快速獲得當前進程的task_struct，於是進程內核堆棧底部存放著struct thread_info。該結構中有一個成員指向當前進程的task_struct。在x86上，struct thread_info在文件`<arch/x86/include/asm/thread_info.h>`中定義：

```c
struct thread_info {
    struct task_struct*
        task;      /* 該指針存放的是指向該任務實際task_struct的指針 */
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

使用current宏就可以獲得當前進程的進程描述符。

每一個進程都有一個父進程，每個進程管理自己的子進程。每個進程都是init進程的子進程，init進程在內 核系統啟動的最後階段啟動init進程，該進程讀取系統的初始化腳本並執行其他相關程序，最終完成系統啟動的整個過程。每個進程有0個或多個子進程，進程間的關係存放在進程描述符中。task_struct中有一個parent的指針，指向其父進程；還有個children的指針指向其子進程的鏈表。所以，對於當前進程，可以通過current宏來獲得父進程和子進程的進程描述符。

下面程序打印當前進程、父進程信息和所有子進程信息：


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

    //獲取當前進程信息
    cur = current;

    printk(KERN_ALERT"Current: %s[%d]\n",
           cur->comm, cur->pid);

    //獲取父進程信息
    parent = current->parent;
    printk(KERN_ALERT"Parent: %s[%d]\n",
           parent->comm, parent->pid);

    //獲取所有祖先進程信息
    for (task = cur; task != &init_task; task = task->parent) {
        printk(KERN_ALERT"ancestor: %s[%d]\n",
               task->comm, task->pid);
    }

    //獲取所有子進程信息
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

執行結果如下：

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

linux操作系統提供產生進程的機制，在Linux下的fork()使用寫時拷貝(copy-on-write)頁實現。這種技術原理是：內存並不複製整個進程地址空間，而是讓父進程和子進程共享同一拷貝，只有在需要寫入的時候，數據才會被複制。也就是資源的複製只是發生在需要寫入的時候才進行，在此之前都是以只讀的方式共享。

linux通過clone()系統調用實現fork()，然後clone()去調用do_fork()，do_fork()完成創建中大部分工作。庫函數vfork()、__clone()都根據各自需要的參數標誌去調用clone()。fork()的實際開銷就是複製父進程的頁表以及給子進程創建唯一的進程描述符。

用戶空間的fork()經過系統調用進入內核，在內核中對應的處理函數為sys_fork()，定義於`<arch/x86/kernel/process.c>`文件中。


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

上面執行完以後，回到do_fork()函數，如果copy_process()函數成功返回。新創建的子進程被喚醒並讓其投入運行。內核有意選擇子進程先運行。因為一般子進程都會馬上調用exec()函數，這樣可以避免寫時拷貝的額外開銷。如果父進程首先執行的話，有可能會開始向地址空間寫入。

線程機制提供了在同一程序內共享內存地址空間運行的一組線程。線程機制支持併發程序設計技術，可以共享打開的文件和其他資源。如果你的系統是多核心的，那多線程技術可保證系統的真正並行。在linux中，並沒有線程這個概念，linux中所有的線程都當作進程來處理，換句話說就是在內核中並沒有什麼特殊的結構和算法來表示線程。在linux中，線程僅僅是一個使用共享資源的進程。每個線程都擁有一個隸屬於自己的task_struct。所以說線程本質上還是進程，只不過該進程可以和其他一些進程共享某些資源信息。

內核有時需要在後臺執行一些操作，這種任務可以通過內核線程完成，內核線程獨立運行在內核空間的標準進程。內核線程和普通的進程間的區別在於內核線程沒有獨立的地址空間。它們只在訥河空間運行，從來不切換到用戶空間去。內核進程和普通進程一樣，可以被調度，也可以被搶佔。內核線程也只能由其它內核線程創建，內核是通過從kthreadd內核進程中衍生出所有新的內核線程來自動處理這一點的。在內核中創建一個的內核線程方法如下：


```c
struct task_struct *kthread_create(int (*threadfn)(void *data),  
                   void *data,                                                            
                   const char namefmt[], ...)  
```
該函數實現如下：

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

新的任務是由kthread內核進程通過clone()系統調用而創建的。新的進程將運行threadfn函數，給其傳遞參數data，新的進程名稱為namefmt，新創建的進程處於不可運行狀態，需要調用wake_up_process()明確的喚醒它，否則它不會主動運行。也可以通過調用kthread_run()來創建一個進程並讓它運行起來。


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
kthread_run其實就是創建了一個內核線程並且喚醒了。內核線程啟動後就一直運行直到調用do_exit()退出或者內核的其他部分調用kthread_stop()退出。

```c
int kthread_stop(struct task_struct *k); 
```

下面為一個使用內核線程的示例：

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

既然有進程的創建，那就有進程的終結，終結時內核必須釋放它所佔有的資源。內核終結時，大部分任務都是靠do_exit()（定義於）來完成。


```c
NORET_TYPE void do_exit(long code)
{
    struct task_struct* tsk = current;
    int group_dead;
    profile_task_exit(tsk);
    WARN_ON(atomic_read(&tsk->fs_excl));

    //不可在中斷上下文中使用該函數
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

        //設置PF_EXITING:表示進程正在退出
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
    //調用__exit_mm()函數放棄進程佔用的mm_struct,如果沒有別的進程使用它們即沒被共享，就徹底釋放它們
    exit_mm(tsk);

    if (group_dead) {
        acct_process();
    }

    trace_sched_process_exit(tsk);
    exit_sem(tsk);    //調用sem_exit()函數。如果進程排隊等候IPC信號，它則離開隊列
    //分別遞減文件描述符，文件系統數據等的引用計數。當引用計數的值為0時，就代表沒有進程在使用這些資源，此時就釋放
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
    //調用exit_notify()向父進程發送信號，將子進程的父進程重新設置為線程組中的其他線程或init進程，並把進程狀態設為TASK_ZOMBIE.
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
    schedule();    //調用schedule()切換到其他進程
    BUG();

    /* Avoid "noreturn function does return".  */
    for (;;) {
        cpu_relax();    /* For when BUG is null */
    }
}
```

進程終結時所需的清理工作和進程描述符的刪除被分開執行，這樣儘管在調用了do_exit()之後，線程已經僵死不能允許情況下，系統還是保留了它的進程描述符。在父進程獲得已經終結的子進程信息後，子進程的task_struct結構才被釋放。Linux中有一系列wait()函數，這些函數都是基於系統調用wait4()實現的。它的動作就是掛起調用它的進程直到其中的一個子進程退出，此時函數會返回該退出子進程的PID。 最終釋放進程描述符時，會調用release_task()。


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
        p);    //釋放目前僵死進程所使用的所有剩餘資源，並進行統計記錄

    zap_leader = 0;
    leader = p->group_leader;

    //如果進程是線程組最後一個進程，並且領頭進程已經死掉，那麼就通知僵死的領頭進程的父進程
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

子進程不一定能保證在父進程前邊退出，所以必須要有機制來保證子進程在這種情況下能找到一個新的父進程。否則的話，這些成為孤兒的進程就會在退出時永遠處於僵死狀態，白白的耗費內存。解決這個問題的辦法，就是給子進程在當前線程組內找一個線程作為父親。一旦系統給進程成功地找到和設置了新的父進程，就不會再有出現駐留僵死進程的危險了，init進程會例行調用wait()來等待子進程，清除所有與其相關的僵死進程。

本節源碼下載：
http://download.csdn.net/detail/gentleliu/8944331
