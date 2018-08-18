package net.macdidi.appwidget05;

import android.app.Notification;
import android.app.Service;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;
import android.support.v4.app.NotificationCompat;

// 繼承自Service的服務元件
public class MyService05 extends Service
{

    // 廣播接收元件
    private MyTimeTick05 myTimeTick;

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Override
    public void onCreate()
    {
        // 建立廣播接收元件物件
        if (myTimeTick == null) {
            myTimeTick = new MyTimeTick05();
        }

        // 建立Notification物件
        Notification notification =
            new NotificationCompat.Builder(this, "").build();
        // 啟動服務元件
        startForeground(1, notification);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // 準備註冊廣播接收元件的IntentFilter物件
        // Action名稱指定為系統每一分鐘發送的廣播事件
        IntentFilter intentFilter = new IntentFilter(Intent.ACTION_TIME_TICK);
        // 註冊廣播接收元件
        registerReceiver(myTimeTick, intentFilter);
        return START_STICKY;
    }

    @Override
    public void onDestroy()
    {
        // 移除廣播接收元件
        unregisterReceiver(myTimeTick);
    }

}
