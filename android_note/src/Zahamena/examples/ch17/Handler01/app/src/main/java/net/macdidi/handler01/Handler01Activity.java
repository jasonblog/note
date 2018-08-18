package net.macdidi.handler01;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.Switch;

import java.lang.ref.WeakReference;

public class Handler01Activity extends AppCompatActivity
{

    private Switch handler01_switch, handler02_switch, handler03_switch;
    private SeekBar handler01_seekbar, handler02_seekbar, handler03_seekbar;

    private Handler handler01 = new Handler01(this);
    private Handler handler02 = new Handler02(this);
    private Handler handler03 = new Handler03(this);

    // 建立第一個SeekBar用的Handler物件
    private static class Handler01 extends Handler
    {

        private final WeakReference<Handler01Activity> hActivity;

        public Handler01(Handler01Activity activity)
        {
            hActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg)
        {
            Handler01Activity activity = hActivity.get();

            if (activity != null) {
                super.handleMessage(msg);

                // 設定第一個SeekBar進度
                activity.handler01_seekbar.setProgress(
                    getNextProgress(activity.handler01_seekbar));
                // 第一個參數固定為0，在第二個參數指定的100ms後送出Message
                activity.handler01.sendEmptyMessageDelayed(0, 100);
            }
        }
    }

    // 建立第二個SeekBar用的Handler物件
    private static class Handler02 extends Handler
    {

        private final WeakReference<Handler01Activity> hActivity;

        public Handler02(Handler01Activity activity)
        {
            hActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg)
        {
            Handler01Activity activity = hActivity.get();

            if (activity != null) {
                super.handleMessage(msg);

                // 設定第二個SeekBar進度
                activity.handler02_seekbar.setProgress(
                    getNextProgress(activity.handler02_seekbar));
                // 第一個參數固定為0，在第二個參數指定的100ms後送出Message
                activity.handler02.sendEmptyMessageDelayed(0, 200);
            }
        }
    }

    // 建立第三個SeekBar用的Handler物件
    private static class Handler03 extends Handler
    {

        private final WeakReference<Handler01Activity> hActivity;

        public Handler03(Handler01Activity activity)
        {
            hActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg)
        {
            Handler01Activity activity = hActivity.get();

            if (activity != null) {
                super.handleMessage(msg);

                // 設定第三個SeekBar進度
                activity.handler03_seekbar.setProgress(
                    getNextProgress(activity.handler03_seekbar));
                // 第一個參數固定為0，在第二個參數指定的100ms後送出Message
                activity.handler03.sendEmptyMessageDelayed(0, 300);
            }
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_handler01);

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
                Handler target = null;

                // 根據元件編號判斷要使用哪一個Handler物件
                if (id == R.id.handler01_switch) {
                    target = handler01;
                } else if (id == R.id.handler02_switch) {
                    target = handler02;
                } else {
                    target = handler03;
                }

                if (isChecked) {
                    // 向指定的Handler物件送出Message，參數固定為0
                    target.sendEmptyMessage(0);
                } else {
                    // 移除指定的Handler物件的Message，參數固定為0
                    target.removeMessages(0);
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
