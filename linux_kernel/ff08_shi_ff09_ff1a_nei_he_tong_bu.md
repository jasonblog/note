# （十）：內核同步


如同linux應用一樣，內核的共享資源也要防止併發，因為如果多個執行線程同時訪問和操作數據有可能發生各個線程之間相互覆蓋共享數據的情況。

在linux只是單一處理器的時候，只有在中斷髮生或內核請求重新調度執行另一個任務時，數據才可能會併發訪問。但自從內核開始支持對稱多處理器之後，內核代碼可以同時運行在多個處理器上，如果此時不加保護，運行在多個處理器上的代碼完全可能在同一時刻併發訪問共享數據。

一般把訪問和操作共享數據的代碼段稱作臨界區，為了避免在臨界區中發生併發訪問，程序員必須保證臨界區代碼原子執行，也就是要麼全部執行，要麼不執行。如果兩個執行線程在同一個臨界區同時執行，就發生了競態（race conditions），避免併發防止競態就是所謂的同步。

在linux內核中造成併發的原因主要有如下:


- 中斷 -- 中斷幾乎可以在任何時刻異步發生，也就可能隨時打斷當前正在執行的代碼。
- 軟中斷和tasklet -- 內核能在任何時刻喚醒或調度軟中斷和tasklet，打斷當前正在執行的代碼。
- 內核搶佔 -- 因為內核具有搶佔性，所以內核中的任務可能會被另一任務搶佔。
- 睡眠及與用戶空間的同步 -- 在內核執行的進程可能會睡眠，這就會喚醒調度程序，從而導致調度一個新的用戶進程執行。
- 對稱多處理 --  兩個或多個處理器可以同時執行代碼。（真併發）


通過鎖可以防止併發執行，並且保護臨界區不受競態影響。任何執行線程要訪問臨界區代碼時首先先獲得鎖，這樣當後面另外的執行線程要訪問臨界區時就不能再獲得該鎖，這樣臨界區就禁止後來執行線程訪問。linux自身實現了多種不同的鎖機制，各種鎖各有差別，區別主要在於當鎖被爭用時，有些會簡單地執行等待，而有些鎖會使當前任務睡眠直到鎖可用為止。本節將會分析各鎖的使用和實現。但是使用鎖也會帶來副作用，鎖使得線程按串行方式對資源進行訪問，所以使用鎖無疑會降低系統性能；並且鎖使用不當還會造成死鎖。

下面來看一下linux下同步的方法，包括原子操作、自旋鎖、信號量等方式。

`1.`原子操作，該操作是其它同步方法的基礎，原子操作可以保證指令以原子的方式執行，執行過程不會被打斷。linux內核提供了兩組原子操作接口：原子整數操作和原子位操作。
針對整數的原子操作只能對atomic_t類型的數據進行處理。該類類型定義與文件`<include/linux/types.h>`:

```c
typedef struct {  
    volatile int counter;  
} atomic_t; 
```

下面舉例說明原子操作的用法：
定義一個atomic_c類型的數據很簡單，還可以定義時給它設定初值：

```sh
(1) atomic_t u;                     /*定義 u*/
(2) atomic_t v = ATOMIC_INIT(0)     /*定義 v 並把它初始化為0*/
```

對其操作：

```sh
(1) atomic_set(&v,4)                /* v = 4 ( 原子地)*/
(2) atomic_add(2,&v)                /* v = v + 2 = 6 (原子地) */
(3) atomic_inc(&v)                   /* v = v + 1 =7（原子地)*/
```

如果需要將atomic_t轉換成int型，可以使用atomic_read()來完成：

```c
printk(“%d\n”,atomic_read(&v));    /* 會打印7*/
```

原子整數操作最常見的用途就是實現計數器。使用複雜的鎖機制來保護一個單純的計數器是很笨拙的，所以，開發者最好使用atomic_inc()和atomic_dec()這兩個相對來說輕便一點的操作。

