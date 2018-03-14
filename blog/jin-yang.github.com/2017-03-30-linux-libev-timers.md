---
title: libev 時間處理
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,program,libev,event loop
description: 在之前的文章中介紹了 libev 庫的基本概念以及常見的使用方法，其中與時間相關的內容只是簡單介紹，不過這一部分涉及到的內容會比較多，而且雜，所以單獨摘出來介紹下。
---

在之前的文章中介紹了 libev 庫的基本概念以及常見的使用方法，其中與時間相關的內容只是簡單介紹，不過這一部分涉及到的內容會比較多，而且雜，所以單獨摘出來介紹下。

<!-- more -->

## 堆

堆 (heap) 又被為優先隊列 (priority queue)，通過堆可以按照元素的優先級取出元素，而不是按照元素進入隊列的先後順序取出元素。

堆的一個經典的實現是完全二叉樹 (complete binary tree)，這樣實現的堆成為二叉堆 (binary heap)，二叉樹要求前 n-1 層必須填滿，第 n 層也必須按照從左到右的順序被填滿，比下圖:

![complete binary tree]({{ site.url }}/images/structure/complete_binary_tree.jpg "complete binary tree"){: .pull-center }

為了實現優先級，還需要要求任意節點的優先級不小於 (或不大於) 它的子節點。

#### 刪除操作

當刪除節點的數值時，原來的位置就會出現一個孔，填充這個孔的方法是把最後的葉子的值賦給該孔，並下調到合適位置，最後把該葉子刪除。

### 定時器管理

在管理定時器時，使用了堆這種數據結構，而且除了常見的最小 2 叉堆之外，它還實現了更高效的 4 叉堆，其主要目的是為了提高 CPU 的緩存利用率。

說白了，就是違背了局部性原理，因為在 2 叉堆中，對元素的操作通常在 `N` 和 `N/2` 之間進行，所以對於含有大量元素的堆來說，兩個操作數之間間隔比較遠，對 CPU 緩存利用不太好。

libev 中的註釋說明，對於元素個數為 50000+ 的堆來說，4 叉堆的效率要提高 5% 左右。

