# （十）：内核同步


如同linux应用一样，内核的共享资源也要防止并发，因为如果多个执行线程同时访问和操作数据有可能发生各个线程之间相互覆盖共享数据的情况。

在linux只是单一处理器的时候，只有在中断发生或内核请求重新调度执行另一个任务时，数据才可能会并发访问。但自从内核开始支持对称多处理器之后，内核代码可以同时运行在多个处理器上，如果此时不加保护，运行在多个处理器上的代码完全可能在同一时刻并发访问共享数据。

一般把访问和操作共享数据的代码段称作临界区，为了避免在临界区中发生并发访问，程序员必须保证临界区代码原子执行，也就是要么全部执行，要么不执行。如果两个执行线程在同一个临界区同时执行，就发生了竞态（race conditions），避免并发防止竞态就是所谓的同步。

在linux内核中造成并发的原因主要有如下:


- 中断 -- 中断几乎可以在任何时刻异步发生，也就可能随时打断当前正在执行的代码。
- 软中断和tasklet -- 内核能在任何时刻唤醒或调度软中断和tasklet，打断当前正在执行的代码。
- 内核抢占 -- 因为内核具有抢占性，所以内核中的任务可能会被另一任务抢占。
- 睡眠及与用户空间的同步 -- 在内核执行的进程可能会睡眠，这就会唤醒调度程序，从而导致调度一个新的用户进程执行。
- 对称多处理 --  两个或多个处理器可以同时执行代码。（真并发）


通过锁可以防止并发执行，并且保护临界区不受竞态影响。任何执行线程要访问临界区代码时首先先获得锁，这样当后面另外的执行线程要访问临界区时就不能再获得该锁，这样临界区就禁止后来执行线程访问。linux自身实现了多种不同的锁机制，各种锁各有差别，区别主要在于当锁被争用时，有些会简单地执行等待，而有些锁会使当前任务睡眠直到锁可用为止。本节将会分析各锁的使用和实现。但是使用锁也会带来副作用，锁使得线程按串行方式对资源进行访问，所以使用锁无疑会降低系统性能；并且锁使用不当还会造成死锁。

下面来看一下linux下同步的方法，包括原子操作、自旋锁、信号量等方式。

`1.`原子操作，该操作是其它同步方法的基础，原子操作可以保证指令以原子的方式执行，执行过程不会被打断。linux内核提供了两组原子操作接口：原子整数操作和原子位操作。
针对整数的原子操作只能对atomic_t类型的数据进行处理。该类类型定义与文件`<include/linux/types.h>`:

```c
typedef struct {  
    volatile int counter;  
} atomic_t; 
```

下面举例说明原子操作的用法：
定义一个atomic_c类型的数据很简单，还可以定义时给它设定初值：

```sh
(1) atomic_t u;                     /*定义 u*/
(2) atomic_t v = ATOMIC_INIT(0)     /*定义 v 并把它初始化为0*/
```

对其操作：

```sh
(1) atomic_set(&v,4)                /* v = 4 ( 原子地)*/
(2) atomic_add(2,&v)                /* v = v + 2 = 6 (原子地) */
(3) atomic_inc(&v)                   /* v = v + 1 =7（原子地)*/
```

如果需要将atomic_t转换成int型，可以使用atomic_read()来完成：

```c
printk(“%d\n”,atomic_read(&v));    /* 会打印7*/
```

原子整数操作最常见的用途就是实现计数器。使用复杂的锁机制来保护一个单纯的计数器是很笨拙的，所以，开发者最好使用atomic_inc()和atomic_dec()这两个相对来说轻便一点的操作。

还可以用原子整数操作原子地执行一个操作并检查结果。一个常见的例子是原子的减操作和检查。

```c
int atomic_dec_and_test(atomic_t *v)
```
这个函数让给定的原子变量减1，如果结果为0，就返回1；否则返回0。特定体系结构的所有原子整数操作可以在文件`<arch/x86/include/asm/atomic.h>`中找到。如下：