還可以用原子整數操作原子地執行一個操作並檢查結果。一個常見的例子是原子的減操作和檢查。

```c
int atomic_dec_and_test(atomic_t *v)
```
這個函數讓給定的原子變量減1，如果結果為0，就返回1；否則返回0。特定體系結構的所有原子整數操作可以在文件`<arch/x86/include/asm/atomic.h>`中找到。如下：


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

除了原子整數之外，內核還提供了一組針對位操作的函數，這些操作也是和體系結構相關的。例如在x86下set_bit實現如下：static __always_inline void


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

原子操作是對普通內存地址指針進行的操作，只要指針指向了任何你希望的數據，你就可以對它進行操作。原子位操作(以x86為例)相關函數定義在文件`<arch/x86/include/asm/bitops.h>`中。

`2.` 自旋鎖

不是所有的臨界區都是像增加或減少變量這麼簡單，有的時候臨界區可能會跨越多個函數，這這時就需要使用更為複雜的同步方法——鎖。linux內核中最常見的鎖是自旋鎖，自旋鎖最多隻能被一個可執行線程持有，如果一個可執行線程視圖獲取一個已經被持有的鎖，那麼該線程將會一直進行忙循環等待鎖重新可用。在任意時候，自旋鎖都可以防止多餘一個執行線程同時進入臨界區。由於自旋忙等過程是很費時間的，所以自旋鎖不應該被長時間持有。

自旋鎖相關方法如下：


<table width="700" cellpadding="2" cellspacing="0" border="1">
<tbody>
<tr>
<td valign="top" width="334" align="center"><span style="font-family:Microsoft YaHei; font-size:14px">方法</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spinlock中的定義</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">定義spin lock並初始化</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">DEFINE_SPINLOC</span><span style="font-family:Microsoft YaHei; font-size:14px">K</span><span style="font-family:Microsoft YaHei; font-size:14px">(</span><span style="font-family:Microsoft YaHei; font-size:14px">)</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">動態初始化spin lock</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_init</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">獲取指定的spin lock</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">獲取指定的spin lock同時disable本CPU中斷</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_irq</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">保存本CPU當前的irq狀態，disable本CPU中斷並獲取指定的spin lock</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_irqsave</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">獲取指定的spin lock同時disable本CPU的bottom half</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_bh</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">釋放指定的spin lock</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_unlock</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">釋放指定的spin lock同時enable本CPU中斷</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_irq</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">釋放指定的spin lock同時恢復本CPU的中斷狀態</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_lock_irqsave</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">獲取指定的spin lock同時enable本CPU的bottom half</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_unlock_bh</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">嘗試去獲取spin lock，如果失敗，不會spin，而是返回非零值</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_trylock</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
<tr>
<td valign="top" width="334"><span style="font-family:Microsoft YaHei; font-size:14px">判斷spin lock是否是locked，如果其他的thread已經獲取了該lock，那麼返回非零值，否則返回0</span></td>
<td valign="top" width="175"><span style="font-family:Microsoft YaHei; font-size:14px">spin_is_locked</span><span style="font-family:Microsoft YaHei; font-size:14px">()</span></td>
</tr>
</tbody>
</table>


自旋鎖的實現和體系結構密切相關，代碼通常通過彙編實現。與體系結構相關的部分定義在`<asm/spinlock.h>`,實際需要用到的接口定義在文件`<linux/spinlock.h>`中。一個實際的鎖的類型為spinlock_t，定義在文件`<include/linux/spinlock_types.h>`中:

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

自旋鎖基本使用形式如下：

```c
DEFINE_SPINLOCK(lock);  
spin_lock(&lock);  
/* 臨界區 */  
spin_unlock(&lock); 
```

實際上有 4 個函數可以加鎖一個自旋鎖:

