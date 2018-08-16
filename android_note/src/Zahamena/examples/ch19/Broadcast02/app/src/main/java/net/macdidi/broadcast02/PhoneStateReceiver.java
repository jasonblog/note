package net.macdidi.broadcast02;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.TelephonyManager;
import android.widget.Toast;

//繼承自BroadcastReceiver的廣播接收元件
public class PhoneStateReceiver extends BroadcastReceiver
{

    // 接收廣播後執行這個方法
    // 第一個參數Context物件，用來顯示訊息框、啟動服務
    // 第二個參數是發出廣播事件的Intent物件，可以包含資料
    @Override
    public void onReceive(Context context, Intent intent)
    {
        // 讀取電話狀態
        String state = intent.getStringExtra(TelephonyManager.EXTRA_STATE);
        String message = "";

        // 來電
        if (state.equals(TelephonyManager.EXTRA_STATE_RINGING)) {
            message = "RINGING : " + intent.getStringExtra(
                          TelephonyManager.EXTRA_INCOMING_NUMBER);
        }
        // 接聽
        else if (state.equals(TelephonyManager.EXTRA_STATE_OFFHOOK)) {
            message = "OFFHOOK";
        }
        // 掛斷
        else if (state.equals(TelephonyManager.EXTRA_STATE_IDLE)) {
            message = "IDLE";
        }

        Toast.makeText(context, message, Toast.LENGTH_LONG).show();
    }

}
