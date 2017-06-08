package com.archer.rainbow;

import java.text.SimpleDateFormat;
import java.util.Date;

import android.app.IntentService;
import android.content.Intent;

public class IntentServiceDemo extends IntentService
{
    private static final String TAG = "IntentServiceDemo";
    private static final SimpleDateFormat SDF_DATE_FORMAT = new
    SimpleDateFormat("yyyy/MM/dd hh:mm:ss.SSS");

    public IntentServiceDemo()
    {
        super(TAG);
        MyServiceActivity.updateLog(TAG + " ----> constructor");
    }

    @Override
    public void onCreate()
    {
        super.onCreate();

        // 打印出该Service所在线程的ID
        long id = Thread.currentThread().getId();
        MyServiceActivity.updateLog(TAG + " ----> onCreate() in thread id: "
                                    + id);
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        MyServiceActivity.updateLog(TAG + " ----> onDestroy()");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        MyServiceActivity.updateLog(TAG + " ----> onStartCommand()");
        // 记录发送此请求的时间
        intent.putExtra("time", System.currentTimeMillis());
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void setIntentRedelivery(boolean enabled)
    {
        MyServiceActivity.updateLog(TAG + " ----> setIntentRedelivery()");
        super.setIntentRedelivery(enabled);
    }

    @Override
    protected void onHandleIntent(Intent intent)
    {
        // 打印出处理intent所用的线程的ID
        long id = Thread.currentThread().getId();
        MyServiceActivity.updateLog(TAG
                                    + " ----> onHandleIntent() in thread id: " + id);
        long time = intent.getLongExtra("time", 0);
        Date date = new Date(time);

        try {
            // 打印出每个请求对应的触发时间
            MyServiceActivity.updateLog(TAG
                                        + " ----> onHandleIntent(): 下载文件中..." + SDF_DATE_FORMAT.format(date));
            Thread.sleep(3000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

}