```c
void spin_lock(spinlock_t *lock);
void spin_lock_irq(spinlock_t *lock);    //相當於spin_lock() + local_irq_disable()。
void spin_lock_irqsave(spinlock_t *lock, unsigned long flags);//禁止中斷(只在本地處理器)在獲得自旋鎖之前; 之前的中斷狀態保存在 flags裡。相當於spin_lock() + local_irq_save()。
void spin_lock_bh(spinlock_t *lock);    //獲取鎖之前禁止軟件中斷, 
```

但是硬件中斷留作打開的，相當於spin_lock() + local_bh_disable()。
也有 4 個方法來釋放一個自旋鎖; 你用的那個必須對應你用來獲取鎖的函數.

```c
void spin_unlock(spinlock_t *lock);
void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);
void spin_unlock_irq(spinlock_t *lock);
void spin_unlock_bh(spinlock_t *lock);
```

下面看一下DEFINE_SPINLOCK()、spin_lock_init()、spin_lock()、spin_lock_irqsave()的實現：

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
  
    local_irq_save(flags);    //spin_lock的實現沒有禁止本地中斷這一步  
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

讀寫自旋鎖一種比自旋鎖粒度更小的鎖機制,它保留了“自旋”的概念,但是在寫操作方面,只能最多有1個寫進程,在讀操作方面,同時可以有多個讀執行單元。當然,讀和寫也不能同時進行。讀者寫者鎖有一個類型 rwlock_t, 在 `<linux/spinlokc.h>` 中定義. 它們可以以 2 種方式被聲明和被初始化:
靜態方式：

```c
rwlock_t my_rwlock = RW_LOCK_UNLOCKED;
```
動態方式：

```c
rwlock_t my_rwlock;
rwlock_init(&my_rwlock);
```
可用函數的列表現在應當看來相當類似。 對於讀者, 有下列函數可用:


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

對於寫存取的函數是類似的:

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

在與下半部配合使用時，鎖機制必須要小心使用。由於下半部可以搶佔進程上下文的代碼，所以當下半部和進程上下文共享數據時，必須對進程上下文的共享數據進行保護，所以需要加鎖的同時還要禁止下半部執行。同樣的，由於中斷處理器可以搶佔下半部，所以如果中斷處理器程序和下半部共享數據，那麼就必須在獲取恰當的鎖的同時還要禁止中斷。 同類的tasklet不可能同時運行，所以對於同類tasklet中的共享數據不需要保護，但是當數據被兩個不同種類的tasklet共享時，就需要在訪問下半部中的數據前先獲得一個普通的自旋鎖。由於同種類型的兩個軟中斷也可以同時運行在一個系統的多個處理器上，所以被軟中斷共享的數據必須得到鎖的保護。


`3.` 信號量


一個被佔有的自旋鎖使得請求它的線程循環等待而不會睡眠，這很浪費處理器時間，所以自旋鎖使用段時間佔有的情況。linux提供另外的同步方式可以在鎖爭用時讓請求線程睡眠，直到鎖重新可用時在喚醒它，這樣處理器就不必循環等待，可以去執行其它代碼。這種方式就是即將討論的信號量。

信號量是一種睡眠鎖，如果有一個任務試圖獲得一個已經被佔用的信號量時，信號量會將其放入一個等待隊列，然後睡眠。當持有的信號量被釋放後，處於等待隊列中的那個任務將被喚醒，並獲得信號量。信號量比自旋鎖提供了更好的處理器利用率，因為沒有把時間花費在忙等帶上。但是信號量也會有一定的開銷，被阻塞的線程換入換出有兩次明顯的上下文切換，這樣的開銷比自旋鎖要大的多。

如果需要在自旋鎖和信號量中做出選擇，應該根據鎖被持有的時間長短做判斷，如果加鎖時間不長並且代碼不會休眠，利用自旋鎖是最佳選擇。相反，如果加鎖時間可能很長或者代碼在持有鎖有可能睡眠，那麼最好使用信號量來完成加鎖功能。信號量一個有用特性就是它可以同時允許任意數量的鎖持有者，而自旋鎖在一個時刻最多允許一個任務持有它。信號量同時允許的持有者數量可以在聲明信號量時指定，當為1時，成為互斥信號量，否則成為計數信號量。

