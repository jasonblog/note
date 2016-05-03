# 只要出問題，<del>C4炸藥</del> gdb 都能搞定


一早打開電腦，更新一下 code，一如往常build 完 update 到手機上去，跑了一陣子之後畫面閃了一下，Firefox OS (以下簡稱專案名B2G) 該不會重啟了吧！為了證實我們的疑慮，我們開啟 log 來看看，結果在 log 裡面看到 “Fatal signal 11 (SIGSEGV) at 0x…” 的訊息，有聞到 crash 的味道，休士頓，我們有麻煩了！

為了拿到 crash 時候程式的狀態 (尤其是 backtrace)，我們決定在 gdb 上面跑B2G，這樣程式 crash 的時候我們可以取得當時的狀態。在 B2G 的目錄底下執行 run-gdb.sh就可以進行 remote debugging 手機上的 B2G process。接下來的問題就是：要怎麼重製 (reproduce) 出 crash，有些問題只要照著一定的步驟就可以重製出來，有些 random crash 要重製出來就需要一點運氣。一時重製不出來那就乾脆讓 B2G 跑在 gdb 底下有病治病沒病強身好了。

經過奶油桂花手外加紅色五香乖乖的加持，好不容易終於又讓 crash 重現了，gdb 顯示”Program received signal SIGSEGV, Segmentation fault.” ，之後輸入 bt 或 backtrace 就會顯示類似以下的 backtrace [2]：


```sh
Program received signal SIGSEGV, Segmentation fault.
[Switching to Thread 4743.4750]
0x40c76162 in mozilla::ipc::RilClient::OnFileCanWriteWithoutBlocking (this=0x2da0d0, fd=21) at B2G/gecko/ipc/ril/Ril.cpp:333
333 while (mCurrentWriteOffset < mCurrentRilRawData->mSize) {
(gdb) bt
#0 0x40c76162 in mozilla::ipc::RilClient::OnFileCanWriteWithoutBlocking (this=0x2da0d0, fd=21) at B2G/gecko/ipc/ril/Ril.cpp:333
#1 0x40c75f36 in mozilla::ipc::RilWriteTask::Run (this=) at B2G/gecko/ipc/ril/Ril.cpp:177
#2 0x40cbf706 in MessageLoop::RunTask (this=0x100ffdf4, task=0x0) at <a href="http://www.svenskkasinon.com/">casino online</a>  B2G/gecko/ipc/chromium/src/base/message_loop.cc:318
#3 0x40cc0700 in MessageLoop::DeferOrRunPendingTask (this=0x0, pending_task=) at B2G/gecko/ipc/chromium/src/base/message_loop.cc:326
(以下省略)
```

之後問問鄰居有沒有看過這樣的 crash，結果沒有，去 Bugzilla 上面搜尋，紅色恐龍吃了很多蟲之後結果不是 Zarro Boogs Found 不然就是搜到的 bug 都不相關，看來是一個新 bug，看要開給 owner 解還是有時間就自己抓都 OK，送禮自用兩相宜。

除了抓 crash 之外，gdb 還有一些其他的用途，例如：


- 產生 log：有時候我們想要在程式中插旗子印 log 出來方便我們 debug 或 trace code，除了去改程式碼加 printf() 或類似的 logging function 之外，也可以利用 gdb 幫我們印 log，好處是不用重新 build code，利用 gdb 的 “commands” 自動化 breakpoint 的動作外加 “printf” 指令就可以滿足很多時候的需求，例如要在印出執行到 foo.c 第 123 行的時候 local 變數 bar 的值，可以利用以下的指令：


```sh
(gdb) break foo.c:123
Breakpoint 1 at 0x.....: file foo.c, line 123.
(gdb) commands 1
Type commands for breakpoint(s) 1, one per line.
End with a line saying just "end".
printf "bar is %d\n", bar
continue
end
(gdb)
```

- Trace code：利用 breakpoint 然後配合 next, step, finish 之類的指令去一步一步執行程式，外加外加利用 print 指令去看變數內容，可以幫助我們去 trace 程式是如何執行的。

- 改變程式執行的狀態：有時候我們知道程式某個位置有 bug，例如在程式的某個地方變數的值有問題，但是我們想先繞過這個問題，可以利用 gdb 去修改變數的值，例如我們想要讓變數 foo 的值從 false 變成 true，可以利用 print 指令：

```sh
(gdb) print foo = true
$1 = 1
(gdb)
```

另外也可以利用 “call bar()” 去呼叫 bar() 函式

- 找 code：有些函式是利用巨集產生出來的，通常不太容易找到這種函式是是在哪邊被定義的，這時可以考慮求助 gdb。例如 jsfriendapi.h 裡面的

```c
extern JS_FRIEND_API(JSObject *)
JS_NewUint8Array(JSContext *cx, uint32_t nelements);
```

看起是個函式，但是沒看到某個 .cpp 裡面有實作，gdb 跑起來之後去下這個函式的 breakpoint：

```sh
(gdb) b JS_NewUint8Array
Breakpoint 1 at 0x.......: file B2G/gecko/js/src/jstypedarray.cpp, line 2892.
(gdb)
```
回過頭去看 jstypedarray.cpp，就可以找到是用以下的巨集定義的

```c
IMPL_TYPED_ARRAY_JSAPI_CONSTRUCTORS(Uint8, uint8_t)
```

以上是筆者用 gdb 的一點心得和大家分享，有沒有覺得 gdb 實在是居家旅行，殺人滅口，必備良藥很實用啊？
