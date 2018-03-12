---
title: libev 时间处理
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,program,libev,event loop
description: 在之前的文章中介绍了 libev 库的基本概念以及常见的使用方法，其中与时间相关的内容只是简单介绍，不过这一部分涉及到的内容会比较多，而且杂，所以单独摘出来介绍下。
---

在之前的文章中介绍了 libev 库的基本概念以及常见的使用方法，其中与时间相关的内容只是简单介绍，不过这一部分涉及到的内容会比较多，而且杂，所以单独摘出来介绍下。

<!-- more -->

## 堆

堆 (heap) 又被为优先队列 (priority queue)，通过堆可以按照元素的优先级取出元素，而不是按照元素进入队列的先后顺序取出元素。

堆的一个经典的实现是完全二叉树 (complete binary tree)，这样实现的堆成为二叉堆 (binary heap)，二叉树要求前 n-1 层必须填满，第 n 层也必须按照从左到右的顺序被填满，比下图:

![complete binary tree]({{ site.url }}/images/structure/complete_binary_tree.jpg "complete binary tree"){: .pull-center }

为了实现优先级，还需要要求任意节点的优先级不小于 (或不大于) 它的子节点。

#### 删除操作

当删除节点的数值时，原来的位置就会出现一个孔，填充这个孔的方法是把最后的叶子的值赋给该孔，并下调到合适位置，最后把该叶子删除。

### 定时器管理

在管理定时器时，使用了堆这种数据结构，而且除了常见的最小 2 叉堆之外，它还实现了更高效的 4 叉堆，其主要目的是为了提高 CPU 的缓存利用率。

说白了，就是违背了局部性原理，因为在 2 叉堆中，对元素的操作通常在 `N` 和 `N/2` 之间进行，所以对于含有大量元素的堆来说，两个操作数之间间隔比较远，对 CPU 缓存利用不太好。

libev 中的注释说明，对于元素个数为 50000+ 的堆来说，4 叉堆的效率要提高 5% 左右。

