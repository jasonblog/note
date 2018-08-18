package net.macdidi.broadcast02;

import android.app.Service;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiManager;
import android.os.IBinder;

// 繼承自Service的服務元件
public class WifiStateService extends Service
{

    // 廣播接收元件
    private WifiStateReceiver wifiStateReceiver;

    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Override
    public void onCreate()
    {
        // 建立廣播接收元件物件
        if (wifiStateReceiver == null) {
            wifiStateReceiver = new WifiStateReceiver();
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        // 準備註冊與移除廣播接收元件的IntentFilter物件
        IntentFilter filter = new IntentFilter();
        // 設定 Wi-Fi設備開啟或關閉的廣播事件
        filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        // 設定 Wi-Fi連線的廣播事件
        filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);

        // 註冊廣播接收元件
        registerReceiver(wifiStateReceiver, filter);
        return Service.START_STICKY;
    }

    @Override
    public void onDestroy()
    {
        // 移除廣播接收元件
        if (wifiStateReceiver != null) {
            unregisterReceiver(wifiStateReceiver);
            wifiStateReceiver = null;
        }
    }

}
