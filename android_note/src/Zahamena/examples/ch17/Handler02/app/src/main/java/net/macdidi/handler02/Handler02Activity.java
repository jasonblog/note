package net.macdidi.handler02;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;

import java.lang.ref.WeakReference;

public class Handler02Activity extends AppCompatActivity
{

    // 啟動與停止Handler的Switch元件
    private Switch handler01_switch, handler02_switch, handler03_switch;
    // 顯示Handler處理進度的SeekBar元件
    private SeekBar handler01_seekbar, handler02_seekbar, handler03_seekbar;

    private SeekBarHandler seekBarHandler = new SeekBarHandler(this);

    // 宣告一個用來處理多個不同工作的Handler類別
    private static class SeekBarHandler extends Handler
    {

        private final WeakReference<Handler02Activity> hActivity;

        public SeekBarHandler(Handler02Activity activity)
        {
            hActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg)
        {
            Handler02Activity activity = hActivity.get();

            if (activity != null) {
                super.handleMessage(msg);

                long uptimeMillis = 0;
                SeekBar target = null;

                // 使用參數Message物件的what變數判斷是哪一個元件編號
                switch (msg.what) {
                case R.id.handler01_switch:
                    // 設定目標為第一個SeekBar與間隔時間為100ms
                    target = activity.handler01_seekbar;
                    uptimeMillis = 100;
                    break;

                case R.id.handler02_switch:
                    // 設定目標為第二個SeekBar與間隔時間為200ms
                    target = activity.handler02_seekbar;
                    uptimeMillis = 200;
                    break;

                case R.id.handler03_switch:
                    // 設定目標為第三個SeekBar與間隔時間為300ms
                    target = activity.handler03_seekbar;
                    uptimeMillis = 300;
                    break;
                }

                // 設定SeekBar進度
                target.setProgress(getNextProgress(target));
                // 使用參數Message物件的what變數（元件編號）在指定的時間後送出Message
                activity.seekBarHandler.sendEmptyMessageDelayed(
                    msg.what, uptimeMillis);
            }
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_handler02);

        processViews();
        processControllers();
    }

    private void processViews()
    {
        handler01_switch = (Switch) findViewById(R.id.handler01_switch);
        handler02_switch = (Switch) findViewById(R.id.handler02_switch);
        handler03_switch = (Switch) findViewById(R.id.handler03_switch);

        handler01_seekbar = (SeekBar) findViewById(R.id.handler01_seekbar);
        handler02_seekbar = (SeekBar) findViewById(R.id.handler02_seekbar);
        handler03_seekbar = (SeekBar) findViewById(R.id.handler03_seekbar);
    }

    private void processControllers()
    {
        CompoundButton.OnCheckedChangeListener listener =
        new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                int id = buttonView.getId();

                if (isChecked) {
                    // 送出指定為元件編號的Message
                    seekBarHandler.sendEmptyMessage(id);
                } else {
                    // 移除元件編號的Message
                    seekBarHandler.removeMessages(id);
                }
            }
        };

        handler01_switch.setOnCheckedChangeListener(listener);
        handler02_switch.setOnCheckedChangeListener(listener);
        handler03_switch.setOnCheckedChangeListener(listener);
    }

    // 設定參數指定的SeekBar元件進度加一，到最大值的時候從0開始
    private static int getNextProgress(SeekBar seekbar)
    {
        int now = seekbar.getProgress();
        int max = seekbar.getMax();

        if (++now >= max) {
            now = 0;
        }

        return now;
    }

}