这个 heap 结构存储在数组中，可以参看静态二叉树、最小堆等概念，可以查看 [Binary Heaps](http://www.cs.cmu.edu/~adamchik/15-121/lectures/Binary%20Heaps/heaps.html) 或者 [本地文档](/reference/programs/libev_Binary_Heaps.maff) 。

### 基本原理

先看一个基本定理：对于 n 叉堆来说，使用数组进行存储时，下标为 x 的元素，其孩子节点的下标范围是 `[nx+1, nx+n]`，比如 2 叉堆，下标为 x 的元素，其孩子节点的下标为 `2x+1` 和 `2x+2` 。

对于二叉堆来说，有如下的特点：

* 堆的根节点存放在数组位置 `array[1] (0)` 的地方；
* 父节点 `i` 的左子节点在位置 `array[2*i] (2*i+1)`；
* 父节点 `i` 的右子节点在位置 `array[2*i+1] (2*i+2)`；
* 子节点 `i` 的父节点在位置 `array[floor(i/2)] (floor((i-1)/2))`，`floor()` 为取整，如 `5/2=2`。


{% highlight text %}
                  0                                       1
          /               \                        /              \
         1                 2                      2                3
      /     \           /     \               /      \          /      \
     3       4        5        6             4        5        6         7
   /   \   /   \    /   \    /   \         /   \    /   \    /    \    /    \
  7    8   9   10  11   12  13   14       8    9   10   11  12    13  14    15
 
             C 语言格式                                正常格式
{% endhighlight %}

默认 2-heap ，只有定义了 `EV_USE_4HEAP` 宏之后才会使用 4-heap ，后者通常用于数据量大时；具体来说，对于前者，任一节点，其父节点的位置为 `floor(k/2)`，子节点位置为 `2*k` 和 `2*k+1` 。

根据定理，对于 4 叉堆而言，下标为 x 的元素，其孩子节点的下标范围是 `[4x+1, 4x+4]`，父节点的下标是 `(x-1)/4`，而在 libev 的代码中，使用数组存储堆时，4 叉堆的第一个元素存放在 `a[3]`，2 叉堆的第一个元素存放在 `a[1]`。

<!--
所以，对于Libev中的4叉堆实现而言，下标为k的元素（对应在正常实现中的下标是k-3），其孩子节点的下标范围是[4(k-3)+1+3, 4(k-3)+4+3]；其父节点的下标是((k-3-1)/4)+3。

对于Libev中的2叉堆实现而言，下标为k的元素（对应在正常实现中，其下标是k-1），其孩子节点的下标范围是[2(k-1)+1+1,  2(k-1)+2+1]，也就是[2k, 2k+1]；其父节点的下标是((k-1-1)/2)+1，也就是k/2。
-->

### 代码实现

接下来看看 libev 中的实现。

#### 堆元素

`ANHE` 就是堆元素，它要么就是一个指向时间监视器结构 `ev_watcher_time` 的指针 `WT`，要么除了包含该指针之外，还缓存了 `ev_watcher_time` 中的成员 `at`，堆中元素就是根据 `at` 的值进行组织的，具有最小 `at` 值得节点就是根节点。

{% highlight c %}
//----- 宏EV_HEAP_CACHE_AT是为了提高在堆中的缓存利用率，主要是为了对at进行缓存
#if EV_HEAP_CACHE_AT
typedef struct {
    ev_tstamp at;
    WT w;
} ANHE;
#else
typedef WT ANHE;
#endif
{% endhighlight %}

在 libev 中，为了提高缓存命中率，在堆中可以选择缓存元素 `at`，文档中的原文是：

{% highlight text %}
Heaps are not very cache-efficient. To improve the cache-efficiency of the timer and
periodics heaps, libev can cache the timestamp (at) within the heap structure(selected
by defining EV_HEAP_CACHE_AT to 1), which uses 8-12 bytes more per watcher and a few
hundred bytes more code, but avoids random read accesses on heap changes. This improves
performance noticeably with many (hundreds) ofwatchers.
{% endhighlight %}

#### 宏定义

{% highlight text %}
#if EV_USE_4HEAP

#define DHEAP 4
#define HEAP0 (DHEAP - 1) /* index of first element in heap */
#define HPARENT(k) ((((k) - HEAP0 - 1) / DHEAP) + HEAP0)
#define UPHEAP_DONE(p,k) ((p) == (k))
...
#else

#define HEAP0 1
#define HPARENT(k) ((k) >> 1)
#define UPHEAP_DONE(p,k) (!(p))
...
{% endhighlight %}

其中的宏 `HEAP0` 表示堆中第一个元素的下标；`HPARENT(k)` 是求下标为 `k` 的节点的父节点下标；`UPHEAP_DONE` 宏用于向上调整堆时，判断是否已经到达了根节点，对于4叉堆而言，根节点下标为3，其父节点的下标根据公式得出，也是3，所以结束的条件 `((p) == (k))`，对于2叉堆而言，根节点下标为1，其父节点根据公式得出下标为0，所以结束的条件是 `(!(p))` 。

实现时它是一个最小堆，权值为 "即将触发的时刻"，所以其根节点总是最近要触发的 timer；对此堆有两个基本操作，`upheap()` 和 `downheap()` 。

#### downheap()

`downheap()` 操作会与子节点比较，如果子节点中有小于当前节点的权，则选择最小的节点进行交换，并一直重复。

{% highlight c %}
#define HEAP0                   1
#define HPARENT(k)              ((k) >> 1)
#define UPHEAP_DONE(p,k)        (!(p))

/* away from the root */
inline_speed void downheap(ANHE *heap, int N, int k)
{
        ANHE he = heap[k];

        for (;;) {
                int c = k << 1;          // 获取左子节点

                if (c >= N + HEAP0)      // 超过了范围
                        break;

                //c += c + 1 < N + HEAP0 && ANHE_at (heap [c]) > ANHE_at(heap[c + 1]) ? 1 : 0;
                // 找到子结点中较小的值
                c += ((c + 1) < (N + HEAP0)) && ANHE_at(heap[c]) > ANHE_at(heap[c + 1]) ? 1 : 0;

                if (ANHE_at(he) <= ANHE_at(heap[c])) // 满足最小栈的条件，则直接退出
                        break;

                heap[k] = heap[c]; // 子结点中的较小值C，赋值给K
                ev_active(ANHE_w(heap[k])) = k;  // 并更新原C在heap中的序号，也就是active字段

                k = c; // 继续准备下一轮的迭代
        }

        heap[k] = he;
        ev_active(ANHE_w(he)) = k;
}
{% endhighlight %}

对于 4 叉堆的实现如下。

{% highlight c %}
// 从根向下调整，N为堆的元素个数，k表示要调整元素的索引
inline_speed void downheap (ANHE *heap, int N, int k)
{
	ANHE he = heap [k];//先获得调整
	ANHE *E = heap + N + HEAP0;//结束的指针
	for (;;) {
		ev_tstamp minat; //最小的元素
		ANHE *minpos; //最小元素的指针
		ANHE *pos = heap + DHEAP * (k - HEAP0) + HEAP0 + 1;//k的第一个孩子的指针
		//查找k的最小孩子
		if (expect_true (pos + DHEAP - 1 < E)) { //最后一个孩子没有越界，有四个孩子
			//在四个孩子中找最小的
			(minpos = pos + 0), (minat = ANHE_at (*minpos));//设置初值
			if (ANHE_at (pos [1]) < minat)
				(minpos = pos + 1), (minat = ANHE_at (*minpos));
			if (ANHE_at (pos [2]) < minat)
				(minpos = pos + 2), (minat = ANHE_at (*minpos));
			if (ANHE_at (pos [3]) < minat)
				(minpos = pos + 3), (minat = ANHE_at (*minpos));
		} else if (pos < E) { //有孩子，但是不是4个孩子
			(minpos = pos + 0), (minat = ANHE_at (*minpos));
			if (pos + 1 < E && ANHE_at (pos [1]) < minat)
				(minpos = pos + 1), (minat = ANHE_at (*minpos));
			if (pos + 2 < E && ANHE_at (pos [2]) < minat)
				(minpos = pos + 2), (minat = ANHE_at (*minpos));
			if (pos + 3 < E && ANHE_at (pos [3]) < minat)
				(minpos = pos + 3), (minat = ANHE_at (*minpos));
		} else { //其他情况，没孩子，不用调整退出循环
			break;
		}
		//当前节点小于最小孩子，已经是最小堆，不用调整退出
		if (ANHE_at (he) <= minat)
			break;
		//否则将最小元素调到k的位置
		heap [k] = *minpos;
		ev_active (ANHE_w (*minpos)) = k;//将时间监测器设置为索引k
		k = minpos - heap;//设置下一次调整的根节点
	}
	heap [k] = he;//将元素填充到k中
	ev_active (ANHE_w (he)) = k;
}
{% endhighlight %}

#### upheap

当对某一节点执行 `upheap()` 时，就是与其父节点进行比较，如果其值比父节点小，则交换，然后在对这个父节点重复 `upheap()` ，直到顶层。

![libev timer watcher]({{ site.url }}/images/programs/libev_timer_watcher.png "libev timer watcher"){: .pull-center }

在 `ev_timer_start()` 函数中，会将定时器监控器注册到事件驱动器上。

{% highlight c %}
inline_speed void upheap(ANHE *heap, int k)
{
	ANHE he = heap[k]; // 保存需要调整的结点

	for (;;) {
		int p = HPARENT(k);  // 找到父结点所在的下标

		// 已经到了头或者满足最小栈的条件(子结点大于父结点)，则直接退出
		if (UPHEAP_DONE(p, k) || ANHE_at(heap[p]) <= ANHE_at(he))
			break;

		heap[k] = heap[p]; // 与上述类似，进行交换
		ev_active(ANHE_w(heap[k])) = k;

		k = p;
	}

	heap [k] = he;
	ev_active (ANHE_w (he)) = k;
}
{% endhighlight %}


## Timer Watcher

在 `ev_timer_init()` 中，分别设置 `after` 和 `repeat` 参数，表示 `after` 秒后执行一次回调函数，之后每隔 `repeat` 秒执行一次。

{% highlight c %}
//----- 结构体定义，对于时间主要是at+repeat参数
#define EV_WATCHER_TIME(type)      \
  EV_WATCHER (type)                \
  ev_tstamp at;     /* private, 函数初始化对应的after参数 */
typedef struct ev_timer {
  EV_WATCHER_TIME (ev_timer)       // 通用
  ev_tstamp repeat;  /* rw, 函数初始化对应的repeat参数 */
} ev_timer;

//----- 扩展后如下，其中前五个成员是监视器的公共成员
typedef struct ev_timer
{
    int active;        // 标明该监视器在堆数组timers中的下标
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_timer *w, int revents);

    ev_tstamp at;      // 定时器第一次触发的时间点，根据mn_now设置
    ev_tstamp repeat;  // 必须>=0，当大于0时表示每隔repeat秒该定时器再次触发；0表示只触发一次
} ev_timer;

