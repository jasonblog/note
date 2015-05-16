# Handler, Message, Looper, MessageQueue


Handler, Message, Looper, MessageQueue 是 android.os 中的class
也是深度開發 Application 時，必須具備的基本觀念，若清楚瞭解，
便可運用的當。

因為網路有太多模糊不清的文章，大家說法看起來也都不太一樣，
很容易讓人猜東猜西，想東想西的。至於，不想瞎猜的話，就不如直接把source code都讀懂吧。

因此本篇文章，目地在於，快速引導大家快速「正確」的瞭解，重點在於「正確性」
並且透過靜態 trace code  的方式，跟大家解釋它 source code 的運作原理。

因此，對於四個 class沒信心的時候，
就直接將文章看下去，文章會完整交代 trace source code的部份。

### 關於這四個 class 的結論：

整個Handler, Message, MessageQueue, Looper 它們四個 class 只有一個共同的目標
就是讓程式碼，可以丟到其它 thread 去執行。這麼作有什麼好處呢 ??

例如 android 的 GUI 元件是 thread safe的 (意思是，元件的使用，無法multi-thread執行)
Activity 的畫面顯示是由 UI Thread所負責的，若是你寫了 mutlti-thread 程式時
又想更新畫面，就必須要將 Thread 內部的一段程式碼，交由 UI Thread 來執行才行。

OK, 上面四個 class 的共同目地已經說明完畢了，那麼這四個 class有其分工方式。
因此每個 class 的設計又有不同目地。說明如下 …

- Handler 的目地，在於提供 callback function，預其給其它 Thread 作執行但Handler又要如何 transfer 至其它 Thread 呢 ?  於是有了 Message

- Message 的目地，將 Handler 包裝起來,傳送給其它 Thread
但是同時有多條 thread 不斷的在系統中傳遞 Message 那麼如何緩衝呢 ?

- MessageQueue 的目地，是為了讓 Message 能夠作緩衝，好讓Message先暫存起來。因此，當Message 已經被放在其它 Thread上的MessageQueue 之後,
它裡麪包著 Handler,而 Handler上的 callback function 總得有人來執行吧 ??

- Looper 的目地 ：
就是為了將 Message 由 Thread 所對應的 MessageQueue 取出來，並且拿出 Handler
來執行它上面的 callback function.

當 Looper.java 中的 loop() 被呼叫起來之後，它就是在反覆作這件事
不斷將Handler由Message拆包出來，並且執行Handler上的callback function。

以上，已經將這四個class的關係完整說明瞭。看到這邊您還有疑慮嗎 ?
接下來小弟就直接講 trace source 的部份，
教你快速 trace 懂這些 code，迅速驗證出這四個 class 的用途。

**以下開始 trace source code .. Follow Me ^____^**
```
// Looper.java:
loop()
{
    MessageQueue queue = myLooper().mQueue //取得CurrentThread下Looper的MsgQueue

    while (true) {
        Message msg = queue.next(); //跳到msg一個message
        msg.target.dispatchMessage(msg);
        //target 被almost設定的方式，是透過Message.obtain(Handler h)設 h 為target
        msg.recycle(); //In Message class, 只有recycle()與obtain() 作sync同步
    }
}
```
上面程式中，所提到的東西，在以下深入探討。

### (1) dispatchMessage(msg) 是如何重要呢 ?
它呼叫 Handler 上的 handleMessage().

PS: 一般來說，我們會寫個 EHandler extends Handler,
並且重寫handleMessage()function 好讓 Handler 上的 handlerMessage()
被 UI Thread呼叫，來更新畫面。

### (2) 至於 loop() 是如何被使用的呢 ?
```
// typical example 大約是這樣子的
class LooperThread extends Thread
{
    public Handler mHandler;

    public void run()
    {
        Looper.prepare();
        mHandler = new Handler() {
            public void handleMessage(Message msg) {
                // process incoming messages here
            }
        };
        Looper.loop();
    }
}
```
額外話 …

此範例trace下去將發現， Looper.mMainLooper 變數被設定為
(Looper)sThreadLocal.get()

許多重要的 android source code 皆會透過 getMainLooper() 函數取出
Looper.mMainLooper

### (3) msg.target 是個 Handler 類別,  又是從何而來的呢 ?
直接copy高煥堂網路文章中的example code過來 …
講義摘錄之28：Anrdroid 的Message Queue(3/3) 的example code如下

