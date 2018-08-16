package net.macdidi.broadcast02;

import android.app.Service;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;

// 繼承自Service的服務元件
public class TimeTickService extends Service
{

    // 廣播接收元件
    private TimeTickReceiver timeTickReceiver;

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Override
    public void onCreate()
    {
        // 建立廣播接收元件物件
        if (timeTickReceiver == null) {
            timeTickReceiver = new TimeTickReceiver();
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // 準備註冊與移除廣播接收元件的IntentFilter物件
        IntentFilter filter = new IntentFilter(Intent.ACTION_TIME_TICK);
        // 註冊廣播接收元件
        registerReceiver(timeTickReceiver, filter);
        return Service.START_STICKY;
    }

    @Override
    public void onDestroy()
    {
        // 移除廣播接收元件
        if (timeTickReceiver != null) {
            unregisterReceiver(timeTickReceiver);
            timeTickReceiver = null;
        }
    }

}
