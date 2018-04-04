# 如何使用HandlerThread

在如何使用Handler中解說了Handler的運作原理, 但是他是預設在Main Thread, 如果要多開Thread來使用Hanlder, 則必須自行控制Looper,
相對上比較麻煩, 因此Android開發了HandlerThread讓你能夠方便操作。

HandlerThread整合了Thread、Looper和MessageQueue。
啟動方式跟Thread相同。


```java
HandlerThread handlerThread = new HandlerThread("HandlerThread");
handlerThread.start();

Handler mHandler = new Hanlder(handlerThread.getLooper()){
    public void handleMessage(Message msg){
        super.handleMessage(msg);
        switch(msg.what){
        case 1:
            Logger.e("message receive");
        break;
                }
    }
}
handler.sendEmptyMessage(1);
//或者

new Hanlder(handlerThread.getLooper()).post(new Runnable(){
    public void run(){
       //長時間任務1

       //長時間任務2

    }
});
```

上面的例子一旦執行, 除非你手動停止, 否則該thread會一直等待執行下去。
利用Handler丟Message,
透過Looper循序message發送, 因此確保是執行緒安全的,
但是相對缺點就是效率會下降。

HandlerThread也有生命週期

- Creation(建立): HandlerThread建構子有名稱, 或者名稱跟優先權


```java
HandlerThread(String name)
HandlerThread(String name, int priority)
```
- Execution(執行): 代表已經被start了, 隨時可以接受訊息到來。
- Reset(重設): 你可以將Queue內的訊息清空, 除了已經送出去的Message或者正在執行的Message

```java
mHandlerThread.removeCallbackAndMessages(null);
```

- Termination(終止):

```java
public void stopHandlerThread(HandlerThread handlerThread){
   handlerThread.quit();
   handlerThread.interrupt();
}
//或者直接在handler上面

handler.post(new Runnable(){
   public void run(){
       Looper.myLooper.quit();
   }
}); 
```


HandlerThread確保是循序且執行緒安全的, 有人會覺得既然如此, Thread也可以達成這樣的需求,
是沒錯, 不過這樣變成你必須在程式碼內同時寫在同一個Thread內。


```java
new Thread(new Runnable(){
   public void run(){
       //task 1

       //task 2

       //task 3

   }
});
```
也許有人會反駁, 那麼我可以寫在另外一個Thread內啊!
也沒錯, 但是相對的你就必須開多個Thread, 這樣一來就沒有確保執行緒安全了!

```java
new Thread(new Runnable(){
   public void run(){
       //task 1

   }
});
new Thread(new Runnable(){
   public void run(){
       //task 2

   }
});
new Thread(new Runnable(){
   public void run(){
       //task 3

   }
});
```

如上述例子 task1,task2,task3有存取到共同的資料結構, 則可能會產生concurrent的問題。
那也許會有人說(還真多人XD), 只要確保資料結構是同步的即可。
還是對的! 但是那個資料結構就必須使用Concurrent系列的, 或者自行實作synchronized,
效能相對會下降, 程式碼也會變得比較複雜。
不過使用方法必須對應到使用情境, 也不一定就是HandlerThread是萬用解藥,
畢竟它只有一個Thread, 要達到多核心必須使用多執行緒才能效能最大化,
之後可以參考`如何使用ThreadPool跟如何使用AsyncTask`