//----- 初始化，意味着在after秒后执行，设置为0则会立即执行一次；然后每隔repeat秒执行一次
#define ev_timer_init(ev,cb,after,repeat)        \
    do { ev_init ((ev), (cb)); ev_timer_set ((ev),(after),(repeat)); } while (0)

//----- 如下为一个示例程序
void cb (EV_P_ ev_timer *w, int revents) {
    ev_break(EV_P_ EVBREAK_ONE);         // 实际是设置loop_done的值，也即退出主循环
}
ev_timer watcher;
ev_timer_init (&watcher, cb, 2.5, 1.0);  // 初始化，分别表示多长时间开始执行第一次，后面为时间间隔
ev_timer_start (loop, &watcher);
{% endhighlight %}


### 示例程序

{% highlight c %}
#include <ev.h>       // a single header file is required
#include <time.h>     // for time()
#include <stdio.h>    // for printf()
#include <stdint.h>   // for uintmax_t

// every watcher type has its own typedef'd struct with the name ev_TYPE
ev_timer timeout_watcher;
ev_timer repeate_watcher;
ev_timer oneshot_watcher;

// another callback, this time for a time-out
static void timeout_cb (EV_P_ ev_timer *w, int revents)
{
        (void) w;
        (void) revents;
        printf("timeout at %ju\n", (uintmax_t)time(NULL));
        // this causes the innermost ev_run to stop iterating
        ev_break (EV_A_ EVBREAK_ONE);
}
static void repeate_cb (EV_P_ ev_timer *w, int revents)
{
        (void) w;
        (void) revents;
        printf("repeate at %ju\n", (uintmax_t)time(NULL));
}
static void oneshot_cb (EV_P_ ev_timer *w, int revents)
{
        (void) w;
        (void) revents;
        printf("oneshot at %ju\n", (uintmax_t)time(NULL));
        ev_timer_stop(EV_A_ w);
}