信號量的實現與體系結構相關，信號量使用struct semaphore類型用來表示信號量，定義於文件`<include/linux/semaphore.h>`中:

```c
struct semaphore {  
     spinlock_t        lock;  
     unsigned int        count;  
     struct list_head    wait_list;  
};  
```

信號量初始化方法有如下:
方法一:

```c
struct semaphore sem;    
void sema_init(struct semaphore *sem, int val);//初始化信號量,並設置信號量 sem 的值為 val。  ```
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

定義一個名為 name 的信號量並初始化為1。
其實現為:

```c
#define DECLARE_MUTEX(name)    \  
    struct semaphore name = __SEMAPHORE_INITIALIZER(name, 1)  
```
方法三:

```c
#define init_MUTEX(sem)        sema_init(sem, 1)    //以不加鎖狀態動態創建信號量  
#define init_MUTEX_LOCKED(sem)    sema_init(sem, 0)    //以加鎖狀態動態創建信號量   
```

信號量初始化後就可以使用了，使用信號量主要有如下方法:


```c
void down(struct semaphore * sem);//該函數用於獲得信號量 sem，它會導致睡眠，因此不能在中斷上下文使用;    
int down_interruptible(struct semaphore * sem);//該函數功能與 down 類似，不同之處為，因為 down()而進入睡眠狀態的進程不能被信號打斷，但因為 down_interruptible()而進入睡眠狀態的進程能被信號打斷，信號也會導致該函數返回，這時候函數的返回值非 0;    
int down_trylock(struct semaphore * sem);//該函數嘗試獲得信號量 sem，如果能夠立刻獲得，它就獲得該信號量並返回0，否則,返回非0值。它不會導致調用者睡眠，可以在中斷上下文使用。  
up(struct semaphore * sem); //釋放指定信號量，如果睡眠隊列不空，則喚醒其中一個隊列。  
信號量一般這樣使用：
```

信號量一般這樣使用：


```c
/* 定義信號量    
DECLARE_MUTEX(mount_sem);    
//down(&mount_sem);/* 獲取信號量，保護臨界區，信號量被佔用之後進入不可中斷睡眠狀態  
down_interruptible(&mount_sem);/* 獲取信號量，保護臨界區，信號量被佔用之後進入不可中斷睡眠狀態  
. . .    
critical section /* 臨界區    
. . .    
up(&mount_sem);/* 釋放信號量   
```

下面看一下這些函數的實現：
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

正如自旋鎖一樣，信號量也有區分讀寫訪問的可能，讀寫信號量在內核中使用rw_semaphore結構表示，x86體系結構定義在`<arch/x86/include/asm/rwsem.h>`文件中：

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
讀寫信號量的使用方法和信號量類似，其操作函數有如下:

```c
DECLARE_RWSEM(name)             //聲明名為name的讀寫信號量，並初始化它。  
void init_rwsem(struct rw_semaphore *sem);  //對讀寫信號量sem進行初始化。  
void down_read(struct rw_semaphore *sem);   //讀者用來獲取sem，若沒獲得時，則調用者睡眠等待。  
void up_read(struct rw_semaphore *sem);     //讀者釋放sem。  
int down_read_trylock(struct rw_semaphore *sem); //讀者嘗試獲取sem，如果獲得返回1，如果沒有獲得返回0。可在中斷上下文使用。  
void down_write(struct rw_semaphore *sem);  //寫者用來獲取sem，若沒獲得時，則調用者睡眠等待。  
int down_write_trylock(struct rw_semaphore *sem);   //寫者嘗試獲取sem，如果獲得返回1，如果沒有獲得返回0。可在中斷上下文使用  
void up_write(struct rw_semaphore *sem);    //寫者釋放sem。  
void downgrade_write(struct rw_semaphore *sem); //把寫者降級為讀者。  
```

