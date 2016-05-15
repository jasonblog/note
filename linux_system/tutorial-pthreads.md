# tutorial-pthreads


原文地址：[https://computing.llnl.gov/tutorials/pthreads/](https://computing.llnl.gov/tutorials/pthreads/ "POSIX Threads Programming")

# POSIX Threads Programming
1. Abstract
2. Pthreads Overview
    1. What is a Thread?
    2. What are Pthreads?
    3. Why Pthreads?
    4. Designing Threaded Programs
3. The Pthreads API
4. Compiling Threaded Programs
5. Thread Management
    1. Creating and Terminating Threads
    2. Passing Arguments to Threads
    3. Joining and Detaching Threads
    4. Stack Management
    5. Miscellaneous Routines
6. Exercise 1
7. Mutex Variables
    1. Mutex Variables Overview
    2. Creating and Destroying Mutexes
    3. Locking and Unlocking Mutexes
8. Condition Variables
    1. Condition Variables Overview
    2. Creating and Destroying Condition Variables
    3. Waiting and Signaling on Condition Variables
9. Monitoring, Debugging and Performance Analysis Tools for Pthreads
10. LLNL Specific Information and Recommendations
11. Topics Not Covered
12. Exercise 2
13. References and More Information
14. Appendix A: Pthread Library Routines Reference

## Abstract
## 摘要
In shared memory multiprocessor architectures, threads can be used to implement parallelism. Historically, hardware vendors have implemented their own proprietary versions of threads, making portability a concern for software developers. For UNIX systems, a standardized C language threads programming interface has been specified by the IEEE POSIX 1003.1c standard. Implementations that adhere to this standard are referred to as POSIX threads, or Pthreads.

在共享内存的多处理器架构中，线程可以被用来实现并行计算。由于历史原因，硬件供应商实现了他们自己私有版本的线程，这使得软件开发者不得不考虑可移植性。对于UNIX系统，一种标准的C语言线程编程接口被IEEE POSIX 1003.1c标准所指定。在此所述的对该标准的实现被称之为POSIX线程，也称之为Pthreads。

The tutorial begins with an introduction to concepts, motivations, and design considerations for using Pthreads. Each of the three major classes of routines in the Pthreads API are then covered: Thread Management, Mutex Variables, and Condition Variables. Example codes are used throughout to demonstrate how to use most of the Pthreads routines needed by a new Pthreads programmer. The tutorial concludes with a discussion of LLNL specifics and how to mix MPI with pthreads. A lab exercise, with numerous example codes (C Language) is also included.

本教程从对概念、动机、设计考虑的介绍开始。然后覆盖Pthreads API中三个主要的例程类型：线程管理、互斥变量、条件变量。从头到尾的示例代码用来展示一个Pthread新手需要的绝大部分Pthread例程。本教程以对LLNL的细节讨论和如何将Pthread与MPI结合作为结束。同时还包含一个拥有大量C语言示例代码的实验练习。

Level/Prerequisites: This tutorial is one of the eight tutorials in the 4+ day "Using LLNL's Supercomputers" workshop. It is deal for those who are new to parallel programming with threads. A basic understanding of parallel programming in C is required. For those who are unfamiliar with Parallel Programming in general, the material covered in [EC3500: Introduction To Parallel Computing](https://computing.llnl.gov/tutorials/parallel_comp) would be helpful. 

水平/前置条件：本教程是4+天“使用LLNL的超级计算机”进修班8篇教程中的一篇。它是那些刚刚接触使用线程进行并行编程的人的理想教程。需要基本理解在C语言中进行并行编程。对于那些连并行编程的大概都不了解的人，建议先看看[EC3500: 并行编程简介](https://computing.llnl.gov/tutorials/parallel_comp)里提到的资料，应该会有帮助。

## Pthreads Overview
## Pthread概述

### What is a Thread?
### 线程是什么？

- Technically, a thread is defined as an independent stream of instructions that can be scheduled to run as such by the operating system. But what does this mean?
- 从技术上讲，线程被定义为一个独立的指令流，它能被作系统调度进而运行。但是这是什么意思？
- To the software developer, the concept of a "procedure" that runs independently from its main program may best describe a thread.
- 对软件开发者而言，某个“过程”能独立于主程序运行这样的概念也许最能描述线程。
- To go one step further, imagine a main program (a.out) that contains a number of procedures. Then imagine all of these procedures being able to be scheduled to run simultaneously and/or independently by the operating system. That would describe a "multi-threaded" program.
- 更进一步，想像一个包含许多过程的主程序（a.out）。然后想像所有这些过程能够被操作系统调度成独立或者不独立地同时执行。这就描述一个“多线程”程序。
- How is this accomplished?
- 这是如何完成的？
- Before understanding a thread, one first needs to understand a UNIX process. A process is created by the operating system, and requires a fair amount of "overhead". Processes contain information about program resources and program execution state, including:
- 在理解线程之前，你需要先理解UNIX的进程。一个进程被操作系统创建，并且请求相当数量的“开销”。进程包含程序资源与程序执行状态的信息，包括：

    - Process ID, process group ID, user ID, and group ID
    - 进程ID、进程组ID、用户ID、组ID
    - Environment
    - 环境变量
    - Working directory
    - 工作目录
    - Program instructions
    - 程序执令
    - Registers
    - 寄存器
    - Stack
    - 栈
    - Heap
    - 堆
    - File descriptors
    - 文件描述符
    - Signal actions
    - 信号动作
    - Shared libraries
    - 共享库
    - Inter-process communication tools (such as message queues, pipes, semaphores, or shared memory)
    - 进程间通讯工具（例如消息队列、管道、信号量以及共享内存）
    - ![UNIX进程](./tutorial-pthreads/unix_process.gif)
    - ![UNIX进程中的线程](./tutorial-pthreads/threads_within_a_unix_process.gif)
- Threads use and exist within these process resources, yet are able to be scheduled by the operating system and run as independent entities largely because they duplicate only the bare essential resources that enable them to exist as executable code.
- 线程使用进程的资源且位于进程资源内，却能被操作系统当成独立实体调度并且独立执行，这主要是因为它们仅复制了让它们成为可执行代码刚刚够的必须的资源。
- This independent flow of control is accomplished because a thread maintains its own:
- 能做到独立控制流是因为线程维护着它自己的：
    - Stack pointer
    - 栈指针
    - Registers
    - 寄存器
    - Scheduling properties (such as policy or priority)
    - 调度属性（例如策略与优先级）
    - Set of pending and blocked signals
    - 一系列等待的与阻塞的信号
    - Thread specific data
    - 线程特有数据
- So, in summary, in the UNIX environment a thread:
- 综上，在UNIX环境下，一个线程：
    - Exists within a process and uses the process resources
    - 存在于一个进程里面并且使用该进程的资源
    - Has its own independent flow of control as long as its parent process exists and the OS supports it
    - 拥有它自己独立的控制流，只要它的父进程存在并且操作系统支持
    - Duplicates only the essential resources it needs to be independently schedulable
    - 仅复制独立调度所必须的资源
    - May share the process resources with other threads that act equally independently (and dependently)
    - 可能与行为同样独立（或者依赖）的其他线程共享进程资源
    - Dies if the parent process dies - or something similar
    - 因父进程挂掉而挂掉——或者其他类似的规则
    - Is "lightweight" because most of the overhead has already been accomplished through the creation of its process
    - 是轻量级的，因为大部分开销已经在创建进程的时候完成了。
- Because threads within the same process share resources:
- 因为属于同一个进程线程们共享资源，所以：
    - Changes made by one thread to shared system resources (such as closing a file) will be seen by all other threads.
    - 由某个线程引起的对共享系统资源的变更（比如关闭一个文件）将会被其他所有线程看见。
    - Two pointers having the same value point to the same data.
    - 拥有相同值的两个指针指向同一块数据。
    - Reading and writing to the same memory locations is possible, and therefore requires explicit synchronization by the programmer.
    - 同时读和写同一个内存地址是可能的，因此需要编程人员作出明确的同步。

### What are Pthreads?
### Pthread是什么？
- Historically, hardware vendors have implemented their own proprietary versions of threads. These implementations differed substantially from each other making it difficult for programmers to develop portable threaded applications.
- 由于历史原因，硬件供应商实现了他们自己的专有的线程版本。这些实现相互之间有大量的不同，这使得程序员开发可移植的多线程应用变得很难。
- In order to take full advantage of the capabilities provided by threads, a standardized programming interface was required.
- 为了充分利用线程提供的能力，一个标准化的编程接口不可缺少。
    - For UNIX systems, this interface has been specified by the IEEE POSIX 1003.1c standard (1995).
    - 对UNIX系统而言，这个接口被IEEE POSIX 1003.1c标准所指定。
    - Implementations adhering to this standard are referred to as POSIX threads, or Pthreads.
    - 在这里所指的对标准的实现称为POSIX线程，也叫Pthread。
    - Most hardware vendors now offer Pthreads in addition to their proprietary API's.
    - 大多数硬件供应商现在除了他们专有的API之外都提供了Pthread。
- The POSIX standard has continued to evolve and undergo revisions, including the Pthreads specification.
- POSIX标准一直在发展并经受修订，包括Pthread设计规格。
- Some useful links:
- 一些有用的链接：
    - [POSIX 1003.1-2008](http://standards.ieee.org/findstds/standard/1003.1-2008.html)
    - [posix faq](www.opengroup.org/austin/papers/posix_faq.html)
    - [ieee std](www.unix.org/version3/ieee_std.html)
- Pthreads are defined as a set of C language programming types and procedure calls, implemented with a pthread.h header/include file and a thread library - though this library may be part of another library, such as libc, in some implementations.
- Pthread被定义为一系列C语言编程里的类型和过程调用，随一个pthread.h头文件/include文件以及一个线程库而实现。尽管在某些实现里，这线程库是作为另一个库的一部分而存在，例如libc。

### Why Pthreads?
### 为什么是Pthread？
#### Light Weight:
#### 轻量：
- When compared to the cost of creating and managing a process, a thread can be created with much less operating system overhead. Managing threads requires fewer system resources than managing processes.
- 与创建和管理一个进程的消耗相比，一个线程能以相当少的操作系统开销创建出来。管理线程比管理进程所需系统资源更少。
- For example, the following table compares timing results for the fork() subroutine and the pthread_create() subroutine. Timings reflect 50,000 process/thread creations, were performed with the time utility, and units are in seconds, no optimization flags.
- 比如说，下表比较了fork()子例程和pthread_create()子例程在时间上的消耗结果。时间反映了50000个进程/线程的创建，用time工具命令执行，单位为秒，没有使用优化标记。

Note: don't expect the sytem and user times to add up to real time, because these are SMP systems with multiple CPUs/cores working on the problem at the same time. At best, these are approximations run on local machines, past and present.

注意：不要妄想系统时间加上用户时间等于实际时间，因为这些是对称多处理器（SMP）系统，拥有多个CPU/核心同时在问题上工作。充其量，在本地机上运行时它们相近，不管是过去还是当今的机器。

<table>
    <thead>
        <tr>
            <th rowspan="2">Platform</th>
            <th colspan="3">fork()</th>
            <th colspan="3">pthread_create()</th>
        </tr>
        <tr>
            <th>real</th>
            <th>user</th>
            <th>sys</th>
            <th>real</th>
            <th>user</th>
            <th>sys</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Intel 2.6 GHz Xeon E5-2670 (16 cores/node)</td>
            <td>8.1</td>
            <td>0.1</td>
            <td>2.9</td>
            <td>0.9</td>
            <td>0.2</td>
            <td>0.3</td>
        </tr>
        <tr>
            <td>Intel 2.8 GHz Xeon 5660 (12 cores/node)</td>
            <td>4.4</td>
            <td>0.4</td>
            <td>4.3</td>
            <td>0.7</td>
            <td>0.2</td>
            <td>0.5</td>
        </tr>
        <tr>
            <td>AMD 2.3 GHz Opteron (16 cores/node)</td>
            <td>12.5</td>
            <td>1.0</td>
            <td>12.5</td>
            <td>1.2</td>
            <td>0.2</td>
            <td>1.3</td>
        </tr>
        <tr>
            <td>AMD 2.4 GHz Opteron (8 cores/node)</td>
            <td>17.6</td>
            <td>2.2</td>
            <td>15.7</td>
            <td>1.4</td>
            <td>0.3</td>
            <td>1.3</td>
        </tr>
        <tr>
            <td>IBM 4.0 GHz POWER6 (8 cpus/node)</td>
            <td>9.5</td>
            <td>0.6</td>
            <td>8.8</td>
            <td>1.6</td>
            <td>0.1</td>
            <td>0.4</td>
        </tr>
        <tr>
            <td>IBM 1.9 GHz POWER5 p5-575 (8 cpus/node)</td>
            <td>64.2</td>
            <td>30.7</td>
            <td>27.6</td>
            <td>1.7</td>
            <td>0.6</td>
            <td>1.1</td>
        </tr>
        <tr>
            <td>IBM 1.5 GHz POWER4 (8 cpus/node)</td>
            <td>104.5</td>
            <td>48.6</td>
            <td>47.2</td>
            <td>2.1</td>
            <td>1.0</td>
            <td>1.5</td>
        </tr>
        <tr>
            <td>INTEL 2.4 GHz Xeon (2 cpus/node)</td>
            <td>54.9</td>
            <td>1.5</td>
            <td>20.8</td>
            <td>1.6</td>
            <td>0.7</td>
            <td>0.9</td>
        </tr>
        <tr>
            <td>INTEL 1.4 GHz Itanium2 (4 cpus/node)</td>
            <td>54.5</td>
            <td>1.1</td>
            <td>22.2</td>
            <td>2.0</td>
            <td>1.2</td>
            <td>0.6</td>
        </tr>
    </tbody>
</table>

Source: [fork vs thread](https://computing.llnl.gov/tutorials/pthreads/fork_vs_thread.txt "fork_vs_thread.txt")

#### Efficient communications/Data Exchange:
#### 高效的交流/数据交换：
- The primary motivation for considering the use of Pthreads on a multi-processor architecture is to achieve optimum performance. In particular, if an application is using MPI for on-node communications, there is a potential that performance could be improved by using Pthreads instead.
- 考虑在多处理器架构上用Pthread的主要动机是达到最佳性能。特别是如果一个应用使用MPI做单点交流，那么有很有可能通过使用Pthread代替MPI让性能得到提升。
- MPI libraries usually implement on-node task communication via shared memory, which involves at least one memory copy operation (process to process).
- MPI库通常通过共享内存实现单点任务沟通，这至少牵扯到内存复制操作（进程到进程）。
- For Pthreads there is no intermediate memory copy required because threads share the same address space within a single process. There is no data transfer, per se. It can be as efficient as simply passing a pointer.
- 对Pthread来说不存在中间内存复制的需求，因为在同一个单独进程里面线程们共享相同的地址空间。本质上没有数据传输。它可以做到仅仅传输一个指针那样的高效。
- In the worst case scenario, Pthread communications become more of a cache-to-CPU or memory-to-CPU bandwidth issue. These speeds are much higher than MPI shared memory communications.
- 在最糟糕的情况下，Pthread交流更多地成为一个缓存至CPU或者CPU至缓存带宽问题。它们的速度远比MPI的内存交流快得多。
- For example: some local comparisons, past and present, are shown below:
- 比如：下面展示了过去与现在的一些本地机器的比较：

<table>
    <thead>
        <tr>
            <th>Platform</th>
            <th>MPI Shared Memory Bandwidth<br>(GB/sec)</th>
            <th>Pthreads Worst Case<br>Memory-to-CPU Bandwidth <br>(GB/sec)</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Intel 2.6 GHz Xeon E5-2670</td>
            <td>4.5</td>
            <td>51.2</td>
        </tr>
        <tr>
            <td>Intel 2.8 GHz Xeon 5660</td>
            <td>5.6</td>
            <td>32</td>
        </tr>
        <tr>
            <td>AMD 2.3 GHz Opteron</td>
            <td>1.8</td>
            <td>5.3</td>
        </tr>
        <tr>
            <td>AMD 2.4 GHz Opteron</td>
            <td>1.2</td>
            <td>5.3</td>
        </tr>
        <tr>
            <td>IBM 1.9 GHz POWER5 p5-575</td>
            <td>4.1</td>
            <td>16</td>
        </tr>
        <tr>
            <td>IBM 1.5 GHz POWER4</td>
            <td>2.1</td>
            <td>4</td>
        </tr>
        <tr>
            <td>Intel 2.4 GHz Xeon</td>
            <td>0.3</td>
            <td>4.3</td>
        </tr>
        <tr>
            <td>Intel 1.4 GHz Itanium 2</td>
            <td>1.8</td>
            <td>6.4</td> 
        </tr>
    </tbody>
</table>

#### Other Common Reasons:
#### 其他常见原因：
- Threaded applications offer potential performance gains and practical advantages over non-threaded applications in several other ways:
- 在其他许多方面，多线程应用提供了潜在的性能收益和在非多线程应用之上的实用的优势：
    - Overlapping CPU work with I/O: For example, a program may have sections where it is performing a long I/O operation. While one thread is waiting for an I/O system call to complete, CPU intensive work can be performed by other threads.
    - 重叠的CPU工作与I/O：例如，一个程序可能拥有某个执行长时间I/O操作的区块。当其中一个线程在等待I/O系统调用的结束时，CPU密集的工作可以在另一个线程执行。
    - Priority/real-time scheduling: tasks which are more important can be scheduled to supersede or interrupt lower priority tasks.
    - 优先级/实时调度：更重要的任务可以在调度时取代或者中断低优先级的任务。
    - Asynchronous event handling: tasks which service events of indeterminate frequency and duration can be interleaved. For example, a web server can both transfer data from previous requests and manage the arrival of new requests.
    - 异步的事件处理：服务不确定频率和持续时间的事件的任务可以交错进行。例如一个Web服务既可以传输先前的请求的数据也可以处理新到来的请求。
- A perfect example is the typical web browser, where many interleaved tasks can be happening at the same time, and where tasks can vary in priority.
- 一个完美的例子是典型的WEB浏览器，在这里许多交错的任务在同时发生，任务的优先级可以不同。
- Another good example is a modern operating system, which makes extensive use of threads. A screenshot of the MS Windows OS and applications using threads is shown below.
- 另一个好例子是现代操作系统，它广泛地使用了线程。下面展示了微软Windows操作系统和应用程序使用的线程的屏幕截图。

![资源监视器截图](./tutorial-pthreads/resource_monitor.jpg)

## Pthreads Overview
## Pthread概述
### Designing Threaded Programs
### 设计多线程程序
#### Parallel Programming:
#### 并行编程：

- On modern, multi-core machines, pthreads are ideally suited for parallel programming, and whatever applies to parallel programming in general, applies to parallel pthreads programs.
- 在现代、多核心的机器上，pthread是理想上适合并行编程，同时不管怎样适用于通用的并行编程，适用于并行的pthread程序。
- There are many considerations for designing parallel programs, such as:
- 设计一个并行程序需要考虑许多，例如：
    - What type of parallel programming model to use?
    - 使用哪种类型的并行编程模型？
    - Problem partitioning
    - 问题分解
    - Load balancing
    - 负载平衡
    - Communications
    - 交流
    - Data dependencies
    - 数据依赖
    - Synchronization and race conditions
    - 同步与竞态条件
    - Memory issues
    - 内存问题
    - I/O issues
    - I/O问题
    - Program complexity
    - 程序复杂性
    - Programmer effort/costs/time
    - 编程人员工作量/费用/时间
    - ...
    - ……
- Covering these topics is beyond the scope of this tutorial, however interested readers can obtain a quick overview in the [Introduction to Parallel Computing](https://computing.llnl.gov/tutorials/parallel_comp) tutorial.
- 这些主题已经超出了本教程的范围，然而对这些感兴趣的读者可以从[并行计算简介](https://computing.llnl.gov/tutorials/parallel_comp)教程中获得简要概述。
- In general though, in order for a program to take advantage of Pthreads, it must be able to be organized into discrete, independent tasks which can execute concurrently. For example, if routine1 and routine2 can be interchanged, interleaved and/or overlapped in real time, they are candidates for threading.
- 但是一般来说，想让一个程序能利用Pthread的优势，它必须能够被组织成离散的独立的任务，这些任务能同时地执行。例如，例程1和例程2能被实时地交换、交叉或者重叠，那么它们是多线程的候选人。

![concurrent](./tutorial-pthreads/concurrent.gif)

- Programs having the following characteristics may be well suited for pthreads:
- 拥有以下特征的程序可能很适合Pthread：
    - Work that can be executed, or data that can be operated on, by multiple tasks simultaneously:
    - 能够被多个任务同时地执行工作，或者操作数据：
    - Block for potentially long I/O waits
    - 因为潜在的常时间I/O等待而阻塞
    - Use many CPU cycles in some places but not others
    - 在某些地方使用了许多CPU周期但是其他地方则不是
    - Must respond to asynchronous events
    - 必须响应异步的事件
    - Some work is more important than other work(priority interrupts)
    - 某些工作比其他工作更重要（优先中断）
- Several common models for threaded programs exist:
- 存在一些常见的多线程程序模型：
    - Manager/worker: a single thread, the manager assigns work to other threads, the workers. Typically, the manager handles all input and parcels out work to the other tasks. At least two forms of the manager/worker model are common: static worker pool and dynamic worker pool.
    - 管理者/工人：一个单独的线程——管理者——分配工作任务给其他线程——工人们。通常，管理者处理所有输入并且把工作分配给其他任务。至少存在两种常用的管理者/工人模型：静态工人池和动态工人池。
    - Pipeline: a task is broken into a series of suboperations, each of which is handled in series, but concurrently, by a different thread. An automobile assembly line best describes this model.
    - 流水线：一个任务被分解成一系列子操作，每个子操作被不同的线程同时地按顺序地接管。汽车装配线能最好地描述这个模型。
    - Peer: similar to the manager/worker model, but after the main thread creates other threads, it participates in the work.
    - 对等：与管理者/工人模型相似，但是在主线程创建其他线程后，它自己参与进工作当中。

#### Shared Memory Model:
#### 共享内存模型：

- All threads have access to the same global, shared memory
- 所有线程有权访问同一个全局的共享的内存
- Threads also have their own private data
- 线程也有他们自己的私有数据
- Programmers are responsible for synchronizing access(protecting) globally shared data.
- 编程人员对同步访问（保护）全局的共享的数据负有责任。

![Shared Memory Model](./tutorial-pthreads/shared_memory_model.gif)

#### Thread-safeness:
#### 线程安全性：

- Thread-safeness: in a nutshell, refers an application's ability to execute multiple threads simulaneously without "clobbering" shared data or creating "race" conditions.
- 线程安全性：简而言之，指的就是应用程序的同时执行多个线程而不“痛打”共享数据或者产生“竞态”条件。
- For example, suppose that your application creates several threads, each of which makes a call to the same library routine:
- 举个例子，假设你的应用程序创建了许多线程，每个线程都调用了同一个库函数：
    - This library routine accesses/modifies a global structure or location in memory.
    - 该库函数访问/修改一个内存中的全局的结构或者位置。
    - As each thread calls this routine it is possible that they may try to modify this global structure/memory location at the same time.
    - 因为每个线程都调用该函数，所以有可能造成它们尝试同时修改这个全局的结构或者内存位置。
    - If the routine does not employ some sort of synchronization constructs to prevent data corruption, then it is not thread-safe.
    - 如果函数没有使用某种同步设计来避免数据损坏，那么它就不是线程安全的。

![Thread-Unsafe](./tutorial-pthreads/thread_unsafe.gif)

- The implication to users of external library routines is that if you aren't 100% certain the routine is thread-safe, then you take your chances with problems that could arise.
- 这暗示着那些使用外部库函数的用户，如果你不能100%确定函数是线程安全的，那么你得自行承担可能出现的问题的风险。
- Recommendation: Be careful if your application uses libraries or other objects that don't explicitly guarantee thread-safeness. When in doubt, assume that they are not thread-safe until proven otherwise. This can be done by "serializing" the calls to the uncertain routine, etc.
- 建议：如果你的应用程序使用的库或者其他对象没有明确地保证线程安全，那么你要小心了。当你不确定时，假定它们非线程安全直到证明它线程安全。通过对不确定的函数的调用进行“序列化”可以解决这个问题。

#### Thread Limits:
#### 线程限制：

- Although the Pthreads API is an ANSI/IEEE standard, implementations can, and usually do, vary in ways not sepcified by the standard.
- 尽管Pthread API是一种ANSI/IEEE标准，但是通常它的实现却是多种多样，因为标准中并没有指定实现。
- Because of this, a program that runs fine on one platform, may fail or produce wrong results on another platform.
- 正因为如此，所以一个程序在某一个平台上运行良好却有可能在另一个平台上失败或者产生错误的结果。
- For example, the maximum number of threads permitted, and the default thread stack size are two important limits to consider when designing your program.
- 例如，允许的最大线程数量以及默认的线程栈大小是你设计你的程序时需要考虑的两个重要限制因素。
- Several thread limits are discussed in more detail later in this tutorial.
- 本教程稍后会深入讨论许多线程限制。

## The Pthreads API
## Pthread API

- The original Pthreads API was defined in the ANSI/IEEE POSIX 1003.1 - 1995 standard. The POSIX standard has continued to evolve and undergo revisions, including the Pthreads specification.
- Pthread API最早在ANSI/IEEE POSIX 1003.1 - 1995标准里被定义。POSIX标准在持续发展并经历修订，包括其中的Pthread规范。
- Copies of the standard can be purchased from IEEE or downloaded for free from other sites online.
- 标准的副本可以从IEEE购买或者从其他在线网站免费下载得到。
- The subroutines which comprise the Pthreads API can be informally grouped into four major groups:
- 构成Pthread API的子函数可以被非正式地分成主要的四组：
    1. Thread management: Routines that work directly on threads - creating, detaching, joining, etc. They also include functions to set/query thread attributes(joinable, scheduling etc.)
    1. 线程管理：那些直接作用于线程的函数——创建、分离、连接等。它们还包含用于设置/查询属性（可连接性、调度等）的函数。
    2. Mutexes: Routines that deal with synchronization, called a "mutex", which is an abbreviation for "mutual exclusion". Mutex functions provide for creating, destroying, locking and unlocking mutexes. These are supplemented by mutex attribute functions that set or modify attributes associated with mutexes.
    2. 互斥体：处理同步的函数，被叫作“mutex”，这个单词是“mutual exclusion”的缩写。互斥体函数提供了创建、销毁、加锁、解锁互斥体的功能。辅以互斥体属性函数，我们可以设置或者修改互斥体所关联的属性。
    3. Condition variables: Routines that address communications between threads that share a mutex. Based upon programmer specified conditions. This group includes functions to create, destroy, wait and signal based upon specified variable values. Functions to set/query condition variable attributes are also included.
    3. 条件变量：涉及共享同一个互斥体的线程之间沟通的函数。根据程序员指定的变量。这组API包括依据指定变量值创建、销毁、等待以及发信号的函数。设置/查询条件变量的属性的函数同样包括在内。
    4. Synchronization: Routines that manage read/write locks and barriers.
    4. 同步：管理读/写锁和屏障的函数。
- Naming conventions: All identifiers in the threads library begin with ** pthread_ **. Some example are shown below.
- 命名规范：线程库里的所有标识符都以 ** pthread_ ** 开头。下面展示了一些例子。

<table>
    <thead>
        <tr>
            <th>函数名前缀</th>
            <th>功能组</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>pthread_</td>
            <td>关于线程自身和其他杂项的子函数</td>
        </tr>
        <tr>
            <td>pthread_attr_</td>
            <td>线程属性对象</td>
        </tr>
        <tr>
            <td>pthread_mutex_</td>
            <td>互斥体</td>
        </tr>
        <tr>
            <td>pthread_mutexattr_</td>
            <td>互斥体属性对象</td>
        </tr>
        <tr>
            <td>pthread_cond_</td>
            <td>条件变量</td>
        </tr>
        <tr>
            <td>pthread_condattr_</td>
            <td>条件变量属性对象</td>
        </tr>
        <tr>
            <td>pthread_key_</td>
            <td>线程专有数据键</td>
        </tr>
        <tr>
            <td>pthread_rwlock_</td>
            <td>读/写锁</td>
        </tr>
        <tr>
            <td>pthread_barrier_</td>
            <td>同步屏障</td>
        </tr>
    </tbody>
</table>

- The concept of opaque objects pervades the design of the API. The basic calls work to create or modify opaque objects - the opaque objects can be modified by calls to attribute functions, which deal with opaque attributes.
- API的设计中弥漫着不透明对象的概念。基本调用作用于创建或者修改不透明对象——该不透明对象能通过调用属性函数进行修改，这些函数处理不透明的属性。
- The Pthreads API contains around 100 subroutines. This tutorial will focus on a subset of these - specifically, those which are most likely to be immediately useful to the beginning Pthreads programmer.
- Pthread API包含大约100个子函数。本教程将专注于其中一个子集——特别是那些很可能立马就能被Pthread编程初学者用到的。
- For portability, the pthread.h header file should be included in each source file using the Pthreads library.
- 为了可移植性，pthread.h头文件需要被每个使用Pthread库的源码文件包含。
- The current POSIX standard is defined only for the C language. Fortran programmers can use wrappers around C function calls. Some Fortran compilers may provide a Fortran pthreads API.
- 当前POSIX标准仅对C语言作了定义。Fortran程序员可以使用C函数调用的包装。某些Fortran编译器提供了Fortran Pthread API。
- A number of excellent books about Pthreads are available. Several of these are listed in the [References](https://computing.llnl.gov/tutorials/pthreads/#References) section of this tutorial.
- 大量的优秀关于Pthread的书籍可用。其中的许多列于本教程的[参考文献](https://computing.llnl.gov/tutorials/pthreads/#References)一节。

## Compiling Threaded Programs
## 编译多线程程序

- Several examples of compile commands used for pthreads codes are listed in the table below.
- 下表列举了一些用于编译Pthread代码的命令例子。

<table>
    <thead>
        <tr>
            <th>编译器/平台</th>
            <th>编译器命令</th>
            <th>描述</th>
        </tr>
    </thead>
        <tr>
            <td rowspan="2">INTEL Linux</td>
            <td>icc -pthread</td>
            <td>C</td>
        </tr>
        <tr>
            <td>icpc -pthread</td>
            <td>C++</td>
        </tr>
        <tr>
            <td rowspan="2">PGI Linux</td>
            <td>pgcc -lpthread</td>
            <td>C</td>
        </tr>
        <tr>
            <td>pgCC -lpthread</td>
            <td>C++</td>
        </tr>
        <tr>
            <td rowspan="2">GNU Linux, Blue Gene</td>
            <td>gcc -pthread</td>
            <td>GNU C</td>
        </tr>
        <tr>
            <td>g++ -pthread</td>
            <td>GNU C++</td>
        </tr>
        <tr>
            <td rowspan="2">IBM Blue Gene</td>
            <td>bgxlc_r / bgcc_r</td>
            <td>C(ANSI / non-ANSI)</td>
        </tr>
        <tr>
            <td>bgxlC_r, bgxlc++_r</td>
            <td>C++</td>
        </tr>
    <tbody>
    </tbody>
</table>

## Thread Management
## 线程管理

### Creating and Terminating Threads
### 创建和终止线程

#### Routines:
#### 函数：

[pthread_create](https://computing.llnl.gov/tutorials/pthreads/man/pthread_create.txt) (thread,attr,start_routine,arg)

[pthread_exit](https://computing.llnl.gov/tutorials/pthreads/man/pthread_exit.txt) (status)

[pthread_cancel](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cancel.txt) (thread)

[pthread_attr_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_init.txt) (attr)

[pthread_attr_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_destroy.txt) (attr)

#### Creating Threads:
#### 创建线程：

- Initially, your main() program comprises a single, default thread. All other threads must be explicitly created by the programmer.
- 一开始，你的main()程序包含一个单独的默认的线程。所有其他线程都必须由程序员明确地创建。
- pthread_create creates a new thread and makes it executable. This routine can be called any number of times from anywhere within your code.
- pthread_create创建一个线程并使它可执行。该函数可以在你代码中的任何地方被市用任意多次。
- pthread_create arguments:
- pthread_create 参数：
    - thread: An opaque, unique identifier for the new thread returned by the subroutine.
    - thread：一个不透明的独一无二的标识符，用于标识该子函数返回的新线程。
    - attr: An opaque attribute object that may be used to set thread attributes. You can specify a thread attributes object, or NULL for the default values.
    - attr：一个不透明的属性对象，可以用于设置线程属性。你可以指定一个线程属性对象，或者用NULL保持默认值。
    - start_routine: the C routine that the thread will execute once it is created.
    - start_routine：一个c函数，一旦当线程被创建会立即执行这个函数。
    - arg: A single argument that may be passed to start_routine. It must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed.
    - arg：一个单独的参数，可以被传递给start_routine函数。它必须以引用方式，以一个void*类型的指针进行传递。如果没有需要传递的参数，那么可以用NULL。
- The maximum number of threads that may be created by a process is implementation dependent. Programs that attempt to exceed the limit can fail or produce wrong results.
- 一个进程能够创建的线程的最大数量是依赖于实现的。程序如果尝试超出这个限制那么有可能失败或者产生错误的结果。
- Querying and setting your implementation's thread limit - Linux example shown. Demonstrates querying the default (soft) limits and then setting the maximum number of processes (including threads) to the hard limit. Then verifying that the limit has been overridden.
- 查询和设置你的实现的线程限制——展示了Linux下的例子。演示里查询默认（软）限制然后设置进程（包括线程）的最大数为硬极限。再然后验证该限制被成功覆写。

<table>
    <thead>
        <tr>
            <th>bash/ksh/sh</th>
            <th>tcsh/csh</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td><pre>$ ulimit -a
core file size          (blocks, -c) 16
data seg size           (kbytes, -d) unlimited
scheduling priority             (-e) 0
file size               (blocks, -f) unlimited
pending signals                 (-i) 255956
max locked memory       (kbytes, -l) 64
max memory size         (kbytes, -m) unlimited
open files                      (-n) 1024
pipe size            (512 bytes, -p) 8
POSIX message queues     (bytes, -q) 819200
real-time priority              (-r) 0
stack size              (kbytes, -s) unlimited
cpu time               (seconds, -t) unlimited
max user processes              (-u) 1024
virtual memory          (kbytes, -v) unlimited
file locks                      (-x) unlimited

$ ulimit -Hu
7168

$ ulimit -u 7168

$ ulimit -a
core file size          (blocks, -c) 16
data seg size           (kbytes, -d) unlimited
scheduling priority             (-e) 0
file size               (blocks, -f) unlimited
pending signals                 (-i) 255956
max locked memory       (kbytes, -l) 64
max memory size         (kbytes, -m) unlimited
open files                      (-n) 1024
pipe size            (512 bytes, -p) 8
POSIX message queues     (bytes, -q) 819200
real-time priority              (-r) 0
stack size              (kbytes, -s) unlimited
cpu time               (seconds, -t) unlimited
max user processes              (-u) 7168
virtual memory          (kbytes, -v) unlimited
file locks                      (-x) unlimited</pre></td>
            <td><pre>% limit 
cputime      unlimited
filesize     unlimited
datasize     unlimited
stacksize    unlimited
coredumpsize 16 kbytes
memoryuse    unlimited
vmemoryuse   unlimited
descriptors  1024 
memorylocked 64 kbytes
maxproc      1024

% limit maxproc unlimited

% limit
cputime      unlimited
filesize     unlimited
datasize     unlimited
stacksize    unlimited
coredumpsize 16 kbytes
memoryuse    unlimited
vmemoryuse   unlimited
descriptors  1024 
memorylocked 64 kbytes
maxproc      7168</pre></td>
        </tr>
    </tbody>
</table>

- Once created, threads are peers, and may create other threads. There is no implied hierarchy or dependency between threads.
- 一旦创建，线程间就是平辈关系，它们可以创建其他线程。线程之间不存在隐含的层级或者依赖。

![Peer Threads](./tutorial-pthreads/peer_threads.gif)

#### Thread Attributes:
#### 线程属性：

- By default, a thread is created with certain attributes. Some of these attributes can be changed by the programmer via the thread attribute object.
- 默认情况下，一个线程以确定的属性创建。程序员可以通过线程属性对象修改其中的某些属性。
- pthread_attr_init and pthread_attr_destroy are used to initialize/destroy the thread attribute object.
- pthread_attr_init 和 pthread_attr_destroy用于初始化/销毁线程属性对象。
- Other routines are then used to query/set specific attributes in the thread attribute object. Attributes include:
- 其他一些函数用于查询/设置线程属性对象的特定属性。属性包括：
    - Detached or joinable state
    - 分离和可连接状态
    - Scheduling inheritance
    - 调度继承
    - Scheduling policy
    - 调度策略
    - Scheduling parameters
    - 调度参数
    - Scheduling contention scope
    - 调度竞争范围
    - Stack size
    - 栈大小
    - Stack address
    - 栈地址
    - Stack guard (overflow) size
    - 栈保护（溢出）大小
- Some of these attributes will be discussed later.
- 其中的一些属性将会在之后被讨论。

#### Thread Binding and Scheduling:
#### 线程绑定和调度：

Question: After a thread has been created, how do you know a)when it will be scheduled to run by the operating system, and b)which processor/core it will run on?

问题：线程创建之后，你如何知道：1.它什么时候被操作系统调度执行；2.它会在哪个处理器/核心上运行？

Answer: Unless you are using the Pthreads scheduling mechanism, it is up to the implementation and/or operating system to decide where and when threads will execute.  Robust programs should not depend upon threads executing in a specific order or on a specific processor/core.

答案：除非你使用Pthread调度机制，否则它由Pthread实现或者操作系统来决定何处以及何时执行线程。强健的程序应该不依赖于线程以特定顺序执行或者在特定处理器/核心上执行。

- The Pthreads API provides several routines that may be used to specify how threads are scheduled for execution. For example, threads can be scheduled to run FIFO (first-in first-out), RR (round-robin) or OTHER (operating system determines). It also provides the ability to set a thread's scheduling priority value.
- Pthread API提供了许多函数用于指定线程如何调度执行。例如，线程可以以FIFO（先进先出）、RR（循环赛）以及其他（操作系统决定）方式运行。API还提供了设置线程调度优先级值的能力。
- These topics are not covered here, however a good overview of "how things work" under Linux can be found in the sched_setscheduler man page.
- 这些主题在本教程不会涉及，但是可以从sched_setscheduler man手册里获得它们在Linux下的“工作原理”。
- The Pthreads API does not provide routines for binding threads to specific cpus/cores. However, local implementations may include this functionality - such as providing the non-standard pthread_setaffinity_np routine. Note that "_np" in the name stands for "non-portable".
- Pthread API未提供函用于绑定线程到指定CPU/核心的函数。但是本地实现有可能包含这个功能——例如提供非标准的pthread_setaffinity_np函数。注意函数名中的“_np”代表的是“不可移植的”(non-portable)。
- Also, the local operating system may provide a way to do this. For example, Linux provides the sched_setaffinity routine.
- 另外，本地操作系统可能提供了这样做的方法。例如Linux提供了sched_setaffinity函数。

#### Terminating Threads & pthread_exit():
#### 终止线程与pthread_exit()：

- There are several ways in which a thread may be terminated:
- 存在多种方法来终止一个线程：
    - The thread returns normally from its starting routine. It's work is done.
    - 线程从它的开始函数正常返回。它的工作完成了。
    - The thread makes a call to the pthread_exit subroutine - whether its work is done or not.
    - 线程调用pthread_exit子函数——无论它的工作是否完成。
    - The thread is canceled by another thread via the pthread_cancel routine.
    - 线程被其他线程通过pthread_cancel函数取消。
    - The entire process is terminated due to making a call to either the exec() or exit()
    - 整个进程因为调用exec()或者exit()而终止。
    - If main() finishes first, without calling pthread_exit explicitly itself
    - 如果main()先结束，且没有在结束前明确地调用pthread_exit。
- The pthread_exit() routine allows the programmer to specify an optional termination status parameter. This optional parameter is typically returned to threads "joining" the terminated thread (covered later).
- pthread_exit()函数允许程序员指定一个可选的终止状态参数。该可选的参数一般上返回给“连接”终止线程的线程（稍后涉及）。
- In subroutines that execute to completion normally, you can often dispense with calling pthread_exit() - unless, of course, you want to pass the optional status code back.
- 在正常执行结束的子函数里，你通常可能免去调用pthread_exit()——当然，除非你想要传回可选的状态码。
- Cleanup: the pthread_exit() routine does not close files; any files opened inside the thread will remain open after the thread is terminated.
- 清扫工作：pthread_exit()函数不会关闭文件；任何在线程里打开的文件会在线程结束后保持打开状态。
- Discussion on calling pthread_exit() from main():
- 讨论在main函数中调用pthread_exit：
    - There is a definite problem if main() finishes before the threads it spawned if you don't call pthread_exit() explicitly. All of the threads it created will terminate because main() is done and no longer exists to support the threads.
    - 如果main函数在它创建的线程结束前先结束，并且没有明确调用pthread_exit函数，那么这里肯定存在问题。它创建的所有线程都将会终止，因为main函数完成然后不再存于以维持这些线程。
    - By having main() explicitly call pthread_exit() as the last thing it does, main() will block and be kept alive to support the threads it created until they are done.
    - 通过使main函数明确地调用pthread_exit函数作为它最后做的一件事，main函数将会阻塞并保持存活，以维持它创建的那些线程直至它们完成。

### Example: Pthread Creation and Termination
### 例子：Pthread创建和终止

- This simple example code creates 5 threads with the pthread_create() routine. Each thread prints a "Hello World!" message, and then terminates with a call to pthread_exit().
- 这个简单的示例代码用pthread_create函数创建了5个线程。每个线程打印一条“Hello World!”消息，然后以调用pthread_exit函数结束。

---

    #include <pthread.h>
    #include <stdio.h>
    #define NUM_THREADS     5

    void *PrintHello(void *threadid)
    {
        long tid;
        tid = (long)threadid;
        printf("Hello World! It's me, thread #%ld!\n", tid);
        pthread_exit(NULL);
    }

    int main (int argc, char *argv[])
    {
        pthread_t threads[NUM_THREADS];
        int rc;
        long t;
        for(t=0; t<NUM_THREADS; t++){
            printf("In main: creating thread %ld\n", t);
            rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
            if (rc){
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }
        }

        /* Last thing that main() should do */
        pthread_exit(NULL);
    }

## Thread Management
## 线程管理

### Passing Arguments to Threads
### 传递参数给线程

- The pthread_create() routine permits the programmer to pass one argument to the thread start routine. For cases where multiple arguments must be passed, this limitation is easily overcome by creating a structure which contains all of the arguments, and then passing a pointer to that structure in the pthread_create() routine.
- pthread_create函数允许程序员传递1个参数给线程的开始函数。对于需要传递多个参数的情况，这个限制是很容易克服的，只需创建一个包括所有参数的结构然后传递该结构的指针给pthread_create函数即可。
- All arguments must be passed by reference and cast to (void *).
- 所有的参数必须以引用方式传递，并且转换类型至(void*)。

Question: How can you safely pass data to newly created threads, given their non-deterministic start-up and scheduling?

问题：你如何安全地传递数据到新创建的线程，考虑到它们不确定的启动与调度？

Answer: Make sure that all passed data is thread safe - that it can not be changed by other threads.  The three examples that follow demonstrate what not and what to do.

答案：确保所有创建的数据是线程安全的——即传递的数据无法被其他线程更改。下面的三个例子演示了什么不该做什么该做。

#### Example 1 - Thread Argument Passing
#### 例子1——线程参数传递

This code fragment demonstrates how to pass a simple integer to each thread. The calling thread uses a unique data structure for each thread, insuring that each thread's argument remains intact throughout the program.

这个代码片段演示了如何传递一个简单的整数给每一个线程。调用线程针对每个线程使用了一个独一无二的数据结构，以确保每个线程的参数在整个程序里保持原封不动。

    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    #define NUM_THREADS 8

    char *messages[NUM_THREADS];

    void *PrintHello(void *threadid)
    {
        int *id_ptr, taskid;

        sleep(1);
        id_ptr = (int *) threadid;
        taskid = *id_ptr;
        printf("Thread %d: %s\n", taskid, messages[taskid]);
        pthread_exit(NULL);
    }

    int main(int argc, char *argv[])
    {
        pthread_t threads[NUM_THREADS];
        int *taskids[NUM_THREADS];
        int rc, t;

        messages[0] = "English: Hello World!";
        messages[1] = "French: Bonjour, le monde!";
        messages[2] = "Spanish: Hola al mundo";
        messages[3] = "Klingon: Nuq neH!";
        messages[4] = "German: Guten Tag, Welt!"; 
        messages[5] = "Russian: Zdravstvytye, mir!";
        messages[6] = "Japan: Sekai e konnichiwa!";
        messages[7] = "Latin: Orbis, te saluto!";

        for(t=0;t<NUM_THREADS;t++) {
            taskids[t] = (int *) malloc(sizeof(int));
            *taskids[t] = t;
            printf("Creating thread %d\n", t);
            rc = pthread_create(&threads[t], NULL, PrintHello, (void *) taskids[t]);
            if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }
        }

        pthread_exit(NULL);
    }

#### Example 2 - Thread Argument Passing
#### 例子2——线程参数传递

This example shows how to setup/pass multiple arguments via a structure. Each thread receives a unique instance of the structure.

这个例子展示了如何通过结构设置/传递多个参数。每个线程接收一个独一无二的结构实例。

    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    #define NUM_THREADS 8

    char *messages[NUM_THREADS];

    struct thread_data
    {
        int thread_id;
        int  sum;
        char *message;
    };

    struct thread_data thread_data_array[NUM_THREADS];

    void *PrintHello(void *threadarg)
    {
        int taskid, sum;
        char *hello_msg;
        struct thread_data *my_data;

        sleep(1);
        my_data = (struct thread_data *) threadarg;
        taskid = my_data->thread_id;
        sum = my_data->sum;
        hello_msg = my_data->message;
        printf("Thread %d: %s  Sum=%d\n", taskid, hello_msg, sum);
        pthread_exit(NULL);
    }

    int main(int argc, char *argv[])
    {
        pthread_t threads[NUM_THREADS];
        int *taskids[NUM_THREADS];
        int rc, t, sum;

        sum=0;
        messages[0] = "English: Hello World!";
        messages[1] = "French: Bonjour, le monde!";
        messages[2] = "Spanish: Hola al mundo";
        messages[3] = "Klingon: Nuq neH!";
        messages[4] = "German: Guten Tag, Welt!"; 
        messages[5] = "Russian: Zdravstvytye, mir!";
        messages[6] = "Japan: Sekai e konnichiwa!";
        messages[7] = "Latin: Orbis, te saluto!";

        for(t=0;t<NUM_THREADS;t++) {
            sum = sum + t;
            thread_data_array[t].thread_id = t;
            thread_data_array[t].sum = sum;
            thread_data_array[t].message = messages[t];
            printf("Creating thread %d\n", t);
            rc = pthread_create(&threads[t], NULL, PrintHello, (void *) 
                &thread_data_array[t]);
            if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }
        }
        pthread_exit(NULL);
    }

#### Example 3 - Thread Argument Passing (Incorrect)
#### 例子3——线程参数传递（不正确）

This example performs argument passing incorrectly. It passes the address of variable t, which is shared memory space and visible to all threads. As the loop iterates, the value of this memory location changes, possibly before the created threads can access it.

这个例子不正确地执行了参数传递。它传递了变量t的地址，该地址是共享内存空间并且对所有线程可见。随着循环的迭代，该内存地址的值不断变更，很有可能在创建的线程访问它之前发生变更。

    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    #define NUM_THREADS     8

    void *PrintHello(void *threadid)
    {
        long taskid;
        sleep(1);
        taskid = *(long *)threadid;
        printf("Hello from thread %ld\n", taskid);
        pthread_exit(NULL);
    }

    int main(int argc, char *argv[])
    {
        pthread_t threads[NUM_THREADS];
        int rc;
        long t;

        for(t=0;t<NUM_THREADS;t++) {
            printf("Creating thread %ld\n", t);
            rc = pthread_create(&threads[t], NULL, PrintHello, (void *) &t);
            if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }
        }

        pthread_exit(NULL);
    }

## Thread Management
## 线程管理

### Joining and Detaching Threads
### 连接和分离线程

#### Routines:
#### 函数：

[pthread_join](https://computing.llnl.gov/tutorials/pthreads/man/pthread_join.txt) (threadid,status)

[pthread_detach](https://computing.llnl.gov/tutorials/pthreads/man/pthread_detach.txt) (threadid)

[pthread_attr_setdetachstate](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setdetachstate.txt) (attr,detachstate)

[pthread_attr_getdetachstate](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getdetachstate.txt) (attr,detachstate) 

#### Joining:
#### 连接：

- "Joining" is one way to accomplish synchronization between threads. For example:
- “连接”是一种完成线程之间同步的方式。例如：

![Joining](./tutorial-pthreads/joining.gif)

- The pthread_join() subroutine blocks the calling thread until the specified threadid thread terminates.
- pthread_join子函数会阻塞调用线程直至指定id的线程终止。
- The programmer is able to obtain the target thread's termination return status if it was specified in the target thread's call to pthread_exit().
- 程序员能够获取目标线程的终止返回状态，如果目标线程在调用pthread_exit时指定了返回状态。
- A joining thread can match one pthread_join() call. It is a logical error to attempt multiple joins on the same thread.
- 一个连接线程能够匹配一个pthread_join调用。尝试在同一个线程上进行多次连接是一个逻辑错误。
- Two other synchronization methods, mutexes and condition variables, will be discussed later.
- 另外两种同步方法——互斥体与条件变量——将会在稍后讨论。

#### Joinable or Not?
#### 可连接还是不可连接？

- When a thread is created, one of its attributes defines whether it is joinable or detached. Only threads that are created as joinable can be joined. If a thread is created as detached, it can never be joined.
- 当一个线程被创建时，它的属性中的某一项定义了它是可连接还是已分离。只有那些创建时设定为可连接的线程才能被连接。如果一个线程创建时设定为已分离，那么它永远不可能被连接。
- The final draft of the POSIX standard specifies that threads should be created as joinable.
- POSIX标准的最终草稿规定，线程被创建时应该设定为可连接。
- To explicitly create a thread as joinable or detached, the attr argument in the pthread_create() routine is used. The typical 4 step process is:
- 为了明确地创建一个可连接或者已分离的线程，需要用到pthread_create函数的attr参数。典型的4步为：
    1. Declare a pthread attribute variable of the pthread_attr_t data type
    1. 声明一个数据类型为pthread_attr_t的线程属性变量
    2. Initialize the attribute variable with pthread_attr_init()
    2. 用pthread_attr_init函数来初始化这个属性变量
    3. Set the attribute detached status with pthread_attr_setdetachstate()
    3. 用pthread_attr_setdetachstate函数设置属性的分离状态
    4. When done, free library resources used by the attribute with pthread_attr_destroy()
    4. 使用完毕时，用pthread_attr_destroy函数释放属性使用的库资源

#### Detaching:
#### 分离：

- The pthread_detach() routine can be used to explicitly detach a thread even though it was created as joinable.
- pthread_detach函数可以被用来明确地分离一个即使创建时设定为可连接的线程。
- There is no converse routine.
- 不存在反向的函数。

#### Recommendations:
#### 建议：

- If a thread requires joining, consider explicitly creating it as joinable. This provides portability as not all implementations may create threads as joinable by default.
- 如果一个线程需要连接，考虑明确地创建成可连接的。这样子提供了可移植性，因为不是所有实现都默认创建可连接的线程。
- If you know in advance that a thread will never need to join with another thread, consider creating it in a detached state. Some system resources may be able to be freed.
- 如果你预先知道一个线程将永远不会需要与其他线程连接，考虑以分离状态创建它。某些系统资源就可能被释放。

### Example: Pthread Joining
### 例子：Pthread连接

#### Example Code - Pthread Joining
#### 示例代码——Pthread连接

This example demonstrates how to "wait" for thread completions by using the Pthread join routine. Since some implementations of Pthreads may not create threads in a joinable state, the threads in this example are explicitly created in a joinable state so that they can be joined later.

这个例子展示了如何使用Pthread的join函数来“等待”线程完成。由于某些Pthread实现可能不是以可连接状态创建线程，故本例子里的线程明确地以可连接状态创建，这样一来之后它们就能被连接。

    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #define NUM_THREADS 4

    void *BusyWork(void *t)
    {
        int i;
        long tid;
        double result=0.0;
        tid = (long)t;
        printf("Thread %ld starting...\n",tid);
        for (i=0; i<1000000; i++)
        {
            result = result + sin(i) * tan(i);
        }
        printf("Thread %ld done. Result = %e\n",tid, result);
        pthread_exit((void*) t);
    }

    int main (int argc, char *argv[])
    {
        pthread_t thread[NUM_THREADS];
        pthread_attr_t attr;
        int rc;
        long t;
        void *status;

        /* Initialize and set thread detached attribute */
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        for(t=0; t<NUM_THREADS; t++) {
            printf("Main: creating thread %ld\n", t);
            rc = pthread_create(&thread[t], &attr, BusyWork, (void *)t); 
            if (rc) {
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }
        }

        /* Free attribute and wait for the other threads */
        pthread_attr_destroy(&attr);
        for(t=0; t<NUM_THREADS; t++) {
            rc = pthread_join(thread[t], &status);
            if (rc) {
                printf("ERROR; return code from pthread_join() is %d\n", rc);
                exit(-1);
            }
            printf("Main: completed join with thread %ld having a status of %ld\n",t,(long)status);
        }

        printf("Main: program completed. Exiting.\n");
        pthread_exit(NULL);
    }

## Thread Management
## 线程管理

### Stack Management
### 栈管理

#### Routines:
#### 函数：

[pthread_attr_getstacksize](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getstacksize.txt) (attr, stacksize)

[pthread_attr_setstacksize](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setstacksize.txt) (attr, stacksize)

[pthread_attr_getstackaddr](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getstackaddr.txt) (attr, stackaddr)

[pthread_attr_setstackaddr](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setstackaddr.txt) (attr, stackaddr) 

#### Preventing Stack Problems:
#### 避免栈问题：

- The POSIX standard does not dictate the size of a thread's stack. This is implementation dependent and varies.
- POSIX标准没有指示一个线程的栈的大小。这依赖于实现，因而可变。
- Exceeding the default stack limit is often very easy to do, with the usual results: program termination and/or corrupted data.
- 超出默认的栈界限通常很容易就办到了，它的通常结果是程序终止或者损坏的数据。
- Safe and portable programs do not depend upon the default stack limit, but instead, explicitly allocate enough stack for each thread by using the pthread_attr_setstacksize routine.
- 安全并且可移植的程序不依赖于默认的栈界限，取而代之的是使用pthread_attr_setstacksize函数明确地为每个线程分配足够的栈空间。
- The pthread_attr_getstackaddr and pthread_attr_setstackaddr routines can be used by applications in an environment where the stack for a thread must be placed in some particular region of memory.
- 当应用的环境要求一个线程的栈必须位于某个特定内存范围时，应用可以使用pthread_attr_getstackaddr和pthread_attr_setstackaddr函数。

#### Some Practical Examples at LC:
#### 在LC上的一些实用例子：

- Default thread stack size varies greatly. The maximum size that can be obtained also varies greatly, and may depend upon the number of threads per node.
- 默认的线程栈大小差异巨大。可以获取的最大大小同样差异巨大，而且有可以依赖于每个节点上的线程数。
- Both past and present architectures are shown to demonstrate the wide variation in default thread stack size.
- 下表中包含过去和现在的架构被用来展示默认线程栈大小的巨大差异。

<table>
    <thead>
        <tr>
            <th>节点架构</th>
            <th>CPU数量</th>
            <th>内存（GB）</th>
            <th>默认大小（bytes）</th>
        </tr>
    </thead>
        <tr>
            <td>Intel Xeon E5-2670</td>
            <td>16</td>
            <td>32</td>
            <td>2,097,152</td>
        </tr>
        <tr>
            <td>Intel Xeon 5660</td>
            <td>12</td>
            <td>24</td>
            <td>2,097,152</td>
        </tr>
        <tr>
            <td>AMD Opteron</td>
            <td>8</td>
            <td>16</td>
            <td>2,097,152</td>
        </tr>
        <tr>
            <td>Intel IA64</td>
            <td>4</td>
            <td>8</td>
            <td>33,554,432</td>
        </tr>
        <tr>
            <td>Intel IA32</td>
            <td>2</td>
            <td>4</td>
            <td>2,097,152</td>
        </tr>
        <tr>
            <td>IBM Power5</td>
            <td>8</td>
            <td>32</td>
            <td>196,608</td>
        </tr>
        <tr>
            <td>IBM Power4</td>
            <td>8</td>
            <td>16</td>
            <td>196,608</td>
        </tr>
        <tr>
            <td>IBM Power3</td>
            <td>16</td>
            <td>16</td>
            <td>98,304</td>
        </tr>
    <tbody>
    </tbody>
</table>

### Example: Stack Management
### 例子：栈管理

#### Example Code - Stack Management
#### 示例代码——栈管理

This example demonstrates how to query and set a thread's stack size.

这个例子展示了如何查询和设置一个线程的栈大小。

    #include <pthread.h>
    #include <stdio.h>
    #define NTHREADS 4
    #define N 1000
    #define MEGEXTRA 1000000

    pthread_attr_t attr;

    void *dowork(void *threadid)
    {
        double A[N][N];
        int i,j;
        long tid;
        size_t mystacksize;

        tid = (long)threadid;
        pthread_attr_getstacksize (&attr, &mystacksize);
        printf("Thread %ld: stack size = %li bytes \n", tid, mystacksize);
        for (i=0; i<N; i++)
            for (j=0; j<N; j++)
                A[i][j] = ((i*j)/3.452) + (N-i);
        pthread_exit(NULL);
    }

    int main(int argc, char *argv[])
    {
        pthread_t threads[NTHREADS];
        size_t stacksize;
        int rc;
        long t;

        pthread_attr_init(&attr);
        pthread_attr_getstacksize (&attr, &stacksize);
        printf("Default stack size = %li\n", stacksize);
        stacksize = sizeof(double)*N*N+MEGEXTRA;
        printf("Amount of stack needed per thread = %li\n",stacksize);
        pthread_attr_setstacksize (&attr, stacksize);
        printf("Creating threads with stack size = %li bytes\n",stacksize);
        for(t=0; t<NTHREADS; t++){
            rc = pthread_create(&threads[t], &attr, dowork, (void *)t);
            if (rc){
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }
        }
        printf("Created %ld threads.\n", t);
        pthread_exit(NULL);
    }

## Thread Management
## 线程管理

### Miscellaneous Routines
### 杂项函数

[pthread_self](https://computing.llnl.gov/tutorials/pthreads/man/pthread_self.txt) ()

[pthread_equal](https://computing.llnl.gov/tutorials/pthreads/man/pthread_equal.txt) (thread1,thread2) 

- pthread_self returns the unique, system assigned thread ID of the calling thread.
- pthread_self返回调用函数自身的一个独一无二的、系统赋予的线程ID。
- pthread_equal compares two thread IDs. If the two IDs are different 0 is returned, otherwise a non-zero value is returned.
- pthread_equal 比较两个线程ID。如果这两个ID不相同那么返回0，否则返回一个非0值。
- Note that for both of these routines, the thread identifier objects are opaque and can not be easily inspected. Because thread IDs are opaque objects, the C language equivalence operator == should not be used to compare two thread IDs against each other, or to compare a single thread ID against another value.
- 注意：对于这两个函数，线程标识符对象是不透明的，无法轻易地检查。因为线程ID是不透明对象，不应该使用C语言中的相等操作符==来比较两个线程ID。

[pthread_once](https://computing.llnl.gov/tutorials/pthreads/man/pthread_once.txt) (once_control, init_routine) 

- pthread_once executes the init_routine exactly once in a process. The first call to this routine by any thread in the process executes the given init_routine, without parameters. Any subsequent call will have no effect.
- 在一个进程中，pthread_once 恰好执行init_routine一次。由进程中的任何线程引起的对这个函数的第一次调用会无参地执行给定的init_routine。任何随后的调用都将不会起作用。
- The init_routine routine is typically an initialization routine.
- init_routine函数一般是一个初始化函数。
- The once_control parameter is a synchronization control structure that requires initialization prior to calling pthread_once. For example:
- once_control参数是一个同步控制结构，在调用pthread_once之前需要初始化。例如：

    pthread_once_t once_control = PTHREAD_ONCE_INIT;

## Pthread Exercise 1
## pthread练习1

### Getting Started and Thread Management Routines
### 准备开始以及线程管理函数

#### Overview:
#### 概览：

- Login to an LC cluster using your workshop username and OTP token
- 使用你的讲习班用户名和OTP令牌登录一台LC集群
- Copy the exercise files to your home directory
- 复制练习文件到你的家目录
- Familiarize yourself with LC's Pthreads environment
- 让你自己熟悉LC的pthread环境
- Write a simple "Hello World" Pthreads program 
- 编写一个简单的“Hello World”pthread程序
- Successfully compile your program
- 成功地编译你的程序
- Successfully run your program - several different ways 
- 成功地运行你的程序——以多种不同的方式
- Review, compile, run and/or debug some related Pthreads programs (provided) 
- 检查、编译、运行以及调试一些相关的Pthread程序（已提供）

[GO TO THE EXERCISE HERE](https://computing.llnl.gov/tutorials/pthreads/exercise.html#Exercise1)

[点击此处跳转到练习](https://computing.llnl.gov/tutorials/pthreads/exercise.html#Exercise1)

## Mutex Variables
## 互斥体变量

### Overview
### 概述

- Mutex is an abbreviation for "mutual exclusion". Mutex variables are one of the primary means of implementing thread synchronization and for protecting shared data when multiple writes occur.
- Mutex是“mutual exclusion”的缩写。互斥体变量是实现线程同步以及当多次写入时保护共享数据的基本方法中的一种。
- A mutex variable acts like a "lock" protecting access to a shared data resource. The basic concept of a mutex as used in Pthreads is that only one thread can lock (or own) a mutex variable at any given time. Thus, even if several threads try to lock a mutex only one thread will be successful. No other thread can own that mutex until the owning thread unlocks that mutex. Threads must "take turns" accessing protected data. 
- 一个互斥体变量就像一个“锁”保护对共享数据资源的访问。Pthread中使用的基本的互斥体概念就是在任意时刻只有一个线程能锁住（或者拥有）一个互斥体变量。因此，尽管有许多线程尝试锁住一个互斥体，只有一个线程会成功。没有其他任何线程能拥有那个互斥体，直到持有线程解锁那个互斥体。线程必须“轮流”地访问被保护的数据。
- Mutexes can be used to prevent "race" conditions. An example of a race condition involving a bank transaction is shown below: 
- 互斥体可被用于阻止“竞态”条件。下面展示了一个涉及银行事务往来的竞态条件的例子：

<table>
    <thead>
        <tr>
            <th>线程1</th>
            <th>线程2</th>
            <th>余额</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>读取余额：1000美元</td>
            <td></td>
            <td>1000美元</td>
        </tr>
        <tr>
            <td></td>
            <td>读取余额：1000美元</td>
            <td>1000美元</td>
        </tr>
        <tr>
            <td></td>
            <td>存入200美元</td>
            <td>1000美元</td>
        </tr>
        <tr>
            <td>存入200美元</td>
            <td></td>
            <td>1000美元</td>
        </tr>
        <tr>
            <td>更新余额1000美元+200美元</td>
            <td></td>
            <td>1200美元</td>
        </tr>
        <tr>
            <td></td>
            <td>更新余额1000美元+200美元</td>
            <td>1200美元</td>
        </tr>
    </tbody>
</table>

- In the above example, a mutex should be used to lock the "Balance" while a thread is using this shared data resource.
- 在上面的例子中，当一个线程正在使用这个共享数据资源时，必须得使用一个互斥体来锁住“余额”。
- Very often the action performed by a thread owning a mutex is the updating of global variables. This is a safe way to ensure that when several threads update the same variable, the final value is the same as what it would be if only one thread performed the update. The variables being updated belong to a "critical section".
- 一个拥有互斥体的线程经常干的事情就是更新全局变量。这是一种安全的方式来确保当有多个线程更新同一个变量时，最终值就像只有一个线程干了全部的更新操作。这个变量更新操作属于一个“临界区”。
- A typical sequence in the use of a mutex is as follows:
- 使用互斥体的一个典型操作序列就像下面这样：
    - Create and initialize a mutex variable
    - 创建并且初始化一个互斥体变量
    - Several threads attempt to lock the mutex
    - 多个线程试图锁定这个互斥体
    - Only one succeeds and that thread owns the mutex 
    - 只有一个线程成功并且拥有这个互斥体
    - The owner thread performs some set of actions 
    - 这个拥有者线程执行一系列动作
    - The owner unlocks the mutex
    - 这个拥有者线程解锁这个互斥体
    - Another thread acquires the mutex and repeats the process 
    - 其他线程获得这个互斥体然后重复这个过程
    - Finally the mutex is destroyed 
    - 最终这个互斥体被销毁
- When several threads compete for a mutex, the losers block at that call - an unblocking call is available with "trylock" instead of the "lock" call. 
- 当有多个线程为一个互斥体竞争时，竞争失败者被阻塞在那个调用上——存在非阻塞调用“trylock”,可用来代替“lock”。
- When protecting shared data, it is the programmer's responsibility to make sure every thread that needs to use a mutex does so. For example, if 4 threads are updating the same data, but only one uses a mutex, the data can still be corrupted.
- 当保护共享数据时，编程人员有责任去确保每个需要使用互斥体的线程这样做。例如，如果有4个线程正在更新同一个数据，但是只有一个使用了互斥体，那么数据依然可能被损坏。

## Mutex Variables
## 互斥体变量

### Creating and Destroying Mutexes
### 创建和销毁互斥体

#### Routines:
#### 函数：

[pthread_mutex_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_init.txt) (mutex,attr)

[pthread_mutex_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_destroy.txt) (mutex)

[pthread_mutexattr_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_init.txt) (attr)

[pthread_mutexattr_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_destroy.txt) (attr) 

#### Usage:
#### 用法：

- Mutex variables must be declared with type pthread_mutex_t, and must be initialized before they can be used. There are two ways to initialize a mutex variable:
- 互斥体变量必须以pthread_mutex_t类型来声明，而且在使用之前必须被初始化。有两种方式初始化一个互斥体变量：
    1. Statically, when it is declared. For example: `pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;`
    1. 静态地，当它声明时。示例：`pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;`
    2. Dynamically, with the pthread_mutex_init() routine. This method permits setting mutex object attributes, attr. 
    2. 动态地，通过pthread_mutex_init函数。这种方法允许设置互斥体对象属性——attr。

The mutex is initially unlocked. 

互斥体被初始化为未锁定。

- The attr object is used to establish properties for the mutex object, and must be of type pthread_mutexattr_t if used (may be specified as NULL to accept defaults). The Pthreads standard defines three optional mutex attributes:
- attr对象用于设置mutex对象的属性，并且如果有用到则必须是pthread_mutexattr_t类型（亦可置为NULL以接受默认值）。Pthread标准定义了3个可选的互斥体属性：
    - Protocol: Specifies the protocol used to prevent priority inversions for a mutex. 
    - 协议：指定互斥体的用于避免优先级反转的协议。
    - Prioceiling: Specifies the priority ceiling of a mutex. 
    - 优先级封顶：指定互斥体的优先级封顶。
    - Process-shared: Specifies the process sharing of a mutex. 
    - 进程共享：指定互斥体的进程共享。

Note that not all implementations may provide the three optional mutex attributes. 

注意：不是所有实现都会提供这三个可选的互斥体属性。

- The pthread_mutexattr_init() and pthread_mutexattr_destroy() routines are used to create and destroy mutex attribute objects respectively. 
- pthread_mutexattr_init和pthread_mutexattr_destroy函数分别用于创建和销毁互斥体属性对象。
- pthread_mutex_destroy() should be used to free a mutex object which is no longer needed. 
- 应该用pthread_mutex_destroy函数来释放一个不再需要的互斥体对象。

## Mutex Variables
## 互斥体变量

### Locking and Unlocking Mutexes
### 锁定与解锁互斥体

#### Routines:
#### 函数：

[pthread_mutex_lock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_lock.txt) (mutex)

[pthread_mutex_trylock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_trylock.txt) (mutex)

[pthread_mutex_unlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_unlock.txt) (mutex) 

#### Usage:
#### 用法：

- The pthread_mutex_lock() routine is used by a thread to acquire a lock on the specified mutex variable. If the mutex is already locked by another thread, this call will block the calling thread until the mutex is unlocked. 
- pthread_mutex_lock函数用于让一个线程在指定的互斥体变量上获得锁。如果这个互斥体已经被其他线程锁定，该调用将会阻塞调用线程直至这个互斥体被解锁。
- pthread_mutex_trylock() will attempt to lock a mutex. However, if the mutex is already locked, the routine will return immediately with a "busy" error code. This routine may be useful in preventing deadlock conditions, as in a priority-inversion situation. 
- pthread_mutex_trylock函数将会试图去锁定一个互斥体。然而，如果该互斥体已经被锁定，这个函数将会立即返回一个指示“忙”的错误码。在一个优先级反转的情况下，这个函数可能有助于预防死锁条件。
- pthread_mutex_unlock() will unlock a mutex if called by the owning thread. Calling this routine is required after a thread has completed its use of protected data if other threads are to acquire the mutex for their work with the protected data. An error will be returned if:
- pthread_mutex_unlock函数将会解锁一个互斥体，如果被锁拥有者线程调用。在一个线程完成保护数据的使用之后，如果其他线程将要获取这个互斥体以完成涉级保护数据的工作，那么需要调用这个函数。如下情况将会返回一个错误：
    - If the mutex was already unlocked
    - 如果互斥体已经被解锁
    - If the mutex is owned by another thread
    - 如果互斥体被其他线程拥有
- There is nothing "magical" about mutexes...in fact they are akin to a "gentlemen's agreement" between participating threads. It is up to the code writer to insure that the necessary threads all make the the mutex lock and unlock calls correctly. The following scenario demonstrates a logical error: 
- 关于互斥体不存在“魔法”。事实上它们类型于参与线程之间的“君子协定”。这取决于代码编写者保证必要的线程都正确地调用互斥体加锁、解锁函数。下面的场景展示了一个逻辑错误：

<br />

    Thread 1     Thread 2     Thread 3
    Lock         Lock         
    A = 2        A = A+1      A = A*B
    Unlock       Unlock    

Question: When more than one thread is waiting for a locked mutex, which thread will be granted the lock first after it is released? 

问题：当超过1个以上的线程等待一个锁定的互斥体时，哪个线程将会在互斥体解锁后被授予这个锁？

Answer: Unless thread priority scheduling (not covered) is used, the assignment will be left to the native system scheduler and may appear to be more or less random.

答案：除非使用了线程优先级调度（未涉及），赋予操作将会留给本地系统调度者，看似或多或少是随机的。

### Example: Using Mutexes
### 例子：使用互斥体

#### Example Code - Using Mutexes
#### 示例代码——使用互斥体

This example program illustrates the use of mutex variables in a threads program that performs a dot product. The main data is made available to all threads through a globally accessible structure. Each thread works on a different part of the data. The main thread waits for all the threads to complete their computations, and then it prints the resulting sum. 

这个示例程序说明了在一个求点积的多线程程序中互斥体变量的用法。主要数据通过一个全局可访问结构使得所有线程都可使用。每个线程工作于数据的不同部分。主线程等待所有线程完成它们的计算，然后打印作为结果的总和。

    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>

    /*   
    The following structure contains the necessary information  
    to allow the function "dotprod" to access its input data and 
    place its output into the structure.  
    */

    typedef struct 
    {
        double      *a;
        double      *b;
        double     sum; 
        int     veclen; 
    } DOTDATA;

    /* Define globally accessible variables and a mutex */

    #define NUMTHRDS 4
    #define VECLEN 100
    DOTDATA dotstr; 
    pthread_t callThd[NUMTHRDS];
    pthread_mutex_t mutexsum;

    /*
    The function dotprod is activated when the thread is created.
    All input to this routine is obtained from a structure 
    of type DOTDATA and all output from this function is written into
    this structure. The benefit of this approach is apparent for the 
    multi-threaded program: when a thread is created we pass a single
    argument to the activated function - typically this argument
    is a thread number. All  the other information required by the 
    function is accessed from the globally accessible structure. 
    */

    void *dotprod(void *arg)
    {

        /* Define and use local variables for convenience */

        int i, start, end, len ;
        long offset;
        double mysum, *x, *y;
        offset = (long)arg;

        len = dotstr.veclen;
        start = offset*len;
        end   = start + len;
        x = dotstr.a;
        y = dotstr.b;

        /*
        Perform the dot product and assign result
        to the appropriate variable in the structure. 
        */

        mysum = 0;
        for (i=start; i<end ; i++) 
        {
            mysum += (x[i] * y[i]);
        }

        /*
        Lock a mutex prior to updating the value in the shared
        structure, and unlock it upon updating.
        */
        pthread_mutex_lock (&mutexsum);
        dotstr.sum += mysum;
        pthread_mutex_unlock (&mutexsum);

        pthread_exit((void*) 0);
    }

    /* 
    The main program creates threads which do all the work and then 
    print out result upon completion. Before creating the threads,
    the input data is created. Since all threads update a shared structure, 
    we need a mutex for mutual exclusion. The main thread needs to wait for
    all threads to complete, it waits for each one of the threads. We specify
    a thread attribute value that allow the main thread to join with the
    threads it creates. Note also that we free up handles when they are
    no longer needed.
    */

    int main (int argc, char *argv[])
    {
        long i;
        double *a, *b;
        void *status;
        pthread_attr_t attr;

        /* Assign storage and initialize values */
        a = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
        b = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));

        for (i=0; i<VECLEN*NUMTHRDS; i++)
        {
            a[i]=1.0;
            b[i]=a[i];
        }

        dotstr.veclen = VECLEN; 
        dotstr.a = a; 
        dotstr.b = b; 
        dotstr.sum=0;

        pthread_mutex_init(&mutexsum, NULL);

        /* Create threads to perform the dotproduct  */
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        for(i=0; i<NUMTHRDS; i++)
        {
            /* 
            Each thread works on a different set of data.
            The offset is specified by 'i'. The size of
            the data for each thread is indicated by VECLEN.
            */
            pthread_create(&callThd[i], &attr, dotprod, (void *)i);
        }

        pthread_attr_destroy(&attr);

        /* Wait on the other threads */
        for(i=0; i<NUMTHRDS; i++)
        {
            pthread_join(callThd[i], &status);
        }

        /* After joining, print out the results and cleanup */
        printf ("Sum =  %f \n", dotstr.sum);
        free (a);
        free (b);
        pthread_mutex_destroy(&mutexsum);
        pthread_exit(NULL);
    }

## Condition Variables
## 条件变量

### Overview
### 概述

- Condition variables provide yet another way for threads to synchronize. While mutexes implement synchronization by controlling thread access to data, condition variables allow threads to synchronize based upon the actual value of data. 
- 条件变量给线程同步提供了另一种方法。虽然互斥体通过控制线程访问数据来实现同步，但是条件变量允许线程依据数据的实际值来同步。
- Without condition variables, the programmer would need to have threads continually polling (possibly in a critical section), to check if the condition is met. This can be very resource consuming since the thread would be continuously busy in this activity. A condition variable is a way to achieve the same goal without polling. 
- 如果没有条件变量，那么程序员需要让线程持续地轮询（很可能位于一个临界区里），来检查条件是否满足。这将会是非常消耗资源的，因为线程将会忙于这个轮询活动。条件变量是一种达到相同目的却无需轮询的方法。
- A condition variable is always used in conjunction with a mutex lock. 
- 条件变量总是结合互斥锁一起使用。
- A representative sequence for using condition variables is shown below. 
- 一个典型的使用条件变量操作序列显示如下。

<table width="100%">
    <tbody>
        <tr>
            <td colspan="2">
                <h3>Main Thread</h3>
                <ul>
                    <li>Declare and initialize global data/variables which require synchronization (such as "count") </li>
                    <li>声明和初始化需要同步的全局数据/变量（例如“count”）</li>
                    <li>Declare and initialize a condition variable object </li>
                    <li>声明和初始化一个条件变量对象</li>
                    <li>Declare and initialize an associated mutex</li>
                    <li>声明和初始化一个关联的互斥体</li>
                    <li>Create threads A and B to do work</li>
                    <li>创建线程A和线程B去做苦工</li>
                </ul>
            </td>
        </tr>
        <tr>
            <td>
                <h3>Thread A</h3>
                <ul>
                    <li>Do work up to the point where a certain condition must occur (such as "count" must reach a specified value) </li>
                    <li>做苦工直到一个特定的条件出现（例如“count”必须达到一个指定值）</li>
                    <li>Lock associated mutex and check value of a global variable</li>
                    <li>锁定相关联的互斥体并且检查全局的变量的值</li>
                    <li>Call pthread_cond_wait() to perform a blocking wait for signal from Thread-B. Note that a call to pthread_cond_wait() automatically and atomically unlocks the associated mutex variable so that it can be used by Thread-B. </li>
                    <li>调用pthread_cond_wait函数以执行一个阻塞的等待，等待来自Thread-B的信号。注意：对pthread_cond_wait的调用会自动地、原子地解锁相关联的互斥体变量，这样一来Thread-B就能使用它。</li>
                    <li>When signalled, wake up. Mutex is automatically and atomically locked. </li>
                    <li>当收到信号时，醒过来。互斥体被自动地、原子地锁定。</li>
                    <li>Explicitly unlock mutex</li>
                    <li>明确地解锁互斥体</li>
                    <li>Continue</li>
                    <li>继续</li>
                </ul>
            </td>
            <td>
                <h3>Thread B</h3>
                <ul>
                    <li>Do work </li>
                    <li>做工作</li>
                    <li>Lock associated mutex </li>
                    <li>锁定相关联的互斥体</li>
                    <li>Change the value of the global variable that Thread-A is waiting upon.</li>
                    <li>改变线程A正在等待的全局变量</li>
                    <li>Check value of the global Thread-A wait variable. If it fulfills the desired condition, signal Thread-A. </li>
                    <li>检查线程A等待的全局变量的值。如果它满足所需条件，发信号给线程A。</li>
                    <li>Unlock mutex.</li>
                    <li>解锁互斥体</li>
                    <li>Continue</li>
                    <li>继续</li>
                </ul>
            </td>
        </tr>
        <tr>
            <td colspan="2">
                <h3>Main Thread</h3>
                <ul>
                    <li>Join / Continue </li>
                    <li>连接/继续</li>
                </ul>
            </td>
        </tr>
    </tbody>
</table>

## Condition Variables
## 条件变量

### Creating and Destroying Condition Variables
### 创建和销毁条件变量

#### Routines:
#### 函数：

[pthread_cond_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_init.txt) (condition,attr)

[pthread_cond_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_destroy.txt) (condition)

[pthread_condattr_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_condattr_init.txt) (attr)

[pthread_condattr_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_condattr_destroy.txt) (attr) 

#### Usage:
#### 用法：

- Condition variables must be declared with type pthread_cond_t, and must be initialized before they can be used. There are two ways to initialize a condition variable: 
- 条件变量必须以pthread_cond_t类型声明，而且必须在使用之前初始化。有两种方法初始化一个条件变量：
    1. Statically, when it is declared. For example: `pthread_cond_t myconvar = PTHREAD_COND_INITIALIZER;`
    1. 静态地，当它被声明时。例如：`pthread_cond_t myconvar = PTHREAD_COND_INITIALIZER;`
    2. Dynamically, with the pthread_cond_init() routine. The ID of the created condition variable is returned to the calling thread through the condition parameter. This method permits setting condition variable object attributes, attr. 
    2. 动态地，利用pthread_cond_init函数。被创建的条件变量的ID通过condition参数被返回调用线程。这个方法允许设置条件变量的属性，attr。
- The optional attr object is used to set condition variable attributes. There is only one attribute defined for condition variables: process-shared, which allows the condition variable to be seen by threads in other processes. The attribute object, if used, must be of type pthread_condattr_t (may be specified as NULL to accept defaults). 
- 可选的attr对象用于设置条件变量属性。只存在一个条件变量属性：process-shared，该属性允许条件变量能被其他进程中的线程看到。如果用到属性对象，那么它必须是pthread_condattr_t类型（也可以置为NULL来接受默认值）。

Note that not all implementations may provide the process-shared attribute.

注意：不是所有实现都提供了process-shared属性。

- The pthread_condattr_init() and pthread_condattr_destroy() routines are used to create and destroy condition variable attribute objects. 
- pthread_condattr_init和pthread_condattr_destroy函数用于创建和销毁条件变量属性对象。
- pthread_cond_destroy() should be used to free a condition variable that is no longer needed.
- 当一个条件变量不再需要时，应该使用pthread_cond_destroy函数销毁它。

## Condition Variables
## 条件变量

### Waiting and Signaling on Condition Variables
### 在条件变量上等待和发信号

#### Routines:
#### 函数：

[pthread_cond_wait](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_wait.txt) (condition,mutex)

[pthread_cond_signal](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_signal.txt) (condition)

[pthread_cond_broadcast](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_broadcast.txt) (condition) 

#### Usage:
#### 用法：

- pthread_cond_wait() blocks the calling thread until the specified condition is signalled. This routine should be called while mutex is locked, and it will automatically release the mutex while it waits. After signal is received and thread is awakened, mutex will be automatically locked for use by the thread. The programmer is then responsible for unlocking mutex when the thread is finished with it. 
- pthread_cond_wait函数会阻塞调用线程直到指定的condition收到信号。这个函数应该在mutex是已锁定的情况下调用，当函数等待时它将会自动地解锁这个mutex。收到信号之后线程被唤醒，mutex将会被自动地上锁以供线程使用。程序员负责在线程使用完之后解锁mutex。

Recommendation: Using a WHILE loop instead of an IF statement (see watch_count routine in example below) to check the waited for condition can help deal with several potential problems, such as: 

建议：使用WHILE环境代替IF语句（见下面例子中的watch_count函数）来检查等待条件，这能帮助处理许多潜在的问题，例如：

- 
    - If several threads are waiting for the same wake up signal, they will take turns acquiring the mutex, and any one of them can then modify the condition they all waited for. 
    - 如果多个线程正在等待同一个唤醒信号，那么它们将会依次取得互斥体，然后它们中的任意一个能修改它们都在等待的条件。
    - If the thread received the signal in error due to a program bug 
    - 如果由于程序bug，线程错误地收到信号
    - The Pthreads library is permitted to issue spurious wake ups to a waiting thread without violating the standard. 
    - 在不违背标准的条件下，Pthread库是允许发出虚假的唤醒信号给一个正在等待的线程。
- The pthread_cond_signal() routine is used to signal (or wake up) another thread which is waiting on the condition variable. It should be called after mutex is locked, and must unlock mutex in order for pthread_cond_wait() routine to complete. 
- pthread_cond_signal函数用于发信号（或者说唤醒）其他正在等待这个条件变量的线程。该函数应该在mutex锁定后才能调用，并且必须解锁mutex以使pthread_cond_wait函数完成。
- The pthread_cond_broadcast() routine should be used instead of pthread_cond_signal() if more than one thread is in a blocking wait state. 
- 如果超过1个以上的线程正处于阻塞状态，那么应该使用pthread_cond_broadcast函数代替pthread_cond_signal函数。
- It is a logical error to call pthread_cond_signal() before calling pthread_cond_wait().
- 在调用pthread_cond_wait之前调用pthread_cond_signal是一个逻辑错误。

Proper locking and unlocking of the associated mutex variable is essential when using these routines. For example: 

使用这些函数时，正确地上锁和解锁相关联的互斥体变量是必要的。例如：

- Failing to lock the mutex before calling pthread_cond_wait() may cause it NOT to block. 
- 在调用pthread_cond_wait之前没能锁定互斥体可能会导致它不能阻塞。
- Failing to unlock the mutex after calling pthread_cond_signal() may not allow a matching pthread_cond_wait() routine to complete (it will remain blocked).
- 在调用pthread_cond_signal函数之后没能解锁互斥体有可能导致配对的pthread_cond_wait函数不能完成（它将会保持阻塞）。

### Example: Using Condition Variables
### 示例：使用条件变量

#### Example Code - Using Condition Variables
#### 示例代码——使用条件变量

This simple example code demonstrates the use of several Pthread condition variable routines. The main routine creates three threads. Two of the threads perform work and update a "count" variable. The third thread waits until the count variable reaches a specified value. 

这个简单的例子展示了多个Pthread条件变量函数的使用。主函数创建了3个线程。其中的2个执行苦工并且更新一个名叫“count”的变量。第三个线程等待count变量达到一个指定的值。

    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>

    #define NUM_THREADS  3
    #define TCOUNT 10
    #define COUNT_LIMIT 12

    int     count = 0;
    int     thread_ids[3] = {0,1,2};
    pthread_mutex_t count_mutex;
    pthread_cond_t count_threshold_cv;

    void *inc_count(void *t) 
    {
        int i;
        long my_id = (long)t;

        for (i=0; i<TCOUNT; i++) {
            pthread_mutex_lock(&count_mutex);
            count++;

            /* 
            Check the value of count and signal waiting thread when condition is
            reached.  Note that this occurs while mutex is locked. 
            */
            if (count == COUNT_LIMIT) {
                pthread_cond_signal(&count_threshold_cv);
                printf("inc_count(): thread %ld, count = %d  Threshold reached.\n", my_id, count);
            }
            printf("inc_count(): thread %ld, count = %d, unlocking mutex\n", my_id, count);
            pthread_mutex_unlock(&count_mutex);

            /* Do some "work" so threads can alternate on mutex lock */
            sleep(1);
        }
        pthread_exit(NULL);
    }

    void *watch_count(void *t) 
    {
        long my_id = (long)t;

        printf("Starting watch_count(): thread %ld\n", my_id);

        /*
        Lock mutex and wait for signal.  Note that the pthread_cond_wait 
        routine will automatically and atomically unlock mutex while it waits. 
        Also, note that if COUNT_LIMIT is reached before this routine is run by
        the waiting thread, the loop will be skipped to prevent pthread_cond_wait
        from never returning. 
        */
        pthread_mutex_lock(&count_mutex);
        while (count<COUNT_LIMIT) {
            pthread_cond_wait(&count_threshold_cv, &count_mutex);
            printf("watch_count(): thread %ld Condition signal received.\n", my_id);
            count += 125;
            printf("watch_count(): thread %ld count now = %d.\n", my_id, count);
        }
        pthread_mutex_unlock(&count_mutex);
        pthread_exit(NULL);
    }

    int main (int argc, char *argv[])
    {
        int i;
        long t1=1, t2=2, t3=3;
        pthread_t threads[3];
        pthread_attr_t attr;

        /* Initialize mutex and condition variable objects */
        pthread_mutex_init(&count_mutex, NULL);
        pthread_cond_init (&count_threshold_cv, NULL);

        /* For portability, explicitly create threads in a joinable state */
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&threads[0], &attr, watch_count, (void *)t1);
        pthread_create(&threads[1], &attr, inc_count, (void *)t2);
        pthread_create(&threads[2], &attr, inc_count, (void *)t3);

        /* Wait for all threads to complete */
        for (i=0; i<NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }
        printf ("Main(): Waited on %d  threads. Done.\n", NUM_THREADS);

        /* Clean up and exit */
        pthread_attr_destroy(&attr);
        pthread_mutex_destroy(&count_mutex);
        pthread_cond_destroy(&count_threshold_cv);
        pthread_exit(NULL);
    }

## Monitoring, Debugging and Performance Analysis Tools for Pthreads
## Pthread的监视、调试以及性能分析工具

### Monitoring and Debugging Pthreads:
### 监视和调试Pthread：

- Debuggers vary in their ability to handle Pthreads. The TotalView debugger is LC's recommended debugger for parallel programs. It is well suited for both monitoring and debugging threaded programs. 
- 不同的调试器处理Pthread的能力有差异。LC推荐TotalView调试器调试并行程序。它非常合适于监视多线程程序也非常合适于调试多线程程序。
- An example screenshot from a TotalView session using a threaded code is shown below.
- 下面展示了TotalView使用一个多线程代码会话的屏幕截图。
    1. Stack Trace Pane: Displays the call stack of routines that the selected thread is executing. 
    1. 堆栈踪迹窗格：显示了选中的正在执行的线程的函数调用栈。
    2. Status Bars: Show status information for the selected thread and its associated process. 
    2. 状态栏：显示选中的线程以及相关联的进程的状态信息。
    3. Stack Frame Pane: Shows a selected thread's stack variables, registers, etc. 
    3. 栈帧窗格：显示选中的线程的栈变量、寄存器等。
    4. Source Pane: Shows the source code for the selected thread.
    4. 源码窗格：显示选中线程的源代码。
    5. Root Window showing all threads 
    5. 根窗口显示了所有线程
    6. Threads Pane: Shows threads associated with the selected process 
    6. 线程窗格：显示与选中的进程关联的线程

<br />

![TotalView Debugger Screenshot](./tutorial-pthreads/totalview_debugger.gif)

- See the [TotalView Debugger tutorial](https://computing.llnl.gov/tutorials/totalview/index.html) for details. 
- 详细请看[TotalView调试器教程](https://computing.llnl.gov/tutorials/totalview/index.html)。
- The Linux ps command provides several flags for viewing thread information. Some examples are shown below. See the man page for details. 
- Linux的ps命令提供了许多用于查看线程信息的标志。下面显示了一例子。查看[man手册](https://computing.llnl.gov/tutorials/pthreads/man/ps.txt)以获取详细信息。

<br />

    % ps -Lf 
    UID        PID  PPID   LWP  C NLWP STIME TTY          TIME CMD
    blaise   22529 28240 22529  0    5 11:31 pts/53   00:00:00 a.out
    blaise   22529 28240 22530 99    5 11:31 pts/53   00:01:24 a.out
    blaise   22529 28240 22531 99    5 11:31 pts/53   00:01:24 a.out
    blaise   22529 28240 22532 99    5 11:31 pts/53   00:01:24 a.out
    blaise   22529 28240 22533 99    5 11:31 pts/53   00:01:24 a.out

    % ps -T 
      PID  SPID TTY          TIME CMD
    22529 22529 pts/53   00:00:00 a.out
    22529 22530 pts/53   00:01:49 a.out
    22529 22531 pts/53   00:01:49 a.out
    22529 22532 pts/53   00:01:49 a.out
    22529 22533 pts/53   00:01:49 a.out

    % ps -Lm 
      PID   LWP TTY          TIME CMD
    22529     - pts/53   00:18:56 a.out
        - 22529 -        00:00:00 -
        - 22530 -        00:04:44 -
        - 22531 -        00:04:44 -
        - 22532 -        00:04:44 -
        - 22533 -        00:04:44 -

- LC's Linux clusters also provide the top command to monitor processes on a node. If used with the -H flag, the threads contained within a process will be visible. An example of the top -H command is shown below. The parent process is PID 18010 which spawned three threads, shown as PIDs 18012, 18013 and 18014. 
- LC的Linux集群也提供了top命令来监视节点上的进程。如果使用-H标识，包括于进程中的线程将会可见。一个top -H命令例子结果如下所示。父进程的PID为18010，它繁衍出了三个线程，显示为PID18012、PID18013、PID18014.

![top-H](./tutorial-pthreads/top-H.gif)

#### Performance Analysis Tools:
#### 性能分析工具：

- There are a variety of performance analysis tools that can be used with threaded programs. Searching the web will turn up a wealth of information. 
- 有许多性能分析工具可被用于多线程程序。搜索网络将会出现大量的信息。
- At LC, the list of supported computing tools can be found at: [computing.llnl.gov/code/content/software_tools.php](https://computing.llnl.gov/code/content/software_tools.php). 
- 在LC，受支持的计算机工具清单可在这里找到：[computing.llnl.gov/code/content/software_tools.php](https://computing.llnl.gov/code/content/software_tools.php)
- These tools vary significantly in their complexity, functionality and learning curve. Covering them in detail is beyond the scope of this tutorial. 
- 这些工具在它们的复杂性、功能性和学习曲线上差异巨大。详细涵盖它们超出了本教程的范围。
- Some tools worth investigating, specifically for threaded codes, include: 
- 一些工具值得研究，特别是那些针对多线程代码的，包括：
    - Open|SpeedShop
    - TAU
    - PAPI 
    - Intel VTune Amplifier
    - ThreadSpotter 

## LLNL Specific Information and Recommendations
## LLNL实验室特有的信息和建议

This section describes details specific to Livermore Computing's systems. 

本节描述Livermore计算机系统的详细特征。

#### Implementations:
#### 实现：

- All LC production systems include a Pthreads implementation that follows draft 10 (final) of the POSIX standard. This is the preferred implementation. 
- 所有的LC产品系统都包含一个遵循POSIX draft 10（最终版）标准的Pthread实现。这是首选的实现。
- Implementations differ in the maximum number of threads that a process may create. They also differ in the default amount of thread stack space. 
- 不同的实现在进程能够创建的线程的最大数量上有差异。同样在默认的线程栈空间大小上有差异。

#### Compiling:
#### 编译：

- LC maintains a number of compilers, and usually several different versions of each - see the LC's [Supported Compilers](https://computing.llnl.gov/code/compilers.html) web page. 
- LC维护了大量的编译器，而且通常是每种编译器的不同版本——见LC的[受支持的编译器](https://computing.llnl.gov/code/compilers.html)网页。
- The compiler commands described in the [Compiling Threaded Programs](https://computing.llnl.gov/tutorials/pthreads/#Compiling) section apply to LC systems.
- 在[编译多线程程序](https://computing.llnl.gov/tutorials/pthreads/#Compiling)一节讲到的编译器命令适用于LC系统。

#### Mixing MPI with Pthreads:
#### 混合MPI与Pthread：

- This is the primary motivation for using Pthreads at LC. 
- 这是在LC使用Pthread的原始动机。
- Design: 
- 设计：
    - Each MPI process typically creates and then manages N threads, where N makes the best use of the available cores/node. 
    - 每个MPI进程典型地创建和管理N个线程，这N个线程使得最佳地利用可用的核心/结点。
    - Finding the best value for N will vary with the platform and your application's characteristics. 
    - 找寻这个最佳N值将会因平台和你的应用程序的特点而有所变化。
    - In general, there may be problems if multiple threads make MPI calls. The program may fail or behave unexpectedly. If MPI calls must be made from within a thread, they should be made only by one thread. 
    - 一般来说，如果多个线程进行MPI调用将会有问题。程序可能会失败或者出现未预期的行为。如果MPI调有必须在线程里调用，那么只能被一个线程调用。
- Compiling: 
- 编译：
    - Use the appropriate MPI compile command for the platform and language of choice 
    - 针对平台和语言选择合适的MPI编译命令
    - Be sure to include the required Pthreads flag as shown in the Compiling Threaded Programs section. 
    - 确认包含了如编译多线程程序一节中所示的需要的Pthread标识。
- An example code that uses both MPI and Pthreads is available below. The serial, threads-only, MPI-only and MPI-with-threads versions demonstrate one possible progression. 
- 下面有可用的同时使用了MPI和Pthread的示例代码。串行、仅线程、仅MPI、MPI与线程混合这四个版展示了一个可能的演进。
    - [Serial](https://computing.llnl.gov/tutorials/pthreads/samples/mpithreads_serial.c)
    - [Pthreads only](https://computing.llnl.gov/tutorials/pthreads/samples/mpithreads_threads.c)
    - [MPI only](https://computing.llnl.gov/tutorials/pthreads/samples/mpithreads_mpi.c)
    - [MPI with pthreads](https://computing.llnl.gov/tutorials/pthreads/samples/mpithreads_both.c)
    - [makefile](https://computing.llnl.gov/tutorials/pthreads/samples/mpithreads.makefile)

## Topics Not Covered
## 未涉及的主题

Several features of the Pthreads API are not covered in this tutorial. These are listed below. See the [Pthread Library Routines Reference](https://computing.llnl.gov/tutorials/pthreads/#AppendixA) section for more information. 

本教程没有涉及Pthread API的许多特性。这些特性罗列如下。详见[Pthread库函数参考](https://computing.llnl.gov/tutorials/pthreads/#AppendixA)一节。

- Thread Scheduling
- 线程调度
    - Implementations will differ on how threads are scheduled to run. In most cases, the default mechanism is adequate. 
    - 不同的实现在线程如何调度运行上有差别。绝大部分情况下，默认的机制是足够的。
    - The Pthreads API provides routines to explicitly set thread scheduling policies and priorities which may override the default mechanisms. 
    - Pthread API提供了函数来明确地设定线程调度策略以及优先级以覆盖默认的机制。
    - The API does not require implementations to support these features. 
    - API没有要求Pthread实现支持这些特性。
- Keys: Thread-Specific Data 
- 钥匙：线程专有数据
    - As threads call and return from different routines, the local data on a thread's stack comes and goes. 
    - 随着线程调用并从不同的函数返回，位于线程的栈上的本地数据来了又走了。
    - To preserve stack data you can usually pass it as an argument from one routine to the next, or else store the data in a global variable associated with a thread. 
    - 为了保留栈上数据你通常可以以传递参数形式从一个函数到下一个函数，或者存储数据于一个与线程关联的全局变量。
    - Pthreads provides another, possibly more convenient and versatile, way of accomplishing this through keys. 
    - Pthread提供了另外一个种——可能更加方便和通用的——方法来完成它，借助钥匙。
- Mutex Protocol Attributes and Mutex Priority Management for the handling of "priority inversion" problems. 
- 互斥体协议属性和针对“优先级反转”问题的互斥体优先级管理。
- Condition Variable Sharing - across processes
- 条件变量的共享——跨进程
- Thread Cancellation 
- 线程取消
- Threads and Signals 
- 线程与信号
- Synchronization constructs - barriers and locks 
- 同步概念——篱笆墙和锁

## Pthread Exercise 2
## Pthread练习2

### Mutexes, Condition Variables and Hybrid MPI with Pthreads

### 互斥体、条件变量以及MPI与Pthread的结合

Overview:

概览：

- Login to the LC workshop cluster, if you are not already logged in 
- 如果你还没有登录，那么登录到LC的讲习班集群
- Mutexes: review and run the provided example codes 
- 互斥体：检查并运行提供的示例代码
- Condition variables: review and run the provided example codes 
- 条件变量：检查并运行提供的示例代码
- Hybrid MPI with Pthreads: review and run the provided example codes 
- MPI与Pthread的结合：检查并运行提供的示例代码

[GO TO THE EXERCISE HERE](https://computing.llnl.gov/tutorials/pthreads/exercise.html#Exercise2)

[点击此处跳转到练习](https://computing.llnl.gov/tutorials/pthreads/exercise.html#Exercise2)

This completes the tutorial.

本教程结束。

[Evaluation Form](https://computing.llnl.gov/tutorials/evaluation/index.html)

Please complete the online evaluation form - unless you are doing the exercise, in which case please complete it at the end of the exercise.

请完成这个在线的评价问卷——除非你正在做练习，若是的话请在完成练习后完成这个评价问卷。

Where would you like to go now?

现在你想去哪里？

- [Exercise](https://computing.llnl.gov/tutorials/pthreads/exercise.html)
- [练习](https://computing.llnl.gov/tutorials/pthreads/exercise.html)
- [Agenda](https://computing.llnl.gov/tutorials/agenda/index.html)
- [日程按排](https://computing.llnl.gov/tutorials/agenda/index.html)
- [Back to the top](https://computing.llnl.gov/tutorials/pthreads/#top)
- [回到顶部](https://computing.llnl.gov/tutorials/pthreads/#top)

## References and More Information
## 参考以及更多信息

- Author: Blaise Barney, Livermore Computing. 
- 作者：Blaise Barney，Livermore Computing。
- POSIX Standard: [www.unix.org/version3/ieee_std.html](http://www.unix.org/version3/ieee_std.html)
- POSIX标准：[www.unix.org/version3/ieee_std.html](http://www.unix.org/version3/ieee_std.html)
- "Pthreads Programming". B. Nichols et al. O'Reilly and Associates. 
- "Threads Primer". B. Lewis and D. Berg. Prentice Hall 
- "Programming With POSIX Threads". D. Butenhof. Addison Wesley 
- "Programming With Threads". S. Kleiman et al. Prentice Hall 

## Appendix A: Pthread Library Routines Reference
## 附录A：Pthread库函数参考

For convenience, an alphabetical list of Pthread routines, linked to their corresponding man page, is provided below. 

为了方便，下面提供了按照字母表排序的链接到相应man手册的Pthread函数清单。

- [pthread_atfork](https://computing.llnl.gov/tutorials/pthreads/man/pthread_atfork.txt)
- [pthread_attr_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_destroy.txt)
- [pthread_attr_getdetachstate](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getdetachstate.txt)
- [pthread_attr_getguardsize](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getguardsize.txt)
- [pthread_attr_getinheritsched](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getinheritsched.txt)
- [pthread_attr_getschedparam](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getschedparam.txt)
- [pthread_attr_getschedpolicy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getschedpolicy.txt)
- [pthread_attr_getscope](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getscope.txt)
- [pthread_attr_getstack](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getstack.txt)
- [pthread_attr_getstackaddr](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getstackaddr.txt)
- [pthread_attr_getstacksize](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_getstacksize.txt)
- [pthread_attr_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_init.txt)
- [pthread_attr_setdetachstate](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setdetachstate.txt)
- [pthread_attr_setguardsize](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setguardsize.txt)
- [pthread_attr_setinheritsched](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setinheritsched.txt)
- [pthread_attr_setschedparam](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setschedparam.txt)
- [pthread_attr_setschedpolicy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setschedpolicy.txt)
- [pthread_attr_setscope](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setscope.txt)
- [pthread_attr_setstack](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setstack.txt)
- [pthread_attr_setstackaddr](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setstackaddr.txt)
- [pthread_attr_setstacksize](https://computing.llnl.gov/tutorials/pthreads/man/pthread_attr_setstacksize.txt)
- [pthread_barrier_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_barrier_destroy.txt)
- [pthread_barrier_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_barrier_init.txt)
- [pthread_barrier_wait](https://computing.llnl.gov/tutorials/pthreads/man/pthread_barrier_wait.txt)
- [pthread_barrierattr_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_barrierattr_destroy.txt)
- [pthread_barrierattr_getpshared](https://computing.llnl.gov/tutorials/pthreads/man/pthread_barrierattr_getpshared.txt)
- [pthread_barrierattr_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_barrierattr_init.txt)
- [pthread_barrierattr_setpshared](https://computing.llnl.gov/tutorials/pthreads/man/pthread_barrierattr_setpshared.txt)
- [pthread_cancel](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cancel.txt)
- [pthread_cleanup_pop](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cleanup_pop.txt)
- [pthread_cleanup_push](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cleanup_push.txt)
- [pthread_cond_broadcast](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_broadcast.txt)
- [pthread_cond_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_destroy.txt)
- [pthread_cond_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_init.txt)
- [pthread_cond_signal](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_signal.txt)
- [pthread_cond_timedwait](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_timedwait.txt)
- [pthread_cond_wait](https://computing.llnl.gov/tutorials/pthreads/man/pthread_cond_wait.txt)
- [pthread_condattr_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_condattr_destroy.txt)
- [pthread_condattr_getclock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_condattr_getclock.txt)
- [pthread_condattr_getpshared](https://computing.llnl.gov/tutorials/pthreads/man/pthread_condattr_getpshared.txt)
- [pthread_condattr_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_condattr_init.txt)
- [pthread_condattr_setclock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_condattr_setclock.txt)
- [pthread_condattr_setpshared](https://computing.llnl.gov/tutorials/pthreads/man/pthread_condattr_setpshared.txt)
- [pthread_create](https://computing.llnl.gov/tutorials/pthreads/man/pthread_create.txt)
- [pthread_detach](https://computing.llnl.gov/tutorials/pthreads/man/pthread_detach.txt)
- [pthread_equal](https://computing.llnl.gov/tutorials/pthreads/man/pthread_equal.txt)
- [pthread_exit](https://computing.llnl.gov/tutorials/pthreads/man/pthread_exit.txt)
- [pthread_getconcurrency](https://computing.llnl.gov/tutorials/pthreads/man/pthread_getconcurrency.txt)
- [pthread_getcpuclockid](https://computing.llnl.gov/tutorials/pthreads/man/pthread_getcpuclockid.txt)
- [pthread_getschedparam](https://computing.llnl.gov/tutorials/pthreads/man/pthread_getschedparam.txt)
- [pthread_getspecific](https://computing.llnl.gov/tutorials/pthreads/man/pthread_getspecific.txt)
- [pthread_join](https://computing.llnl.gov/tutorials/pthreads/man/pthread_join.txt)
- [pthread_key_create](https://computing.llnl.gov/tutorials/pthreads/man/pthread_key_create.txt)
- [pthread_key_delete](https://computing.llnl.gov/tutorials/pthreads/man/pthread_key_delete.txt)
- [pthread_kill](https://computing.llnl.gov/tutorials/pthreads/man/pthread_kill.txt)
- [pthread_mutex_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_destroy.txt)
- [pthread_mutex_getprioceiling](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_getprioceiling.txt)
- [pthread_mutex_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_init.txt)
- [pthread_mutex_lock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_lock.txt)
- [pthread_mutex_setprioceiling](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_setprioceiling.txt)
- [pthread_mutex_timedlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_timedlock.txt)
- [pthread_mutex_trylock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_trylock.txt)
- [pthread_mutex_unlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutex_unlock.txt)
- [pthread_mutexattr_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_destroy.txt)
- [pthread_mutexattr_getprioceiling](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_getprioceiling.txt)
- [pthread_mutexattr_getprotocol](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_getprotocol.txt)
- [pthread_mutexattr_getpshared](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_getpshared.txt)
- [pthread_mutexattr_gettype](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_gettype.txt)
- [pthread_mutexattr_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_init.txt)
- [pthread_mutexattr_setprioceiling](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_setprioceiling.txt)
- [pthread_mutexattr_setprotocol](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_setprotocol.txt)
- [pthread_mutexattr_setpshared](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_setpshared.txt)
- [pthread_mutexattr_settype](https://computing.llnl.gov/tutorials/pthreads/man/pthread_mutexattr_settype.txt)
- [pthread_once](https://computing.llnl.gov/tutorials/pthreads/man/pthread_once.txt)
- [pthread_rwlock_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlock_destroy.txt)
- [pthread_rwlock_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlock_init.txt)
- [pthread_rwlock_rdlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlock_rdlock.txt)
- [pthread_rwlock_timedrdlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlock_timedrdlock.txt)
- [pthread_rwlock_timedwrlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlock_timedwrlock.txt)
- [pthread_rwlock_tryrdlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlock_tryrdlock.txt)
- [pthread_rwlock_trywrlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlock_trywrlock.txt)
- [pthread_rwlock_unlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlock_unlock.txt)
- [pthread_rwlock_wrlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlock_wrlock.txt)
- [pthread_rwlockattr_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlockattr_destroy.txt)
- [pthread_rwlockattr_getpshared](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlockattr_getpshared.txt)
- [pthread_rwlockattr_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlockattr_init.txt)
- [pthread_rwlockattr_setpshared](https://computing.llnl.gov/tutorials/pthreads/man/pthread_rwlockattr_setpshared.txt)
- [pthread_self](https://computing.llnl.gov/tutorials/pthreads/man/pthread_self.txt)
- [pthread_setcancelstate](https://computing.llnl.gov/tutorials/pthreads/man/pthread_setcancelstate.txt)
- [pthread_setcanceltype](https://computing.llnl.gov/tutorials/pthreads/man/pthread_setcanceltype.txt)
- [pthread_setconcurrency](https://computing.llnl.gov/tutorials/pthreads/man/pthread_setconcurrency.txt)
- [pthread_setschedparam](https://computing.llnl.gov/tutorials/pthreads/man/pthread_setschedparam.txt)
- [pthread_setschedprio](https://computing.llnl.gov/tutorials/pthreads/man/pthread_setschedprio.txt)
- [pthread_setspecific](https://computing.llnl.gov/tutorials/pthreads/man/pthread_setspecific.txt)
- [pthread_sigmask](https://computing.llnl.gov/tutorials/pthreads/man/pthread_sigmask.txt)
- [pthread_spin_destroy](https://computing.llnl.gov/tutorials/pthreads/man/pthread_spin_destroy.txt)
- [pthread_spin_init](https://computing.llnl.gov/tutorials/pthreads/man/pthread_spin_init.txt)
- [pthread_spin_lock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_spin_lock.txt)
- [pthread_spin_trylock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_spin_trylock.txt)
- [pthread_spin_unlock](https://computing.llnl.gov/tutorials/pthreads/man/pthread_spin_unlock.txt)
- [pthread_testcancel](https://computing.llnl.gov/tutorials/pthreads/man/pthread_testcancel.txt)

----

https://computing.llnl.gov/tutorials/pthreads/

Last Modified: 07/25/2014 03:56:40 blaiseb@llnl.gov

UCRL-MI-133316