int main (void)
{
        time_t result;
        EV_P EV_DEFAULT; /* OR ev_default_loop(0) */

        result = time(NULL);
        printf("  start at %ju\n", (uintmax_t)result);

        // 2秒后执行函数
        //ev_timer_init (&oneshot_watcher, oneshot_cb, 2.0, 0.);
        ev_timer_init (&oneshot_watcher, oneshot_cb, 2.0, 1.);
        ev_timer_start (EV_A_ &oneshot_watcher);

        // 5秒后开始循环，每次间隔1秒，如果最后一个参数为0，则只执行一次
        ev_timer_init (&repeate_watcher, repeate_cb, 5., 1.);
        ev_timer_start (EV_A_ &repeate_watcher);

        // 10秒后执行超时，设置为-1表示不退出
        ev_timer_init (&timeout_watcher, timeout_cb, 10., 0.);
        ev_timer_start (EV_A_ &timeout_watcher);

        // now wait for events to arrive
        ev_run (EV_A_ 0);

        // break was called, so exit
        return 0;
}
{% endhighlight %}


### 函数执行流程

函数的初始化过程就不再做过多的介绍了，实际上就是对各个成员变量设置初始值，一般都是设置为 0 ，详细内容可以直接查看下源码。

另外，对于 timer 来说，需要注意的是，其中的 active 成员有特殊的含义，实际上就是该对象在 heap 数组中的序号，而其序号是从 1 开始。

