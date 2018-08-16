package net.macdidi.activitylife001;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity
{

    // 宣告使用Log類別時需要的標籤變數，通常設定為Activity元件類別名稱
    private static final String TAG = "ActivityLife01";

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        // 指定元件使用的畫面配置檔
        setContentView(R.layout.activity_main);
        // 執行元件需要的準備工作
        // 呼叫Log類別的d方法，記錄一個除錯訊息
        Log.d(TAG, "onCreate");

        class MyListener implements View.OnClickListener
        {
            @Override
            public void onClick(View arg0)
            {
                // 因為在巢狀類別中，所以要加上Activity元件類別名稱
                Toast.makeText(MainActivity.this, "Toast massage",
                               Toast.LENGTH_SHORT).show();
            }
        }

    }

    @Override
    protected void onRestart()
    {
        super.onRestart();
        // 設定畫面元件的內容
        Log.d(TAG, "onRestart");
    }

    @Override
    protected void onStart()
    {
        super.onStart();
        // 設定畫面元件的內容
        Log.d(TAG, "onStart");
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        // 啟動或回復元件需要的工作與執行緒
        Log.d(TAG, "onResume");
    }

    @Override
    protected void onPause()
    {
        // 停止執行中的工作與執行緒
        Log.d(TAG, "onPause");

        super.onPause();
    }

    @Override
    protected void onStop()
    {
        // 停止執行中的工作與執行緒
        Log.d(TAG, "onStop");
        super.onStop();
    }

    @Override
    protected void onDestroy()
    {
        // 清除所有元件建立的資源
        Log.d(TAG, "onDestroy");
        super.onDestroy();
    }

    public void goSecond(View view)
    {
        startActivity(new Intent(this, SecondActivity.class));
    }

    public void goThird(View view)
    {
        startActivity(new Intent(this, ThirdActivity.class));
    }

}