```c
#ifndef _ASM_X86_ATOMIC_32_H  
#define _ASM_X86_ATOMIC_32_H  
  
#include <linux/compiler.h>  
#include <linux/types.h>  
#include <asm/processor.h>  
#include <asm/cmpxchg.h>  
  
  
#define ATOMIC_INIT(i)    { (i) }  
  
static inline int atomic_read(const atomic_t *v)  
{  
    return v->counter;  
}  
  
static inline void atomic_set(atomic_t *v, int i)  
{  
    v->counter = i;  
}  
  
static inline void atomic_add(int i, atomic_t *v)  
{  
    asm volatile(LOCK_PREFIX "addl %1,%0"  
             : "+m" (v->counter)  
             : "ir" (i));  
}  
  
static inline void atomic_sub(int i, atomic_t *v)  
{  
    asm volatile(LOCK_PREFIX "subl %1,%0"  
             : "+m" (v->counter)  
             : "ir" (i));  
}  
  
static inline int atomic_sub_and_test(int i, atomic_t *v)  
{  
    unsigned char c;  
  
    asm volatile(LOCK_PREFIX "subl %2,%0; sete %1"  
             : "+m" (v->counter), "=qm" (c)  
             : "ir" (i) : "memory");  
    return c;  
}  
```

除了原子整数之外，内核还提供了一组针对位操作的函数，这些操作也是和体系结构相关的。例如在x86下set_bit实现如下：static __always_inline void


```c

set_bit(unsigned int nr, volatile unsigned long *addr)  
{  
    if (IS_IMMEDIATE(nr)) {  
        asm volatile(LOCK_PREFIX "orb %1,%0"  
            : CONST_MASK_ADDR(nr, addr)  
            : "iq" ((u8)CONST_MASK(nr))  
            : "memory");  
    } else {  
        asm volatile(LOCK_PREFIX "bts %1,%0"  
            : BITOP_ADDR(addr) : "Ir" (nr) : "memory");  
    }  
} 
```

原子操作是对普通内存地址指针进行的操作，只要指针指向了任何你希望的数据，你就可以对它进行操作。原子位操作(以x86为例)相关函数定义在文件`<arch/x86/include/asm/bitops.h>`中。

`2.` 自旋锁

不是所有的临界区都是像增加或减少变量这么简单，有的时候临界区可能会跨越多个函数，这这时就需要使用更为复杂的同步方法——锁。linux内核中最常见的锁是自旋锁，自旋锁最多只能被一个可执行线程持有，如果一个可执行线程视图获取一个已经被持有的锁，那么该线程将会一直进行忙循环等待锁重新可用。在任意时候，自旋锁都可以防止多余一个执行线程同时进入临界区。由于自旋忙等过程是很费时间的，所以自旋锁不应该被长时间持有。

自旋锁相关方法如下：


<table width="700" cellpadding="2" cellspacing="0" border="1">
<tbody>
<tr>
<td valign="top" width="334" align="center"><span style="font-family:Microsoft YaHei; font-size:14px">方法</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spinlock中的定义</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">定义spin lock并初始化</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">DEFINE_SPINLOC</span><span style="font-family:Microsoft YaHei; font-size:14px">K</span><span style="font-family:Microsoft YaHei; font-size:14px">(</span><span style="font-family:Microsoft YaHei; font-size:14px">)</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">动态初始化spin lock</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_init</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">获取指定的spin lock</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">获取指定的spin lock同时disable本CPU中断</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_irq</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">保存本CPU当前的irq状态，disable本CPU中断并获取指定的spin lock</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_irqsave</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">获取指定的spin lock同时disable本CPU的bottom half</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_bh</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">释放指定的spin lock</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_unlock</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">释放指定的spin lock同时enable本CPU中断</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_irq</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">释放指定的spin lock同时恢复本CPU的中断状态</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_irqsave</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">获取指定的spin lock同时enable本CPU的bottom half</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_unlock_bh</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">尝试去获取spin lock，如果失败，不会spin，而是返回非零值</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_trylock</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">判断spin lock是否是locked，如果其他的thread已经获取了该lock，那么返回非零值，否则返回0</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_is_locked</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
</tbody>
</table>


自旋锁的实现和体系结构密切相关，代码通常通过汇编实现。与体系结构相关的部分定义在`<asm/spinlock.h>`,实际需要用到的接口定义在文件`<linux/spinlock.h>`中。一个实际的锁的类型为spinlock_t，定义在文件`<include/linux/spinlock_types.h>`中:

```c
typedef struct {  
    raw_spinlock_t raw_lock;  
#ifdef CONFIG_GENERIC_LOCKBREAK  
    unsigned int break_lock;  
#endif  
#ifdef CONFIG_DEBUG_SPINLOCK  
    unsigned int magic, owner_cpu;  
    void *owner;  
#endif  
#ifdef CONFIG_DEBUG_LOCK_ALLOC  
    struct lockdep_map dep_map;  
#endif  
} spinlock_t;  
```

