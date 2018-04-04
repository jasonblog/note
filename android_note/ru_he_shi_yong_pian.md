# 如何使用篇


- 為什麼要使用Service?
    - 將元件的生命週期跟Thread的生命週期分開(避免前述Thread參考到元件, 在Thread結束前無法釋放物件導致Memory leak)
    - 當一個Process內只剩下Thread在執行, 避免Process被系統意外回收, 導致Thread被提前結束
    - 跨行程演出


Service分成兩種: startService和bindService。

##startService
    - 由第一個元件啟動, 由第一個元件結束, 這個是最常見的, 也是最簡單的啟動方式。

一開始先宣告一個Service的類別

```java
 public class ServiceDemo extends Service {
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
```

由於是啟動Service, 因此onBind會是return null, 由於Service是運作在UI Thread上面, 
因此你必須將長任務開個Thread並且執行在onStartCommand方法內。

接著Actvitiy對此類別進行啟動。


```java
Intent intent = new Intent(this, ServiceDemo.class);
startService(intent);
```


有時候在系統記憶體吃緊的時候, 會將系統某些Service收起來, 這時候有幾個參數可以讓系統替你重新啟動Service。

- START_STICKY : Service被殺掉, 系統會重啟, 但是Intent會是null。
- START_NOT_STICKY : Service被系統殺掉, 不會重啟。
- START_REDELIVER_INTENT : Service被系統殺掉, 重啟且Intent會重傳。 透過以上的參數, 放在onStartCommand的return參數就可以使用重啟的功能了。

##bindService

- 由第一個元件進行連繫(bind), 此時Service就會啟動, 接著後面可以多個元件進行bind, 而當所有的元件都結束連繫(unbind), 則Service會進行銷毀。


```java
public class ServiceDemo extends Service {
    private MyBinder mBinder = new MyBinder();
    public IBinder onBind(Intent intent) {
        return mBinder;
    }
    @Override
    public boolean onUnbind(Intent intent) {
        return super.onUnbind(intent);
    }
    public String getServiceName(){
        return ServiceDemo.class.getName();
    }
    private class MyBinder extends Binder{
        public ServiceDemo getService(){
            return ServiceDemo.this;
        }
    }
}
```
以上程式碼可以看到Service的宣告, 此時你可以宣告一個Binder, 透過Binder來取得整個Service的實體, 因此你可以透過這個實體來引用Service上任何一個公開的方法。

##在Activtiy部分

```java

private LoaclServiceConnection mLoaclServiceConnection = new LoaclServiceConnection();
public class MainActivity extends Activity {
    protected void onCreate(Bundle savedInstanceState) {
        bindService(new Intent(this, ServiceDemo.class), mLoaclServiceConnection, Context.BIND_AUTO_CREATE);
    }
}
private class LoaclServiceConnection implements ServiceConnection{
    @Override
    public void onServiceConnected(ComponentName name, IBinder service) {
        //透過Binder調用Service內的方法

        String name = ((ServiceDemo)service.getService()).getServiceName();
    }
    @Override
    public void onServiceDisconnected(ComponentName name) {
        //service 物件設為null

    }
}
```

在這邊連繫的Service必須傳入一個ServiceConnection物件, 而當連繫成功以後, 就會呼叫ServiceConnection的onStartConnection方法, 將Service的Binder回傳回來, 透過轉型為Service本身的實體, 就可以操作Service上的任意方法。
這樣就是一個最簡單的Service雙向溝通。