這個 heap 結構存儲在數組中，可以參看靜態二叉樹、最小堆等概念，可以查看 [Binary Heaps](http://www.cs.cmu.edu/~adamchik/15-121/lectures/Binary%20Heaps/heaps.html) 或者 [本地文檔](/reference/programs/libev_Binary_Heaps.maff) 。

### 基本原理

先看一個基本定理：對於 n 叉堆來說，使用數組進行存儲時，下標為 x 的元素，其孩子節點的下標範圍是 `[nx+1, nx+n]`，比如 2 叉堆，下標為 x 的元素，其孩子節點的下標為 `2x+1` 和 `2x+2` 。

對於二叉堆來說，有如下的特點：

* 堆的根節點存放在數組位置 `array[1] (0)` 的地方；
* 父節點 `i` 的左子節點在位置 `array[2*i] (2*i+1)`；
* 父節點 `i` 的右子節點在位置 `array[2*i+1] (2*i+2)`；
* 子節點 `i` 的父節點在位置 `array[floor(i/2)] (floor((i-1)/2))`，`floor()` 為取整，如 `5/2=2`。


{% highlight text %}
                  0                                       1
          /               \                        /              \
         1                 2                      2                3
      /     \           /     \               /      \          /      \
     3       4        5        6             4        5        6         7
   /   \   /   \    /   \    /   \         /   \    /   \    /    \    /    \
  7    8   9   10  11   12  13   14       8    9   10   11  12    13  14    15
 
             C 語言格式                                正常格式
{% endhighlight %}

默認 2-heap ，只有定義了 `EV_USE_4HEAP` 宏之後才會使用 4-heap ，後者通常用於數據量大時；具體來說，對於前者，任一節點，其父節點的位置為 `floor(k/2)`，子節點位置為 `2*k` 和 `2*k+1` 。

根據定理，對於 4 叉堆而言，下標為 x 的元素，其孩子節點的下標範圍是 `[4x+1, 4x+4]`，父節點的下標是 `(x-1)/4`，而在 libev 的代碼中，使用數組存儲堆時，4 叉堆的第一個元素存放在 `a[3]`，2 叉堆的第一個元素存放在 `a[1]`。

<!--
所以，對於Libev中的4叉堆實現而言，下標為k的元素（對應在正常實現中的下標是k-3），其孩子節點的下標範圍是[4(k-3)+1+3, 4(k-3)+4+3]；其父節點的下標是((k-3-1)/4)+3。

對於Libev中的2叉堆實現而言，下標為k的元素（對應在正常實現中，其下標是k-1），其孩子節點的下標範圍是[2(k-1)+1+1,  2(k-1)+2+1]，也就是[2k, 2k+1]；其父節點的下標是((k-1-1)/2)+1，也就是k/2。
-->

### 代碼實現

接下來看看 libev 中的實現。

#### 堆元素

`ANHE` 就是堆元素，它要麼就是一個指向時間監視器結構 `ev_watcher_time` 的指針 `WT`，要麼除了包含該指針之外，還緩存了 `ev_watcher_time` 中的成員 `at`，堆中元素就是根據 `at` 的值進行組織的，具有最小 `at` 值得節點就是根節點。

{% highlight c %}
//----- 宏EV_HEAP_CACHE_AT是為了提高在堆中的緩存利用率，主要是為了對at進行緩存
#if EV_HEAP_CACHE_AT
typedef struct {
    ev_tstamp at;
    WT w;
} ANHE;
#else
typedef WT ANHE;
#endif
{% endhighlight %}

在 libev 中，為了提高緩存命中率，在堆中可以選擇緩存元素 `at`，文檔中的原文是：

{% highlight text %}
Heaps are not very cache-efficient. To improve the cache-efficiency of the timer and
periodics heaps, libev can cache the timestamp (at) within the heap structure(selected
by defining EV_HEAP_CACHE_AT to 1), which uses 8-12 bytes more per watcher and a few
hundred bytes more code, but avoids random read accesses on heap changes. This improves
performance noticeably with many (hundreds) ofwatchers.
{% endhighlight %}

#### 宏定義

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

其中的宏 `HEAP0` 表示堆中第一個元素的下標；`HPARENT(k)` 是求下標為 `k` 的節點的父節點下標；`UPHEAP_DONE` 宏用於向上調整堆時，判斷是否已經到達了根節點，對於4叉堆而言，根節點下標為3，其父節點的下標根據公式得出，也是3，所以結束的條件 `((p) == (k))`，對於2叉堆而言，根節點下標為1，其父節點根據公式得出下標為0，所以結束的條件是 `(!(p))` 。

實現時它是一個最小堆，權值為 "即將觸發的時刻"，所以其根節點總是最近要觸發的 timer；對此堆有兩個基本操作，`upheap()` 和 `downheap()` 。

#### downheap()

`downheap()` 操作會與子節點比較，如果子節點中有小於當前節點的權，則選擇最小的節點進行交換，並一直重複。

{% highlight c %}
#define HEAP0                   1
#define HPARENT(k)              ((k) >> 1)
#define UPHEAP_DONE(p,k)        (!(p))

/* away from the root */
inline_speed void downheap(ANHE *heap, int N, int k)
{
        ANHE he = heap[k];

        for (;;) {
                int c = k << 1;          // 獲取左子節點

                if (c >= N + HEAP0)      // 超過了範圍
                        break;

                //c += c + 1 < N + HEAP0 && ANHE_at (heap [c]) > ANHE_at(heap[c + 1]) ? 1 : 0;
                // 找到子結點中較小的值
                c += ((c + 1) < (N + HEAP0)) && ANHE_at(heap[c]) > ANHE_at(heap[c + 1]) ? 1 : 0;

                if (ANHE_at(he) <= ANHE_at(heap[c])) // 滿足最小棧的條件，則直接退出
                        break;

                heap[k] = heap[c]; // 子結點中的較小值C，賦值給K
                ev_active(ANHE_w(heap[k])) = k;  // 並更新原C在heap中的序號，也就是active字段

                k = c; // 繼續準備下一輪的迭代
        }

        heap[k] = he;
        ev_active(ANHE_w(he)) = k;
}
{% endhighlight %}

對於 4 叉堆的實現如下。

{% highlight c %}
// 從根向下調整，N為堆的元素個數，k表示要調整元素的索引
inline_speed void downheap (ANHE *heap, int N, int k)
{
	ANHE he = heap [k];//先獲得調整
	ANHE *E = heap + N + HEAP0;//結束的指針
	for (;;) {
		ev_tstamp minat; //最小的元素
		ANHE *minpos; //最小元素的指針
		ANHE *pos = heap + DHEAP * (k - HEAP0) + HEAP0 + 1;//k的第一個孩子的指針
		//查找k的最小孩子
		if (expect_true (pos + DHEAP - 1 < E)) { //最後一個孩子沒有越界，有四個孩子
			//在四個孩子中找最小的
			(minpos = pos + 0), (minat = ANHE_at (*minpos));//設置初值
			if (ANHE_at (pos [1]) < minat)
				(minpos = pos + 1), (minat = ANHE_at (*minpos));
			if (ANHE_at (pos [2]) < minat)
				(minpos = pos + 2), (minat = ANHE_at (*minpos));
			if (ANHE_at (pos [3]) < minat)
				(minpos = pos + 3), (minat = ANHE_at (*minpos));
		} else if (pos < E) { //有孩子，但是不是4個孩子
			(minpos = pos + 0), (minat = ANHE_at (*minpos));
			if (pos + 1 < E && ANHE_at (pos [1]) < minat)
				(minpos = pos + 1), (minat = ANHE_at (*minpos));
			if (pos + 2 < E && ANHE_at (pos [2]) < minat)
				(minpos = pos + 2), (minat = ANHE_at (*minpos));
			if (pos + 3 < E && ANHE_at (pos [3]) < minat)
				(minpos = pos + 3), (minat = ANHE_at (*minpos));
		} else { //其他情況，沒孩子，不用調整退出循環
			break;
		}
		//當前節點小於最小孩子，已經是最小堆，不用調整退出
		if (ANHE_at (he) <= minat)
			break;
		//否則將最小元素調到k的位置
		heap [k] = *minpos;
		ev_active (ANHE_w (*minpos)) = k;//將時間監測器設置為索引k
		k = minpos - heap;//設置下一次調整的根節點
	}
	heap [k] = he;//將元素填充到k中
	ev_active (ANHE_w (he)) = k;
}
{% endhighlight %}

#### upheap

當對某一節點執行 `upheap()` 時，就是與其父節點進行比較，如果其值比父節點小，則交換，然後在對這個父節點重複 `upheap()` ，直到頂層。

![libev timer watcher]({{ site.url }}/images/programs/libev_timer_watcher.png "libev timer watcher"){: .pull-center }

在 `ev_timer_start()` 函數中，會將定時器監控器註冊到事件驅動器上。

{% highlight c %}
inline_speed void upheap(ANHE *heap, int k)
{
	ANHE he = heap[k]; // 保存需要調整的結點

	for (;;) {
		int p = HPARENT(k);  // 找到父結點所在的下標

		// 已經到了頭或者滿足最小棧的條件(子結點大於父結點)，則直接退出
		if (UPHEAP_DONE(p, k) || ANHE_at(heap[p]) <= ANHE_at(he))
			break;

		heap[k] = heap[p]; // 與上述類似，進行交換
		ev_active(ANHE_w(heap[k])) = k;

		k = p;
	}

	heap [k] = he;
	ev_active (ANHE_w (he)) = k;
}
{% endhighlight %}


## Timer Watcher

在 `ev_timer_init()` 中，分別設置 `after` 和 `repeat` 參數，表示 `after` 秒後執行一次回調函數，之後每隔 `repeat` 秒執行一次。

{% highlight c %}
//----- 結構體定義，對於時間主要是at+repeat參數
#define EV_WATCHER_TIME(type)      \
  EV_WATCHER (type)                \
  ev_tstamp at;     /* private, 函數初始化對應的after參數 */
typedef struct ev_timer {
  EV_WATCHER_TIME (ev_timer)       // 通用
  ev_tstamp repeat;  /* rw, 函數初始化對應的repeat參數 */
} ev_timer;

//----- 擴展後如下，其中前五個成員是監視器的公共成員
typedef struct ev_timer
{
    int active;        // 標明該監視器在堆數組timers中的下標
    int pending;
    int priority;
    void *data;
    void (*cb)(struct ev_loop *loop, struct ev_timer *w, int revents);

    ev_tstamp at;      // 定時器第一次觸發的時間點，根據mn_now設置
    ev_tstamp repeat;  // 必須>=0，當大於0時表示每隔repeat秒該定時器再次觸發；0表示只觸發一次
} ev_timer;

//----- 初始化，意味著在after秒後執行，設置為0則會立即執行一次；然後每隔repeat秒執行一次
#define ev_timer_init(ev,cb,after,repeat)        \
    do { ev_init ((ev), (cb)); ev_timer_set ((ev),(after),(repeat)); } while (0)

//----- 如下為一個示例程序
void cb (EV_P_ ev_timer *w, int revents) {
    ev_break(EV_P_ EVBREAK_ONE);         // 實際是設置loop_done的值，也即退出主循環
}
ev_timer watcher;
ev_timer_init (&watcher, cb, 2.5, 1.0);  // 初始化，分別表示多長時間開始執行第一次，後面為時間間隔
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

        // 2秒後執行函數
        //ev_timer_init (&oneshot_watcher, oneshot_cb, 2.0, 0.);
        ev_timer_init (&oneshot_watcher, oneshot_cb, 2.0, 1.);
        ev_timer_start (EV_A_ &oneshot_watcher);

        // 5秒後開始循環，每次間隔1秒，如果最後一個參數為0，則只執行一次
        ev_timer_init (&repeate_watcher, repeate_cb, 5., 1.);
        ev_timer_start (EV_A_ &repeate_watcher);

        // 10秒後執行超時，設置為-1表示不退出
        ev_timer_init (&timeout_watcher, timeout_cb, 10., 0.);
        ev_timer_start (EV_A_ &timeout_watcher);

        // now wait for events to arrive
        ev_run (EV_A_ 0);

        // break was called, so exit
        return 0;
}
{% endhighlight %}