自旋锁基本使用形式如下：

```c
DEFINE_SPINLOCK(lock);  
spin_lock(&lock);  
/* 临界区 */  
spin_unlock(&lock); 
```

实际上有 4 个函数可以加锁一个自旋锁:

```c
void spin_lock(spinlock_t *lock);
void spin_lock_irq(spinlock_t *lock);    //相当于spin_lock() + local_irq_disable()。
void spin_lock_irqsave(spinlock_t *lock, unsigned long flags);//禁止中断(只在本地处理器)在获得自旋锁之前; 之前的中断状态保存在 flags里。相当于spin_lock() + local_irq_save()。
void spin_lock_bh(spinlock_t *lock);    //获取锁之前禁止软件中断, 
```

但是硬件中断留作打开的，相当于spin_lock() + local_bh_disable()。
也有 4 个方法来释放一个自旋锁; 你用的那个必须对应你用来获取锁的函数.

```c
void spin_unlock(spinlock_t *lock);
void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);
void spin_unlock_irq(spinlock_t *lock);
void spin_unlock_bh(spinlock_t *lock);
```

下面看一下DEFINE_SPINLOCK()、spin_lock_init()、spin_lock()、spin_lock_irqsave()的实现：

```c
#define DEFINE_SPINLOCK(x)    spinlock_t x = __SPIN_LOCK_UNLOCKED(x)  
  
# define spin_lock_init(lock)                    \  
    do { *(lock) = SPIN_LOCK_UNLOCKED; } while (0)  
#endif  
  
spin_lock:  
#define spin_lock(lock)            _spin_lock(lock)  
void __lockfunc _spin_lock(spinlock_t *lock)  
{  
    __spin_lock(lock);  
}  
static inline void __spin_lock(spinlock_t *lock)  
{  
    preempt_disable();  
    spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);  
    LOCK_CONTENDED(lock, _raw_spin_trylock, _raw_spin_lock);  
}  
```

spin_lock_irqsave:

```c
#define spin_lock_irqsave(lock, flags)            \  
    do {                        \  
        typecheck(unsigned long, flags);    \  
        flags = _spin_lock_irqsave(lock);    \  
    } while (0)  
unsigned long __lockfunc _spin_lock_irqsave(spinlock_t *lock)  
{  
    return __spin_lock_irqsave(lock);  
}  
static inline unsigned long __spin_lock_irqsave(spinlock_t *lock)  
{  
    unsigned long flags;  
  
    local_irq_save(flags);    //spin_lock的实现没有禁止本地中断这一步  
    preempt_disable();  
    spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);  
  
#ifdef CONFIG_LOCKDEP  
    LOCK_CONTENDED(lock, _raw_spin_trylock, _raw_spin_lock);  
#else  
    _raw_spin_lock_flags(lock, &flags);  
#endif  
    return flags;  
}  
```

读写自旋锁一种比自旋锁粒度更小的锁机制,它保留了“自旋”的概念,但是在写操作方面,只能最多有1个写进程,在读操作方面,同时可以有多个读执行单元。当然,读和写也不能同时进行。读者写者锁有一个类型 rwlock_t, 在 `<linux/spinlokc.h>` 中定义. 它们可以以 2 种方式被声明和被初始化:
静态方式：

```c
rwlock_t my_rwlock = RW_LOCK_UNLOCKED;
```
动态方式：

```c
rwlock_t my_rwlock;
rwlock_init(&my_rwlock);
```
可用函数的列表现在应当看来相当类似。 对于读者, 有下列函数可用:


```c
void read_lock(rwlock_t *lock);
void read_lock_irqsave(rwlock_t *lock, unsigned long flags);
void read_lock_irq(rwlock_t *lock);
void read_lock_bh(rwlock_t *lock);
void read_unlock(rwlock_t *lock);
void read_unlock_irqrestore(rwlock_t *lock, unsigned long flags);
void read_unlock_irq(rwlock_t *lock);
void read_unlock_bh(rwlock_t *lock);
```

对于写存取的函数是类似的:

```c
void write_lock(rwlock_t *lock);
void write_lock_irqsave(rwlock_t *lock, unsigned long flags);
void write_lock_irq(rwlock_t *lock);
void write_lock_bh(rwlock_t *lock);
int write_trylock(rwlock_t *lock);
void write_unlock(rwlock_t *lock);
void write_unlock_irqrestore(rwlock_t *lock, unsigned long flags);
void write_unlock_irq(rwlock_t *lock);
void write_unlock_bh(rwlock_t *lock);
```