{% highlight c %}
void noinline ev_timer_start (EV_P_ ev_timer *w) EV_THROW
{
	// 通过init()初始化后active为0，也就是说还没有初始化过，如果已经初始化则不会重新设置
	if (expect_false (ev_is_active (w)))
		return;

   // 设置距离当前多久之后触发，注意，这里设置的是距离当前时间，该值会作为栈的权重
	ev_at (w) += mn_now;

	EV_FREQUENT_CHECK;

	++timercnt;
	// 启动定时器，如上所述，active作为栈的序号，也就是(timercnt+HEAP0-1)
	ev_start (EV_A_ (W)w, timercnt + HEAP0 - 1);
	// 调整timers内存的大小
	array_needsize (ANHE, timers, timermax, ev_active (w) + 1, EMPTY2);
	// 将上述设置的timer保存，如上ev_active()返回的是序号
	ANHE_w (timers [ev_active (w)]) = (WT)w;
	// 从watcher中的at值更新到缓存中，注意这里可以通过宏进行配置
	ANHE_at_cache (timers [ev_active (w)]);
	// 因为是从最后插入的值，所以一定是向上更新栈
	upheap (timers, ev_active (w));

	EV_FREQUENT_CHECK;
}
{% endhighlight %}

代码比较简单，首先设置监视器的 `at` 成员，表明在 `at` 时间点超时事件会触发，注意 `at` 是根据 `mn_now` 设置的，也就是相对于系统启动时间而言的（或者是日历时间）。

之后，就是将该监视器加入到堆 `timers` 中，首先将该监视器加到堆中的最后一个元素，然后调用 `upheap` 调整堆。注意监视器的 `active` 成员，表明该监视器在堆数组中的下标。

#### 定时器触发

接着，看下在一个事件驱动器循环中是如何处理定时器监控器的，这里我们依然抛开其他的部分，只找定时器相关的看。

在 `/* calculate blocking time */` 块里面，可以看到计算 blocking time 的时候会先：

{% highlight c %}
if (timercnt) {
	ev_tstamp to = ANHE_at (timers [HEAP0]) - mn_now;
	if (waittime > to) waittime = to;
}
{% endhighlight %}

如果有定时器，那么就从定时器堆 `timers` 中取得堆顶上最小的一个时间，这样就保证了在这个时间前可以从 `backend_poll()` 中超时出来；跳出 `poll()` 等待后执行 `timers_reify()` 处理 `pengding` 的定时器。

{% highlight text %}
inline_size void timers_reify (EV_P)
{
	// 依次取最小堆的堆顶，如果ANHE.at小于当前时间，表示该定时器watcher超时
	if (timercnt && ANHE_at(timers[HEAP0]) < mn_now) {
		do {
			ev_timer *w = (ev_timer *)ANHE_w(timers[HEAP0]);

			if (w->repeat) { /* first reschedule or stop timer */
				ev_at(w) += w->repeat;
				if (ev_at(w) < mn_now)
					ev_at(w) = mn_now;

				ANHE_at_cache(timers[HEAP0]);
				downheap(timers, timercnt, HEAP0);
			} else {
				ev_timer_stop(EV_A_ w); /* nonrepeating: stop timer */
			}

			EV_FREQUENT_CHECK;
			feed_reverse(EV_A_ (W)w);
		} while (timercnt && ANHE_at(timers[HEAP0]) < mn_now);

		feed_reverse_done(EV_A_ EV_TIMER);
	}
}
{% endhighlight %}