### 函數執行流程

函數的初始化過程就不再做過多的介紹了，實際上就是對各個成員變量設置初始值，一般都是設置為 0 ，詳細內容可以直接查看下源碼。

另外，對於 timer 來說，需要注意的是，其中的 active 成員有特殊的含義，實際上就是該對象在 heap 數組中的序號，而其序號是從 1 開始。

{% highlight c %}
void noinline ev_timer_start (EV_P_ ev_timer *w) EV_THROW
{
	// 通過init()初始化後active為0，也就是說還沒有初始化過，如果已經初始化則不會重新設置
	if (expect_false (ev_is_active (w)))
		return;

   // 設置距離當前多久之後觸發，注意，這裡設置的是距離當前時間，該值會作為棧的權重
	ev_at (w) += mn_now;

	EV_FREQUENT_CHECK;

	++timercnt;
	// 啟動定時器，如上所述，active作為棧的序號，也就是(timercnt+HEAP0-1)
	ev_start (EV_A_ (W)w, timercnt + HEAP0 - 1);
	// 調整timers內存的大小
	array_needsize (ANHE, timers, timermax, ev_active (w) + 1, EMPTY2);
	// 將上述設置的timer保存，如上ev_active()返回的是序號
	ANHE_w (timers [ev_active (w)]) = (WT)w;
	// 從watcher中的at值更新到緩存中，注意這裡可以通過宏進行配置
	ANHE_at_cache (timers [ev_active (w)]);
	// 因為是從最後插入的值，所以一定是向上更新棧
	upheap (timers, ev_active (w));

	EV_FREQUENT_CHECK;
}
{% endhighlight %}