`4.` 互斥體
除了信號量之外，內核擁有一個更簡單的且可睡眠的鎖，那就是互斥體。互斥體的行為和計數是1的信號量類似，其接口簡單，實現更高效。 互斥體在內核中使用mutex表示，定義於`<include/linux/mutex.h>`：

```c
struct mutex {  
    /* 1: unlocked, 0: locked, negative: locked, possible waiters */  
    atomic_t        count;  
    spinlock_t        wait_lock;  
    struct list_head    wait_list;  
    ......  
};  
```

靜態定義mutex：

```c
DEFINE_MUTEX(name);  
```
實現如下：
 
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
動態定義mutex：

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
鎖定和解鎖如下：

```c
mutex_lock(&mutex);  
/* 臨界區 */  
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
int mutex_trylock(struct mutex *);    //視圖獲取指定互斥體，成功返回1；否則返回0。  
int mutex_is_locked(struct mutex *lock);    //判斷鎖是否被佔用，是返回1，否則返回0。  
```

使用mutex時，要注意一下：
mutex的使用技術永遠是1；在同一上下文中上鎖解鎖；當進程持有一個mutex時，進程不可退出；mutex不能在中斷或下半部中使用。

`5.` 搶佔禁止

在前面章節講進程管理的時候聽到過內核搶佔，由於內核可搶佔，內核中的進程隨時都可能被另外一個具有更高優先權的進程打斷，這也就意味著一個任務與被搶佔的任務可能會在同一個臨界區運行。所以才有本節前面自旋鎖來避免競態的發生，自旋鎖有禁止內核搶佔的功能。但像每cpu變量的數據只能被一個處理器訪問，可以不需要使用鎖來保護，如果沒有使用鎖，內核又是搶佔式的，那麼新調度的任務就可能訪問同一個變量。這個時候就可以通過禁止內核搶佔來避免競態的發生，禁止內核搶佔使用preetmpt_disable()函數，這是一個可以嵌套調用的函數，可以使用任意次。每次調用都必須有一個相應的preempt_enable()調用，當最後一次preempt_enable()被調用時，內核搶佔才重新啟用。內核搶佔相關函數如下：

```c
preempt_enable() //內核搶佔計數preempt_count減1  
preempt_disable() //內核搶佔計數preempt_count加1，當該值降為0時檢查和執行被掛起的需要調度的任務  
preempt_enable_no_resched()//內核搶佔計數preempt_count減1，但不立即搶佔式調度  
preempt_check_resched () //如果必要進行調度  
preempt_count() //返回搶佔計數  
preempt_schedule() //內核搶佔時的調度程序的入口點 
```


以preempt_enable()為例，看一下其實現：

```c
#define preempt_enable() \
    do { \
        preempt_enable_no_resched(); \
        barrier(); \        // 加內存屏障，阻止gcc編譯器對內存進行優化
preempt_check_resched();
\

} while (0)
#define preempt_enable_no_resched() \
    do { \
        barrier(); \
        dec_preempt_count(); \
    } while (0)
#define dec_preempt_count() sub_preempt_count(1)
# define sub_preempt_count(val)    do { preempt_count() -= (val); } while (0) //此處減少搶佔計數  

#define preempt_check_resched() \
    do { \
        if (unlikely(test_thread_flag(TIF_NEED_RESCHED))) \
            preempt_schedule(); \
    } while (0)

    asmlinkage void __sched preempt_schedule(void)
{
    struct thread_info* ti = current_thread_info();

    /*
     * If there is a non-zero preempt_count or interrupts are disabled,
     * we do not want to preempt the current task. Just return..
     */
    if (likely(ti->preempt_count || irqs_disabled())) {
        return;
    }

    do {
        add_preempt_count(PREEMPT_ACTIVE);
        schedule();
        sub_preempt_count(PREEMPT_ACTIVE);

        /*
         * Check again in case we missed a preemption opportunity
         * between schedule and now.
         */
        barrier();
    } while (need_resched());
}
```