了，那么将其压入一个数组中，由于在实际执行pendings二维数组上对应优先级上的watcher是从尾往头方向的，因此这里先用一个数组依时间先后次存下到一个中间数组loop->rfeeds中。然后将其逆序调用ev_invoke_pending插入到pendings二维数组中。这样在执行pending事件的触发动作的时候就可以保证，时间靠前的定时器优先执行。函数 feed_reverse和 feed_reverse_done就是将超时的定时器加入到loop->rfeeds暂存数组以及将暂存数组中的pending的watcher插入到pengdings数组的操作。把pending的watcher加入到pendings数组，后续的操作就和之前的一样了。回依次执行相应的回调函数。


这个过程中还判断定时器的 w->repeat 的值，如果不为0，那么会重置该定时器的时间，并将其压入堆中正确的位置，这样在指定的时间过后又会被执行。如果其为0，那么调用ev_timer_stop关闭该定时器。 其首先通过clear_pending置pendings数组中记录的该watcher上的回调函数为一个不执行任何动作的哑动作。

总结一下定时器就是在backend_poll之前通过定时器堆顶的超时时间，保证blocking的时间不超过最近的定时器时间，在backend_poll返回后，从定时器堆中取得超时的watcher放入到pendings二维数组中，从而在后续处理中可以执行其上注册的触发动作。然后从定时器管理堆上删除该定时器。最后调用和ev_start呼应的ev_stop修改驱动器loop的状态，即loop->activecnt减少一。并将该watcher的active置零。

对于周期性的事件监控器是同样的处理过程。只是将timers_reify换成了periodics_reify。其内部会对周期性事件监控器派生类的做类似定时器里面是否repeat的判断操作。判断是否重新调整时间，或者是否重复等逻辑，这些看下代码比较容易理解，这里不再赘述。·






{% highlight c %}
#include <ev.h>       // a single header file is required
#include <time.h>     // a single header file is required
#include <stdio.h>    // for puts

ev_periodic periodic_timer;

void periodic_action(EV_P_ ev_periodic *w, int revents)
{
	(void) w;
	(void) revents;
	time_t now = time(NULL);
	printf("current time is %s", ctime(&now));
}

static ev_tstamp rescheduler(ev_periodic *w, ev_tstamp now)
{
	(void) w;
	return now + 60;
}

int main(void)
{
	time_t now = time(NULL);
	EV_P EV_DEFAULT; /* OR ev_default_loop(0) */

	// 调用rescheduler()返回下次执行的时间，如果存在回调函数，则会忽略其它参数
	// 包括offset+interval，其输出示例如下：
	//     begin time is Fri Apr 14 21:51:47 2016
	//   current time is Fri Apr 14 21:52:47 2016
	//   current time is Fri Apr 14 21:53:47 2016
	//   current time is Fri Apr 14 21:54:47 2016
	ev_periodic_init(&periodic_timer, periodic_action, 0, 1, rescheduler);

	// 一般offset在[0, insterval]范围内，如下，也就是在最近的一个5秒整触发第一
	// 次回调函数，其输出示例如下：
	//     begin time is Fri Apr 21 23:24:18 2016
	//   current time is Fri Apr 21 23:24:25 2016
	//   current time is Fri Apr 21 23:24:35 2016
	//   current time is Fri Apr 21 23:24:45 2016
	//ev_periodic_init(&periodic_timer, periodic_action, 5, 10, NULL);

	// 如下只执行一次，也就是在20秒后触发
	//     begin time is Fri Apr 21 23:27:04 2016
	//   current time is Fri Apr 21 23:27:24 2016
	//ev_periodic_init(&periodic_timer, periodic_action, now+20, 0, NULL);      //3

	ev_periodic_start(EV_A_ &periodic_timer);

	printf("  begin time is %s", ctime(&now));

	ev_run (EV_A_ 0);

	return 0;
}
{% endhighlight %}




### Periodic Wather

这是绝对时间定时器，不同于 `ev_timer`，它是基于日历时间的；例如，指定一个 `ev_periodic` 在 10 秒之后触发 `(ev_now()+10)`，然后在 10 秒内将系统时间调整为去年，则该定时器会在一年后才触发超时事件，而 `ev_timer` 依然会在 10 秒之后触发。

