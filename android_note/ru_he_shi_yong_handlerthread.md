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

HandlerThread也有生命周期

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


