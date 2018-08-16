package net.macdidi.broadcast02;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

//繼承自BroadcastReceiver的廣播接收元件
public class BootCompletedReceiver extends BroadcastReceiver
{

    // 接收廣播後執行這個方法
    // 第一個參數Context物件，用來顯示訊息框、啟動服務
    // 第二個參數是發出廣播事件的Intent物件，可以包含資料
    @Override
    public void onReceive(Context context, Intent intent)
    {
        // 建立準備啟動服務的Intent物件，
        // 第一個參數使用onReceive方法的Context物件參數
        Intent serviceIntent =
            new Intent(context, BootCompletedService.class);
        // 使用參數Context物件呼叫startService方法啟動服務
        context.startService(serviceIntent);
    }

}
