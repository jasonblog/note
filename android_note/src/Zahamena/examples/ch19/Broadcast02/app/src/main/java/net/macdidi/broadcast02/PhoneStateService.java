package net.macdidi.broadcast02;

import android.app.Service;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;

// 繼承自Service的服務元件
public class PhoneStateService extends Service
{

    // 廣播接收元件
    private PhoneStateReceiver phoneStateReceiver;

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Override
    public void onCreate()
    {
        // 建立廣播接收元件物件
        if (phoneStateReceiver == null) {
            phoneStateReceiver = new PhoneStateReceiver();
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // 準備註冊與移除廣播接收元件的IntentFilter物件
        IntentFilter filter =
            new IntentFilter("android.intent.action.PHONE_STATE");
        // 註冊廣播接收元件
        registerReceiver(phoneStateReceiver, filter);
        return Service.START_STICKY;
    }

    @Override
    public void onDestroy()
    {
        // 移除廣播接收元件
        if (phoneStateReceiver != null) {
            unregisterReceiver(phoneStateReceiver);
            phoneStateReceiver = null;
        }
    }

}
