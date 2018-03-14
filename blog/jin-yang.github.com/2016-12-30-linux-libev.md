---
title: libev 事件庫
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,program,libev,event loop
description: libev 與 libevent 類似，是一個 C 語言編寫的，高性能的事件循環庫，在此詳細介紹下。
---

libev 是一個 C 語言編寫的，高性能的事件循環庫，與此類似的事件循環庫還有 libevent、libubox 等，在此詳細介紹下 libev 相關的內容。

<!-- more -->

## 簡介

關於 libev 詳見官網 [http://software.schmorp.de](http://software.schmorp.de/pkg/libev.html)，其幫助文檔可以參考 [官方文檔](http://pod.tst.eu/http://cvs.schmorp.de/libev/ev.pod)，安裝完之後，可通過 `man 3 ev` 查看幫助信息，文檔也在源碼中保存了一份，可以通過 `man -l ev.3` 命令查看。

### 安裝

安裝可以源碼安裝，或者在 CentOS 中，最簡單可通過如下方式安裝。

{% highlight text %}
----- 安裝庫
# yum install libev libev-devel
{% endhighlight %}

### 示例程序

如下是一個簡單的示例程序。

{% highlight c %}
#include <ev.h>       // a single header file is required
#include <stdio.h>    // for puts

// every watcher type has its own typedef'd struct with the name ev_TYPE
ev_io stdin_watcher;
ev_timer timeout_watcher;

// all watcher callbacks have a similar signature
// this callback is called when data is readable on stdin
static void stdin_cb (EV_P_ ev_io *w, int revents)
{
    puts ("stdin ready");
    // for one-shot events, one must manually stop the watcher
    // with its corresponding stop function.
    ev_io_stop (EV_A_ w);

    // this causes all nested ev_run's to stop iterating
    ev_break (EV_A_ EVBREAK_ALL);
}

// another callback, this time for a time-out
static void timeout_cb (EV_P_ ev_timer *w, int revents)
{
    puts ("timeout");
    // this causes the innermost ev_run to stop iterating
    ev_break (EV_A_ EVBREAK_ONE);
}

int main (void)
{
    // use the default event loop unless you have special needs
    struct ev_loop *loop = EV_DEFAULT; /* OR ev_default_loop(0) */

    // initialise an io watcher, then start it
    // this one will watch for stdin to become readable
    ev_io_init (&stdin_watcher, stdin_cb, /*STDIN_FILENO*/ 0, EV_READ);
    ev_io_start (EV_A_ &stdin_watcher);

    // initialise a timer watcher, then start it
    // simple non-repeating 5.5 second timeout
    ev_timer_init (&timeout_watcher, timeout_cb, 5.5, 0.);
    ev_timer_start (EV_A_ &timeout_watcher);

    // now wait for events to arrive
    ev_run (loop, 0);

    // break was called, so exit
    return 0;
}
{% endhighlight %}

可以通過如下命令編譯。

{% highlight text %}
----- 編譯示例程序
$ gcc -lev example.c -o example
{% endhighlight %}

### 執行過程

libev 是一個事件循環，首先需要註冊感興趣的事件，libev 會監控這些事件，當事件發生時調用相應的處理函數，也就是回調函數。其處理過程為：

1. 初始化一個事件循環<br>可以通過 ev_default_loop(0) 或者 EV_DEFAULT 初始化，兩者等價。
2. 定義數據<br>在 ev.h 中定義了各種類型的，如 ev_io、ev_timer、ev_signal等。
3. 註冊感興趣的事件<br>在此被稱為 watchers ，這個是 C 結構體。
4. 啟動監控<br>啟動上步註冊的事件，如 ev_io_start()、ev_timer_start() 等。
5. 啟動 libev<br>重複 1,2 步，然後啟動 libev 事件循環，直接執行 ev_run() 即可。

## 源碼詳解

libev 通過觀察器 (watcher) 來監聽各種事件，watcher 包括了事件類型、優先級、觸發條件和回調函數等參數；將其註冊到事件循環上，在滿足註冊的條件時，會觸發觀察器，調用它的回調函數。


{% highlight text %}
enum {
  EV_UNDEF    = (int)0xFFFFFFFF, /* guaranteed to be invalid */
  EV_NONE     =            0x00, /* no events */
  EV_READ     =            0x01, /* ev_io detected read will not block */
  EV_WRITE    =            0x02, /* ev_io detected write will not block */
  EV__IOFDSET =            0x80, /* internal use only */
  EV_IO       =         EV_READ, /* alias for type-detection */
  EV_TIMER    =      0x00000100, /* timer timed out */
#if EV_COMPAT3
  EV_TIMEOUT  =        EV_TIMER, /* pre 4.0 API compatibility */
#endif
  EV_PERIODIC =      0x00000200, /* periodic timer timed out */
  EV_SIGNAL   =      0x00000400, /* signal was received */
  EV_CHILD    =      0x00000800, /* child/pid had status change */
  EV_STAT     =      0x00001000, /* stat data changed */
  EV_IDLE     =      0x00002000, /* event loop is idling */
  EV_PREPARE  =      0x00004000, /* event loop about to poll */
  EV_CHECK    =      0x00008000, /* event loop finished poll */
  EV_EMBED    =      0x00010000, /* embedded event loop needs sweep */
  EV_FORK     =      0x00020000, /* event loop resumed in child */
  EV_CLEANUP  =      0x00040000, /* event loop resumed in child */
  EV_ASYNC    =      0x00080000, /* async intra-loop signal */
  EV_CUSTOM   =      0x01000000, /* for use by user code */
  EV_ERROR    = (int)0x80000000  /* sent when an error occurs */
{% endhighlight %}


libev 中的觀察器分為 4 種狀態：初始化、啟動/活動、等待、停止。

首先需要對 watcher 初始化，可通過 `ev_TYPE_init()` 或者 `ev_init()`+`ev_TYPE_set()` 初始化，兩者等效；實際就是設置對應結構體的初始值。

{% highlight c %}
#define ev_io_init(ev,cb,fd,events)              \
    do { ev_init ((ev), (cb)); ev_io_set ((ev),(fd),(events)); } while (0)
#define ev_timer_init(ev,cb,after,repeat)        \
    do { ev_init ((ev), (cb)); ev_timer_set ((ev),(after),(repeat)); } while (0)
#define ev_periodic_init(ev,cb,ofs,ival,rcb)     \
    do { ev_init ((ev), (cb)); ev_periodic_set ((ev),(ofs),(ival),(rcb)); } while (0)
#define ev_signal_init(ev,cb,signum)             \
    do { ev_init ((ev), (cb)); ev_signal_set ((ev), (signum)); } while (0)
#define ev_child_init(ev,cb,pid,trace)           \
    do { ev_init ((ev), (cb)); ev_child_set ((ev),(pid),(trace)); } while (0)
#define ev_stat_init(ev,cb,path,interval)        \
    do { ev_init ((ev), (cb)); ev_stat_set ((ev),(path),(interval)); } while (0)
#define ev_idle_init(ev,cb)                      \
    do { ev_init ((ev), (cb)); ev_idle_set ((ev)); } while (0)
#define ev_prepare_init(ev,cb)                   \
    do { ev_init ((ev), (cb)); ev_prepare_set ((ev)); } while (0)
#define ev_check_init(ev,cb)                     \
    do { ev_init ((ev), (cb)); ev_check_set ((ev)); } while (0)
#define ev_embed_init(ev,cb,other)               \
    do { ev_init ((ev), (cb)); ev_embed_set ((ev),(other)); } while (0)
#define ev_fork_init(ev,cb)                      \
    do { ev_init ((ev), (cb)); ev_fork_set ((ev)); } while (0)
#define ev_cleanup_init(ev,cb)                   \
    do { ev_init ((ev), (cb)); ev_cleanup_set ((ev)); } while (0)
#define ev_async_init(ev,cb)                     \
    do { ev_init ((ev), (cb)); ev_async_set ((ev)); } while (0)
{% endhighlight %}

接下來，通過 `ev_TYPE_start()`、`ev_TYPE_stop()` 來啟動、停止觀察器，停止同時會釋放內存。

### 結構體

libev 通過 C 語言實現，其中通過宏實現了一種類似的繼承機制，也就是其中各種 Watchers 的部分成員變量是相同的，只有少部分成員為各自獨有，接下來簡單介紹下。

每個 watcher 都會包含 EV_WATCHER 宏定義的內容，該宏實際會包含如下內容，其中 type 對應類型，如 ev_io 等。

{% highlight c %}
# define EV_CB_DECLARE(type) void (*cb)(EV_P_ struct type *w, int revents);

#define EV_WATCHER(type) \
  int active;          /* private，是否激活，通過start()/stop()處理 */ \
  int pending;         /* private，有事件就緒等待處理，對應了等待隊列的下標 */ \
  EV_DECL_PRIORITY     /* private，定義優先級，如果沒有使用優先級則是空 */ \
  EV_COMMON            /* rw，私有數據，一般是void *data */ \
  EV_CB_DECLARE (type) /* private，回調函數 */

#define EV_WATCHER_LIST(type)           \
  EV_WATCHER (type)                     \
  struct ev_watcher_list *next; /* private */

#define EV_WATCHER_TIME(type)           \
  EV_WATCHER (type)                     \
  ev_tstamp at;                 /* private */

typedef struct ev_watcher {
  EV_WATCHER (ev_watcher)
} ev_watcher;

typedef struct ev_watcher_list {
  EV_WATCHER_LIST (ev_watcher_list)
} ev_watcher_list;

typedef struct ev_io {
  EV_WATCHER_LIST (ev_io)

  int fd;     /* ro */
  int events; /* ro */
} ev_io;

typedef struct ev_timer {
  EV_WATCHER_TIME (ev_timer)

  ev_tstamp repeat; /* rw */
} ev_timer;
{% endhighlight %}

如上的 ev_watcher 結構體可以時為 “基類”，通過宏 EV_WATCHER 定義了它的所有成員；而像 IO Watcher、Signal Watcher 是以鏈表的形式進行組織的，所以在 ev_watcher 基類的基礎上，定義了 ev_watcher 的子類 ev_watcher_list 。










#### 多實例支持

ev_loop 是主循環，保存了與循環相關的很多變量，而 `EV_MULTIPLICITY` 是一個條件編譯的宏，表明是否支持有多個 ev_loop 實例存在，表現在源碼中表示是否需要傳遞 `struct ev_loop *loop` 參數，一般來說，每個線程中有且僅有一個 ev_loop 實例。

<!--
如果支持多個loop，則default_loop_struct就是一個靜態的struct ev_loop類型的結構體，其中包含了各種成員，比如ev_tstamp ev_rt_now;  int  pendingpri;等等。

ev_default_loop_ptr就是指向struct  ev_loop 類型的指針。

如果不支持多個loop，則上述的struct  ev_loop結構就不復存在，其成員都是以靜態變量的形式進行定義，而ev_default_loop_ptr也只是一個int變量，用來表明”loop”是否已經初始化成功。
-->

### 系統時間

先看下 libev 是如何使用時間的，因為該庫中很多與時間相關的操作；在 libev.m4 中，定義了與之相關的宏，如下所示。

{% highlight text %}
AC_CHECK_FUNCS(clock_gettime, [], [
   dnl on linux, try syscall wrapper first
   if test $(uname) = Linux; then
      AC_MSG_CHECKING(for clock_gettime syscall)
      AC_LINK_IFELSE([AC_LANG_PROGRAM(
         [#include <unistd.h>
          #include <sys/syscall.h>
          #include <time.h>],
         [struct timespec ts; int status = syscall (SYS_clock_gettime, CLOCK_REALTIME, &ts)])],
         [ac_have_clock_syscall=1
          AC_DEFINE(HAVE_CLOCK_SYSCALL, 1, Define to 1 to use the syscall interface for clock_gettime)
          AC_MSG_RESULT(yes)],
         [AC_MSG_RESULT(no)])
   fi
   if test -z "$LIBEV_M4_AVOID_LIBRT" && test -z "$ac_have_clock_syscall"; then
      AC_CHECK_LIB(rt, clock_gettime)
      unset ac_cv_func_clock_gettime
      AC_CHECK_FUNCS(clock_gettime)
   fi
])

AC_CHECK_FUNCS(nanosleep, [], [
   if test -z "$LIBEV_M4_AVOID_LIBRT"; then
      AC_CHECK_LIB(rt, nanosleep)
      unset ac_cv_func_nanosleep
      AC_CHECK_FUNCS(nanosleep)
   fi
])
{% endhighlight %}

`clock_gettime()` 函數的調用有兩種方式，分別是系統調用和 `-lrt` 庫；在上述的 `libev.m4` 中，會進行檢測，首先會檢測 `clock_gettime()` 系統調用是否可用，如果可用會定義 `HAVE_CLOCK_SYSCALL` 宏。

libev 提供了單調遞增 (monotonic) 以及實時時間 (realtime) 兩種記時方式，其宏定義的方式如下，而 `HAVE_CLOCK_SYSCALL` 和 `HAVE_CLOCK_GETTIME` 的詳見 libev.m4 中定義，優先使用 `SYS_clock_gettime()` 系統調用 API 函數。

{% highlight c %}
# if HAVE_CLOCK_SYSCALL
#  ifndef EV_USE_CLOCK_SYSCALL
#   define EV_USE_CLOCK_SYSCALL 1
#   ifndef EV_USE_REALTIME
#    define EV_USE_REALTIME  0
#   endif
#   ifndef EV_USE_MONOTONIC
#    define EV_USE_MONOTONIC 1
#   endif
#  endif
# elif !defined EV_USE_CLOCK_SYSCALL
#  define EV_USE_CLOCK_SYSCALL 0
# endif

# if HAVE_CLOCK_GETTIME
#  ifndef EV_USE_MONOTONIC
#   define EV_USE_MONOTONIC 1
#  endif
#  ifndef EV_USE_REALTIME
#   define EV_USE_REALTIME  0
#  endif
# else
#  ifndef EV_USE_MONOTONIC
#   define EV_USE_MONOTONIC 0
#  endif
#  ifndef EV_USE_REALTIME
#   define EV_USE_REALTIME  0
#  endif
# endif
{% endhighlight %}

優先使用系統調用和單調遞增時間，在 CentOS 7 中通常定義為。

{% highlight text %}
#define HAVE_CLOCK_GETTIME 1
#define EV_USE_REALTIME  0
#define EV_USE_MONOTONIC 1
{% endhighlight %}

在如下的初始化函數中介紹詳細的細節。

<!--
場景：
    1. 使用系統調用
    syscall (SYS_clock_gettime, CLOCK_REALTIME, &ts);
-->



### 初始化

無論是通過 `EV_DEFAULT` 宏還是 `ev_default_loop()` 函數進行初始化，實際上功能都相同，也就是都調用了 `ev_default_loop(0)` 進行初始化，主要流程為 `ev_default_loop()->loop_init()` 。

如下主要介紹 `loop_init()` 函數。

{% highlight c %}
#ifndef EV_HAVE_EV_TIME
ev_tstamp
ev_time (void) EV_THROW
{
#if EV_USE_REALTIME
  if (expect_true (have_realtime))
    {
      struct timespec ts;
      clock_gettime (CLOCK_REALTIME, &ts);
      return ts.tv_sec + ts.tv_nsec * 1e-9;
    }
#endif

  struct timeval tv;
  gettimeofday (&tv, 0);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}
#endif

inline_size ev_tstamp
get_clock (void)
{
#if EV_USE_MONOTONIC
  if (expect_true (have_monotonic))
    {
      struct timespec ts;
      clock_gettime (CLOCK_MONOTONIC, &ts);
      return ts.tv_sec + ts.tv_nsec * 1e-9;
    }
#endif

  return ev_time ();
}

void noinline ecb_cold loop_init (EV_P_ unsigned int flags) EV_THROW
{
  if (!backend) {  // 如果backend還沒有確定
      origflags = flags;

#if EV_USE_REALTIME
      if (!have_realtime)
        {
          struct timespec ts;

          if (!clock_gettime (CLOCK_REALTIME, &ts))
            have_realtime = 1;
        }
#endif

#if EV_USE_MONOTONIC
      if (!have_monotonic)
        {
          struct timespec ts;

          if (!clock_gettime (CLOCK_MONOTONIC, &ts))
            have_monotonic = 1;
        }
#endif

      /* pid check not overridable via env */
#ifndef _WIN32
      if (flags & EVFLAG_FORKCHECK)
        curpid = getpid ();
#endif

      if (!(flags & EVFLAG_NOENV)
          && !enable_secure ()
          && getenv ("LIBEV_FLAGS"))
        flags = atoi (getenv ("LIBEV_FLAGS"));

      ev_rt_now          = ev_time ();
      mn_now             = get_clock ();
      now_floor          = mn_now;
      rtmn_diff          = ev_rt_now - mn_now;
#if EV_FEATURE_API
      invoke_cb          = ev_invoke_pending;
#endif

      io_blocktime       = 0.;
      timeout_blocktime  = 0.;
      backend            = 0;
      backend_fd         = -1;
      sig_pending        = 0;
#if EV_ASYNC_ENABLE
      async_pending      = 0;
#endif
      pipe_write_skipped = 0;
      pipe_write_wanted  = 0;
      evpipe [0]         = -1;
      evpipe [1]         = -1;
#if EV_USE_INOTIFY
      fs_fd              = flags & EVFLAG_NOINOTIFY ? -1 : -2;
#endif
#if EV_USE_SIGNALFD
      sigfd              = flags & EVFLAG_SIGNALFD  ? -2 : -1;
#endif

      if (!(flags & EVBACKEND_MASK))
        flags |= ev_recommended_backends ();

#if EV_USE_IOCP
      if (!backend && (flags & EVBACKEND_IOCP  )) backend = iocp_init   (EV_A_ flags);
#endif
#if EV_USE_PORT
      if (!backend && (flags & EVBACKEND_PORT  )) backend = port_init   (EV_A_ flags);
#endif
#if EV_USE_KQUEUE
      if (!backend && (flags & EVBACKEND_KQUEUE)) backend = kqueue_init (EV_A_ flags);
#endif
#if EV_USE_EPOLL
      if (!backend && (flags & EVBACKEND_EPOLL )) backend = epoll_init  (EV_A_ flags);
#endif
#if EV_USE_POLL
      if (!backend && (flags & EVBACKEND_POLL  )) backend = poll_init   (EV_A_ flags);
#endif
#if EV_USE_SELECT
      if (!backend && (flags & EVBACKEND_SELECT)) backend = select_init (EV_A_ flags);
#endif

      ev_prepare_init (&pending_w, pendingcb);

#if EV_SIGNAL_ENABLE || EV_ASYNC_ENABLE
      ev_init (&pipe_w, pipecb);
      ev_set_priority (&pipe_w, EV_MAXPRI);
#endif
    }
}
{% endhighlight %}

其中有兩個比較重要的時間變量，也就是 `ev_rt_now` 和 `mn_now`，前者表示當前的日曆時間，也就是自 1970.01.01 以來的秒數，該值通過 `gettimeofday()` 得到。

### 調用流程

在介紹各個 Watcher 的流程之前，首先看下主循環的執行過程。

該函數通常是在各個事件初始化完成之後調用，也就是等待操作系統的事件，然後調用已經註冊的回調函數，並一直重複循環執行。

{% highlight c %}
int ev_run (EV_P_ int flags)
{
  ++loop_depth;      // 如果定義了EV_FEATURE_API宏
  loop_done = EVBREAK_CANCEL;
  EV_INVOKE_PENDING; // 在執行前確認所有的事件已經執行

  do {
      ev_verify (EV_A);  // 當EV_VERIFY >= 2時，用於校驗當前的結構體是否正常
      if (expect_false (curpid)) /* penalise the forking check even more */
        if (expect_false (getpid () != curpid)) {
            curpid = getpid ();
            postfork = 1;
        }

      /* we might have forked, so queue fork handlers */
      if (expect_false (postfork))
        if (forkcnt) {
            queue_events (EV_A_ (W *)forks, forkcnt, EV_FORK);
            EV_INVOKE_PENDING;
        }

      /* queue prepare watchers (and execute them) */
      if (expect_false (preparecnt)) {
          queue_events (EV_A_ (W *)prepares, preparecnt, EV_PREPARE);
          EV_INVOKE_PENDING;
      }

      if (expect_false (loop_done))
        break;

      /* we might have forked, so reify kernel state if necessary */
      if (expect_false (postfork))
        loop_fork (EV_A);

      /* update fd-related kernel structures */
      fd_reify (EV_A);

      /* calculate blocking time */
      {
        ev_tstamp waittime  = 0.;
        ev_tstamp sleeptime = 0.;

        /* remember old timestamp for io_blocktime calculation */
        ev_tstamp prev_mn_now = mn_now;

        /* 會更新當前時間mn_now和ev_rt_now，如果發現時間被調整，則調用
         * timers_reschedule()函數調整堆loop->timers()中的每個節點。
         */
        time_update (EV_A_ 1e100);

        /* from now on, we want a pipe-wake-up */
        pipe_write_wanted = 1;

        ECB_MEMORY_FENCE; /* make sure pipe_write_wanted is visible before we check for potential skips */

        if (expect_true (!(flags & EVRUN_NOWAIT || idleall || !activecnt || pipe_write_skipped))) {
            waittime = MAX_BLOCKTIME;

            if (timercnt) {    // 如果有定時器存在則重新計算等待時間
                ev_tstamp to = ANHE_at (timers [HEAP0]) - mn_now;
                if (waittime > to) waittime = to;
            }
            if (periodiccnt) { // 如果定義了EV_PERIODIC_ENABLE宏
                ev_tstamp to = ANHE_at (periodics [HEAP0]) - ev_rt_now;
                if (waittime > to) waittime = to;
            }

            /* don't let timeouts decrease the waittime below timeout_blocktime */
            if (expect_false (waittime < timeout_blocktime)) // 默認timeout_blocktime為0
              waittime = timeout_blocktime;

            /* at this point, we NEED to wait, so we have to ensure */
            /* to pass a minimum nonzero value to the backend */
            if (expect_false (waittime < backend_mintime))
              waittime = backend_mintime;  // 不同的後端最小等待時間不同

            /* extra check because io_blocktime is commonly 0 */
            if (expect_false (io_blocktime)) {
                sleeptime = io_blocktime - (mn_now - prev_mn_now);

                if (sleeptime > waittime - backend_mintime)
                  sleeptime = waittime - backend_mintime;

                if (expect_true (sleeptime > 0.)) {
                    ev_sleep (sleeptime);
                    waittime -= sleeptime;
                }
            }
        }

#if EV_FEATURE_API
        ++loop_count;
#endif
        /* 調用IO複用函數，例如epoll_poll()，在此需要保證阻塞時間小於loop->timers，
         * 以及loop->periodics的棧頂元素的觸發時間。
         */
        assert ((loop_done = EVBREAK_RECURSE, 1)); /* assert for side effect */
        backend_poll (EV_A_ waittime);
        assert ((loop_done = EVBREAK_CANCEL, 1)); /* assert for side effect */

        pipe_write_wanted = 0; /* just an optimisation, no fence needed */

        ECB_MEMORY_FENCE_ACQUIRE;
        if (pipe_write_skipped) {
            assert (("libev: pipe_w not active, but pipe not written", ev_is_active (&pipe_w)));
            ev_feed_event (EV_A_ &pipe_w, EV_CUSTOM);
        }

        /* update ev_rt_now, do magic */
        time_update (EV_A_ waittime + sleeptime); // 更新時間，防止timejump
      }

      /* 如果棧頂元素的超時時間已經超過了當前時間，則將棧頂元素的監控器添加到
       * loop->pendings中，並調整堆結構，接著判斷棧頂元素是否仍超時，一致重複，
       * 直到棧頂元素不再超時。
       */
      timers_reify (EV_A); /* relative timers called last */
      periodics_reify (EV_A); /* absolute timers called first */

      /* queue idle watchers unless other events are pending */
      idle_reify (EV_A);

      /* queue check watchers, to be executed first */
      if (expect_false (checkcnt))
        queue_events (EV_A_ (W *)checks, checkcnt, EV_CHECK);

      /* 按照優先級，順序遍厲loop->pendings數組，調用其中每個監視器的回調函數 */
      EV_INVOKE_PENDING;
    } while (expect_true (
        activecnt
        && !loop_done
        && !(flags & (EVRUN_ONCE | EVRUN_NOWAIT))
    ));

  if (loop_done == EVBREAK_ONE)
    loop_done = EVBREAK_CANCEL;

#if EV_FEATURE_API
  --loop_depth;
#endif

  return activecnt;
}
{% endhighlight %}



<!--
       - Increment loop depth.
       - Reset the ev_break status.
       - Before the first iteration, call any pending watchers.
       LOOP:
       - If EVFLAG_FORKCHECK was used, check for a fork.
       - If a fork was detected (by any means), queue and call all fork watchers.
       - Queue and call all prepare watchers.
       - If ev_break was called, goto FINISH.
       - If we have been forked, detach and recreate the kernel state
         as to not disturb the other process.
       - Update the kernel state with all outstanding changes.
       - Update the "event loop time" (ev_now ()).
       - Calculate for how long to sleep or block, if at all
         (active idle watchers, EVRUN_NOWAIT or not having
         any active watchers at all will result in not sleeping).
       - Sleep if the I/O and timer collect interval say so.
       - Increment loop iteration counter.
       - Block the process, waiting for any events.
       - Queue all outstanding I/O (fd) events.
       - Update the "event loop time" (ev_now ()), and do time jump adjustments.
       - Queue all expired timers.
       - Queue all expired periodics.
       - Queue all idle watchers with priority higher than that of pending events.
       - Queue all check watchers.
       - Call all queued watchers in reverse order (i.e. check watchers first).
         Signals and child watchers are implemented as I/O watchers, and will
         be handled here by queueing them when their watcher gets executed.
       - If ev_break has been called, or EVRUN_ONCE or EVRUN_NOWAIT
         were used, or there are no active watchers, goto FINISH, otherwise
         continue with step LOOP.
       FINISH:
       - Reset the ev_break status iff it was EVBREAK_ONE.
       - Decrement the loop depth.
       - Return.

    Example: Queue some jobs and then loop until no events are outstanding anymore.

       ... queue jobs here, make sure they register event watchers as long
       ... as they still have work to do (even an idle watcher will do..)
       ev_run (my_loop, 0);
       ... jobs done or somebody called break. yeah!
-->





















<!--
{% highlight text %}
void ev_invoke_pending (struct ev_loop *loop);
  調用所有pending的watchers。

ev_default_loop()/ev_loop_new()
 |-loop_init()
   |-ev_recommended_backends()    如果沒有設置backend則會嘗試選擇
     |-ev_supported_backends()
 |-ev_prepare_init()

ev_run()
 |-backend_poll()
{% endhighlight %}
-->


### IO Watcher

對 IO 事件的監控的函數，會在 loop_init() 中初始化 backend_poll 變量，正是通過該函數監控 io 事件，如下是一個簡單的示例。

{% highlight text %}
void cb (struct ev_loop *loop, ev_io *w, int revents)
{
    ev_io_stop (loop, w);
    // .. read from stdin here (or from w->fd) and handle any I/O errors
}
ev_io watcher;
ev_io_init (&watcher, cb, STDIN_FILENO, EV_READ);  // 初始化，第三個是文件描述符，第四個是監聽事件
ev_io_start (loop, &watcher);
{% endhighlight %}

其中，ev_io_init() 用來設置結構體的參數，除了初始化通用的變量之外，還包括 io 觀察器對應的 fd 和 event 。

#### ev_io_start()

作用是設置 `ANFD anfds[]`，其中文件描述符為其序號，並將相應的 IO Watcher 插入到對應 fd 的鏈表中。由於對應 fd 的監控條件已有改動了，同時會在 `int fdchanges[]` 中記錄下該 fd ，並在後續的步驟中調用系統的接口修改對該 fd 監控條件。

{% highlight c %}
void noinline ev_io_start (EV_P_ ev_io *w) EV_THROW
{
  int fd = w->fd;
  if (expect_false (ev_is_active (w))) // 如果已經啟動則直接退出
    return;
  EV_FREQUENT_CHECK;                   // 通過ev_verify()校驗數據格式是否正常

  ev_start (EV_A_ (W)w, 1);            // 設置watch->active變量
  array_needsize (ANFD, anfds, anfdmax, fd + 1, array_init_zero);
  wlist_add (&anfds[fd].head, (WL)w);

  // 添加到fdchanges[]數組中
  fd_change (EV_A_ fd, w->events & EV__IOFDSET | EV_ANFD_REIFY);
  w->events &= ~EV__IOFDSET;

  EV_FREQUENT_CHECK;                   // 如上，通過ev_verify()校驗數據格式是否正常
}
{% endhighlight %}

![libev io watcher]({{ site.url }}/images/programs/libev_io_watcher_anfds.png "libev io watcher"){: .pull-center }

調用 ev_run() 開始等待事件的觸發，該函數中首先會調用 fd_reify()，該函數根據 fdchanges[] 中記錄的描述符，將該描述符上的事件添加到 backend 所使用的數據結構中；調用 time_update() 更新當前時間。

接著計算超時時間，並調用 backend_poll() 開始等待事件的發生，如果事件在規定時間內觸發的話，則會調用 fd_event() 將觸發的監視器記錄到 pendings 中；

backend 監聽函數 (如 select()、poll()、epoll_wait()等) 返回後，再次調用 time_update() 更新時間，然後調用 ev_invoke_pending() ，依次處理 pendings 中的監視器，調用該監視器的回調函數。



#### fd_reify()

該函數在 ev_run() 的每輪循環中都會調用；會將 fdchanges 中記錄的這些新事件一個個的處理，並調用後端 IO 複用的 backend_modify 宏。

<!--
這裡需要注意fd_reify()中的思想，anfd[fd] 結構體中，還有一個events事件，它是原先的所有watcher 的事件的 "|" 操作，向系統的epoll 從新添加描述符的操作 是在下次事件迭代開始前進行的，當我們依次掃描fdchangs，找到對應的anfd 結構，如果發現先前的events 與 當前所有的watcher 的"|" 操作結果不等，則表示我們需要調用epoll_ctrl 之類的函數來進行更改，反之不做操作。

實際上 Linux 在分配 fd 時，總是選擇系統可用的最小 fd ，所以 anfd 這個數組長度不會太大，而且這個數組會動態分配。

然後啟動事件驅動器，最後實際會阻塞到 backend_poll() 中，等待對應的 IO 事件。<br><br>

以 epoll 為例，實際調用的是 ev_epoll.c@epoll_poll() 。當 epoll_wait() 返回一個 fd_event 時 ，就可以直接定位到對應 fd 的 watchers-list ，而這個 watchers-list 的長度與註冊的事件相關。<br><br>

fd_event 會有一個導致觸發的事件，依次檢查對應的 wathers-list ，也即用這個事件依次和各個 watch 註冊的 event 做 & 操作，如果不為 0 ，則把對應的 watch 加入到待處理隊列 pendings 中。<br><br>

當我們啟用 watcher 優先級模式時，pendings 是個 2 維數組，此時僅考慮普通模式。
-->


#### 多路複用

當前支持的多路複用通過如下方式定義，

{% highlight c %}
/* method bits to be ored together */
enum {
  EVBACKEND_SELECT  = 0x00000001U, /* about anywhere */
  EVBACKEND_POLL    = 0x00000002U, /* !win */
  EVBACKEND_EPOLL   = 0x00000004U, /* linux */
  EVBACKEND_KQUEUE  = 0x00000008U, /* bsd */
  EVBACKEND_DEVPOLL = 0x00000010U, /* solaris 8 */ /* NYI */
  EVBACKEND_PORT    = 0x00000020U, /* solaris 10 */
  EVBACKEND_ALL     = 0x0000003FU, /* all known backends */
  EVBACKEND_MASK    = 0x0000FFFFU  /* all future backends */
};
{% endhighlight %}

而在通過 configure 進行編譯時，會對宏進行處理，以 epoll 為例，可以查看 ev.c 中的內容；在通過 configure 編譯時，如果支持 EPOLL 會在 config.h 中生成 `HAVE_POLL` 和 `HAVE_POLL_H` 宏定義。

{% highlight c %}
# if HAVE_POLL && HAVE_POLL_H
#  ifndef EV_USE_POLL
#   define EV_USE_POLL EV_FEATURE_BACKENDS
#  endif
# else
#  undef EV_USE_POLL
#  define EV_USE_POLL 0
# endif
{% endhighlight %}

之後調用 ev_recommended_backends() 得到當前系統支持的 backend 類型，比如 select、poll、epoll 等；然後，接下來就是根據系統支持的 backend，按照一定的優先順序，去初始化 backend 。

<!--
接下來，初始化loop中的ev_prepare監視器pending_w，以及ev_io監視器pipe_w

loop_init返回後，backend已經初始化完成，接著，初始化並啟動信號監視器ev_signal childev。暫不深入。

至此，初始化默認loop的工作就完成了。
-->


### Signal Watcher

在收到 SIGINT 時做些清理，直接退出。

{% highlight text %}
static void sigint_cb (struct ev_loop *loop, ev_signal *w, int revents)
{
    ev_break (EV_A_ EVBREAK_ALL);
}
ev_signal signal_watcher;
ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
ev_signal_start (loop, &signal_watcher);
{% endhighlight %}


{% highlight c %}
/* catch a SIGINT signal, ctrl-c */
#include <ev.h>       // a single header file is required
#include <stdio.h>    // for puts

ev_signal signal_watcher;
static void sigint_cb (struct ev_loop *loop, ev_signal *w, int revents)
{
    puts("catch SIGINT");
    ev_break (EV_A_ EVBREAK_ALL);
}

int main (void)
{
    // use the default event loop unless you have special needs
    struct ev_loop *loop = EV_DEFAULT; /* OR ev_default_loop(0) */

    ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
    ev_signal_start (loop, &signal_watcher);

    // now wait for events to arrive
    ev_run (loop, 0);

    // break was called, so exit
    return 0;
}
{% endhighlight %}














### Child Watcher

fork 一個新進程，給它安裝一個child處理器等待進程結束。

{% highlight text %}
ev_child cw;
static void child_cb (EV_P_ ev_child *w, int revents)
{
	ev_child_stop (EV_A_ w);
	printf ("process %d exited with status %x\n", w->rpid, w->rstatus);
}
pid_t pid = fork ();
if (pid < 0) {            // error
	perror("fork()");
	exit(EXIT_FAILURE);
} else if (pid == 0) {    // child
	// the forked child executes here
	sleep(1);
	exit (EXIT_SUCCESS);
} else {                  // parent
	ev_child_init (&cw, child_cb, pid, 0);
	ev_child_start (EV_DEFAULT_ &cw);
}
{% endhighlight %}

實際上，是通過註冊一個 `SIGCHILD` 信號進行處理的，其回調函數是 `childcb` 。


### Filestat Watcher

監控 Makefile 是否有變化，可以通過修改文件觸發事件。

{% highlight text %}
static void filestat_cb (struct ev_loop *loop, ev_stat *w, int revents)
{
    // "Makefile" changed in some way
    if (w->attr.st_nlink) {
        printf ("Makefile current size  %ld\n", (long)w->attr.st_size);
        printf ("Makefile current atime %ld\n", (long)w->attr.st_mtime);
        printf ("Makefile current mtime %ld\n", (long)w->attr.st_mtime);
    } else { /* you shalt not abuse printf for puts */
        puts ("wow, Makefile is not there, expect problems. "
              "if this is windows, they already arrived\n");
    }
}
ev_stat makefile;
ev_stat_init (&makefile, filestat_cb, "Makefile", 0.);
ev_stat_start (loop, &makefile);
{% endhighlight %}

## 代碼優化

libev 可以通過很多宏進行調優，默認會通過 EV_FEATURES 宏定義一些特性，定義如下。

{% highlight c %}
#ifndef EV_FEATURES
# if defined __OPTIMIZE_SIZE__
#  define EV_FEATURES 0x7c  /* 0111 1100 */
# else
#  define EV_FEATURES 0x7f  /* 0111 1111 */
# endif
#endif

#define EV_FEATURE_CODE     ((EV_FEATURES) &  1) /* 0000 0001 */
#define EV_FEATURE_DATA     ((EV_FEATURES) &  2) /* 0000 0010 */
#define EV_FEATURE_CONFIG   ((EV_FEATURES) &  4) /* 0000 0100 */
#define EV_FEATURE_API      ((EV_FEATURES) &  8) /* 0000 1000 */
#define EV_FEATURE_WATCHERS ((EV_FEATURES) & 16) /* 0001 0000 */
#define EV_FEATURE_BACKENDS ((EV_FEATURES) & 32) /* 0010 0000 */
#define EV_FEATURE_OS       ((EV_FEATURES) & 64) /* 0100 0000 */
{% endhighlight %}




## 內存分配

可以看到很多數組會通過 `array_needsize()` 函數分配內存，簡單來說，為了防止頻繁申請內存，每次都會嘗試申請 `MALLOC_ROUND` 宏指定大小的內存，一般是 4K 。

如下是在 `ev_timer_start()` 函數中的使用方法。

{% highlight text %}
array_needsize(ANHE, timers, timermax, ev_active (w) + 1, EMPTY2);
{% endhighlight %}

簡單來說，`ANHE` 表示數組中的成員類型；`timers` 表示數組的基地址；`timermax` 表示當其值，因為可能會預分配一部分內存，所以在分配完成後，同時會將真正分配的內存數返回；`ev_active(w)+1` 表示需要申請的大小。

在分配內存時，默認會採用 `realloc()` 函數，如果想要自己定義，可以通過 `ev_set_allocator()` 函數進行設置。


## 參考

源碼可以從 [freenode - libev](http://freecode.com/projects/libev) 上下載，不過最近的更新是 2011 年，也可以從 [github](https://github.com/enki/libev) 上下載，或者下載 [本地保存版本 libev-4.22](/reference/linux/libev-4.22.tar.bz2)；幫助文檔可以參考 [本地文檔](/reference/linux/libev.html) 。

對於 python ，提供了相關的擴展 [Python libev interface - pyev](http://packages.python.org/pyev/) 。

<!--
libev and libevent對比
https://blog.gevent.org/2011/04/28/libev-and-libevent/
-->


{% highlight text %}
{% endhighlight %}
