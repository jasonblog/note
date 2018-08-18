package net.macdidi.activityevent01;

import android.content.res.Configuration;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class ActivityEvent01Activity extends AppCompatActivity
{

    private TextView text, info;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_activityevent01);

        processViews();
        processControllers();
    }

    private void processViews()
    {
        text = (TextView) findViewById(R.id.text);
        info = (TextView) findViewById(R.id.info);
    }

    private void processControllers()
    {
        // 註冊觸控事件
        text.setOnTouchListener(new MyTouch());
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig)
    {
        super.onConfigurationChanged(newConfig);

        int orientation = newConfig.orientation;
        String message = "";

        if (orientation == Configuration.ORIENTATION_LANDSCAPE) {
            message = "LANDSCAPE";
        } else if (orientation == Configuration.ORIENTATION_PORTRAIT) {
            message = "ORIENTATION_PORTRAIT";
        }

        info.setText(message);
    }

    // 觸控監聽類別
    private class MyTouch implements View.OnTouchListener
    {

        // 參數MotionEvent物件包裝觸控操作與資訊
        @Override
        public boolean onTouch(View v, MotionEvent event)
        {
            // 取得使用者操作種類
            int action = event.getAction();

            // 碰觸螢幕
            if (action == MotionEvent.ACTION_DOWN) {
                Toast.makeText(ActivityEvent01Activity.this,
                               "TextView DOWN",
                               Toast.LENGTH_SHORT).show();
            }

            // true表示事件不會往後傳遞
            // false表示事件會繼續傳遞給容器與Activity元件
            return false;
        }

    }

    // 按下返回按鍵，只有在沒有覆寫onKeyDown方法的時候才會呼叫
    @Override
    public void onBackPressed()
    {
        super.onBackPressed();
        Toast.makeText(ActivityEvent01Activity.this,
                       "BACK in onBackPressed", Toast.LENGTH_SHORT).show();
    }

    // 實體按鍵事件，參數KeyEvent物件包裝按鍵操作資訊
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event)
    {

        if (keyCode == KeyEvent.KEYCODE_BACK) {
            Toast.makeText(ActivityEvent01Activity.this,
                           "BACK in onKeyDown", Toast.LENGTH_SHORT).show();
        }

        return super.onKeyDown(keyCode, event);
    }

    // 觸控事件，參數MotionEvent物件包裝觸控操作與資訊
    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        // 呼叫getAction方法取得觸控操作，
        //     MotionEvent.ACTION_DOWN是碰觸螢幕
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            Toast.makeText(ActivityEvent01Activity.this,
                           "Activity DOWN", Toast.LENGTH_SHORT).show();
        }

        return super.onTouchEvent(event);
    }

}