在与下半部配合使用时，锁机制必须要小心使用。由于下半部可以抢占进程上下文的代码，所以当下半部和进程上下文共享数据时，必须对进程上下文的共享数据进行保护，所以需要加锁的同时还要禁止下半部执行。同样的，由于中断处理器可以抢占下半部，所以如果中断处理器程序和下半部共享数据，那么就必须在获取恰当的锁的同时还要禁止中断。 同类的tasklet不可能同时运行，所以对于同类tasklet中的共享数据不需要保护，但是当数据被两个不同种类的tasklet共享时，就需要在访问下半部中的数据前先获得一个普通的自旋锁。由于同种类型的两个软中断也可以同时运行在一个系统的多个处理器上，所以被软中断共享的数据必须得到锁的保护。


`3.` 信号量


一个被占有的自旋锁使得请求它的线程循环等待而不会睡眠，这很浪费处理器时间，所以自旋锁使用段时间占有的情况。linux提供另外的同步方式可以在锁争用时让请求线程睡眠，直到锁重新可用时在唤醒它，这样处理器就不必循环等待，可以去执行其它代码。这种方式就是即将讨论的信号量。

信号量是一种睡眠锁，如果有一个任务试图获得一个已经被占用的信号量时，信号量会将其放入一个等待队列，然后睡眠。当持有的信号量被释放后，处于等待队列中的那个任务将被唤醒，并获得信号量。信号量比自旋锁提供了更好的处理器利用率，因为没有把时间花费在忙等带上。但是信号量也会有一定的开销，被阻塞的线程换入换出有两次明显的上下文切换，这样的开销比自旋锁要大的多。

如果需要在自旋锁和信号量中做出选择，应该根据锁被持有的时间长短做判断，如果加锁时间不长并且代码不会休眠，利用自旋锁是最佳选择。相反，如果加锁时间可能很长或者代码在持有锁有可能睡眠，那么最好使用信号量来完成加锁功能。信号量一个有用特性就是它可以同时允许任意数量的锁持有者，而自旋锁在一个时刻最多允许一个任务持有它。信号量同时允许的持有者数量可以在声明信号量时指定，当为1时，成为互斥信号量，否则成为计数信号量。

信号量的实现与体系结构相关，信号量使用struct semaphore类型用来表示信号量，定义于文件`<include/linux/semaphore.h>`中:

```c
struct semaphore {  
     spinlock_t        lock;  
     unsigned int        count;  
     struct list_head    wait_list;  
};  
```

信号量初始化方法有如下:
方法一:

```c
struct semaphore sem;    
void sema_init(struct semaphore *sem, int val);//初始化信号量,并设置信号量 sem 的值为 val。  ```
```c
static inline void sema_init(struct semaphore *sem, int val)  
{  
    static struct lock_class_key __key;  
    *sem = (struct semaphore) __SEMAPHORE_INITIALIZER(*sem, val);  
    lockdep_init_map(&sem->lock.dep_map, "semaphore->lock", &__key, 0);  
}  
```

方法二:
```c
DECLARE_MUTEX(name);  
````

定义一个名为 name 的信号量并初始化为1。
其实现为:

```c
#define DECLARE_MUTEX(name)    \  
    struct semaphore name = __SEMAPHORE_INITIALIZER(name, 1)  
