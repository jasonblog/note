# 多執行緒-Handler和Thread的關係 （2）


我重新敘述一下

 

多執行緒的部分，有幾個名詞

Runnable 工作包 (要做的事情) <br>
Thread 執行緒<br>
Handler<br>
Message<br>

## Runnable

就是像是專案管理裡的工作包，說穿了就是要做的事情啦，像是這樣

```java
private Runnable r1=new Runnable () {
    public void run() {
        //做了很多事
    }
};
```
這裡的 r1 就是一個runnable


## Thread

在Android的世界裡，Thread分成二種

`1.` 單次型 (Java原有的)

`2.`  常駐型 (Android特有的)



`1.`  單次型

意指就是給他`一件`艱巨的任務
他做完就會`關閉`了
寫法實在有夠簡單


```java
Thread t1=new Thread(r1);
t1.start();
```

`2.`  常駐型

就是做完事情他不會自動關閉，而是變成一個idle (閒置) 的狀態
閒置意思就是他沒事幹啦~  要給他事情做

```java
mThread = new HandlerThread("name");
//讓Worker待命，等待其工作 (開啟Thread)
mThread.start();
```

這樣子就可以建立且執行Thread了
連Runnable也不想打的話可以合併起來

```java
Thread XXX = new Thread(new Runnable()
{
   publicvoid run()
   {
      // ......工作
   }
}).start();
```

要給他事情做要這樣寫

```java
mThreadHandler.post(r1);
```
 

 

有沒有發現，`單次型`的Thread就是把事情定義下來然後呼叫start() `開始跑，跑完關閉`


而`常駐型`的話，反而是讓你先start()
然後post(r1)   給他事情做，做完就閒置
請務必記得不用這個Thread的時候要把他關閉

```java
if (mThread != null)
    mThread.quit();
```
大部分的情況，常駐型的Thread<br>
在`onCreate()裡面建立Thread` <br>
在`onDestory()裡面關閉Thread`

## Handler

那甚麼是Handler呢?
你可以想成是一個`服務的窗口`
給他事情做的地方


寫法有二種

`1.`  mUI_Handler.post(r2); <br>
`2.`  mUI_Handler.sendEmptyMessage(MSG_UPLOAD_OK); <br>


- 第一種就是直接`給他一個Runnable，讓他去執行`
- 第二種就是傳一個`Message`給他





Handler的建立
```java
private Handler mUI_Handler = new Handler();
```

這樣會建立一個基於Main Thread (UI Thread)的Handler


##Message

這東西不複雜，剛剛不是還在講Runnable嘛?
Message就是要一言以蔽之，用一個值  (一句話)
代表一堆事情(Runnable)

先看看Handler的變形吧

Handler的建立

```java
private Handler mUI_Handler = new Handler();
```

這樣會建立一個基於Main Thread (UI Thread)的Handler

以下是他的變形

```java
private Handler mUI_Handler = new Handler()
{
    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
        case MSG_UPLOAD_OK:
            // ..............
            break;
        }
    }
};
```

這樣會建立一個基於Main Thread (UI Thread)的Handler

有一個窗口，有一個地方可以處理Message的地方  ( 就是handleMessage() )

這裡用一個switch case的格式表示

msg.what  就是你訊息的內容

MSG_UPLOAD_OK  這就是你的訊息了(自己自訂)

像這樣

```java
privatestaticfinalint MSG_UPLOAD_OK= 0x00000001;
```

說穿了就是個`int`而已



要使用的時候就這樣

```java
mUI_Handler.sendEmptyMessage(MSG_UPLOAD_OK);
```
可以使用`帶參數`的寫法

```java
mUI_Handler.obtainMessage(MSG_UPLOAD_OK, arg1, arg2).sendToTarget();
```

或是

```java
mUI_Handler.obtainMessage(MSG_UPLOAD_OK, obj1).sendToTarget();
```

這樣可以帶二個int去，或是直接帶object給他(收到了之後再去轉型...)