```
class AnyClass implements Runnable
{
    public void run()
    {
        Looper.prepare();
        h = new Handler() {
            public void handleMessage(Message msg) {
                EventHandler ha = new EventHandler(Looper.getMainLooper());
                String obj = (String)msg.obj + ", myThread";
                Message m = ha.obtainMessage(1, 1, 1, obj);
                ha.sendMessage(m); //sendMessage的原理，請見(4)的說明
            }
        };
        Looper.loop();
    }
}

```
我們直接由此來作解釋，
追蹤當中的 obtainMssage 可發現 target的由來。原理如下
```
// Handler.java: Message obtainMessage(int what, int arg1, int arg2)
// Message.java:
static Message obtain(Handler h, int what, int arg1, int arg2)
{
    Message m = obtain();
    m.target = h;  // 這邊就是 msg.target 的由來
    m.what = what;
    m.arg1 = arg1;
    m.arg2 = arg2;
}
```
而 Message m = obtain() 是執行下面這段程式
```
public static Message obtain()
{
    synchronized(mPoolSync) {  //與 recycle() 共用 mPoolSync
        if (mPool != null) {
            Message m = mPool;
            mPool = m.next;
            m.next = null;
            return m;
        }
    }
    return new Message();
}
```
因此你可從 sample code 知道

Handler 呼叫 obtainMessage 的時候，其實是由 mPool 取出 Message來
將 msg.target 設為原 Handler. 並且設定好 what, arg1, arg2 等參數
好讓 Looper 來執行它 …

### (4) 接續 (3) 中的 example code, 它的 sendMessage() 又作了什麼事呢 ?
```
class AnyClass implements Runnable
{
    public void run()
    {
        Looper.prepare();
        h = new Handler() {
            public void handleMessage(Message msg) {
                EventHandler ha = new EventHandler(Looper.getMainLooper());
                String obj = (String)msg.obj + ", myThread";
                Message m = ha.obtainMessage(1, 1, 1, obj);
                ha.sendMessage(m); //sendMessage 作什麼事呢?
            }
        };
        Looper.loop();
    }
}
```
以這個例子，簡單來說，Looper.getMainLooper() 會回傳一個ActivityThread的
Looper object, 即為 Looper.mMainLooper. 而mMainLooper有自己的mQueue

在此穿插一小段 sendMessage() 的作用
Handler本身在暫存一個mQueue, 當Handler的成員函數sendMessage 被呼叫時，即是把帶著 Handler ha 的 Message m，enqueue 至 Handler自己存存的mQueue中。而mQueue的設置，通常是在建構子就被決定好的。因此你得特別注意 Handler 的建構子。

像上面的例子中 sendMessage 即是把帶著 Handler ha 的 Message m，enqueue 至 mMainLooper.mQueue
sendMessage 即是把帶著 Handler ha 的 Message m，enqueue 至 mMainLooper.mQueue

好讓 mMainLooper.loop() 函數把 m 由這個 mMainLooper.mQueue取出(取出時名為msg)
來dispatchMessage，因此就會執行到 msg.target.handleMessage(0
也就是 exmaple code 中的 ha.handleMessage();

因為在 Handler ha = new Handler(Looper looper) 這 Ctor 時，
ha.mLooper = looper 便被紀錄下來，而且ha.mQueue=looper.mQueue也被紀錄下來
也就是 looper.mQueue    (PS:若是用 new Handler(), 則looper取Looper.myLooper())

當 ha.sendMessage 被執行時，便將 msg 塞入 looper.mQueue

### (5) 所以整個 Looper, Message, MessageQueue, Handler 的運作原理是什麼?

因此你的 ha = JohnHandler(MaryLooper) 就像信紙一樣，上面寫著Dear MaryLooper:
上面寫著要執行的程式碼 handleMessage(msg)
透過信封(Message)，以Handler.java 的 sendMessage 將信紙(Handler)傳出去
傳到 MaryLooper 的個人信箱 MessageQueue (也就是MaryLooper.mQueue)

在 MaryLooper 中，有個有個固定的 loop() 會不斷被執行
(假設當初宣告此looper的thread, 有 去running 此 function loop 的話 )

那麼 loop 會收到 Message msg. 而 msg.target (Handler) 即為 JohnHandler這封信紙
看著 JohnHandler 上有 handleMessage() 的信紙內容，
故對 Handler 執行了 dipsatchMessage()，因此執行了 JohnHandler
當初信紙內容的交辦事項。