```
方法三:

```c
#define init_MUTEX(sem)        sema_init(sem, 1)    //以不加锁状态动态创建信号量  
#define init_MUTEX_LOCKED(sem)    sema_init(sem, 0)    //以加锁状态动态创建信号量   
```

信号量初始化后就可以使用了，使用信号量主要有如下方法:


```c
void down(struct semaphore * sem);//该函数用于获得信号量 sem，它会导致睡眠，因此不能在中断上下文使用;    
int down_interruptible(struct semaphore * sem);//该函数功能与 down 类似，不同之处为，因为 down()而进入睡眠状态的进程不能被信号打断，但因为 down_interruptible()而进入睡眠状态的进程能被信号打断，信号也会导致该函数返回，这时候函数的返回值非 0;    
int down_trylock(struct semaphore * sem);//该函数尝试获得信号量 sem，如果能够立刻获得，它就获得该信号量并返回0，否则,返回非0值。它不会导致调用者睡眠，可以在中断上下文使用。  
up(struct semaphore * sem); //释放指定信号量，如果睡眠队列不空，则唤醒其中一个队列。  
信号量一般这样使用：
```

信号量一般这样使用：


```c
/* 定义信号量    
DECLARE_MUTEX(mount_sem);    
//down(&mount_sem);/* 获取信号量，保护临界区，信号量被占用之后进入不可中断睡眠状态  
down_interruptible(&mount_sem);/* 获取信号量，保护临界区，信号量被占用之后进入不可中断睡眠状态  
. . .    
critical section /* 临界区    
. . .    
up(&mount_sem);/* 释放信号量   
```

下面看一下这些函数的实现：
down():


```c
void down(struct semaphore *sem)  
{  
    unsigned long flags;  
  
    spin_lock_irqsave(&sem->lock, flags);  
    if (likely(sem->count > 0))  
        sem->count--;  
    else  
        __down(sem);  
    spin_unlock_irqrestore(&sem->lock, flags);  
}  

static noinline void __sched __down(struct semaphore *sem)  
{  
    __down_common(sem, TASK_UNINTERRUPTIBLE, MAX_SCHEDULE_TIMEOUT);  
}  
```

down_interruptible():

```c
int down_interruptible(struct semaphore *sem)  
{  
    unsigned long flags;  
    int result = 0;  
  
    spin_lock_irqsave(&sem->lock, flags);  
    if (likely(sem->count > 0))  
        sem->count--;  
    else  
        result = __down_interruptible(sem);  
    spin_unlock_irqrestore(&sem->lock, flags);  
  
    return result;  
}  

static noinline int __sched __down_interruptible(struct semaphore *sem)  
{  
    return __down_common(sem, TASK_INTERRUPTIBLE, MAX_SCHEDULE_TIMEOUT);  
}  
```

down_trylock():

```c
int down_trylock(struct semaphore *sem)  
{  
    unsigned long flags;  
    int count;  
  
    spin_lock_irqsave(&sem->lock, flags);  
    count = sem->count - 1;  
    if (likely(count >= 0))  
        sem->count = count;  
    spin_unlock_irqrestore(&sem->lock, flags);  
  
    return (count < 0);  
}  
```

up():

```c
void up(struct semaphore *sem)  
{  
    unsigned long flags;  
  
    spin_lock_irqsave(&sem->lock, flags);  
    if (likely(list_empty(&sem->wait_list)))  
        sem->count++;  
    else  
        __up(sem);  
    spin_unlock_irqrestore(&sem->lock, flags);  
}  

static noinline void __sched __up(struct semaphore *sem)  
{  
    struct semaphore_waiter *waiter = list_first_entry(&sem->wait_list,  
                        struct semaphore_waiter, list);  
    list_del(&waiter->list);  
    waiter->up = 1;  
    wake_up_process(waiter->task);  
}  
```

正如自旋锁一样，信号量也有区分读写访问的可能，读写信号量在内核中使用rw_semaphore结构表示，x86体系结构定义在`<arch/x86/include/asm/rwsem.h>`文件中：

```c
struct rw_semaphore {  
    signed long        count;  
    spinlock_t        wait_lock;  
    struct list_head    wait_list;  
#ifdef CONFIG_DEBUG_LOCK_ALLOC  
    struct lockdep_map dep_map;  
#endif  
};  
```
读写信号量的使用方法和信号量类似，其操作函数有如下:

```c
DECLARE_RWSEM(name)             //声明名为name的读写信号量，并初始化它。  
void init_rwsem(struct rw_semaphore *sem);  //对读写信号量sem进行初始化。  
void down_read(struct rw_semaphore *sem);   //读者用来获取sem，若没获得时，则调用者睡眠等待。  
void up_read(struct rw_semaphore *sem);     //读者释放sem。  
int down_read_trylock(struct rw_semaphore *sem); //读者尝试获取sem，如果获得返回1，如果没有获得返回0。可在中断上下文使用。  
void down_write(struct rw_semaphore *sem);  //写者用来获取sem，若没获得时，则调用者睡眠等待。  
int down_write_trylock(struct rw_semaphore *sem);   //写者尝试获取sem，如果获得返回1，如果没有获得返回0。可在中断上下文使用  
void up_write(struct rw_semaphore *sem);    //写者释放sem。  
void downgrade_write(struct rw_semaphore *sem); //把写者降级为读者。  
```

`4.` 互斥体
除了信号量之外，内核拥有一个更简单的且可睡眠的锁，那就是互斥体。互斥体的行为和计数是1的信号量类似，其接口简单，实现更高效。 互斥体在内核中使用mutex表示，定义于`<include/linux/mutex.h>`：

```c
struct mutex {  
    /* 1: unlocked, 0: locked, negative: locked, possible waiters */  
    atomic_t        count;  
    spinlock_t        wait_lock;  
    struct list_head    wait_list;  
    ......  
};  
```

静态定义mutex：

```c
DEFINE_MUTEX(name);  
```
实现如下：
 
```c
#define DEFINE_MUTEX(mutexname) \  
    struct mutex mutexname = __MUTEX_INITIALIZER(mutexname)  
  