代碼比較簡單，首先設置監視器的 `at` 成員，表明在 `at` 時間點超時事件會觸發，注意 `at` 是根據 `mn_now` 設置的，也就是相對於系統啟動時間而言的（或者是日曆時間）。

之後，就是將該監視器加入到堆 `timers` 中，首先將該監視器加到堆中的最後一個元素，然後調用 `upheap` 調整堆。注意監視器的 `active` 成員，表明該監視器在堆數組中的下標。

#### 定時器觸發

接著，看下在一個事件驅動器循環中是如何處理定時器監控器的，這裡我們依然拋開其他的部分，只找定時器相關的看。

在 `/* calculate blocking time */` 塊裡面，可以看到計算 blocking time 的時候會先：

{% highlight c %}
if (timercnt) {
	ev_tstamp to = ANHE_at (timers [HEAP0]) - mn_now;
	if (waittime > to) waittime = to;
}
{% endhighlight %}

如果有定時器，那麼就從定時器堆 `timers` 中取得堆頂上最小的一個時間，這樣就保證了在這個時間前可以從 `backend_poll()` 中超時出來；跳出 `poll()` 等待後執行 `timers_reify()` 處理 `pengding` 的定時器。

{% highlight text %}
inline_size void timers_reify (EV_P)
{
	// 依次取最小堆的堆頂，如果ANHE.at小於當前時間，表示該定時器watcher超時
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

了，那麼將其壓入一個數組中，由於在實際執行pendings二維數組上對應優先級上的watcher是從尾往頭方向的，因此這裡先用一個數組依時間先後次存下到一箇中間數組loop->rfeeds中。然後將其逆序調用ev_invoke_pending插入到pendings二維數組中。這樣在執行pending事件的觸發動作的時候就可以保證，時間靠前的定時器優先執行。函數 feed_reverse和 feed_reverse_done就是將超時的定時器加入到loop->rfeeds暫存數組以及將暫存數組中的pending的watcher插入到pengdings數組的操作。把pending的watcher加入到pendings數組，後續的操作就和之前的一樣了。回依次執行相應的回調函數。


這個過程中還判斷定時器的 w->repeat 的值，如果不為0，那麼會重置該定時器的時間，並將其壓入堆中正確的位置，這樣在指定的時間過後又會被執行。如果其為0，那麼調用ev_timer_stop關閉該定時器。 其首先通過clear_pending置pendings數組中記錄的該watcher上的回調函數為一個不執行任何動作的啞動作。

總結一下定時器就是在backend_poll之前通過定時器堆頂的超時時間，保證blocking的時間不超過最近的定時器時間，在backend_poll返回後，從定時器堆中取得超時的watcher放入到pendings二維數組中，從而在後續處理中可以執行其上註冊的觸發動作。然後從定時器管理堆上刪除該定時器。最後調用和ev_start呼應的ev_stop修改驅動器loop的狀態，即loop->activecnt減少一。並將該watcher的active置零。

對於週期性的事件監控器是同樣的處理過程。只是將timers_reify換成了periodics_reify。其內部會對週期性事件監控器派生類的做類似定時器裡面是否repeat的判斷操作。判斷是否重新調整時間，或者是否重複等邏輯，這些看下代碼比較容易理解，這裡不再贅述。·






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

	// 調用rescheduler()返回下次執行的時間，如果存在回調函數，則會忽略其它參數
	// 包括offset+interval，其輸出示例如下：
	//     begin time is Fri Apr 14 21:51:47 2016
	//   current time is Fri Apr 14 21:52:47 2016
	//   current time is Fri Apr 14 21:53:47 2016
	//   current time is Fri Apr 14 21:54:47 2016
	ev_periodic_init(&periodic_timer, periodic_action, 0, 1, rescheduler);

	// 一般offset在[0, insterval]範圍內，如下，也就是在最近的一個5秒整觸發第一
	// 次回調函數，其輸出示例如下：
	//     begin time is Fri Apr 21 23:24:18 2016
	//   current time is Fri Apr 21 23:24:25 2016
	//   current time is Fri Apr 21 23:24:35 2016
	//   current time is Fri Apr 21 23:24:45 2016
	//ev_periodic_init(&periodic_timer, periodic_action, 5, 10, NULL);

	// 如下只執行一次，也就是在20秒後觸發
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

這是絕對時間定時器，不同於 `ev_timer`，它是基於日曆時間的；例如，指定一個 `ev_periodic` 在 10 秒之後觸發 `(ev_now()+10)`，然後在 10 秒內將系統時間調整為去年，則該定時器會在一年後才觸發超時事件，而 `ev_timer` 依然會在 10 秒之後觸發。

首先看下 libev 中定義的結構體。

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

// 上述結構體實際上等價於如下
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

如上結構體，其中前六個成員與 ev_timer 一樣，而且offset、interval和reschedule_cb都是用來設置觸發時間的，這個會在下面說明。

#### 初始化

與其它 Watcher 相似，初始化可通過 ev_init()+ev_periodic_set() 或直接通過 ev_periodic_init() 初始化，可以查看如下內容。

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

#### 啟動定時器

其中啟動函數如下。

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

共有三種設置超時時間 at 的方法，也就是：

<ol><li>

如果 reschedule_cb() 不為空，則忽略 interval 和 offset，而使用該函數設置超時時間 at，該函數以 ev_rt_now 為參數，設置下次超時事件觸發的時間，示例程序如下。

{% highlight text %}
static ev_tstamp my_rescheduler (ev_periodic *w, ev_tstamp now)
{
    return now + 60.;
}
{% endhighlight %}

這也就是將 at 設置為 1 分鐘之後的時間點開始。</li><br><li>

當 reschedule_cb() 為空且 interval>0 時，調用 periodic_recalc() 函數設置 at，也就是將 at 設置為下一個的 offset+N*interval 時間點，其中的 offset 一般處於 [0, interval] 範圍內。<br>

比如置 offset 為 0，interval 為 3600，意味著當系統時間是完整的 1 小時的時候，也就是系統時間可以被 3600 整除的時候，比如 8:00、9:00 等，就會觸發超時事件。</li><br><li>

如果 reschedule_cb 為空且 interval 為 0，則直接將 at 置為 offset，此時不會重複觸發，觸發一次之後就會停止；而且該監視器也會無視時間調整，比如置 at 為 20110101000000，則只要系統日曆時間超過了改時間，就會觸發超時事件。
</li></ol>

設置好 at 後，就將該監視器加入到堆 periodics 中，這與 ev_timer 的代碼是一樣的，不再贅述。

#### 計算觸發點時間

如下的 periodic_recalc() 函數會重新計算下一個觸發時間點。

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

該函數的作用就是將 at 置為下一個的 offset+N*interval 時間點，其中 `ev_floor(x)` 返回小於 x，且最接近 x 的整數。

<!--
舉個例子可能會容易明白該代碼：interval為10分鐘（600），offset為2分鐘（120），表示將at置為下一個分鐘數為2的時間點。

假設當前為8:01:23，則最終會使得at為8:02:00。計算過程是 ：interval * ev_floor ((ev_rt_now - w->offset) / interval)就表示7:50:00，然後再加上offset就是7:52:00，進入循環，最終調整得at=8:02:00。

假設當前為8:03:56，則最終會使得at為8:12:00。計算過程是：interval * ev_floor ((ev_rt_now -w->offset) / interval)就表示8:00:00，然後再加上offset就是8:02:00，進入循環，最終調整得at=8:12:00。
-->

#### 超時時間調整

通過 periodics_reschedule() 函數，用於重新調整超時時間。

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

在 time_update() 函數中，如果發現日曆時間被調整了，則會通過調用 periodics_reschedule() 調整 ev_periodic 的超時時間點 at；調整的方法與啟動函數中一樣，要麼使用 reschedule_cb() 調整，要麼調用 periodic_recalc() 重新計算 at。

最後，將 periodics 堆中所有元素都調整完畢後，調用 reheap() 使 periodics 恢復堆結構。

<!--
http://blog.csdn.net/gqtcgq/article/details/49531625
####

將激活的超時事件排隊periodics_reify

主要流程跟timers_reify一樣，只不過在重新計算下次觸發時間點at的時候，計算方法跟ev_periodic_start中的一樣。
-->






{% highlight text %}
{% endhighlight %}
