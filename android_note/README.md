# Service與Thread的區別


- 五種交互方式，分別是：通過廣播交互、通過共享文件交互、通過Messenger(信使)交互、通過自定義接口交互、通過AIDL交互。（可能更多）


### Service與Thread的區別
Thread：Thread 是程序執行的最小單元，可以用 Thread 來執行一些異步的操作。

Service：Service 是android的一種機制，當它運行的時候如果是Local Service，那麼對應的 Service 是運行在主進程的 main 線程上的。如果是Remote Service，那麼對應的 Service 則是運行在獨立進程的 main 線程上。



Thread 的運行是獨立的，也就是說當一個 Activity 被 finish 之後，如果沒有主動停止 Thread 或者 Thread 裏的 run 方法沒有執行完畢的話，Thread 也會一直執行。因此這裏會出現一個問題：當 Activity 被 finish 之後，不再持有該 Thread 的引用，也就是不能再控制該Thread。另一方面，沒有辦法在不同的 Activity 中對同一 Thread 進行控制。

**例如：如果 一個Thread 需要每隔一段時間連接服務器校驗數據，該Thread需要在後臺一直運行。這時候如果創建該Thread的Activity被結束了而該Thread沒有停止，那麼將沒有辦法再控制該Thread，除非kill掉該程序的進程。這時候如果創建並啓動一個 Service ，在 Service 裏面創建、運行並控制該 Thread，這樣便解決了該問題(因爲任何 Activity 都可以控制同一個Service，而系統也只會創建一個對應 Service 的實例)。**

因此可以把 Service 想象成一種消息服務，可以在任何有 Context 的地方調用 Context.startService、Context.stopService、Context.bindService、Context.unbindService來控制它，也可以在 Service 裏註冊 BroadcastReceiver，通過發送 broadcast 來達到控制的目的，這些都是 Thread 做不到的。


### Service的生命週期
1. 被啓動的服務(startService())的生命週期。
如果一個Service被某個Activity 調用Context.startService() 方法啓動，那麼不管是否有Activity使用bindService()綁定或unbindService()解除綁定到該Service，該Service都在後臺運行。如果一個Service被多次執行startService()，它的onCreate()方法只會調用一次，也就是說該Service只會創建一個實例，而它的onStartCommand()將會被調用多次(對應調用startService()的次數)。該Service將會一直在後臺運行，直到被調用stopService()，或自身的stopSelf方法。當然如果系統資源不足，系統也可能結束服務。

2. 被綁定的服務(bindService())的生命週期。如果一個Service被調用 Context.bindService ()方法綁定啓動，不管調用bindService()調用幾次，onCreate()方法都只會調用一次，而onStartCommand()方法始終不會被調用，這時會調用onBind()方法。當連接建立之後，Service將會一直運行，除非調用Context.unbindService() 斷開連接或者之前調用bindService() 的 Context 不存在了(如該Activity被finish)，系統將會自動停止Service，對應onDestroy()將被調用。

3. 被啓動又被綁定的服務的生命週期。如果一個Service又被啓動又被綁定，則該Service將會一直在後臺運行。調用unbindService()將不會停止Service，而必須調用stopService()或Service的stopSelf()方法來停止服務。

4. 當服務被停止時清除服務。
當一個Service被終止時，Service的onDestroy()方法將會被調用，在這裏應當做一些清除工作，如停止在Service中創建並運行的線程等。


### Process的生命週期

當Service運行在低內存的環境時，系統會kill掉一些進程。因此進程的優先級將會狠重要：
1.    如果Service當前正在執行onCreate()、onStartCommand()、onDestroy()方法，那麼此時主進程將會成爲前臺進程來保證代碼可以執行完成而避免被kill。

2.    如果Service已經啓動，那麼主進程將會比其他可見的進程的重要性低，但比其他看不見的進程高。這裡說的可見指的是對用戶來講，可見的進程優先級永遠是最高的，用戶至上嘛。但只有少部分進程始終是用戶可見的，因此除非系統處於極度低內存的時候，不然 service是不會被kill的。

3.    如果有Client端連到Service，那麼Service永遠比Client端重要。

4.    Service可以使用startForeground()將Service放到前臺狀態。這樣在低內存時被kill的機率更低，但如果在極低內存的情況下，該Service理論上還是會被kill掉。但這個情況基本不用考慮。
5.

