# [GDB] 除錯 python 程式 thread 間互相等待 mutex，造成 deadlock 的問題


最近專案的 python 程式莫名的爆出了一堆卡住的問題，

研究了一下，是程式內部出現了 deadlock 死結，簡單記錄一下除錯經過吧～

 

首先是同事回報，某隻 python 寫的 daemon 開始沒有回應，

用 gdb -p <pid> 開始除錯，先用 info thr 看一下目前的 thread，

發現 15 根 thread 裡面，幾乎大家都在執行 sem_wait()，也就是在等 mutex：


```sh
(gdb) info thr
Id   Target Id         Frame
15   Thread 0x7f21ef1fe740 (LWP 47280) 0x00007f21ee034413 in select () at ../sysdeps/unix/syscall-template.S:81
14   Thread 0x7f21a67fc700 (LWP 47388) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
13   Thread 0x7f21a6ffd700 (LWP 47387) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
12   Thread 0x7f21a77fe700 (LWP 47386) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
11   Thread 0x7f21a7fff700 (LWP 47385) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
10   Thread 0x7f21c4ff9700 (LWP 47384) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
9    Thread 0x7f21c57fa700 (LWP 47383) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
8    Thread 0x7f21c5ffb700 (LWP 47382) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
7    Thread 0x7f21c67fc700 (LWP 47381) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
6    Thread 0x7f21c6ffd700 (LWP 47380) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
5    Thread 0x7f21c77fe700 (LWP 47379) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
4    Thread 0x7f21c7fff700 (LWP 47378) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
3    Thread 0x7f21cce4f700 (LWP 47377) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
2    Thread 0x7f21d776a700 (LWP 47325) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
* 1    Thread 0x7f21de57a700 (LWP 47295) sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
```

雖然大家看起來都在等 mutex，但事實上並不是都在等同一個 mutex…

用 bt 查看 call stack，可以看到 PyThread_acquire_lock() 呼叫時的 lock 參數，

以 thread 1, 2, 3, 13 來說都是同一個` lock 0x234a050`：


- `lock=0x234a050`

```
(gdb) bt
#0  sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
#1  0x00007f21eed397b5 in PyThread_acquire_lock (lock=0x234a050, waitflag=1) at /usr/src/debug/Python-2.7.5/Python/thread_pthread.h:323
#2  0x00007f21eed3d442 in lock_PyThread_acquire_lock (self=0x1bd1ef0, args=<optimized out>) at /usr/src/debug/Python-2.7.5/Modules/threadmodule.c:52
#3  0x00007f21eed0daa4 in call_function (oparg=<optimized out>, pp_stack=0x7f21a6ffb690) at /usr/src/debug/Python-2.7.5/Python/ceval.c:4098
#4  PyEval_EvalFrameEx (f=f@entry=Frame 0x7f213c00cbd0, for file /usr/lib64/python2.7/threading.py, line 173, in acquire (self=<_RLock(_Verbose__verbose=False, _RLock__owner=139782504040192, _RLock__block=<thread.lock at remote 0x1bd1ef0>, _RLock__count=2) at remote 0x246e1d0>, blocking=1, me=139782512432896), throwflag=throwflag@entry=0) at /usr/src/debug/Python-2.7.5/Python/ceval.c:2740
```

而 thread 4, 5, 6, 7, 8, 9, 10, 11, 12, 14 則都是在等 `lock 0x24b0560`：


```sh
(gdb) bt
#0  sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
#1  0x00007f21eed397b5 in PyThread_acquire_lock (lock=0x24b0560, waitflag=1) at /usr/src/debug/Python-2.7.5/Python/thread_pthread.h:323
#2  0x00007f21eed3d442 in lock_PyThread_acquire_lock (self=0x1bd1e10, args=<optimized out>) at /usr/src/debug/Python-2.7.5/Modules/threadmodule.c:52
#3  0x00007f21eed0daa4 in call_function (oparg=<optimized out>, pp_stack=0x7f21c7ffdcd0) at /usr/src/debug/Python-2.7.5/Python/ceval.c:4098
#4  PyEval_EvalFrameEx (f=f@entry=Frame 0x7f21c0329fd0, for file /usr/lib64/python2.7/threading.py, line 297, in _acquire_restore (self=<_Condition(_Verbose__verbose=False, _Condition__lock=<thread.lock at remote 0x1bd1e10>, acquire=<built-in method acquire of thread.lock object at remote 0x1bd1e10>, _Condition__waiters=[], release=<built-in method release of thread.lock object at remote 0x1bd1e10>) at remote 0x240e350>, x=None), throwflag=throwflag@entry=0) at /usr/src/debug/Python-2.7.5/Python/ceval.c:2740
```

可以想像的是，這兩群 thread 裡面，

應該至少各有一根 thread 拿住了某個 mutex，而另一群裡面有另一根 thread 拿了另一個 mutex，

而這兩根 thread 正在互相等待對方釋放它們手上的 mutex…

 

先切到第一群裡面的 thread 1，用 py-bt 看看 python 部分的 call stack…

可以看到它在等的是一個 RLock (Re-entrant lock)，

而目前 lock 的擁有者是 139782504040192：


