# 線程通信基礎流程分析 Handler、Looper、Message、MessageQueue


> 老司機們都知道，Android的線程間通信就靠Handler、Looper、Message、MessageQueue這四個麻瓜兄弟了，那麼，他們是怎麼運作的呢？下面做一個基於主要源代碼的大學生水平的分析。 [原文鏈接](http://anangryant.leanote.com/post/Handler%E3%80%81Looper%E3%80%81Message%E3%80%81MessageQueue%E5%88%86%E6%9E%90)

##Looper(先分析這個是因為能夠引出四者的關係)
在Looper中，維持一個`Thread`對象以及`MessageQueue`，通過Looper的構造函數我們可以知道:
```
    private Looper(boolean quitAllowed) {
        mQueue = new MessageQueue(quitAllowed);//傳入的參數代表這個Queue是否能夠被退出
        mThread = Thread.currentThread();
    }
```
`Looper`在構造函數裡幹了兩件事情：
1. 將線程對象指向了創建`Looper`的線程
2. 創建了一個新的`MessageQueue`

分析完構造函數之後，接下來我們主要分析兩個方法:
1. `looper.loop()`
2. `looper.prepare()`

###looper.loop()（在當前線程啟動一個Message loop機制，此段代碼將直接分析出Looper、Handler、Message、MessageQueue的關係）
```
 public static void loop() {
        final Looper me = myLooper();//獲得當前線程綁定的Looper
        if (me == null) {
            throw new RuntimeException("No Looper; Looper.prepare() wasn't called on this thread.");
        }
        final MessageQueue queue = me.mQueue;//獲得與Looper綁定的MessageQueue

        // Make sure the identity of this thread is that of the local process,
        // and keep track of what that identity token actually is.
        Binder.clearCallingIdentity();
        final long ident = Binder.clearCallingIdentity();
        
        //進入死循環，不斷地去取對象，分發對象到Handler中消費
        for (;;) {
            Message msg = queue.next(); // 不斷的取下一個Message對象，在這裡可能會造成堵塞。
            if (msg == null) {
                // No message indicates that the message queue is quitting.
                return;
            }

            // This must be in a local variable, in case a UI event sets the logger
            Printer logging = me.mLogging;
            if (logging != null) {
                logging.println(">>>>> Dispatching to " + msg.target + " " +
                        msg.callback + ": " + msg.what);
            }
            
            //在這裡，開始分發Message了
            //至於這個target是神馬？什麼時候被賦值的？ 
            //我們一會分析Handler的時候就會講到
            msg.target.dispatchMessage(msg);

            if (logging != null) {
                logging.println("<<<<< Finished to " + msg.target + " " + msg.callback);
            }

            // Make sure that during the course of dispatching the
            // identity of the thread wasn't corrupted.
            final long newIdent = Binder.clearCallingIdentity();
            if (ident != newIdent) {
                Log.wtf(TAG, "Thread identity changed from 0x"
                        + Long.toHexString(ident) + " to 0x"
                        + Long.toHexString(newIdent) + " while dispatching to "
                        + msg.target.getClass().getName() + " "
                        + msg.callback + " what=" + msg.what);
            }
            
            //當分發完Message之後，當然要標記將該Message標記為 *正在使用* 啦
            msg.recycleUnchecked();
        }
    }
```
*分析了上面的源代碼，我們可以意識到，最重要的方法是：*
1. `queue.next()`
2. `msg.target.dispatchMessage(msg)`
3. `msg.recycleUnchecked()`

其實Looper中最重要的部分都是由`Message`、`MessageQueue`組成的有木有！這段最重要的代碼中涉及到了四個對象,他們與彼此的關係如下：
1. `MessageQueue`：裝食物的容器
2. `Message`：被裝的食物
3. `Handler`（msg.target實際上就是`Handler`）：食物的消費者
4. `Looper`：負責分發食物的人


###looper.prepare()（在當前線程關聯一個Looper對象）
```
 private static void prepare(boolean quitAllowed) {
        if (sThreadLocal.get() != null) {
            throw new RuntimeException("Only one Looper may be created per thread");
        }
        //在當前線程綁定一個Looper
        sThreadLocal.set(new Looper(quitAllowed));
    }
```
以上代碼只做了兩件事情：
1. 判斷當前線程有木有`Looper`，如果有則拋出異常（在這裡我們就可以知道，Android規定一個線程只能夠擁有一個與自己關聯的`Looper`）。
2. 如果沒有的話，那麼就設置一個新的`Looper`到當前線程。

--------------
##Handler
由於我們使用Handler的通常性的第一步是:
```
 Handler handler = new Handler(){
        //你們有沒有很好奇這個方法是在哪裡被回調的？
        //我也是！所以接下來會分析到喲！
        @Override
        public void handleMessage(Message msg) {
            //Handler your Message
        }
    };
```
所以我們先來分析`Handler`的構造方法
```
//空參數的構造方法與之對應，這裡只給出主要的代碼，具體大家可以到源碼中查看
public Handler(Callback callback, boolean async) {
        //打印內存洩露提醒log
        ....
        
        //獲取與創建Handler線程綁定的Looper
        mLooper = Looper.myLooper();
        if (mLooper == null) {
            throw new RuntimeException(
                "Can't create handler inside thread that has not called Looper.prepare()");
        }
        //獲取與Looper綁定的MessageQueue
        //因為一個Looper就只有一個MessageQueue，也就是與當前線程綁定的MessageQueue
        mQueue = mLooper.mQueue;
        mCallback = callback;
        mAsynchronous = async;
        
    }
```
*帶上問題：*
1. `Looper.loop()`死循環中的`msg.target`是什麼時候被賦值的？
2. `handler.handleMessage(msg)`在什麼時候被回調的？

###A1：`Looper.loop()`死循環中的`msg.target`是什麼時候被賦值的？
要分析這個問題，很自然的我們想到從發送消息開始，無論是`handler.sendMessage(msg)`還是`handler.sendEmptyMessage(what)`，我們最終都可以追溯到以下方法
```
public boolean sendMessageAtTime(Message msg, long uptimeMillis) {
        //引用Handler中的MessageQueue
        //這個MessageQueue就是創建Looper時被創建的MessageQueue
        MessageQueue queue = mQueue;
        
        if (queue == null) {
            RuntimeException e = new RuntimeException(
                    this + " sendMessageAtTime() called with no mQueue");
            Log.w("Looper", e.getMessage(), e);
            return false;
        }
        //將新來的Message加入到MessageQueue中
        return enqueueMessage(queue, msg, uptimeMillis);
    }
```

我們接下來分析`enqueueMessage(queue, msg, uptimeMillis)`:
```
private boolean enqueueMessage(MessageQueue queue, Message msg, long uptimeMillis) {
        //顯而易見，大寫加粗的賦值啊！
        **msg.target = this;**
        if (mAsynchronous) {
            msg.setAsynchronous(true);
        }
        return queue.enqueueMessage(msg, uptimeMillis);
    }
```


###A2：`handler.handleMessage(msg)`在什麼時候被回調的？
通過以上的分析，我們很明確的知道`Message`中的`target`是在什麼時候被賦值的了，我們先來分析在`Looper.loop()`中出現過的過的`dispatchMessage(msg)`方法

```
public void dispatchMessage(Message msg) {
        if (msg.callback != null) {
            handleCallback(msg);
        } else {
            if (mCallback != null) {
                if (mCallback.handleMessage(msg)) {
                    return;
                }
            }
            //看到這個大寫加粗的方法調用沒！
            **handleMessage(msg);**
        }
    }
```

加上以上分析，我們將之前分析結果串起來，就可以知道了某些東西：
`Looper.loop()`不斷地獲取`MessageQueue`中的`Message`，然後調用與`Message`綁定的`Handler`對象的`dispatchMessage`方法，最後，我們看到了`handleMessage`就在`dispatchMessage`方法裡被調用的。

------------------
通過以上的分析，我們可以很清晰的知道Handler、Looper、Message、MessageQueue這四者的關係以及如何合作的了。

#總結：
當我們調用`handler.sendMessage(msg)`方法發送一個`Message`時，實際上這個`Message`是發送到**與當前線程綁定**的一個`MessageQueue`中，然後**與當前線程綁定**的`Looper`將會不斷的從`MessageQueue`中取出新的`Message`，調用`msg.target.dispathMessage(msg)`方法將消息分發到與`Message`綁定的`handler.handleMessage()`方法中。

一個`Thread`對應多個`Handler`
一個`Thread`對應一個`Looper`和`MessageQueue`，`Handler`與`Thread`共享`Looper`和`MessageQueue`。
`Message`只是消息的載體，將會被髮送到**與線程綁定的唯一的**`MessageQueue`中，並且被**與線程綁定的唯一的**`Looper`分發，被與其自身綁定的`Handler`消費。

------
- Enjoy Android :) 如果有誤，輕噴，歡迎指正。


