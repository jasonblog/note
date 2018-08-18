package net.macdidi.service01;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class MyServiceTemp extends Service
{

    // 提供別的元件取得這個服務元件物件，使用它執行一些工作
    // 參數是執行Binding工作的Intent物件
    @Override
    public IBinder onBind(Intent intent)
    {
        // 如果沒有這個需求，回傳null值就可以了
        return null;
    }

    // 啟動或Bind這個服務元件時，第一個呼叫的方法
    @Override
    public void onCreate()
    {
        // 執行服務元件初始化的工作，建立元件需要的資源
    }

    // 啟動服務元件時呼叫的方法
    // 第一個參數是啟動服務的Intent物件
    // 第二個參數是啟動服務的額外資訊，為0、START_FLAG_REDELIVERY或
    //    START_FLAT_RETRY
    // 第三個參數是啟動服務的編號，與stopSelfResult(int)搭配使用
    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // 執行服務元件的工作
        // 根據回傳值決定服務元件的重新啟動的行為
        return Service.START_STICKY;
    }

    // 停止服務的時候呼叫這個方法
    @Override
    public void onDestroy()
    {
        // 執行清除服務元件建立的資源、停止執行緒這類工作
    }
}