```sh
(gdb) thr 1
[Switching to thread 1 (Thread 0x7f21de57a700 (LWP 47295))]
#0  sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
85      movq    %rax, %rcx
(gdb) py-bt
#1 Waiting for a lock (e.g. GIL)
#2 Waiting for a lock (e.g. GIL)
#4 Frame 0x7f21d86ce0a0, for file /usr/lib64/python2.7/threading.py, line 173, in acquire (self=<_RLock(_Verbose__verbose=False, _RLock__owner=139782504040192, _RLock__block=<thread.lock at remote 0x1bd1ef0>, _RLock__count=2) at remote 0x246e1d0>, blocking=1, me=139783440934656)
rc = self.__block.acquire(blocking)
#11 Frame 0x37e6d20, for file ./flow/task.py, line 2776, in add_task_to_queue ()
with self._task_lock:
```

這個 139782504040192 是什麼呢？

把它換成 16 進位變成 0x7f21a67fc700：

```sh
(gdb) p/x 139782504040192
$2 = 0x7f21a67fc700
 ```

`原來 0x7f21a67fc700 就是 thread 14 的代碼`，

也就是說 thread 1 在等待的 RLock，目前是被 thread 14 咬住了～

那 thread 14 又在做什麼呢？

看起來是正在執行 Queue.put() 函式，要把一個東西塞到 Queue 裡面去。

這個塞進去的動作會需要先取得 Queue 內部的一個名叫 not_full 的 mutex，

因此可以看到 thread 14 卡在 self.not_full.acquire()：


```sh
(gdb) thr 14
[Switching to thread 14 (Thread 0x7f21a67fc700 (LWP 47388))]
#0  sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
85      movq    %rax, %rcx
(gdb) py-bt
#1 Waiting for a lock (e.g. GIL)
#2 Waiting for a lock (e.g. GIL)
#4 Frame 0x7f213c0025e0, for file /usr/lib64/python2.7/Queue.py, line 118, in put (self=<ResultQueue()>)
self.not_full.acquire()
#8 Frame 0x7f21d000eb90, for file ./flow/result.py, line 98, in add_to_queue ()
self._out_queue.put((priority, queue_data))
#12 Frame 0x7f21c8a66710, for file ./flow/task.py, line 3237, in handle_error_task ()
mgr.add_to_queue(result)
#16 Frame 0x7f21c8405b60, for file ./flow/task.py, line 3177, in _cleanup_tasks ()
self.handle_error_task(task)
```

第二群中的 thread 14 咬住了 RLock，但有其他人咬住了 thread 14 想要的 not_full mutex…

但要查出來誰咬住了 not_full 這個 mutex 比較困難，因為它不像 RLock 會記錄擁有者…

到第一群的 thread 1, 2, 3, 13 分別看看它們在做些什麼事，

結果在 thread 13 看到比較可疑的部分：

它先呼叫了 Queue.get()，最後想去取得 thread 14 目前咬住的 RLock：


```sh
(gdb) thr 13
[Switching to thread 13 (Thread 0x7f21a6ffd700 (LWP 47387))]
#0  sem_wait () at ../nptl/sysdeps/unix/sysv/linux/x86_64/sem_wait.S:85
85      movq    %rax, %rcx
(gdb) py-bt
#1 Waiting for a lock (e.g. GIL)
#2 Waiting for a lock (e.g. GIL)
#4 Frame 0x7f213c00cbd0, for file /usr/lib64/python2.7/threading.py, line 173, in acquire (self=<_RLock(_Verbose__verbose=False, _RLock__owner=139782504040192, _RLock__block=<thread.lock at remote 0x1bd1ef0>, _RLock__count=2) at remote 0x246e1d0>, blocking=1, me=139782512432896)
rc = self.__block.acquire(blocking)
#11 Frame 0x7f21b8001d40, for file ./flow/task.py, line 2936, in get_failed_task ()
with self._task_lock:
#14 Frame 0x7f21b8002d60, for file ./flow/result.py, line 35, in _get (self=<ResultQueue()>)
if get_failed_task():
#18 Frame 0x7f21c82ac800, for file /usr/lib64/python2.7/Queue.py, line 178, in get (self=<ResultQueue()>)
item = self._get()
#22 Frame 0x7f21a0435710, for file ./flow/result.py, line 151, in get_result ()
priority, queue_data = self._out_queue.get(True, timeout)
```

如果去查 python 的 Queue 的實作，就可以看到 Queue.get() 裡會先取得 not_empty 這個 mutex，

再去呼叫 _get()，而這個我們改寫過的 _get() 後來再去等待 RLock 而卡住…

不過看官可能會覺得奇怪，thread 14 咬住了 RLock，接著它在等 not_full 這個 mutex；

而 thread 13 是咬住了 not_empty，接著它在等 RLock；

看起來 not_full 和 not_empty 是兩個 mutex，不應該造成 deadlock 啊？

 

答案還是藏在 python 的 Queue 的實作裡面…

Queue 的 __init__() 裡定義了 not_empty 和 not_full 兩個 Condition 成員，

但其實底層操作到的都是同一個 mutex：


```sh
def __init__(self, maxsize=0):
self.mutex = _threading.Lock()
self.not_empty = _threading.Condition(self.mutex)
self.not_full = _threading.Condition(self.mutex)
```

也就是因為有這一層關係，才會導致 thread 14 和 13 互相 deadlock 了…

解決方法就是更正取得 mutex 的順序，或者是減少不需要的 mutex 取得囉～