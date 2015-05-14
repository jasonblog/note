# 什麼時候用startService，什麼時候用bindService



1. 採用startService方式啟動Service，然後互相發送廣播，使用Intent中傳遞數據，這個方式顯然效率不高

2. 更挫一點的方法，是用一中的方法startService之後，在onStartCommand中保存一個service對象的static全局引用，後面直接用

3. 採用bindService的方式啟動Service，然後在ServiceConnection::onServiceConnected(0中保存Service對象引用，後面直接調用Service的public方法就OK了，不需要在使用Server::onBider()函數返回的Binder繞著道調用Service


其實bindService()這樣用是大材小用了，我們一般用1、2兩種方法就夠了，必須用到bindService的情況很少。

本人的理解bindService真正的用場：

   a. 管理Service的什麼週期，就是講Servcie和調用方綁定，做到共患難，同生死。(其實你用startServcie，在你需要的地方stopServcie，是一樣的效果)

   b. 進程間通信，這個恐怕非bindService不可了




```
private ServiceConnection serviceConn = new ServiceConnection()
{
    @Override
    public void onServiceConnected(ComponentName className, IBinder service) {
        Logger.d("onServiceConnected called");
        // We've bound to LocalService, cast the IBinder and get LocalService
        // instance
        ServiceBinder binder = (ServiceBinder) service;
        scheduler = binder.getService();
    }
}

```

```
public class WalkseeService  extends Service
{

    private final IBinder binder = new ServiceBinder();

    /**
     * The Binder class that returns an instance of running service
     */
    public class ServiceBinder extends Binder
    {
        public WalkseeService getService()
        {
            return WalkseeService.this;   //这里返回Service对象
        }
    }

    @Override
    public IBinder onBind(Intent arg0)
    {

        return this.binder;
    }
}

```