#define __MUTEX_INITIALIZER(lockname) \  
        { .count = ATOMIC_INIT(1) \  
        , .wait_lock = __SPIN_LOCK_UNLOCKED(lockname.wait_lock) \  
        , .wait_list = LIST_HEAD_INIT(lockname.wait_list) \  
        __DEBUG_MUTEX_INITIALIZER(lockname) \  
        __DEP_MAP_MUTEX_INITIALIZER(lockname) }  
```
动态定义mutex：

```c
mutex_init(&mutex);  
```

```c
# define mutex_init(mutex) \  
do {                            \  
    static struct lock_class_key __key;        \  
                            \  
    __mutex_init((mutex), #mutex, &__key);        \  
} while (0)  
  
void  
__mutex_init(struct mutex *lock, const char *name, struct lock_class_key *key)  
{  
    atomic_set(&lock->count, 1);  
    spin_lock_init(&lock->wait_lock);  
    INIT_LIST_HEAD(&lock->wait_list);  
    mutex_clear_owner(lock);  
  
    debug_mutex_init(lock, name, key);  
}  
```
锁定和解锁如下：

```c
mutex_lock(&mutex);  
/* 临界区 */  
mutex_unlock(&mutex);  
```

```c
void __sched mutex_lock(struct mutex *lock)  
{  
    might_sleep();  
    /* 
     * The locking fastpath is the 1->0 transition from 
     * 'unlocked' into 'locked' state. 
     */  
    __mutex_fastpath_lock(&lock->count, __mutex_lock_slowpath);  
    mutex_set_owner(lock);  
}  
  
void __sched mutex_unlock(struct mutex *lock)  
{  
    /* 
     * The unlocking fastpath is the 0->1 transition from 'locked' 
     * into 'unlocked' state: 
     */  
#ifndef CONFIG_DEBUG_MUTEXES  
    /* 
     * When debugging is enabled we must not clear the owner before time, 
     * the slow path will always be taken, and that clears the owner field 
     * after verifying that it was indeed current. 
     */  
    mutex_clear_owner(lock);  
#endif  
    __mutex_fastpath_unlock(&lock->count, __mutex_unlock_slowpath);  
}  
```

其他mutex方法:


```c
int mutex_trylock(struct mutex *);    //视图获取指定互斥体，成功返回1；否则返回0。  
int mutex_is_locked(struct mutex *lock);    //判断锁是否被占用，是返回1，否则返回0。  
```

使用mutex时，要注意一下：
mutex的使用技术永远是1；在同一上下文中上锁解锁；当进程持有一个mutex时，进程不可退出；mutex不能在中断或下半部中使用。

`5.` 抢占禁止

在前面章节讲进程管理的时候听到过内核抢占，由于内核可抢占，内核中的进程随时都可能被另外一个具有更高优先权的进程打断，这也就意味着一个任务与被抢占的任务可能会在同一个临界区运行。所以才有本节前面自旋锁来避免竞态的发生，自旋锁有禁止内核抢占的功能。但像每cpu变量的数据只能被一个处理器访问，可以不需要使用锁来保护，如果没有使用锁，内核又是抢占式的，那么新调度的任务就可能访问同一个变量。这个时候就可以通过禁止内核抢占来避免竞态的发生，禁止内核抢占使用preetmpt_disable()函数，这是一个可以嵌套调用的函数，可以使用任意次。每次调用都必须有一个相应的preempt_enable()调用，当最后一次preempt_enable()被调用时，内核抢占才重新启用。内核抢占相关函数如下：

```c
preempt_enable() //内核抢占计数preempt_count减1  
preempt_disable() //内核抢占计数preempt_count加1，当该值降为0时检查和执行被挂起的需要调度的任务  
preempt_enable_no_resched()//内核抢占计数preempt_count减1，但不立即抢占式调度  
preempt_check_resched () //如果必要进行调度  
preempt_count() //返回抢占计数  
preempt_schedule() //内核抢占时的调度程序的入口点 
```






