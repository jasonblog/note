package net.macdidi.broadcast01;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

// 繼承自BroadcastReceiver的廣播接收元件
public class MyBroadcastReceiver extends BroadcastReceiver
{

    // 接收廣播後執行這個方法
    // 第一個參數Context物件，用來顯示訊息框、啟動服務
    // 第二個參數是發出廣播事件的Intent物件，可以包含資料
    @Override
    public void onReceive(Context context, Intent intent)
    {
        // 讀取包含在Intent物件中的資料
        String name = intent.getStringExtra("name");
        int age = intent.getIntExtra("age", -1);

        // 使用讀取的資料顯示訊息框
        String message = String.format("Name: %s%nAge: %d", name, age);
        Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
    }

}
