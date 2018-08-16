package net.macdidi.broadcast02;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

//繼承自BroadcastReceiver的廣播接收元件
public class TimeTickReceiver extends BroadcastReceiver
{

    private static int times = 0;

    // 接收廣播後執行這個方法
    // 第一個參數Context物件，用來顯示訊息框、啟動服務
    // 第二個參數是發出廣播事件的Intent物件，可以包含資料
    @Override
    public void onReceive(Context context, Intent intent)
    {
        Toast.makeText(context, ++times + " minute(s)",
                       Toast.LENGTH_LONG).show();
    }

}
