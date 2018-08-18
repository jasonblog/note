package net.macdidi.broadcast02;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.NetworkInfo;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.widget.Toast;

//繼承自BroadcastReceiver的廣播接收元件
public class WifiStateReceiver extends BroadcastReceiver
{

    private boolean isShowSSID = false;

    // 接收廣播後執行這個方法
    // 第一個參數Context物件，用來顯示訊息框、啟動服務
    // 第二個參數是發出廣播事件的Intent物件，可以包含資料
    @Override
    public void onReceive(Context context, Intent intent)
    {
        // 讀取 Action 名稱
        String action = intent.getAction();

        // 如果是開啟或關閉 WIFI 設備的廣播事件
        if (WifiManager.WIFI_STATE_CHANGED_ACTION.equals(action)) {
            // 讀取 Wi-Fi 設備的狀態
            int state = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, 0);

            switch (state) {
            // Wi-Fi 設備已開啟
            case WifiManager.WIFI_STATE_ENABLED:
                Toast.makeText(context, "WIFI_STATE_ENABLED",
                               Toast.LENGTH_SHORT).show();
                break;

            // Wi-Fi 設備已關閉
            case WifiManager.WIFI_STATE_DISABLED:
                Toast.makeText(context, "WIFI_STATE_DISABLED",
                               Toast.LENGTH_SHORT).show();
                break;
            }
        }

        // 如果是 Wi-Fi 連線的廣播事件
        if (WifiManager.NETWORK_STATE_CHANGED_ACTION.equals(action)) {
            // 讀取 Wi-Fi 連線資訊
            NetworkInfo networkInfo =
                intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);

            if (networkInfo != null) {
                // 如果已經連線
                if (networkInfo.isConnected()) {
                    WifiManager manager = (WifiManager)
                                          context.getSystemService(Context.WIFI_SERVICE);
                    WifiInfo wifiInfo = manager.getConnectionInfo();

                    if (!isShowSSID) {
                        // 顯示網路設備名稱（SSID）
                        Toast.makeText(context, wifiInfo.getSSID(),
                                       Toast.LENGTH_SHORT).show();
                        isShowSSID = true;
                    }
                } else {
                    isShowSSID = false;
                }
            }
        }
    }

}
