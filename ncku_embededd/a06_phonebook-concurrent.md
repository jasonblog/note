# A06: phonebook-concurrent


﻿# A06: phonebook-concurrent

## 預期目標

* 學習第二週提及的 [concurrency](/s/H10MXXoT) 程式設計
* 學習 POSIX Thread
* 學習效能分析工具
* code refactoring 練習
* 探索 clz 的應用

## Code Refactoring
* refactoring（重構）的定義：「在不改變軟體的外在行為之下，改善既有軟體的內部設計」
* 侯捷：「作為一個程式員，任誰都有看不順眼手上程式碼的經驗 —— 程式碼來自你鄰桌那個菜鳥，或三個月前的自己。面臨此境，有人選擇得過且過；然而根據我對『程式員』人格特質的瞭解，更多人盼望插手整頓。挽起袖子劍及履及，其勇可嘉其慮未縝。過去或許不得不暴虎憑河,忍受風險。現在，有了嚴謹的重構準則和嚴密的重構手法，『穩定中求發展』終於有了保障。」
* 延伸閱讀: 
	* 《重構》(原文書名: [Refactoring - Improving the Design of Existing Code](https://www.csie.ntu.edu.tw/~r95004/Refactoring_improving_the_design_of_existing_code.pdf))
	* [什麼是Refactoring？](http://teddy-chen-tw.blogspot.tw/2014/04/refactoring.html)
	* [對於重構的兩則常見誤解](http://www.ithome.com.tw/node/79813)

## clz 的應用: 加速記憶體管理
* 摘錄自 MIT 的論文〈[SuperMalloc: A Super Fast Multithreaded Malloc for 64-bit Machine](http://supertech.csail.mit.edu/papers/Kuszmaul15.pdf)〉
:::info
Small objects are as small as 8 bytes, and increase in size by at most 25% to limit internal fragmentation. Small object sizes are regularly spaced: the sizes are 8, 10, 12, 14, 16, 20, 24, . . . , 224, and their sizes take the form k · 2^i^
for 4 ≤ k ≤ 7 and 1 ≤ i ≤ 5. In other words, a small object size, when written in binary is a 1, followed by two arbitrary digits, followed by zeros. Thus bin 0 contains 8-byte objects, bin 1 contains 10-byte objects, and so forth. To compute the bin number from a small size can be done with bit hacking in O(1) operations.
:::
* 以下程式碼轉換 size 為 bin number，透過 clz() 可得到一個數值用二進位表示中開頭的 "0" 的數量:
```c
int size_2_bin(size_t s)
{
    if (s <= 8) return 0;
    if (s <= 320) {
        // Number of leading zeros in s.
        int z = clz(s);
        // Round up to the relevant
        // power of 2.
        size_t r = s + (1ul << (61 - z)) -1;
        int y = clz(r);
        // y indicates which power of two.
        // r shifted and masked indicates
        // what the low-order bits are.
        return 4 * (60 - y)+ ((r >> (61 - y)) & 3);
    }
    if (s <= 448) return 22;
    if (s <= 512) return 23;
    ...
    if (size <= 1044480) return 45;
    return 45 + ceil(size-1044480, 4096);
}
```
* [SuperMalloc](https://github.com/sysprog21/SuperMalloc) (我們課程修改過的版本)，編譯和測試方式:
```shell
$ git clone https://github.com/sysprog21/SuperMalloc
$ cd SuperMalloc/release
$ make
$ LD_PRELOAD=lib/libsupermalloc_pthread.so ./test-malloc_test
```
:::warning
執行最後一行需要等待，請抱持耐心，預期會得到類似 `8996967` 的數值輸出
:::
* 之後可在執行時期 (runtime)，透過下方命令指定用 SuperMalloc (不需要重新編譯程式碼)
```shell
$ LD_PRELOAD=libsupermalloc_pthread.so所在的路徑 接著想執行的程式路徑
```

## 透過 POSIX Thread 搭配 mmap 的初步實驗
* 由吳彥寬貢獻的[實驗](/s/BkN1JNQp)
	* [程式解說 video](https://www.youtube.com/watch?v=sWtWslsr4Ac) (必看!)
* 透過 [mmap](http://man7.org/linux/man-pages/man2/mmap.2.html) 避免 blocking I/O
* 透過 POSIX Thread 建立各自獨立的 linked list，最終再合併為單一 list

- [ ] thread num = 1:
![](https://i.imgur.com/ouXKygv.png)

- [ ] thread num = 2
![](https://i.imgur.com/VOd5j2c.png)

- [ ] thread num = 4
![](https://i.imgur.com/JHQSsBI.png)

- [ ] thread num = 8:
![](https://i.imgur.com/6zCQjCh.png)

- [ ] thread num = 16:
![](https://i.imgur.com/OYfVPrl.png)

這邊很清楚可見，太多 thread 會導致更慢的執行時間，而執行緒數量為 1, 2, 4 時，沒有顯著差異，問題出在前述實做使用 sync 的方式，所以若有一個thread跑比較慢，那總體時間也就會比較慢。


## 作業要求

* 在 GitHub 上 fork [phonebook-concurrent](https://github.com/sysprog21/phonebook-concurrent)，然後適度修改 `phonebook_opt.c` 和相關的檔案
* 除了修改程式，也要編輯「[作業區](/s/HykJUcnT)」，增添開發紀錄和 GitHub 連結
    * 至少涵蓋研讀 [concurrency](/s/H10MXXoT) 教材的認知、程式正確性驗證、效能分析實驗 (必須要有圖表)，以及充份說明你如何改善效能
* 延續 [A01: phonebook](/s/S1RVdgza) 的開發方向，本作業著重於透過 POSIX Thread 來縮減 `alloc()` 的時間成本
	* 詳細閱讀吳彥寬的[實驗](/s/BkN1JNQp)，指出他的實做缺失，並提出改進縮減 append() 時間的可行提案，接著開發程式來驗證
	* 提示：可透過建立 thread pool 來管理 worker thread
	* 第一週 phonebook 未完成和待改進項目也一併在 [phonebook-concurrent](https://github.com/sysprog21/phonebook-concurrent) 的基礎下進行
* 學習 [concurrent-ll](https://github.com/jserv/concurrent-ll) (concurrent linked-list 實作) 的 scalability 分析方式，透過 gnuplot 製圖比較 list 操作的效能
* 一併嘗試重構 (refactor) 給定的程式碼，使得程式更容易閱讀和維護。延續 [A05: introspect](/s/BkhIF92p)，不只是在共筆上用文字提出良性詳盡的批評，也該反映在程式碼的變革
* 務必使用 astyle 來對程式碼進行排版，詳細使用方式見 [README.md](https://github.com/sysprog21/phonebook-concurrent/blob/master/README.md)
* 截止日期：
    * 08:00AM Oct 7, 2016 (含) 之前
    * 越早在 GitHub 上有動態、越早接受 code review，評分越高

## 挑戰題
* 針對電話簿的使用情境，提出 lock-free concurrent linked list 的實作，應該包含 insert, delete, search 等等，而且實際分析其 scalability
* 分析 SuperMalloc 對於多執行緒程式的效能影響
* 用 C11 Atomic 改寫原本的 POSIX Thread 程式碼
。參考資料: 
	* [Atomics in C programming](https://www2.informatik.hu-berlin.de/~keil/docs/keil_-_c11_atomics_20140202.pdf)
	* [You Can Do Any Kind of Atomic Read-Modify-Write Operation](http://preshing.com/20150402/you-can-do-any-kind-of-atomic-read-modify-write-operation/)
	* [Boost atomic examples](http://www.boost.org/doc/libs/1_61_0/doc/html/atomic/usage_examples.html)