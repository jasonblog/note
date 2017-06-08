package com.archer.rainbow;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;

public class MyServiceActivity extends Activity
{
    private Button startSer1;
    private Button stopSer1;
    private Button startSer2;
    private Button stopSer2;
    private static TextView log;
    private Intent intent;
    private static ScrollView logView;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        startSer1 = (Button) findViewById(R.id.startSer1);
        stopSer1 = (Button) findViewById(R.id.stopSer1);

        startSer2 = (Button) findViewById(R.id.startSer2);
        stopSer2 = (Button) findViewById(R.id.stopSer2);

        log = (TextView) findViewById(R.id.log);

        logView = (ScrollView)findViewById(R.id.logView);

        startSer1.setOnClickListener(btnListener);
        stopSer1.setOnClickListener(btnListener);

        startSer2.setOnClickListener(btnListener);
        stopSer2.setOnClickListener(btnListener);

        intent = new Intent(MyServiceActivity.this, MyService.class);
    }

    private OnClickListener btnListener = new OnClickListener()
    {
        @Override
        public void onClick(View v) {
            switch (v.getId()) {
            case R.id.startSer1:
                updateLog("Start Service 1 pressed");
                startService(intent);
                break;

            case R.id.startSer2:
                updateLog("Start Service 2 pressed");
                startService(intent);
                break;

            case R.id.stopSer1:
                updateLog("Stop Service 1 pressed");
                stopService(intent);
                break;

            case R.id.stopSer2:
                updateLog("Stop Service 2 pressed");
                stopService(intent);
                break;

            default:
                break;
            }
        }
    };

    public static void updateLog(String text)
    {
        CharSequence ch = log.getText();
        log.setText(((ch == null || ch.length() == 0) ? text : ch.toString()
                     + "\r\n" + text));
        logView.fullScroll(ScrollView.FOCUS_DOWN);

    }
}