首先看下 libev 中定义的结构体。

{% highlight c %}
#define EV_WATCHER(type)              \
  int active; /* private */           \
  int pending; /* private */          \
  EV_DECL_PRIORITY /* private */      \
  EV_COMMON /* rw */                  \
  EV_CB_DECLARE (type) /* private */

#define EV_WATCHER_TIME(type)         \
  EV_WATCHER (type)                   \
  ev_tstamp at;     /* private */

typedef struct ev_periodic
{
  EV_WATCHER_TIME (ev_periodic)

  ev_tstamp offset; /* rw */
  ev_tstamp interval; /* rw */
  ev_tstamp (*reschedule_cb)(struct ev_periodic *w, ev_tstamp now) EV_THROW; /* rw */
} ev_periodic;

// 上述结构体实际上等价于如下
typedef struct ev_periodic
{
    int active;
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_periodic *w, int revents);
    ev_tstamp at;

    ev_tstamp offset; /* rw */
    ev_tstamp interval; /* rw */
    ev_tstamp (*reschedule_cb)(struct ev_periodic *w, ev_tstamp now) EV_THROW; /* rw */
} ev_periodic;
{% endhighlight %}

如上结构体，其中前六个成员与 ev_timer 一样，而且offset、interval和reschedule_cb都是用来设置触发时间的，这个会在下面说明。

#### 初始化

与其它 Watcher 相似，初始化可通过 ev_init()+ev_periodic_set() 或直接通过 ev_periodic_init() 初始化，可以查看如下内容。

{% highlight c %}
#define ev_init(ev,cb_) do {                      \
  ((ev_watcher *)(void *)(ev))->active  =         \
  ((ev_watcher *)(void *)(ev))->pending = 0;      \
  ev_set_priority ((ev), 0);                      \
  ev_set_cb ((ev), cb_);                          \
} while (0)

#define ev_periodic_set(ev,ofs_,ival_,rcb_)  do { \
  (ev)->offset = (ofs_);                          \
  (ev)->interval = (ival_);                       \
  (ev)->reschedule_cb = (rcb_);                   \
} while (0)

#define ev_periodic_init(ev,cb,ofs,ival,rcb) do { \
  ev_init ((ev), (cb));                           \
  ev_periodic_set ((ev),(ofs),(ival),(rcb));      \
} while (0)
{% endhighlight %}

#### 启动定时器

其中启动函数如下。

{% highlight c %}
void noinline ev_periodic_start (EV_P_ ev_periodic *w) EV_THROW
{
  if (expect_false (ev_is_active (w)))
    return;

  if (w->reschedule_cb)
    ev_at (w) = w->reschedule_cb (w, ev_rt_now);
  else if (w->interval)
    {
      assert (("libev: ev_periodic_start called with negative interval value", w->interval >= 0.));
      periodic_recalc (EV_A_ w);
    }
  else
    ev_at (w) = w->offset;

  EV_FREQUENT_CHECK;

  ++periodiccnt;
  ev_start (EV_A_ (W)w, periodiccnt + HEAP0 - 1);
  array_needsize (ANHE, periodics, periodicmax, ev_active (w) + 1, EMPTY2);
  ANHE_w (periodics [ev_active (w)]) = (WT)w;
  ANHE_at_cache (periodics [ev_active (w)]);
  upheap (periodics, ev_active (w));

  EV_FREQUENT_CHECK;
}
{% endhighlight %}

共有三种设置超时时间 at 的方法，也就是：

<ol><li>

如果 reschedule_cb() 不为空，则忽略 interval 和 offset，而使用该函数设置超时时间 at，该函数以 ev_rt_now 为参数，设置下次超时事件触发的时间，示例程序如下。

{% highlight text %}
static ev_tstamp my_rescheduler (ev_periodic *w, ev_tstamp now)
{
    return now + 60.;
}
{% endhighlight %}

