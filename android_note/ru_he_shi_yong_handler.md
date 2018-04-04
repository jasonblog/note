# 如何使用Handler

在Android使用執行緒要非常的小心,
使用者在進行操作時, 執行緒也在進行大量運算,
會造成使用者畫面卡死不動, 這樣的使用者體驗是不好的。

Android將Main Thread用來處理UI,
因此需要使用Thread讓大量運算在背景跑, 卻不影響使用者操作的畫面,
而如果需要畫面更新, 則會透過Handler機制去更新。

執行緒處理訊息機制(Handler、Looper、Message and MessageQueue)

```java
/*
* 一個Thread只能有一個Looper。
* 當Message處理完畢後, 會將Message發送給Handler。
*/
android.os.Looper

/*
* 一個Thread可以有多個Handler。
* 負責將Message送往MessageQueue, 並且接收Looper丟出來的Message。
*/
android.os.Handler

/*
* 一個Thread只能有一個MessageQueue。
* 負責裝載Message的佇列, 對Message進行管理, 是一個無限制的鏈結串列。
*/
android.os.MessageQueue

//執行緒上要處理的訊息。
```

如上圖, Handler負責派送訊息, 交給MessageQueue進行排隊, 
再透過Looper將每一個Message Object丟給Handler處理。

也許上面這些東西會有點陌生, 是因為Android Main Thread 一開始就先幫你綁定好了,
你不需要自訂初始化Looper並且綁定Handler，
下面的做法都是開啟Thread運算完後, 去呼叫Main Thread進行畫面更新。