这也就是将 at 设置为 1 分钟之后的时间点开始。</li><br><li>

当 reschedule_cb() 为空且 interval>0 时，调用 periodic_recalc() 函数设置 at，也就是将 at 设置为下一个的 offset+N*interval 时间点，其中的 offset 一般处于 [0, interval] 范围内。<br>

比如置 offset 为 0，interval 为 3600，意味着当系统时间是完整的 1 小时的时候，也就是系统时间可以被 3600 整除的时候，比如 8:00、9:00 等，就会触发超时事件。</li><br><li>

如果 reschedule_cb 为空且 interval 为 0，则直接将 at 置为 offset，此时不会重复触发，触发一次之后就会停止；而且该监视器也会无视时间调整，比如置 at 为 20110101000000，则只要系统日历时间超过了改时间，就会触发超时事件。
</li></ol>

设置好 at 后，就将该监视器加入到堆 periodics 中，这与 ev_timer 的代码是一样的，不再赘述。

#### 计算触发点时间

如下的 periodic_recalc() 函数会重新计算下一个触发时间点。

{% highlight c %}
static void noinline periodic_recalc (EV_P_ ev_periodic *w)
{
  ev_tstamp interval = w->interval > MIN_INTERVAL ? w->interval : MIN_INTERVAL;
  ev_tstamp at = w->offset + interval * ev_floor ((ev_rt_now - w->offset) / interval);

  /* the above almost always errs on the low side */
  while (at <= ev_rt_now) {
      ev_tstamp nat = at + w->interval;

      /* when resolution fails us, we use ev_rt_now */
      if (expect_false (nat == at)) {
          at = ev_rt_now;
          break;
      }

      at = nat;
  }

  ev_at (w) = at;
}
{% endhighlight %}

该函数的作用就是将 at 置为下一个的 offset+N*interval 时间点，其中 `ev_floor(x)` 返回小于 x，且最接近 x 的整数。

<!--
举个例子可能会容易明白该代码：interval为10分钟（600），offset为2分钟（120），表示将at置为下一个分钟数为2的时间点。

假设当前为8:01:23，则最终会使得at为8:02:00。计算过程是 ：interval * ev_floor ((ev_rt_now - w->offset) / interval)就表示7:50:00，然后再加上offset就是7:52:00，进入循环，最终调整得at=8:02:00。

假设当前为8:03:56，则最终会使得at为8:12:00。计算过程是：interval * ev_floor ((ev_rt_now -w->offset) / interval)就表示8:00:00，然后再加上offset就是8:02:00，进入循环，最终调整得at=8:12:00。
-->

#### 超时时间调整

通过 periodics_reschedule() 函数，用于重新调整超时时间。

{% highlight c %}
static void noinline ecb_cold periodics_reschedule (EV_P)
{
  int i;

  /* adjust periodics after time jump */
  for (i = HEAP0; i < periodiccnt + HEAP0; ++i)
    {
      ev_periodic *w = (ev_periodic *)ANHE_w (periodics [i]);

      if (w->reschedule_cb)
        ev_at (w) = w->reschedule_cb (w, ev_rt_now);
      else if (w->interval)
        periodic_recalc (EV_A_ w);

      ANHE_at_cache (periodics [i]);
    }

  reheap (periodics, periodiccnt);
}
{% endhighlight %}

在 time_update() 函数中，如果发现日历时间被调整了，则会通过调用 periodics_reschedule() 调整 ev_periodic 的超时时间点 at；调整的方法与启动函数中一样，要么使用 reschedule_cb() 调整，要么调用 periodic_recalc() 重新计算 at。

最后，将 periodics 堆中所有元素都调整完毕后，调用 reheap() 使 periodics 恢复堆结构。

<!--
http://blog.csdn.net/gqtcgq/article/details/49531625
####

将激活的超时事件排队periodics_reify

主要流程跟timers_reify一样，只不过在重新计算下次触发时间点at的时候，计算方法跟ev_periodic_start中的一样。
-->






{% highlight text %}
{% endhighlight %}
